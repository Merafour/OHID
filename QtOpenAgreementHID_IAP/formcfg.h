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
#ifndef FORMCFG_H
#define FORMCFG_H

#include <QWidget>

namespace Ui {
class FormCFG;
}

class FormCFG : public QWidget
{
    Q_OBJECT

public:
    explicit FormCFG(QWidget *parent = nullptr);
    ~FormCFG();

private:
    Ui::FormCFG *ui;
};

#endif // FORMCFG_H
