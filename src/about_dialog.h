#ifndef __ABOUT_DIALOG_H__
#define __ABOUT_DIALOG_H__

#include <QDialog>

namespace Ui
{
class AboutDialog;
};

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog();

private slots:
    void OnOkClicked();

private:
    Ui::AboutDialog* form = nullptr;
};

#endif
