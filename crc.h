typedef struct crc
{
    unsigned char digest[2];
}Crc;

Crc generateCRC(unsigned char *message, size_t size);
