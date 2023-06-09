#ifndef RESULT_SETTINGS_FORM_H
#define RESULT_SETTINGS_FORM_H

#include <QWidget>
#include <yutovo_editor/config.h>

namespace Ui
{
class ResultSettingsForm;
}

using namespace yutovo;

class ResultSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ResultSettingsForm(yutovo::Config& _config, QWidget *parent = nullptr);
    ~ResultSettingsForm();

public slots:
    void OnUpResultOrderClicked();
    void OnDownResultOrderClicked();

private:
    void FillResultsOrder();

private:
    Ui::ResultSettingsForm *ui;
    yutovo::Config& config;
};

#endif
