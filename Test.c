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
/*
 * @brief   Application entry point.
 */
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
    //                        24     25    26   23
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
void delay_10us(){
    int count = 48;
    while(count--){
        __asm("nop");   // no operation
    }
}
void delay_100ms(){
    for (int i = 0; i < 10000; i++){
        delay_10us();
    }
}
void send_trigger(){
    PRINTF("Trigger send\n");
    GPIOD->PSOR = (1 << 2);  // set PTD2 GPIO high
    delay_10us();
    GPIOD->PCOR = (1 << 2);  // set PTD2 GPIO low
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
    setup_PTD2();

    while(1) {
        send_trigger();
        delay_100ms();
        PRINTF("------------\n");
    }
    return 0 ;
}
