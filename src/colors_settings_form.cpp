#include "colors_settings_form.h"
#include "ui_colors_settings_form.h"
#include <QColorDialog>
#include <yutovo_editor/util.h>

//ColorsSettingsForm

ColorsSettingsForm::ColorsSettingsForm(yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    form(new Ui::ColorsSettingsForm),
    config(_config)
{
    form->setupUi(this);

    QColor c = QColor::fromRgb(config.code_block_border_color.ToInt());
    form->code_block_border_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->code_block_border_color, SIGNAL(clicked()), this, SLOT(OnCodeBlockBorderColorClicked()));
}

ColorsSettingsForm::~ColorsSettingsForm()
{
    delete form;
}

void ColorsSettingsForm::OnCodeBlockBorderColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.code_block_border_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.code_block_border_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->code_block_border_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}
