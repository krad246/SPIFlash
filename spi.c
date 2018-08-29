#include "spi.h"

void InitializeSPI() {
    P1DIR |= (BIT2 | BIT4 | BIT5);
    P2DIR |= BIT0;
    P1OUT = 0x00;
}

void SPISendByte(unsigned char SendValue) {
    unsigned char SV = SendValue;
    unsigned char i;
    for (i = 8; i > 0; i--) {
        if (((1 << (i - 1)) & SV) > 0)  {
            P1OUT |= BIT2;
        } else {
            P1OUT &= ~BIT2;
        }
        P1OUT |= BIT4;
        P1OUT &= ~BIT4;
    }
}

unsigned char SPIReceiveByte() {
    unsigned char ReceiveValue = 0;
    unsigned char i;
    for (i = 8; i > 0; i--) {
        ReceiveValue |= (((P1IN & BIT1) >> 1) << (i - 1));
        P1OUT |= BIT4;
        P1OUT &= ~BIT4;
    }
    return ReceiveValue;
}

