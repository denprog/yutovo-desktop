#ifndef __LINK_DIALOG_H__
#define __LINK_DIALOG_H__

#include <QDialog>

namespace Ui
{
class LinkDialog;
}

class LinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinkDialog(QString _text, QString _url, QString caption, QWidget *parent = nullptr);
    ~LinkDialog();

    void OnTextChanged(const QString& _text);
    void OnUrlChanged(const QString& _text);

public:
    QString text;
    QString url;

private:
    Ui::LinkDialog *ui;
};

#endif