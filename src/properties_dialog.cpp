#include "properties_dialog.h"
#include "result_settings_form.h"
#include "ui_properties_dialog.h"

//PropertiesDialog

PropertiesDialog::PropertiesDialog(yutovo::Config& _config) :
    form(new Ui::PropertiesDialog()),
    config(_config)
{
    form->setupUi(this);

    form->results_layout->addWidget(new ResultSettingsForm(config));

    form->language->setCurrentIndex((int)config.language - 1);
    
    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
}

PropertiesDialog::~PropertiesDialog()
{
    config.language = (yutovo_calculator::Language)(form->language->currentIndex() + 1);
}
