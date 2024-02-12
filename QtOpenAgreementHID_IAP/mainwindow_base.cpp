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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDir>
#include <QFileDialog>
#include <QDateTime>

void MainWindow::scanning(union OpenAgreementHID_t* const Pack, const uint16_t vid, const uint16_t pid, InterfaceList &list, InterfaceList &FoundList, const uint8_t SN[16], const uint32_t delay)
{
    static char path[256+4];
    static uint8_t cnt = 0;
    int res=0;
    int len=-1;
    int Size;
    uint16_t ii;
    uint8_t rSN[20];
    switch(device_status_cnt)
    {
        case 0:
            list.init();
            HID.Init(list, vid, pid);
            timeout_update(delay);
            qDebug("[%s--%d] list Max:%d\r\n", __func__, __LINE__, list.size());
            for(ii=0; ii<list.size(); ii++)
            {
                HID.close();
                memset(path, 0x00, sizeof(path));
                list.pull(ii, path, sizeof(path));
                qDebug("   path[%d]: [%s]", ii, path);
            }
            cnt = 0;
            if(list.size()>0)
            {
                device_status_cnt++;
                HID.close();
                qDebug("have Device!!!");
            }
            else
            {
                HID.close();
                device_status = DEVICE_STOP;
                Print(QString().asprintf("[%s--%d] Don't have Device!!!", __func__, __LINE__));
            }
            break;
        case 1:
            HID.close();
            memset(path, 0x00, sizeof(path));
            list.pull(cnt, path, sizeof(path));
            qDebug("   path[%d]: [%s]", cnt, path);
            cnt++;
            if(0==HID.Open(path))
            {
                HID.clear();
                timeout_update(delay);
                OHIDM_None(Pack, OHID_CMD_BASE_SYNC);
                qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X 0x%02X 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], Pack->bin[6], Pack->bin[7]);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                device_status_cnt++;
            }
            break;
        case 2:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            else break;
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X 0x%02X 0x%02X End: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], Pack->bin[6], Pack->bin[7], Pack->ohid.data[Pack->ohid.length-1]);
            qDebug("[%s--%d] CRC: 0x%02X [%d]\r\n", __func__, __LINE__, OHID_Checksum(Pack), len);
            if(len<(OHID_HEAD_SIZE+20))
            {
                if(cnt>=list.size())
                {
                    device_status = DEVICE_STOP;
                    HID.close();
                    break;
                }
                device_status_cnt--;
                break;
            }
            memset(rSN, 0x00, sizeof(rSN));
            OHID_get_bin(Pack, 4+2+1, rSN, sizeof(rSN));
            /*printf("[%s--%d] Pack", __func__, __LINE__);
            for(int ii=0; ii<OHID_ppack_size(Pack); ii++) printf("0x%02X ", Pack->bin[ii]);
            printf("\r\n");*/
            qDebug("[%s--%d] SN      :%s ", __func__, __LINE__, SN);
            qDebug("[%s--%d] rSN     :%s ", __func__, __LINE__, rSN);
            timeout_update(delay);
            if(NULL == SN)
            {
                if((len >= 4) && (OHID_CMD_BASE_SYNC==Pack->ohid.cmd) )
                {
                    qDebug("[%s--%d] HID Device%d:%s\r\n", __func__, __LINE__, cnt-1, path);
                    FoundList.push(path);
                }
            }
            else
            {
                if((len >= 4) && (OHID_CMD_BASE_SYNC==Pack->ohid.cmd) && (0==memcmp(SN, rSN, 16)))
                {
                    qDebug("[%s--%d] HID Device%d:%s\r\n", __func__, __LINE__, cnt-1, path);
                    FoundList.push(path);
                }
            }
            device_status_cnt = 0;
            device_status = DEVICE_OPEN;
            HID.close();
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}

