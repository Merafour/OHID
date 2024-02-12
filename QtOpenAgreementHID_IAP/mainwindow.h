/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 主页面
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Config/config.h"
#include "device_status.h"
#include "Firmware/Firmware.h"
#include "OHID/OHID_Pack.h"
#include "hid/InterfaceHID.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct OHID_boardInfo {
    uint8_t SN[32];
    uint8_t Version[32];
    uint32_t fwSize;
    uint16_t bSize;
    enum OHID_RUN_MODE  Mode;
    enum OHID_BOARDS_ID_LIST board_id;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static uint32_t get_ms(void);

private:
    void Progress(int progress, QString des);
    void Print(QString msg);
    int is_timeout(void)
    {
        const uint32_t sys_ms = get_ms();
        if(sys_ms>=delay_tick_ms)
        {
            if((DEVICE_IDEL!=device_status) && (DEVICE_STOP!=device_status)) device_status = DEVICE_STOP;
            return 1;
        }
        return 0;
    }
    void timeout_update(const uint32_t delay)
    {
        delay_tick_ms = get_ms() + delay;
    }
    void scanning(union OpenAgreementHID_t* const Pack, const uint16_t vid, const uint16_t pid, InterfaceList &list, InterfaceList &FoundList, const uint8_t SN[16], const uint32_t delay);
    void sync(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);
    void reboot(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);
    void erase(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);
    void write(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);
    void read(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);
    void get_crc(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);
    void jump(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay);

public slots:
    void timeout(void);

private slots:
    void on_pushButton_Program_clicked();

private:
    Ui::MainWindow *ui;
    Config cfg;
    QTimer *timer;
    enum DEVICE_STATUS device_status;
    uint16_t device_status_cnt;
    Firmware FW;
    InterfaceList List;
    InterfaceList FoundList;
    InterfaceHID HID;
    struct OHID_boardInfo board;
    uint32_t delay_tick_ms;
    uint32_t wAddress;
    uint8_t Rand;
    uint8_t reboot_flag;
    uint32_t reboot_delay;
};
#endif // MAINWINDOW_H
