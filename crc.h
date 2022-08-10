typedef struct crc
{
    unsigned char digest[2];
}Crc;

Crc crc(unsigned char *message, int size);