void MainWindow::sync(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    int res=0;
    int len=-1;
    uint16_t Size;
    uint16_t ii;
    switch(device_status_cnt)
    {
        case 0:
            HID.clear();
            OHIDM_None(Pack, OHID_CMD_BASE_SYNC);
            res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
            if (res < 0)
            {
                qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                break;
            }
            device_status_cnt++;
            break;
        case 1:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X 0x%02X 0x%02X End: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], Pack->bin[6], Pack->bin[7], Pack->ohid.data[Pack->ohid.length-1]);
            qDebug("[%s--%d] CRC: 0x%02X \r\n", __func__, __LINE__, OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE+20))
            {
                device_status_cnt--;
                break;
            }
            for(ii=0; ii<sizeof(board.SN); ii++)      board.SN[ii]      = 0x00;
            for(ii=0; ii<sizeof(board.Version); ii++) board.Version[ii] = 0x00;
            board.board_id = (enum OHID_BOARDS_ID_LIST) OHID_get_32b(Pack, 0);
            board.fwSize   = OHID_get_16b(Pack, 4)<<8;
            board.Mode     = (enum OHID_RUN_MODE) OHID_get_8b(Pack, 4+2);
            OHID_get_bin(Pack, 4+2+1, board.SN, sizeof(board.SN));
            OHID_get_bin(Pack, 4+2+1+17, board.Version, sizeof(board.Version));
            qDebug("[%s--%d] SN      :%s ", __func__, __LINE__, board.SN);
            qDebug("[%s--%d] Version :%s ", __func__, __LINE__, board.Version);
            qDebug("[%s--%d] board_id:0x%04X ", __func__, __LINE__, board.board_id);
            qDebug("[%s--%d] fwSize  :%8d (%3dKB)", __func__, __LINE__, board.fwSize, board.fwSize/1024);
            qDebug("[%s--%d] Mode    :0x%02X (%s)", __func__, __LINE__, board.Mode, (OHID_RUN_MODE_BOOT==board.Mode)?"BOOT Mode":"APP Mode");
            if((len >= 4) && (OHID_CMD_BASE_SYNC==Pack->ohid.cmd) )
            {
                /*if(OHID_RUN_MODE_BOOT==board.Mode) device_status = DEVICE_GCRC;
                else
                {
                    device_status = DEVICE_ERASE;//DEVICE_REBOOT;
                    reboot_flag=1;
                }*/
                device_status = DEVICE_GCRC;
            }
            timeout_update(delay);
            device_status_cnt = 0;
            timeout_update(delay);
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}

void MainWindow::timeout()
{
    const Config::usb_vid_pid* device = NULL;
    if(cfg.size()>0)
    {
        device = cfg.read(ui->comboBox_hid->currentIndex());
        setWindowTitle(QString().asprintf("OHID-IAP Device[%04X:%04X]", device->vendor, device->product));
    }
    char path[256+4];
    static int Index = 0;
    union OpenAgreementHID_t Pack;
    uint32_t ii;
    Rand++;
    switch(device_status)
    {
    case DEVICE_NONE:
        device_status_cnt=0;
        reboot_flag = 0;
        List.init();
        FoundList.init();
        break;
    case DEVICE_IDEL:
        ui->pushButton_Program->setEnabled(true);
        ui->pushButton_Program->setText("Program");
        reboot_flag = 0;
        device_status_cnt=0;
        List.init();
        FoundList.init();
        wAddress = 0;
        break;
    case DEVICE_STOP:  // 高级功能测试
        device_status = DEVICE_IDEL;
        List.init();
        FoundList.init();
        reboot_flag = 0;
        device_status_cnt=0;
        break;
    case DEVICE_OPEN:
        if(Index<List.size())
        {
            for(ii=0; ii<sizeof(path); ii++) path[ii] = 0x00;
            FoundList.pull(Index, path, sizeof(path));
            if(0==HID.Open(path))
            {
                device_status = DEVICE_SYNC;
                ui->textBrowser->append("Open Succeed!");
                device_status_cnt = 0;
                timeout_update(3000);
            }
        }
        else
        {
            qDebug("[%s--%d] Don't Have Device!", __func__, __LINE__);
            HID.close();
            device_status = DEVICE_STOP;
        }
        break;
    case DEVICE_SYNC:
        sync(&Pack, board, 3000);
        break;
    case DEVICE_SCAN:
        if(NULL==device)
        {
            device_status = DEVICE_IDEL;
            break;
        }
        scanning(&Pack, device->vendor, device->product, List, FoundList, NULL, 3000);
        Index = 0;
//        read_delay=0;
//        idxKey = -1;
        break;
    case DEVICE_REBOOT:
        reboot(&Pack, board, 13000); // 13s
        break;
    case DEVICE_ERASE:
        erase(&Pack, board, 10000);  // 10s
        break;
    case DEVICE_WRITE:
        write(&Pack, board, 300);  // 300ms
        break;
    case DEVICE_READ:
        read(&Pack, board, 3000);
        break;
    case DEVICE_GCRC:
        get_crc(&Pack, board, 3000);
        break;
    case DEVICE_JUMP:
        jump(&Pack, board, 3000);
        break;
    case DEVICE_WATTING:
        if(reboot_delay++<((1000/5)*2)) break;
        qDebug("[%s--%d] Watting...%3d %3d", __func__, __LINE__, reboot_delay, ((1000/5)*5));
        List.init();
        FoundList.init();
        HID.close();
        HID.Init(List, device->vendor, device->product);
        if(List.size()>0)
        {
            if(reboot_delay> ((1000/5)*2+50) ) device_status = DEVICE_SCAN;
            timeout_update(1000);
            List.init();
            FoundList.init();
        }
        else reboot_delay = 0;
        break;
    default:
        ui->textBrowser->append("操作错误");
        device_status = DEVICE_IDEL;
        reboot_flag = 0;
        break;
    }
    is_timeout();
}
