#include "system_settings_form.h"
#include "ui_system_settings_form.h"

//SystemSettingsForm

SystemSettingsForm::SystemSettingsForm(yutovo::Config& _config, QWidget* parent) :
    QWidget(parent),
    form(new Ui::SystemSettingsForm()),
    config(_config)
{
    form->setupUi(this);

    if (config.language == "en")
        form->language->setCurrentIndex(0);
    else if (config.language == "ru")
        form->language->setCurrentIndex(1);
    
    form->log_level->setCurrentIndex((int)config.log_level);
}

SystemSettingsForm::~SystemSettingsForm()
{
    switch (form->language->currentIndex())
    {
    case 0:
        config.language = "en";
        break;
    case 1:
        config.language = "ru";
        break;
    }

    config.log_level = (yutovo::LogLevel)form->log_level->currentIndex();
    
    delete form;
}
