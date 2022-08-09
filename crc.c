#include <stdio.h>
#include "crc.h"

#define POLYNOME 0x11021 // CRC-16 - x^16 + x^12 + x^5 + 1
#define POLYNOME_DEGREE 16
#define MSB (1 << (POLYNOME_DEGREE - 1))

unsigned char* preProcessMessage(unsigned char* message, int size);
void arrayXor(unsigned char* message);
void arrayLeftShift(unsigned char* message, int size);

Crc crc(unsigned char *message, int size)
{
    unsigned char* dividend = preProcessMessage(message, size);
    Crc result;

    printf("message: ");
    for (int i = 0; i < size+2; i++)
    {
        printf("%u ", dividend[i]);
    }
    printf("\n");


    for (int division = 0; division < size*8; division++)
    {
        if (dividend[0] & 0x80)
        {
            arrayLeftShift(dividend, size);
            arrayXor(dividend);
        }
        else
        {
            arrayLeftShift(dividend, size);
        }
    }

    result.digest[0] = dividend[0];
    result.digest[1] = dividend[1];

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

    Crc crcRedundancy = crc(message, sizeof(message));

    printf("\n");
    printf("%x %x \n", crcRedundancy.digest[0], crcRedundancy.digest[1]);

    return 0;
}

