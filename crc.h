typedef struct crc
{
    unsigned char digest[2];
}Crc;

Crc generateCRC(unsigned char *message, int size);
Crc validateCRC(unsigned char *message, int size);
