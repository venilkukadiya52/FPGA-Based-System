#include <xio.h>
#include <string.h>
#include <mb_interface.h>
#include "xparameters.h"

// Define the UART_BASE_ADDRESS.
#define UART_BASE_ADDRESS XPAR_UARTLITE_0_BASEADDR

// Define the UART register offsets.
#define UART_RX_REG      0x00
#define UART_TX_REG      0x04
#define UART_STAT_REG    0x08
#define UART_CTRL_REG    0x0C

// Define the bit masks for the appropriate status flags.
#define TX_FIFO_FULL_MASK    (1 << 3)
#define RX_FIFO_VALID_DATA   (1 << 0)
#define UART_INTR_ENABLE     (1 << 4)  // Example: Enable interrupt (check your product guide)

// Define interrupt enable bit for the UART module.
#define UART_INTR_ENABLE_MASK    (1 << 4)  // Adjust according to the product guide

// Global variable to keep track of the number of interrupts.
volatile u32 count = 0;

// Custom Interrupt Service Routine
//void MyISR(void *CallbackRef);
void MyISR() __attribute__((interrupt_handler));

void MyISR(void *CallbackRef) {
    // Service the interrupt.
    if (XIo_In32((volatile u32 *)(UART_BASE_ADDRESS + UART_STAT_REG)) & RX_FIFO_VALID_DATA) {
        // Read the received character.
        char receivedChar = XIo_In32((volatile u32 *)(UART_BASE_ADDRESS + UART_RX_REG)) & 0xFF;
//        char s[] = "Hello World, this is a demonstrator application for interrupt-driven IO!\r\n";
        print('H');

        // Echo the received character back.
        while (XIo_In32((volatile u32 *)(UART_BASE_ADDRESS + UART_STAT_REG)) & TX_FIFO_FULL_MASK);
        XIo_Out32((volatile u32 *)(UART_BASE_ADDRESS + UART_TX_REG), receivedChar);
    }

    // Clear the interrupt (if necessary, depending on the UART implementation).
    // Usually, reading the RX register clears the interrupt, but if required:
//    XIo_Out32((volatile u32 *)(UART_BASE_ADDRESS + UART_STAT_REG), RX_FIFO_VALID_DATA);

    count++;
}

void set_bit_in_reg(u32 addr, u32 bit) {
    // Read-modify-write
    u32 val = XIo_In32((volatile u32 *) addr);
    val |= bit;
    XIo_Out32((volatile u32 *) addr, val);
}

void clear_bit_in_reg(u32 addr, u32 bit) {
    // Read-modify-write
    u32 val = XIo_In32((volatile u32 *) addr);
    val &= ~bit;
    XIo_Out32((volatile u32 *) addr, val);
}

void print(char *s) {
    u32 status = 0;
    u32 len = strlen(s);

    for (u32 i = 0; i < len; i++) {
        do {
            status = XIo_In32((volatile u32 *)(UART_BASE_ADDRESS + UART_STAT_REG));
        } while (status & TX_FIFO_FULL_MASK);

        XIo_Out32((volatile u32 *)(UART_BASE_ADDRESS + UART_TX_REG), s[i]);
    }
}

int main(void) {


	u32 status = 0;
    // Initialize the interrupt controller.
//    Xil_ExceptionInit();

    // Register the custom ISR.
//    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//                                 (Xil_ExceptionHandler)MyISR,
//                                 NULL);

    // Enable the UART to generate interrupts.
    set_bit_in_reg(UART_BASE_ADDRESS + UART_CTRL_REG, UART_INTR_ENABLE_MASK);

    // Enable MicroBlaze to respond to interrupts.
//    Xil_ExceptionEnable();

    microblaze_enable_interrupts();


    // Print initial message

    print("s");

    while (1) {
    	// Main loop can perform other tasks, UART is handled by ISR.
    }

    // Disable interrupts (if needed).
//    Xil_ExceptionDisable();

    microblaze_disable_interrupts();


    // Exit normally.
    return 0;
}
