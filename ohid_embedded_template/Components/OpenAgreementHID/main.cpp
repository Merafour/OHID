#include <QCoreApplication>
#include <math.h>
#include <stdio.h>
#include "OHID_Pack_Test.h"

uint32_t _get_key(void)
{
    uint32_t key = 0;
    key = (rand()&0xFF);
    key = (key<<8) | (rand()&0xFF);
    key = (key<<8) | (rand()&0xFF);
    key = (key<<8) | (rand()&0xFF);
    return key;
}
void get_key(uint32_t key[4])
{
    key[0] = _get_key();
    key[1] = _get_key();
    key[2] = _get_key();
    key[3] = _get_key();
    printf("key[4] = {0x%08X, 0x%08X, 0x%08X, 0x%08X};\r\n", key[0], key[1], key[2], key[3]);
}
int main(int argc, char *argv[])
{
    uint32_t key[4];
    QCoreApplication a(argc, argv);

    srand(0x0BB13965);
    get_key(key);
    get_key(key);
    get_key(key);
    OHID_Pack_Test();
    //OHID_signature_test();
    fflush(stdout);

    exit(0);
    return a.exec();
}
