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

    form->icon->setPixmap(QIcon(":/icons/images/mainicon.png").pixmap(64, 64));
    form->text->setText(tr("about_text"));

    connect(form->ok, SIGNAL(clicked()), this, SLOT(OnOkClicked()));
}

void AboutDialog::OnOkClicked()
{
    close();
}
