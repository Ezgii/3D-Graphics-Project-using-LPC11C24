/*
 * lcd__api.c
 *
 *  Created on: Sep 30, 2022
 *      Author: brucejiang
 */

#include "lcd__api.h"
#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
#define LOOPBACK_TEST       0
#define SSP_MODE_TEST       1	/*1: Master, 0: Slave */
#define POLLING_MODE        1
#if POLLING_MODE
#define INTERRUPT_MODE      0
#else
#define INTERRUPT_MODE      1
#endif
//#define BUFFER_SIZE                         (0x100) // 256 bytes
#define BUFFER_SIZE                         2 // 2 bytes
#define SSP_DATA_BITS                       (SSP_BITS_8)
#define SSP_DATA_BIT_NUM(databits)          (databits + 1)
#define SSP_DATA_BYTES(databits)            (((databits) > SSP_BITS_8) ? 2 : 1)
#define SSP_LO_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? 0xFF : (0xFF >> \
																					  (8 - SSP_DATA_BIT_NUM(databits))))
#define SSP_HI_BYTE_MSK(databits)           ((SSP_DATA_BYTES(databits) > 1) ? (0xFF >> \
																			   (16 - SSP_DATA_BIT_NUM(databits))) : 0)
#define LPC_SSP           LPC_SSP0
#define SSP_IRQ           SSP0_IRQn
#define SSPIRQHANDLER     SSP0_IRQHandler

int height = ST7735_TFTHEIGHT;
int width = ST7735_TFTWIDTH;

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
/* Tx buffer */
static uint8_t Tx_Buf[BUFFER_SIZE];

/* Rx buffer*/
static uint8_t Rx_Buf[BUFFER_SIZE];

static SSP_ConfigFormat ssp_format;
static Chip_SSP_DATA_SETUP_T xf_setup;
static volatile uint8_t  isXferCompleted = 0;
/*****************************************************************************
 * Private functions
 ****************************************************************************/
/* private functions list*/
static void Init_SSP_PinMux(void);
static void Buffer_Init(void);
static uint8_t Buffer_Verify(void);
static void sspSend(uint8_t *buf, int buf_size);
static void spiwrite_single_transfer(uint8_t c);
static void writecommand(uint8_t c);
static void writedata(uint8_t c);
static void writecoord(uint16_t c);
static void writecolor(uint32_t color,uint32_t repeat);
static void setAddrWindow(uint16_t x0,uint16_t y0, uint16_t x1, uint16_t y1);

#if INTERRUPT_MODE
static void SSPIRQHANDLER(void);
#endif

/* Set pin mux for SSP operation */
static void Init_SSP_PinMux(void)
{
#if (defined(BOARD_NXP_XPRESSO_11U14) || defined(BOARD_NGX_BLUEBOARD_11U24))
	/* Only SSP0 is supported */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2, (IOCON_FUNC1 | IOCON_MODE_PULLUP));	/* SSEL0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 29, (IOCON_FUNC1 | IOCON_MODE_PULLUP));	/* SCK0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, (IOCON_FUNC1 | IOCON_MODE_PULLUP));	/* MISO0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_FUNC1 | IOCON_MODE_PULLUP));	/* MOSI0 */
#elif defined(BOARD_NXP_XPRESSO_11C24)
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MISO0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MOSI0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* SSEL0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO2_11, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* SCK0 */
	Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO2_11);
#elif defined(BOARD_MCORE48_1125)
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_8, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MISO0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_9, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* MOSI0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_2, (IOCON_FUNC1 | IOCON_MODE_INACT));	/* SSEL0 */
	Chip_IOCON_PinMuxSet(LPC_IOCON, IOCON_PIO0_6, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SCK0 */
	Chip_IOCON_PinLocSel(LPC_IOCON, IOCON_SCKLOC_PIO0_6);
#else
	#error "SSP Pin muxing not configured"
#endif
}
/* Initialize buffer */
static void Buffer_Init(void)
{
	uint16_t i;
	uint8_t ch = 0;

	for (i = 0; i < BUFFER_SIZE; i++) {
		Tx_Buf[i] = ch++;
		Rx_Buf[i] = 0xAA;
	}
}

