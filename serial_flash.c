#include "serial_flash.h"

inline void EnableMemory(unsigned char ComponentNumber) {
    if (ComponentNumber == FLASH_MEMORY_U3) {
        ENABLE_FLASH_MEMORY_U3;
    } else {
        ENABLE_FLASH_MEMORY_U2;
    }
}

inline void DisableMemory(unsigned char ComponentNumber) {
    if (ComponentNumber == FLASH_MEMORY_U3) {
        DISABLE_FLASH_MEMORY_U3;
    } else {
        DISABLE_FLASH_MEMORY_U2;
    }
}

inline void WriteEnable(unsigned char ComponentNumber) {
    EnableMemory(ComponentNumber);
    SPISendByte(0x06);
    DisableMemory(ComponentNumber);
}

inline void WriteDisable(unsigned char ComponentNumber) {
    EnableMemory(ComponentNumber);
    SPISendByte(0x04);
    DisableMemory(ComponentNumber);
}

void InitializeSerialFlash()
{
    ENABLE_FLASH_MEMORY_U3;
    ENABLE_FLASH_MEMORY_U2;

    SET_ENABLE_U3_AS_AN_OUTPUT;
    SET_ENABLE_U2_AS_AN_OUTPUT;

    SET_HOLD_U3_AS_AN_OUTPUT;
    SET_HOLD_U2_AS_AN_OUTPUT;

    SET_WRITE_PROTECT_AS_AN_OUTPUT;

    DISABLE_HOLD_U3;
    DISABLE_HOLD_U2;

    DISABLE_WRITE_PROTECT;

    DISABLE_FLASH_MEMORY_U3;
    DISABLE_FLASH_MEMORY_U2;
}

unsigned int ReadFlashMemoryID(unsigned char ComponentNumber)
{
    unsigned char ID[2];
    unsigned int ReturnValue = 0;

    EnableMemory(ComponentNumber);

    SPISendByte(READ_ID);

    SPISendByte(0x00); SPISendByte(0x00); SPISendByte(0x00);

    ID[0] = SPIReceiveByte();
    ID[1] = SPIReceiveByte();

    DisableMemory(ComponentNumber);

    ReturnValue = ((unsigned int) (ID[0] << 8)) + ((unsigned int) ID[1]);
    return ReturnValue;
}

unsigned char ReadFlashMemoryStatusRegister(unsigned char ComponentNumber)
{
    EnableMemory(ComponentNumber);

    SPISendByte(0x05);

    unsigned char status = SPIReceiveByte();

    DisableMemory(ComponentNumber);

    return status;
}

void WriteFlashMemoryStatusRegister(unsigned char WriteValue,unsigned char ComponentNumber)
{
    EnableMemory(ComponentNumber);

    SPISendByte(0x50);

    DisableMemory(ComponentNumber);

    EnableMemory(ComponentNumber);

    SPISendByte(0x01);

    SPISendByte(WriteValue);

    DisableMemory(ComponentNumber);
}

void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
                     unsigned int NumberOfDataValues, unsigned char ComponentNumber, unsigned char ReadMode)
{
    EnableMemory(ComponentNumber);

    SPISendByte(ReadMode);

    SPISendByte((unsigned char) (StartAddress >> 16));
    SPISendByte((unsigned char) (StartAddress >> 8));
    SPISendByte((unsigned char) (StartAddress >> 0));

    if (ReadMode == 0x0B) {
        SPISendByte(0x00);
    }

    unsigned int i;

    for (i = 0; i < NumberOfDataValues; i++) {
        DataValuesArray[i] = SPIReceiveByte();
    }

    DisableMemory(ComponentNumber);
}

void ByteProgramFlashMemory(unsigned long MemoryAddress, unsigned char WriteValue, unsigned char ComponentNumber)
{
    WriteEnable(ComponentNumber);
    EnableMemory(ComponentNumber);

    SPISendByte(0x02);

    SPISendByte((unsigned char) (MemoryAddress >> 16));
    SPISendByte((unsigned char) (MemoryAddress >> 8));
    SPISendByte((unsigned char) (MemoryAddress >> 0));

    SPISendByte(WriteValue);

    while (FlashMemoryBusy(ComponentNumber));

    DisableMemory(ComponentNumber);
}

void AAIProgramFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
                           unsigned int NumberOfDataValues, unsigned char ComponentNumber)
{
    WriteEnable(ComponentNumber);
    EnableMemory(ComponentNumber);

    unsigned int i;

    for (i = 0; i < NumberOfDataValues; i++) {
        SPISendByte(0xAF);
        if (i == 0) {
            SPISendByte((unsigned char) (StartAddress >> 16));
            SPISendByte((unsigned char) (StartAddress >> 8));
            SPISendByte((unsigned char) (StartAddress >> 0));
        }
        SPISendByte(DataValuesArray[i]);
        DisableMemory(ComponentNumber);
        while (FlashMemoryBusy(ComponentNumber));
        EnableMemory(ComponentNumber);
    }

    WriteDisable(ComponentNumber);

    EnableMemory(ComponentNumber);

    SPISendByte(0x05);

    DisableMemory(ComponentNumber);

}

void ChipEraseFlashMemory(unsigned char ComponentNumber)
{
    WriteEnable(ComponentNumber);
    EnableMemory(ComponentNumber);

    SPISendByte(0xC7);
    while (FlashMemoryBusy(ComponentNumber));

    DisableMemory(ComponentNumber);
}

void SectorBlockEraseFlashMemory(unsigned long StartAddress, unsigned char ComponentNumber, unsigned char EraseMode)
{
    WriteEnable(ComponentNumber);
    EnableMemory(ComponentNumber);

    SPISendByte(EraseMode);

    SPISendByte((unsigned char) (StartAddress >> 16));
    SPISendByte((unsigned char) (StartAddress >> 8));
    SPISendByte((unsigned char) (StartAddress >> 0));
    while (FlashMemoryBusy(ComponentNumber));

    DisableMemory(ComponentNumber);

}

void SetBlockProtection(unsigned char ProtectionLevel, unsigned char ComponentNumber)
{
    ENABLE_WRITE_PROTECT;

    WriteFlashMemoryStatusRegister(ProtectionLevel << 2, ComponentNumber);

    DISABLE_WRITE_PROTECT;
}

unsigned char FlashMemoryBusy(unsigned char ComponentNumber)
{
    return ReadFlashMemoryStatusRegister(ComponentNumber) % 2;
}
