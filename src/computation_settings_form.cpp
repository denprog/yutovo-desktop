#include "computation_settings_form.h"
#include "ui_computation_settings_form.h"

//ComputationSettingsForm

ComputationSettingsForm::ComputationSettingsForm(yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    form(new Ui::ComputationSettingsForm()),
    config(_config)
{
    form->setupUi(this);

    form->solve_delay->setValue(config.solve_delay / 1000);
}

ComputationSettingsForm::~ComputationSettingsForm()
{
    config.solve_delay = form->solve_delay->value() * 1000;
    
    delete form;
}