#if INTERRUPT_MODE
/**
 * @brief	SSP interrupt handler sub-routine
 * @return	Nothing
 */
static void SSPIRQHANDLER(void)
{
	Chip_SSP_Int_Disable(LPC_SSP);	/* Disable all interrupt */
	if (SSP_DATA_BYTES(ssp_format.bits) == 1) {
		Chip_SSP_Int_RWFrames8Bits(LPC_SSP, &xf_setup);
	}
	else {
		Chip_SSP_Int_RWFrames16Bits(LPC_SSP, &xf_setup);
	}

	if ((xf_setup.rx_cnt != xf_setup.length) || (xf_setup.tx_cnt != xf_setup.length)) {
		Chip_SSP_Int_Enable(LPC_SSP);	/* enable all interrupts */
	}
	else {
		isXferCompleted = 1;
	}
}

#endif /*INTERRUPT_MODE*/

/* Verify buffer after transfer */
static uint8_t Buffer_Verify(void)
{
	uint16_t i;
	uint8_t *src_addr = (uint8_t *) &Tx_Buf[0];
	uint8_t *dest_addr = (uint8_t *) &Rx_Buf[0];

	for ( i = 0; i < BUFFER_SIZE; i++ ) {

		if (((*src_addr) & SSP_LO_BYTE_MSK(ssp_format.bits)) !=
			((*dest_addr) & SSP_LO_BYTE_MSK(ssp_format.bits))) {
			return 1;
		}
		src_addr++;
		dest_addr++;

		if (SSP_DATA_BYTES(ssp_format.bits) == 2) {
			if (((*src_addr) & SSP_HI_BYTE_MSK(ssp_format.bits)) !=
				((*dest_addr) & SSP_HI_BYTE_MSK(ssp_format.bits))) {
				return 1;
			}
			src_addr++;
			dest_addr++;
			i++;
		}
	}
	return 0;
}

static void sspSend(uint8_t *buf,int buf_size){
#if INTERRUPT_MODE
	/* Setting SSP interrupt */
	NVIC_EnableIRQ(SSP_IRQ);
#endif

#if LOOPBACK_TEST
	Chip_SSP_EnableLoopBack(LPC_SSP);
#endif
	xf_setup.length = buf_size;
	xf_setup.tx_data = buf;
	xf_setup.rx_data = Rx_Buf;
	xf_setup.rx_cnt = xf_setup.tx_cnt = 0;

#if POLLING_MODE
	Chip_SSP_RWFrames_Blocking(LPC_SSP, &xf_setup);
#elif INTERRUPT_MODE
	//Chip_SSP_Int_FlushData(LPC_SSP);/* flush dummy data from SSP FiFO */
	if (SSP_DATA_BYTES(ssp_format.bits) == 1) {
		Chip_SSP_Int_RWFrames8Bits(LPC_SSP, &xf_setup);
		//buf++;
	}
	else {
		Chip_SSP_Int_RWFrames16Bits(LPC_SSP, &xf_setup);
		//buf++;
	}

	Chip_SSP_Int_Enable(LPC_SSP);			/* enable interrupt */
	while (!isXferCompleted) {}
#endif /*INTERRUPT_MODE*/

//	if (Buffer_Verify()) {
//		Board_LED_Set(0, false);	/* Error */
//		//Buffer_Init();
//	}
//	else {
//		Board_LED_Set(0, true);	/* Success */
//		//Buffer_Init();
//	}

#if LOOPBACK_TEST
	Chip_SSP_DisableLoopBack(LPC_SSP);
#endif
	/* DeInitialize SPI peripheral */
	//Chip_SSP_DeInit(LPC_SSP);
	Buffer_Init();
}

static void spiwrite_single_transfer(uint8_t c){
	Tx_Buf[0] = c;

	sspSend(Tx_Buf, 1);
}

static void writecommand(uint8_t c){
	//toggle D/C off at PIO2_1
	Chip_GPIO_SetPinState(LPC_GPIO,2,1,0);

	spiwrite_single_transfer(c);

}

static void writedata(uint8_t c){
	//toggle D/C on at PIO2_1
	Chip_GPIO_SetPinState(LPC_GPIO,2,1,1);

	spiwrite_single_transfer(c);

}

