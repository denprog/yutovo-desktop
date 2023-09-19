#include "result_settings_form.h"
#include "ui_result_settings_form.h"

//ResultSettingsForm

ResultSettingsForm::ResultSettingsForm(yutovo::Config& _config, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultSettingsForm),
    config(_config)
{
    ui->setupUi(this);

    ui->result_auto_advance->setChecked(config.auto_result.result_auto_advance);

    FillResultsOrder();

    ui->real_precision->setValue(config.real_result.precision);
    ui->real_exp->setValue(config.real_result.exp);
    auto r = (int)config.real_result.result_angle_measure;
    ui->real_result_angle_measure->setCurrentIndex((int)config.real_result.result_angle_measure);
    ui->real_show_angle_measure->setChecked(config.real_result.show_angle_measure);

    ui->integer_default_notation->setCurrentIndex((int)config.integer_result.default_notation);
    ui->integer_result_notation->setCurrentIndex((int)config.integer_result.result_notation);
    ui->integer_show_notation->setChecked(config.integer_result.show_notation);

    ui->rational_fraction_form->setCurrentIndex((int)config.rational_result.fraction_form);

    ui->complex_precision->setValue((int)config.complex_result.precision);
    ui->complex_exp->setValue(config.complex_result.exp);
    ui->complex_form->setCurrentIndex((int)config.complex_result.form);
    ui->complex_results_count->setValue(config.complex_result.max_count);
    ui->complex_result_angle_measure->setCurrentIndex((int)config.complex_result.result_angle_measure);
    ui->complex_show_angle_measure->setChecked(config.complex_result.show_angle_measure);

    connect(ui->up_result_order, &QAbstractButton::clicked, this, &ResultSettingsForm::OnUpResultOrderClicked);
    connect(ui->down_result_order, &QAbstractButton::clicked, this, &ResultSettingsForm::OnDownResultOrderClicked);
}

ResultSettingsForm::~ResultSettingsForm()
{
    config.auto_result.result_auto_advance = ui->result_auto_advance->isChecked();

    config.real_result.precision = ui->real_precision->value();
    config.real_result.exp = ui->real_exp->value();
    config.real_result.result_angle_measure = (AngleMeasure)ui->real_result_angle_measure->currentIndex();
    config.real_result.show_angle_measure = ui->real_show_angle_measure->isChecked();

    config.integer_result.default_notation = (Notation)ui->integer_default_notation->currentIndex();
    config.integer_result.result_notation = (Notation)ui->integer_result_notation->currentIndex();
    config.integer_result.show_notation = ui->integer_show_notation->isChecked();

    config.rational_result.fraction_form = (ui->rational_fraction_form->currentIndex() == 0 ? FractionForm::PROPER : FractionForm::IMPROPER);

    config.complex_result.precision = ui->complex_precision->value();
    config.complex_result.exp = ui->complex_exp->value();
    config.complex_result.form = (ComplexForm)ui->complex_form->currentIndex();
    config.complex_result.max_count = ui->complex_results_count->value();
    config.complex_result.result_angle_measure = (AngleMeasure)ui->complex_result_angle_measure->currentIndex();
    config.complex_result.show_angle_measure = ui->complex_show_angle_measure->isChecked();

    delete ui;
}

void ResultSettingsForm::OnUpResultOrderClicked()
{
    int p = ui->auto_result_order->currentRow();
    if (p <= 0)
        return;
    auto& results_order = config.auto_result.results_order;
    auto r = results_order[p - 1];
    results_order[p - 1] = results_order[p];
    results_order[p] = r;
    FillResultsOrder();
    ui->auto_result_order->setCurrentRow(p - 1);
}

void ResultSettingsForm::OnDownResultOrderClicked()
{
    int p = ui->auto_result_order->currentRow();
    if (p >= 3)
        return;
    auto& results_order = config.auto_result.results_order;
    auto r = results_order[p + 1];
    results_order[p + 1] = results_order[p];
    results_order[p] = r;
    FillResultsOrder();
    ui->auto_result_order->setCurrentRow(p + 1);
}

void ResultSettingsForm::FillResultsOrder()
{
    ui->auto_result_order->clear();

    for (size_t i = 0; i < 4; ++i)
    {
        switch (config.auto_result.results_order[i])
        {
        case ResultType::REAL:
            ui->auto_result_order->addItem(tr("Real"));
            break;
        case ResultType::INTEGER:
            ui->auto_result_order->addItem(tr("Integer"));
            break;
        case ResultType::RATIONAL:
            ui->auto_result_order->addItem(tr("Rational"));
            break;
        case ResultType::COMPLEX:
            ui->auto_result_order->addItem(tr("Complex"));
            break;
        default:
            break;
        }
    }
}
