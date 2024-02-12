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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    const Config::usb_vid_pid* device;
    uint16_t ii;
    Rand = 0;
    device_status = DEVICE_NONE;
    ui->setupUi(this);
    cfg.Init();
    cfg.write(0x0483, 0x5750); // ST
    cfg.Init();
    ui->comboBox_hid->clear();
    for(ii=0; ii<cfg.size(); ii++)
    {
        device = cfg.read(ii);
        ui->comboBox_hid->addItem(QString().asprintf("[0x%04X:0x%04X]", device->vendor, device->product));
    }
    adjustSize();
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    if(cfg.size()>0)
    {
        device = cfg.read(0);
        setWindowTitle(QString().asprintf("OHID-IAP Device[%04X:%04X]", device->vendor, device->product));
    }
    ui->progressBar->setOrientation(Qt::Horizontal);
    ui->progressBar->setTextVisible(true);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(100);
    ui->progressBar->setValue(0000);
    ui->progressBar->setFormat(QString::fromLocal8Bit("Watting..."));
    ui->progressBar->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    Progress(0, "Watting...");

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::timeout));
    timer->start(5);
}

MainWindow::~MainWindow()
{
    delete ui;
}

uint32_t MainWindow::get_ms(void)
{
    long long currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    return currentTime;
}

void MainWindow::Progress(int progress, QString des)
{
    QProgressBar *progressBar = ui->progressBar;
    if((progress>0) && (progress<100))
    {
        progressBar->setValue(progress);
        double dProgress = (progressBar->value() - progressBar->minimum()) * 100.0
                        / (progressBar->maximum() - progressBar->minimum());
        progressBar->setFormat(des+QString::fromLocal8Bit(": %1%").arg(QString::number(dProgress, 'f', 1)));
        ui->textBrowser->append(des+QString::fromLocal8Bit(": %1%").arg(QString::number(dProgress, 'f', 1)));
    }
    else
    {
        progressBar->setValue(100);
        progressBar->setFormat(des);
        ui->pushButton_Program->setEnabled(true);
        ui->pushButton_Program->setText("Program");
        ui->textBrowser->append(des);
    }
}

void MainWindow::Print(QString msg)
{
    ui->textBrowser->append(msg);
}

void MainWindow::on_pushButton_Program_clicked()
{
    if((DEVICE_NONE==device_status) || (DEVICE_IDEL==device_status))
    {
        Print("Program...");
        if(/*(FW.size()<1024) || */(false == ui->checkBox_auto->isChecked()))
        {
            qDebug("[%s--%d] ...\r\n", __func__, __LINE__);
            QString curPath = QString(cfg.readPath());
            qDebug() << "curPath:" << curPath;
            if(curPath.isEmpty()) curPath=QDir::currentPath();
            QString filter="HEX(*.HEX *.hex);;Binary(*.BIN *.bin)";
            QString fwPath=QFileDialog::getOpenFileName(this, "Open", curPath, filter);
            if (fwPath.isEmpty())
            {
                Print("操作无效");
                return ;
            }
            ui->EditFile->setText(/*"File: "+*/fwPath);
            if(0==FW.Load(fwPath))
            {
                cfg.writePath(fwPath.toLocal8Bit().data());
                ui->pushButton_Program->setEnabled(false);
                ui->pushButton_Program->setText("Stop");
                device_status = DEVICE_SCAN;
            }
        }
        else //if(ui->checkBox_auto->isChecked())
        {
            Print("Auto Reload...");
            qDebug("[%s--%d] ...\r\n", __func__, __LINE__);
            QString curPath = QString(cfg.readPath());
            QString fwPath;
            qDebug() << "curPath:" << curPath;
            fwPath = curPath;
            if(curPath.isEmpty())
            {
                curPath=QDir::currentPath();
                QString filter="HEX(*.HEX *.hex);;Binary(*.BIN *.bin)";
                fwPath=QFileDialog::getOpenFileName(this, "Open", curPath, filter);
                if (fwPath.isEmpty())
                {
                    Print("操作无效");
                    return ;
                }
            }
            ui->EditFile->setText(/*"File: "+*/fwPath);
            if(0==FW.Load(fwPath))
            {
                cfg.writePath(fwPath.toLocal8Bit().data());
                ui->pushButton_Program->setEnabled(false);
                ui->pushButton_Program->setText("Stop");
                device_status = DEVICE_SCAN;
            }
        }
//        else
//        {
//            Print("Continue...");
//            ui->pushButton_Program->setEnabled(false);
//            ui->pushButton_Program->setText("Stop");
//            device_status = DEVICE_SCAN;
//        }
    }
    else
    {
        device_status = DEVICE_STOP;
    }
}

