#include <stdio.h>
#include <stdlib.h>
#include "crc.h"

#define POLYNOME 0x11021 // CRC-16 - x^16 + x^12 + x^5 + 1

Crc crc(unsigned char *message, int size);

unsigned char* copyMessage(unsigned char* message, int size);
unsigned char* preProcessMessage(unsigned char* message, int size);

void arrayXor(unsigned char* message);
void arrayLeftShift(unsigned char* message, int size);

Crc generateCRC(unsigned char *message, int size)
{
    unsigned char* dividend = preProcessMessage(message, size);
    return crc(dividend, size);
}

Crc validateCRC(unsigned char *message, int size)
{
    unsigned char* dividend = copyMessage(message, size);
    return crc(dividend, size);
}

Crc crc(unsigned char *message, int size)
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

unsigned char* preProcessMessage(unsigned char* message, int size)
{
    unsigned char* newMessage = (unsigned char*) malloc(sizeof(unsigned char)*(size+2));
    int i;

    for (i = 0; i < size; i++)
    {
        newMessage[i] = message[i];
    }

    newMessage[i] = 0x0;
    newMessage[i+1] = 0x0;

    return newMessage;
}

unsigned char* copyMessage(unsigned char* message, int size)
{
    unsigned char* newMessage = (unsigned char*) malloc(sizeof(unsigned char)*(size+2));
    int i;

    for (i = 0; i < size; i++)
    {
        newMessage[i] = message[i];
    }

    return newMessage;
}

void arrayLeftShift(unsigned char* message, int size)
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
    unsigned char message[] = {0x3B, 0xB6, 0xC2};
    unsigned char response[5] = {0x3B, 0xB6, 0xC2};

    Crc crcRedundancy = generateCRC(message, sizeof(message));
    response[3] = crcRedundancy.digest[0];
    response[4] = crcRedundancy.digest[1];

    Crc crcValidation = validateCRC(response, sizeof(response));

    printf("%x %x %x \n", message[0], message[1], message[2]);

    printf("%x %x \n", crcRedundancy.digest[0], crcRedundancy.digest[1]);

    printf("%x %x %x %x %x \n", response[0], response[1], response[2], response[3], response[4]);

    printf("%x %x \n", crcValidation.digest[0], crcValidation.digest[1]);

    return 0;
}

