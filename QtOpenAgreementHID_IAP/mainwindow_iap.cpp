/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : IAP 核心部分
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::reboot(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    int res=0;
    int len=-1;
    uint16_t Size;
    uint16_t ii;
    uint8_t SIGN[16];
    uint8_t sign_datas[16];
    enum OHID_UNLOCK unlock;
    switch(device_status_cnt)
    {
        case 0:
            HID.clear();
            OHID_Signature(board.SN, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, SIGN);
            OHIDM_IAP_Sign(Pack, OHID_UNLOCK_SIGN, SIGN, board.SN);
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
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_SIGN==unlock))
            {
                HID.clear();
                OHID_Signature(sign_datas, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_REBOOT, SIGN);
                OHIDM_IAP_Sign(Pack, OHID_UNLOCK_REBOOT, SIGN, sign_datas);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 2:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_REBOOT==unlock))
            {
                HID.clear();
                OHIDM_IAP_Reboot(Pack, Rand);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 3:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                qDebug("[%s--%d] Fail!\n", __func__, __LINE__);
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_REBOOT==Pack->ohid.cmd))
            {
                device_status = DEVICE_WATTING;
                reboot_delay = 0;
                timeout_update(delay);
                device_status_cnt=0;
                break;
            }
            device_status_cnt=0;
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}
void MainWindow::erase(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    int res=0;
    int len=-1;
    uint16_t Size;
    uint16_t ii;
    uint8_t SIGN[16];
    uint8_t sign_datas[16];
    enum OHID_UNLOCK unlock;
    switch(device_status_cnt)
    {
        case 0:
            HID.clear();
            OHID_Signature(board.SN, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, SIGN);
            OHIDM_IAP_Sign(Pack, OHID_UNLOCK_SIGN, SIGN, board.SN);
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
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_SIGN==unlock))
            {
                HID.clear();
                //qDebug("[%s--%d] sign_datas: 0x%02X 0x%02X 0x%02X 0x%02X\r\n", __func__, __LINE__, sign_datas[0], sign_datas[1], sign_datas[2], sign_datas[3]);
                OHID_Signature(sign_datas, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_ERASE, SIGN);
                OHIDM_IAP_Sign(Pack, OHID_UNLOCK_ERASE, SIGN, sign_datas);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 2:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_ERASE==unlock))
            {
                HID.clear();
                OHIDM_IAP_Erase(Pack, 0, board.fwSize);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 3:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            //ui->textBrowser->append("Erase ...");
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                //device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_ERASE==Pack->ohid.cmd))
            {
                qDebug("[%s--%d] Erase Down!", __func__, __LINE__);
                ui->textBrowser->append("Erase Down!");
                device_status = DEVICE_WRITE;
                if(reboot_flag)
                {
                    reboot_flag = 0;
                    device_status = DEVICE_REBOOT;
                    ui->textBrowser->append("Reboot");
                }
                timeout_update(delay);
                device_status_cnt=0;
                break;
            }
            device_status = DEVICE_STOP;
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}
void MainWindow::write(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    int res=0;
    int len=-1;
    uint16_t Size;
    uint16_t ii;
    uint8_t SIGN[16];
    uint8_t sign_datas[16];
    enum OHID_UNLOCK unlock;
    //const uint16_t wSize = (OHID_DATA_SIZE-4-2)&0xFC;
    const uint16_t wSize = (64-OHID_HEAD_SIZE-4-2)&0xFC;
    uint8_t binary[256];
    switch(device_status_cnt)
    {
        case 0:
            HID.clear();
            OHID_Signature(board.SN, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, SIGN);
            OHIDM_IAP_Sign(Pack, OHID_UNLOCK_SIGN, SIGN, board.SN);
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
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_SIGN==unlock))
            {
                HID.clear();
                //qDebug("[%s--%d] sign_datas: 0x%02X 0x%02X 0x%02X 0x%02X\r\n", __func__, __LINE__, sign_datas[0], sign_datas[1], sign_datas[2], sign_datas[3]);
                OHID_Signature(sign_datas, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_PROGRAM, SIGN);
                OHIDM_IAP_Sign(Pack, OHID_UNLOCK_PROGRAM, SIGN, sign_datas);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 2:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_PROGRAM==unlock))
            {
                qDebug("[%s--%d] Program Start!", __func__, __LINE__);
                HID.clear();
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 3:
            FW.read(wAddress, sizeof(binary), binary, wSize);
            OHIDM_IAP_Program(Pack, 1, wAddress, wSize, binary);
            res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
            device_status_cnt++;
            //qDebug("[%s--%d] Program ...", __func__, __LINE__);
            if (res < 0)
            {
                qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                break;
            }
            break;
        case 4:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            //qDebug("[%s--%d] Program ... Size:%3d len:%3d", __func__, __LINE__, Size, len);
            //ui->textBrowser->append("Erase ...");
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                //device_status_cnt=0;
                device_status = DEVICE_STOP;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_PROGRAM==Pack->ohid.cmd))
            {
                //qDebug("[%s--%d] Program wAddress:0x%08X wSize:%3d FW.size:0x%08X", __func__, __LINE__, wAddress, wSize, FW.size());
                wAddress += wSize;
                Progress(wAddress*100/FW.size(), "Program...");
                //ui->progressBar->setValue(wAddress*100/FW.size());
                if(FW.size()<=wAddress)
                {
                    qDebug("[%s--%d] Program Down!", __func__, __LINE__);
                    ui->textBrowser->append("Program Down!");
                    device_status = DEVICE_JUMP;
                    timeout_update(delay);
                    device_status_cnt=0;
                }
                else
                {
                    FW.read(wAddress, sizeof(binary), binary, wSize);
                    OHIDM_IAP_Program(Pack, 1, wAddress, wSize, binary);
                    res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                    if (res < 0)
                    {
                        qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                        break;
                    }
                    timeout_update(delay);
                }
                break;
            }
            device_status = DEVICE_STOP;
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}
void MainWindow::read(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    ;
}
void MainWindow::get_crc(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    int res=0;
    int len=-1;
    uint16_t Size;
    uint16_t ii;
    uint32_t crc1, crc2;
    switch(device_status_cnt)
    {
        case 0:
            HID.clear();
            OHIDM_IAP_RCRC(Pack, 0, FW.size());
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
            //ui->textBrowser->append("Erase ...");
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                //device_status_cnt=0;
                break;
            }
            crc1 = OHID_get_32b(Pack, 4+4);
            crc2 = FW.fw_crc16();
//            qDebug("[%s--%d] CRC:0x%04X 0x%04X", __func__, __LINE__, crc1, crc2);
//            crc2 = FW.fw_crc16();
//            qDebug("[%s--%d] CRC:0x%04X 0x%04X", __func__, __LINE__, crc1, crc2);
//            crc2 = FW.fw_crc16();
//            qDebug("[%s--%d] CRC:0x%04X 0x%04X", __func__, __LINE__, crc1, crc2);
//            crc2 = FW.fw_crc16();
            if((len >= 4) && (OHID_CMD_IAP_RCRC==Pack->ohid.cmd))
            {
                qDebug("[%s--%d] CRC:0x%04X 0x%04X", __func__, __LINE__, crc1, crc2);
                //ui->textBrowser->append("Get CRC");
                ui->textBrowser->append(QString().asprintf("CRC:0x%04X 0x%04X(device)", crc1, crc2));
                if(crc1!=crc2)
                {
                    device_status = DEVICE_ERASE;
                    if(OHID_RUN_MODE_BOOT!=board.Mode)
                    {
                        reboot_flag=1;
                        reboot_delay=0;
                    }
                }
                else
                {
                    device_status = DEVICE_STOP;
                    ui->textBrowser->append("Don't have Update!");
                }
                timeout_update(delay);
                device_status_cnt=0;
                break;
            }
            device_status = DEVICE_STOP;
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}
void MainWindow::jump(union OpenAgreementHID_t* const Pack, struct OHID_boardInfo &board, const uint32_t delay)
{
    int res=0;
    int len=-1;
    uint16_t Size;
    uint16_t ii;
    uint8_t SIGN[16];
    uint8_t sign_datas[16];
    enum OHID_UNLOCK unlock;
    uint32_t crc1, crc2;
    switch(device_status_cnt)
    {
        case 0:
            qDebug("[%s--%d]", __func__, __LINE__);
            HID.clear();
            OHIDM_IAP_RCRC(Pack, 0, FW.size());
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
            //ui->textBrowser->append("Erase ...");
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                //device_status_cnt=0;
                break;
            }
            crc1 = OHID_get_32b(Pack, 4+4);
            crc2 = FW.fw_crc16();
            if((len >= 4) && (OHID_CMD_IAP_RCRC==Pack->ohid.cmd))
            {
                qDebug("[%s--%d] CRC:0x%04X 0x%04X", __func__, __LINE__, crc1, crc2);
                ui->textBrowser->append(QString().asprintf("CRC:0x%04X 0x%04X(device)", crc1, crc2));
                if(crc1!=crc2)
                {
                    device_status = DEVICE_STOP;
                    ui->textBrowser->append("Update Fail!");
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status = DEVICE_STOP;
            break;
        case 2:
            HID.clear();
            OHID_Signature(board.SN, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_SIGN, SIGN);
            OHIDM_IAP_Sign(Pack, OHID_UNLOCK_SIGN, SIGN, board.SN);
            res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
            if (res < 0)
            {
                qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                break;
            }
            device_status_cnt++;
            break;
        case 3:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_SIGN==unlock))
            {
                HID.clear();
                //qDebug("[%s--%d] sign_datas: 0x%02X 0x%02X 0x%02X 0x%02X\r\n", __func__, __LINE__, sign_datas[0], sign_datas[1], sign_datas[2], sign_datas[3]);
                OHID_Signature(sign_datas, OHID_signature_key_main, OHID_signature_params1, OHID_UNLOCK_JUMP, SIGN);
                OHIDM_IAP_Sign(Pack, OHID_UNLOCK_JUMP, SIGN, sign_datas);
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 4:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_SIGN==Pack->ohid.cmd) && (OHID_UNLOCK_JUMP==unlock))
            {
                HID.clear();
                OHIDM_IAP_Jump(Pack, 0, FW.size(), FW.fw_crc16());
                res = HID.Writes(Pack->bin, OHID_ppack_size(Pack));
                if (res < 0)
                {
                    qDebug("[%s--%d] Unable to write() (2)\n", __func__, __LINE__);
                    break;
                }
                timeout_update(delay);
                device_status_cnt++;
                break;
            }
            device_status_cnt=0;
            break;
        case 5:
            for(ii=0; ii<sizeof(Pack->bin); ii++) Pack->bin[ii] = 0;
            Size = HID.Reads(Pack->bin, 64);
            if(Size>4) len = _OHIDS_Decode(Pack, Size);
            //ui->textBrowser->append("Erase ...");
            //qDebug("[%s--%d] res:%d BIN: 0x%02X 0x%02X 0x%02X 0x%02X | 0x%02X 0x%02X CRC: 0x%02X\r\n", __func__, __LINE__, res, Pack->bin[0], Pack->bin[1], Pack->bin[2], Pack->bin[3], Pack->bin[4], Pack->bin[5], OHID_Checksum(Pack));
            if(len<(OHID_HEAD_SIZE))
            {
                //device_status_cnt=0;
                break;
            }
            for(ii=0; ii<sizeof(sign_datas); ii++)      sign_datas[ii]      = 0x00;
            OHID_get_bin(Pack, 1, sign_datas, sizeof(sign_datas));
            unlock = (enum OHID_UNLOCK) Pack->ohid.data[0];
            if((len >= 4) && (OHID_CMD_IAP_JUMP==Pack->ohid.cmd))
            {
                qDebug("[%s--%d] Download Down!", __func__, __LINE__);
                ui->textBrowser->append("Download Down!");
                device_status = DEVICE_STOP;
                timeout_update(delay);
                device_status_cnt=0;
                break;
            }
            ui->textBrowser->append(QString().asprintf("Download Fail[0x%02X]!", Pack->ohid.data[0]));
            device_status = DEVICE_STOP;
            break;
        default:
            device_status = DEVICE_STOP;
            break;
    }
}

