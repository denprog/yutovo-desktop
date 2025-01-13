#include "colors_settings_form.h"
#include "ui_colors_settings_form.h"
#include <QColorDialog>
#include <yutovo_editor/editor_utils.h>

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

    c = QColor::fromRgb(config.numbers_color.ToInt());
    form->numbers_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->numbers_color, SIGNAL(clicked()), this, SLOT(OnNumbersColorClicked()));

    c = QColor::fromRgb(config.functions_color.ToInt());
    form->functions_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->functions_color, SIGNAL(clicked()), this, SLOT(OnFunctionsColorClicked()));

    c = QColor::fromRgb(config.variables_color.ToInt());
    form->variables_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->variables_color, SIGNAL(clicked()), this, SLOT(OnVariablesColorClicked()));

    c = QColor::fromRgb(config.units_color.ToInt());
    form->units_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->units_color, SIGNAL(clicked()), this, SLOT(OnUnitsColorClicked()));

    c = QColor::fromRgb(config.shapes_color.ToInt());
    form->shapes_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->shapes_color, SIGNAL(clicked()), this, SLOT(OnShapesColorClicked()));

    c = QColor::fromRgb(config.error_marks_color.ToInt());
    form->error_marks_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->error_marks_color, SIGNAL(clicked()), this, SLOT(OnErrorsColorClicked()));

    c = QColor::fromRgb(config.formula_bg_color.ToInt());
    form->formula_bg_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->formula_bg_color, SIGNAL(clicked()), this, SLOT(OnFormulaBgColorClicked()));

    c = QColor::fromRgb(config.bg_selection_color.ToInt());
    form->bg_selection_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    connect(form->bg_selection_color, SIGNAL(clicked()), this, SLOT(OnSelectionBgColorClicked()));
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

void ColorsSettingsForm::OnNumbersColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.numbers_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.numbers_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->numbers_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnFunctionsColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.functions_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.functions_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->functions_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnVariablesColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.variables_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.variables_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->variables_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnUnitsColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.units_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.units_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->units_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnShapesColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.shapes_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.shapes_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->shapes_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnErrorsColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.error_marks_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.error_marks_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->error_marks_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnFormulaBgColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.formula_bg_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.formula_bg_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->formula_bg_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}

void ColorsSettingsForm::OnSelectionBgColorClicked()
{
    QColorDialog d(QColor::fromRgba(config.bg_selection_color.ToInt()), this);
    if (d.exec() == QDialog::Accepted)
    {
        QColor c = d.selectedColor();
        config.bg_selection_color = yutovo::Color{(uint8_t)c.alpha(), (uint8_t)c.red(), (uint8_t)c.green(), (uint8_t)c.blue()};
        form->bg_selection_color->setStyleSheet(QString("background-color: %1").arg(c.name()));
    }
}
