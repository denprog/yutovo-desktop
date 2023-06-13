#include "set_uint_dialog.h"
#include "ui_set_uint_dialog.h"

//SetUintDialog

SetUintDialog::SetUintDialog(uint _value, QString caption, QString label, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetUintDialog)
{
    ui->setupUi(this);

    setWindowTitle(caption);
    ui->label->setText(label);
    ui->precision->setValue(_value);
}

SetUintDialog::~SetUintDialog()
{
    delete ui;
}

void SetUintDialog::accept()
{
    value = ui->precision->value();
    QDialog::accept();
}
