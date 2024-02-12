/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 固件封装
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "Firmware.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include "OHID/OHID_Pack.h"

extern "C" int hex2bin_main (const int argc, const char *const argv[]);

int hex2bin(const char fwPath[])
{
    // call hex2bin
    const char *const argv[] = { "./hex2bin", fwPath};
    int argc = sizeof(argv)/sizeof(argv[0]);
    qDebug("path: %s\n\n", fwPath);
    return hex2bin_main (argc, argv);
}

Firmware::Firmware()
{
    FW.resize(10);
    qDebug("[%s--%d] FW.length:%d", __func__, __LINE__, FW.length());
}

int Firmware::Load(QString fwPath)
{
    qDebug("[%s--%d] fwPath:%s", __func__, __LINE__, fwPath.toLocal8Bit().data());
    QString fwName, Path, fwSuffix;
    const char* name = NULL;
    const char* suffix = NULL;
    const char* dir = NULL;
    QFileInfo fileInfo = QFileInfo(fwPath);
    fwName = fileInfo.baseName();
    fwSuffix = fileInfo.suffix();
    Path = fileInfo.canonicalPath();
    name = fwName.toLocal8Bit().data();
    suffix = fwSuffix.toLocal8Bit().data();
    dir = Path.toLocal8Bit().data();
    qDebug("[%s--%d]  dir[%s] name[%s] suffix[%s]\r\n", __func__, __LINE__, dir, name, suffix);
    qDebug("[%s--%d] Path[%s]\r\n", __func__, __LINE__, QString(Path+"/"+fwName+".bin").toLocal8Bit().data());

    if(('b'==suffix[0]) && ('i'==suffix[1]) && ('n'==suffix[2]))
    {
        return LoadBinary(fwPath);
    }
    LoadHEX(fwPath);
    return LoadBinary(Path+"/"+fwName+".bin");
}

int Firmware::LoadHEX(QString fwPath)
{
    qDebug("[%s--%d] fwPath:%s", __func__, __LINE__, fwPath.toLocal8Bit().data());
    if(fwPath.isEmpty()) return -1;
    QFile fwFile(fwPath);
    if (!fwFile.exists()) return -2;
    if (!fwFile.open(QIODevice::ReadOnly | QIODevice::Text)) return -3;
    fwFile.close();
    hex2bin(fwPath.toLocal8Bit().data());
    return  true;
}
int Firmware::LoadBinary(QString fwPath)
{
    QByteArray fw;
    uint32_t ii;
    qDebug("[%s--%d] fwPath:%s", __func__, __LINE__, fwPath.toLocal8Bit().data());
    if(fwPath.isEmpty()) return -1;
    QFile fwFile(fwPath);
    if (!fwFile.exists()) return -2;
    if (!fwFile.open(QIODevice::ReadOnly)) return -3;
    fw = fwFile.readAll();
    fwFile.close();
    FW.resize(size(fw));
    for(ii=0; ii<(uint32_t)FW.size(); ii++) FW[ii] = 0xFF;
    for(ii=0; ii<(uint32_t)fw.size(); ii++) FW[ii] = fw[ii];
    qDebug("[%s--%d] FW: %d %d\r\n", __func__, __LINE__, FW.length(), FW.size());
    QFile File(fwPath+".bak");
    qDebug() << "FilePath:" << fwPath+".bak";
    if (File.open(QIODevice::ReadWrite/* | QIODevice::NewOnly*/))
    {
        File.write(FW);
        File.close();
        return 0;
    }
    return  true;
}


