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
#ifndef _FIRMWARE_H_
#define _FIRMWARE_H_

#include <QString>
#include <QByteArray>
#include <QDebug>
class Firmware
{
public:
    Firmware();
    int Load(QString fwPath);
    uint32_t size(void);
    uint32_t fw_crc16(void);
    uint32_t fw_crc16(const uint32_t fw_size);

    void Print(const uint8_t bin[]);
    uint32_t Print(const uint32_t pos, const uint32_t len);
    uint32_t Print(const uint8_t fw[], const uint32_t len);
    uint32_t read(const uint32_t pos, const uint32_t bin_size, uint8_t bin[], const uint32_t rBlock);

private:
    uint32_t size(QByteArray &fw);
    int LoadHEX(QString fwPath);
    int LoadBinary(QString fwPath);

private:
    QByteArray FW;
};

#endif // FIRMWARE_H
