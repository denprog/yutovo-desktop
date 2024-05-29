#include "about_dialog.h"
#include "ui_about.h"

//AboutDialog

AboutDialog::AboutDialog() :
    form(new Ui::AboutDialog())
{
    form->setupUi(this);

    setFixedSize(width(), height());
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowIcon(QIcon(":/icons/images/mainicon.png")); 

    connect(form->ok, SIGNAL(clicked()), this, SLOT(OnOkClicked()));
}

void AboutDialog::OnOkClicked()
{
    close();
}
