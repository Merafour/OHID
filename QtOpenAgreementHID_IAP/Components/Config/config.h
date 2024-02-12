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
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <QString>

class Config
{
public:
    struct usb_vid_pid {
        uint16_t vendor;
        uint16_t product;
    };
    Config();
    void Init(void);

    const struct usb_vid_pid* read(const uint8_t index);
    const char* readPath(void);
    uint16_t size(void);
    int write(const uint16_t vendor, const uint16_t product);
    int writePath(const char fwPath[]);
    int remove(const uint16_t vendor, const uint16_t product);
private:

    QString GetPath(void);
    union Config_Datas {
        uint8_t bin[1024];
        struct {
            struct usb_vid_pid list[256];
            uint16_t cnt;
            char path[512];
        };
    };
    static union Config_Datas devices;
    QString FilePath;
    int save(union Config_Datas &List);
};

#endif // CONFIG_H
