#include <msp430.h>
#include "spi.h"
#include "serial_flash.h"

int main(void) {
    volatile unsigned int ReceiveValue;
    volatile unsigned char SendValue = 0xAA;

    WDTCTL = WDTPW | WDTHOLD;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;

    InitializeSPI();
    InitializeSerialFlash();

    for (;;) {
        SPISendByte(SendValue);
        ReceiveValue = ReadFlashMemoryID(FLASH_MEMORY_U3);
        _delay_cycles(100000);
    }
    return 0;
}
