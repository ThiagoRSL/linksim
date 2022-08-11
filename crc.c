#include <stdio.h>
#include <stdlib.h>
#include "crc.h"
#include "link.h"

#define POLYNOME 0x11021 // CRC-16 - x^16 + x^12 + x^5 + 1

Crc crc(unsigned char *message, size_t size);

unsigned char* copyMessage(unsigned char* message, unsigned char* newMessage, size_t size);

void arrayXor(unsigned char* message);
void arrayLeftShift(unsigned char* message, size_t size);

Crc generateCRC(unsigned char *message, size_t size)
{
    unsigned char newMessage[LINK_FRAME_BYTE_MAX] = {0};
    unsigned char* dividend = copyMessage(message, newMessage, size);
    return crc(dividend, size);
}

Crc crc(unsigned char *message, size_t size)
{
    Crc result;

    for (int division = 0; division < size*8; division++)
    {
        if (message[0] & 0x80)
        {
            arrayLeftShift(message, size);
            arrayXor(message);
        }
        else
        {
            arrayLeftShift(message, size);
        }
    }

    result.digest[0] = message[0];
    result.digest[1] = message[1];

    return result;
}

unsigned char* copyMessage(unsigned char* message, unsigned char* newMessage, size_t size)
{
    int i;

    for (i = 0; i < size; i++)
    {
        newMessage[i] = message[i];
    }

    return newMessage;
}

void arrayLeftShift(unsigned char* message, size_t size)
{
    int i;

    for (i = 0; i < size-1; i++)
    {
        message[i] = message[i] << 1;
        message[i] = message[i] | (message[i+1] >> 7);
    }
    message[i] = message[i] << 1;
}

void arrayXor(unsigned char* message)
{
    message[0] = message[0] ^ ((POLYNOME & 0xFFFF) >> 8);
    message[1] = message[1] ^ (POLYNOME & 0xFF);
}

int main(int argc, char *argv[])
{
    unsigned char message[] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};
    unsigned char response[11] = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39};

    Crc crcRedundancy = generateCRC(message, sizeof(message));
    response[9] = crcRedundancy.digest[0];
    response[10] = crcRedundancy.digest[1];

    Crc crcValidation = generateCRC(response, sizeof(response));

    for (int i = 0; i < 9; i++)
    {
        printf("%x \n", message[i]);    
    }

    printf("%x %x \n", crcRedundancy.digest[0], crcRedundancy.digest[1]);

    for (int i = 0; i < 11; i++)
    {
        printf("%x \n", response[i]);    
    }

    printf("%x %x \n", crcValidation.digest[0], crcValidation.digest[1]);

    return 0;
}

