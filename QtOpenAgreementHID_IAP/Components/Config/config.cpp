/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 配置文件
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "config.h"
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QByteArray>

Config::Config_Datas Config::devices;

Config::Config()
{
    ;
}

void Config::Init(void)
{
    uint16_t ii=0;
    for(ii=0; ii<((sizeof(devices.list)/sizeof(devices.list[0]))); ii++)
    {
        devices.list[ii].product = 0x00;
        devices.list[ii].vendor  = 0x00;
    }
    devices.cnt = 0;
    FilePath = "";
//    if(GetPath().isEmpty())
//    {
//        qDebug() << "File path is empty";
//    }
    GetPath();
    QFile aFile(FilePath);
    if (!aFile.exists()) //文件不存在
        return ;
    if (!aFile.open(QIODevice::ReadOnly))
        return ;
    QByteArray cfg = aFile.readAll();
    for(ii=0; ii<(cfg.size()); ii++)
    {
        devices.bin[ii] = cfg.at(ii)&0xFF;
    }
    aFile.close();
    qDebug("[%s--%d] cfg.length: %d cfg.size:%d cnt:%d\r\n", __func__, __LINE__, cfg.length(), cfg.size(), devices.cnt);
}

const Config::usb_vid_pid *Config::read(const uint8_t index)
{
    if(index<devices.cnt) return (&(devices.list[index]));
    return nullptr;
}

const char *Config::readPath()
{
    if('\0'==devices.path[0]) return nullptr;
    return devices.path;
}

uint16_t Config::size()
{
    return devices.cnt;
}

int Config::write(const uint16_t vendor, const uint16_t product)
{
    uint16_t ii=0;
    uint16_t repetition = 0;
    for(ii=0; ii<(devices.cnt); ii++)
    {
        if((product==devices.list[ii].product) && (vendor==devices.list[ii].vendor))
        {
            repetition = 1;
        }
    }
    qDebug() << "repetition:" << repetition;
    if(0==repetition)
    {
        devices.list[devices.cnt].product = product;
        devices.list[devices.cnt].vendor  = vendor;
        devices.cnt++;
    }
    return save(devices);
}
int Config::writePath(const char fwPath[])
{
    uint16_t ii=0;
    for(ii=0; ii<sizeof(devices.path); ii++) devices.path[ii] = '\0';
    qDebug() << "fwPath:" << fwPath;
    for(ii=0; ii<sizeof(devices.path)-1; ii++)
    {
        if('\0'==fwPath[ii]) break;
        devices.path[ii] = fwPath[ii];
    }
    return save(devices);
}
int Config::remove(const uint16_t vendor, const uint16_t product)
{
    uint16_t ii=0;
    for(ii=0; ii<(devices.cnt); ii++)
    {
        if((product==devices.list[ii].product) && (vendor==devices.list[ii].vendor))
        {
            do {
                devices.list[ii].product = devices.list[ii+1].product;
                devices.list[ii].vendor  = devices.list[ii+1].vendor;
                ii++;
            } while(ii<(devices.cnt-1));
            devices.list[ii].product = 0x00;
            devices.list[ii].vendor  = 0x00;
            devices.cnt--;
        }
    }
    return save(devices);
}

QString Config::GetPath()
{
    QString curPath=QDir::currentPath();//获取系统当前目录
    /*QString*/ FilePath = curPath + "/Config.bin";
    qDebug() << "FilePath:" << FilePath;
    QFile aFile(FilePath);
    if (!aFile.exists())
    {
        qDebug() << "File not exists";
        return nullptr;
    }
    else
    {
        qDebug() << "File exists";
        return FilePath;
    }
}

int Config::save(union Config_Datas &List)
{
    uint16_t ii=0;
    QByteArray cfg;
    cfg.resize(sizeof(List));
    for(ii=0; ii<(cfg.size()); ii++)
    {
        for(ii=0; ii<(cfg.size()); ii++)
        {
            cfg[ii] = List.bin[ii]&0xFF;
        }
    }
    qDebug("[%s--%d] cfg.length: %d cfg.size:%d cnt:%d\r\n", __func__, __LINE__, cfg.length(), cfg.size(), devices.cnt);
    QFile File(FilePath);
    qDebug() << "FilePath:" << FilePath;
    if (File.open(QIODevice::ReadWrite/* | QIODevice::NewOnly*/))
    {
        File.write(cfg);
        File.close();
        return 0;
    }
    return -2;
}

