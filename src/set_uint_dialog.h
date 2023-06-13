#ifndef SET_UINT_DIALOG_H
#define SET_UINT_DIALOG_H

#include <QDialog>

namespace Ui
{
class SetUintDialog;
}

class SetUintDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetUintDialog(uint _value, QString caption, QString label, QWidget *parent = nullptr);
    ~SetUintDialog();

    virtual void accept();

public:
    uint value = 0;

private:
    Ui::SetUintDialog *ui;
};

#endif