static void writecoord(uint16_t c){
	//toggle D/C on at PIO2_1
	Chip_GPIO_SetPinState(LPC_GPIO,2,1,1);

	//size of uint16 is 2 bytes
	Tx_Buf[0] = c >> 8; // Separate 2 bytes in half in 1 byte each
	Tx_Buf[1] = c & 0xFF;

	sspSend(Tx_Buf, 2);// sending pay load
}

static void writecolor(uint32_t color, uint32_t repeat){
	 uint8_t red, green, blue;

	 red = (color >> 16);

	 green = (color >> 8) & 0xFF;

	 blue = color & 0xFF;

	 for (int i = 0; i< repeat; i++) {

		writedata(red);

		writedata(green);

		writedata(blue);

	 }
}
static void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){

	 writecommand(ST7735_CASET);//set columns

	 writecoord(x0);
	 writecoord(x1);

	 writecommand(ST7735_RASET);//set rows

	 writecoord(y0);
	 writecoord(y1);

}
/*****************************************************************************
 * Public functions
 ****************************************************************************/
#define swap(x, y) {x = x + y; y = x - y; x = x - y ;}

void fillrect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color){

	 int16_t width, height;

	 width = x1-x0+1;

	 height = y1-y0+1;

	 setAddrWindow(x0,y0,x1,y1);

	 writecommand(ST7735_RAMWR);

	 writecolor(color,width*height);

}

void lcddelay(int ms){
 int count = 24000;

 int i;

 for ( i = count*ms; i>0; i--);
}

void lcd_init(){
//	printf("LCD Demo Begins!!!\n");
	// Set pins PIO2_1, PIO2_2 as output
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,2,1);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO,2,2);

	// Hardware Reset Sequence
	Chip_GPIO_SetPinState(LPC_GPIO,2,2,1);
	lcddelay(100);

	Chip_GPIO_SetPinState(LPC_GPIO,2,2,0);
	lcddelay(100);

	Chip_GPIO_SetPinState(LPC_GPIO,2,2,1);
	lcddelay(100);

	Buffer_Init();

	// Take LCD display out of sleep mode
	writecommand(ST7735_SLPOUT);
	lcddelay(100);

	// Turn LCD display on
	writecommand(ST7735_DISPON);
	lcddelay(100);
}

void board_init(void){
	/* Board initialization */
	SystemCoreClockUpdate();
	Board_Init();
}

void ssp0_init(void){
	/* SSP initialization */
	Init_SSP_PinMux();
	//Board_LED_Set(0, 0);
	Chip_SSP_Init(LPC_SSP);

	ssp_format.frameFormat = SSP_FRAMEFORMAT_SPI;
	ssp_format.bits = SSP_DATA_BITS;
	ssp_format.clockMode = SSP_CLOCK_MODE0;
	Chip_SSP_SetFormat(LPC_SSP, ssp_format.bits, ssp_format.frameFormat, ssp_format.clockMode);
	Chip_SSP_SetMaster(LPC_SSP, SSP_MODE_TEST);
	Chip_SSP_Enable(LPC_SSP);
}

void drawPixel(int16_t x, int16_t y, uint32_t color){

	if ((x < 0) || (x >= width) || (y < 0) || (y >= height)){return;}

	setAddrWindow(x, y, x + 1, y + 1);

	writecommand(ST7735_RAMWR);

	writecolor(color, 1);

}
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{

	int16_t slope = abs(y1 - y0) > abs(x1 - x0);

	if (slope) {

		swap(x0, y0);

		swap(x1, y1);

	}

	if (x0 > x1) {

		swap(x0, x1);

		swap(y0, y1);

	}

	int16_t dx, dy;

	dx = x1 - x0;

	dy = abs(y1 - y0);

	int16_t err = dx / 2;

	int16_t ystep;

	if (y0 < y1) {

		ystep = 1;

	}

	else {

		ystep = -1;

	}

	for (; x0 <= x1; x0++) {

		if (slope) {

			drawPixel(y0, x0, color);

		}

		else {

			drawPixel(x0, y0, color);

		}

			err -= dy;

		if (err < 0) {

			y0 += ystep;

			err += dx;

		}

	}

}



