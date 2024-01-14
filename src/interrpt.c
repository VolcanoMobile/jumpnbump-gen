#include <genesis.h>

char keyb[256];

void initKeyb()
{
    memset(&keyb[0], 0, 256);
}

void addkey(unsigned int key)
{
    if (!(key & 0x8000)) {
        keyb[key & 0x7fff] = 1;
    } else
        keyb[key & 0x7fff] = 0;
}

int key_pressed(int key)
{
    return keyb[(unsigned char) key];
}
