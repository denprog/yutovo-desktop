#include "system_settings_form.h"
#include "ui_system_settings_form.h"

//SystemSettingsForm

SystemSettingsForm::SystemSettingsForm() :
    form(new Ui::SystemSettingsForm())
{
    form->setupUi(this);
}

SystemSettingsForm::~SystemSettingsForm()
{
    delete form;
}
