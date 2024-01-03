#include "documents_settings_form.h"
#include "ui_documents_settings_form.h"

//DocumentsSettingsForm

DocumentsSettingsForm::DocumentsSettingsForm(QHash<QString, QVariant>& _settings, QWidget *parent) :
    QWidget(parent),
    form(new Ui::DocumentsSettingsForm),
    settings(_settings)
{
    form->setupUi(this);

    form->load_last_documents->setChecked(settings.value("Documents/load_last_documents", false).toBool());
}

DocumentsSettingsForm::~DocumentsSettingsForm()
{
    settings["Documents/load_last_documents"] = form->load_last_documents->isChecked();

    delete form;
}
