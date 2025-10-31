/**
 * @file    Test.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
bool echo = false;
int tpm0_overflow_count = 0;
int pulse_width = 0;
/* TODO: insert other definitions and declarations here. */
/*
 * @brief   Application entry point.
 */
bool SW1_press(){
	return !(GPIOC->PDIR & (1<<3));
}
bool SW2_press(){
	return !(GPIOC->PDIR & (1<<12));
}
void setup_PortC(){
	//PTC3 (SW1) to GPIO, pull enable, pull select, input
	PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
	GPIOC->PDDR &= ~(1<<3); // Clear Data direction (input)
	//PTC12 (SW2) to GPIO, pull enable, pull select, input
	PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1) | PORT_PCR_PS(1);
	GPIOC->PDDR &= ~(1<<12); // Clear Data direction (input)
}
void setup_SIM_SCGC5(){
	//enable clock gating Port A,B,C,D
	//                        9,10,11,12
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1) |
				  SIM_SCGC5_PORTB(1) |
				  SIM_SCGC5_PORTC(1) |
				  SIM_SCGC5_PORTD(1);
}
void setup_SIM_SCGC6(){
	//enable clock gating TPM0, TPM1, TPM2, PIT
	// 					   24	 25    26   23
	SIM->SCGC6 |= SIM_SCGC6_TPM0(1) |
				  SIM_SCGC6_TPM1(1) |
				  SIM_SCGC6_TPM2(1) |
				  SIM_SCGC6_PIT(1);
}
void setup_PTD2(){
	PORTD->PCR[2] &= ~(0x700);
	PORTD->PCR[2] |= PORT_PCR_MUX(1);
	GPIOD->PDDR |= (1<<2);
}
void PIT_init(){
	PIT->MCR = 0x00;
	PIT->CHANNEL[0].LDVAL = 24; // interrupt every 1us
	PIT->CHANNEL[0].TCTRL = 0x0;
	PIT->CHANNEL[1].LDVAL = 2400000; //interrupt every 100ms
	NVIC_EnableIRQ(22);
}
void PIT_IRQHandler(){
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK; //clear flag
	pulse_width++;
}
void delay_10us(){
	int count = 48;
	while(count--){
		__asm("nop");   // no operation
	}
}
void delay_ms(float ms){
	float count = ms*48000;
	while(count--){
		__asm("nop");   // no operation
	}
	PRINTF("Done delay 60ms\n");
}
void delay_100ms(){
	for (int i = 0; i < 10000; i++){
		delay_10us();
	}
}
void start_receving(){
	TPM0->CNT = 0;
	TPM0->SC |= TPM_SC_CMOD(1);
	PRINTF("Start counting\n");
}
void send_trigger(){
	PRINTF("Trigger send\n");
	GPIOD->PSOR = (1 << 2);  // set PTD2 GPIO high
	start_receving();
	delay_10us();
	GPIOD->PCOR = (1 << 2);  // set PTD2 GPIO low
}
void setup_TPM0(){
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(2); // OSCERCLK 8MHz
	TPM0->SC |= TPM_SC_TOF_MASK; // clear overflow flag
	TPM0->MOD = 0xFFFF;                // reload every 1us
	TPM0->CNT = 0;                     // reset counter
	TPM0->SC |= TPM_SC_PS(3);           // prescaler = 8
	TPM0->SC |= TPM_SC_TOIE(1);    // enable overflow interrupt
	NVIC_EnableIRQ(17);
}
void TPM0_IRQHandler(){
	TPM0->SC |= TPM_SC_TOF_MASK;    // clear flag by writing 1
	tpm0_overflow_count++;
}
void stop_receiving(){
	//PIT->CHANNEL[0].TCTRL = 0x0;
	TPM0->SC &= ~TPM_SC_CMOD_MASK;
	PRINTF("Stop counting\n");
}

void setup_PortA_Interrupt(){
	//PTA13 to GPIO, pull enable, pull select, interrupt on raising edge,  input
	PORTA->PCR[13] &= ~(0xF700);
	PORTA->PCR[13] |= PORT_PCR_MUX(1) | PORT_PCR_IRQC(9);
	GPIOA->PDDR &= ~(1<<13); // Clear Data direction (input)
	NVIC_EnableIRQ(30);
}
void PORTA_IRQHandler(){
	stop_receiving();
	PRINTF("TPM0_CNT value: %d %d\n", TPM0->CNT, tpm0_overflow_count);
	TPM0->CNT = 0;
	PORTA->ISFR |= (1 << 13); // clear flag
}
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();

#endif
    setup_SIM_SCGC5();
    setup_SIM_SCGC6();
    setup_TPM0();
    setup_PTD2();
    setup_PortA_Interrupt();
    PIT_init();

    while(1) {
    	send_trigger();
    	delay_100ms();
		PRINTF("------------\n");
		//TPM0->CNT = 0;
		//tpm0_overflow_count = 0;

    }
    return 0 ;
}
