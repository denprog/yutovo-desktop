#include "properties_dialog.h"
#include "ui_properties_dialog.h"

//PropertiesDialog

PropertiesDialog::PropertiesDialog(yutovo::Config& _config) :
    form(new Ui::PropertiesDialog()),
    config(_config)
{
    form->setupUi(this);

    form->language->setCurrentIndex((int)config.language - 1);
    
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

PropertiesDialog::~PropertiesDialog()
{
    config.language = (yutovo_calculator::Language)(form->language->currentIndex() + 1);
}