uint32_t Firmware::size(void)
{
    return size(FW);
}
uint32_t Firmware::fw_crc16(void)
{
    uint32_t CRCin;
    uint32_t fw_size = size();
    uint8_t code[64];
    uint32_t len, pos;
    //qDebug("\r\n[%s--%d] CRC fw_size:%d [0x%08X] %dKB", __func__, __LINE__, fw_size, fw_size, fw_size/1024);
    CRCin = 0;
    for(pos=0; pos<fw_size; pos+=sizeof(code))
    {
        len = fw_size-pos;
        if(len>sizeof(code)) len=sizeof(code);
        read(pos, sizeof(code), code, len);
        CRCin = ohid_crc32(code, len, CRCin);
    }
    return CRCin;
}
uint32_t Firmware::fw_crc16(const uint32_t fw_size)
{
    uint32_t CRCin;
    //uint32_t fw_size = size();
    uint8_t code[64];
    uint32_t len, pos;
    //qDebug("\r\n[%s--%d] CRC fw_size:%d [0x%08X] %dKB", __func__, __LINE__, fw_size, fw_size, fw_size/1024);
    CRCin = 0;
    for(pos=0; pos<fw_size; pos+=sizeof(code))
    {
        len = fw_size-pos;
        if(len>sizeof(code)) len=sizeof(code);
        read(pos, sizeof(code), code, len);
        CRCin = ohid_crc32(code, len, CRCin);
    }
    return CRCin;
}

void Firmware::Print(const uint8_t bin[])
{
    qDebug("[%s--%d] %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", __func__, __LINE__, \
           bin[0], bin[1], bin[2], bin[3], bin[4], bin[5], bin[6], bin[7], bin[8], bin[9], bin[10], bin[11], bin[12], bin[13], bin[14], bin[15]);
}
uint32_t Firmware::Print(const uint32_t pos, const uint32_t len)
{
    uint32_t i;
    uint8_t bin[32];
    const uint32_t rBlock=16;
    uint32_t fw_pos;
    fw_pos = pos;
    qDebug("\r\n[%s--%d] pos:%d len:%d ", __func__, __LINE__, fw_pos, len);
    for(i=0; i<len; i+=rBlock)
    {
        memset(bin, 0x00, sizeof(bin));
        read(fw_pos, sizeof(bin), bin, rBlock);
        qDebug("[%s--%d] 0x%08X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", __func__, __LINE__, fw_pos, \
               bin[0], bin[1], bin[2], bin[3], bin[4], bin[5], bin[6], bin[7], bin[8], bin[9], bin[10], bin[11], bin[12], bin[13], bin[14], bin[15]);
        fw_pos+=rBlock;
    }
    qDebug("\r\n");
    return i;
}
uint32_t Firmware::Print(const uint8_t fw[], const uint32_t len)
{
    uint32_t i;
    uint8_t bin[32];
    const uint32_t rBlock=16;
    uint32_t fw_pos;
    fw_pos = 0;
    qDebug("[%s--%d] pos:%d len:%d ", __func__, __LINE__, fw_pos, len);
    for(i=0; i<len; i+=rBlock)
    {
        memset(bin, 0x00, sizeof(bin));
        memcpy(bin, &fw[fw_pos], rBlock);
        qDebug("[%s--%d] 0x%08X: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", __func__, __LINE__, fw_pos, \
               bin[0], bin[1], bin[2], bin[3], bin[4], bin[5], bin[6], bin[7], bin[8], bin[9], bin[10], bin[11], bin[12], bin[13], bin[14], bin[15]);
        fw_pos+=rBlock;
    }
    return i;
}
uint32_t Firmware::read(const uint32_t pos, const uint32_t bin_size, uint8_t bin[], const uint32_t rBlock)
{
    uint32_t cnt;
    memset(bin, 0xFF, bin_size);
    for(cnt=0; cnt<rBlock; cnt++)
    {
        //bin[cnt] = 0xFF;
        if((pos+cnt)<(uint32_t)FW.length()) bin[cnt] = FW.at(pos+cnt)&0xFF;
    }
    return cnt;
}

uint32_t Firmware::size(QByteArray &fw)
{
    uint32_t fw_size;
    uint32_t length;
    fw_size = fw.length();
    // align (512)
    if(0!=(fw_size&0x01FF)) fw_size = fw_size + 512 - (fw_size&0x01FF);
    length = fw.length();
    //qDebug() << QString().asprintf("[%s--%d] FW Size: %d %dKB Write Size %d %dKB", __func__, __LINE__, length, length/1024, fw_size, fw_size/1024);
    return fw_size;
}

