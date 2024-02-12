/******************** (C) COPYRIGHT 2018 merafour ********************
* Author             : 冷月追风@merafour.blog.163.com
* Version            : V1.0.0
* Date               : 01/01/2024
* Description        : OpenAgreementHID.
* Description        : 配置页面
********************************************************************************
* merafour.blog.163.com
* merafour@163.com
* github.com/Merafour
*******************************************************************************/
#include "formcfg.h"
#include "ui_formcfg.h"

FormCFG::FormCFG(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormCFG)
{
    ui->setupUi(this);
}

FormCFG::~FormCFG()
{
    delete ui;
}
