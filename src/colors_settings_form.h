#ifndef __COLORS_SETTINGS_FORM_H__
#define __COLORS_SETTINGS_FORM_H__

#include <QWidget>
#include <QSettings>
#include <yutovo_editor/config.h>

namespace Ui
{
class ColorsSettingsForm;
}

class ColorsSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit ColorsSettingsForm(yutovo::Config& _config, QWidget *parent = nullptr);
    ~ColorsSettingsForm();

private slots:
    void OnCodeBlockBorderColorClicked();
    void OnNumbersColorClicked();
    void OnFunctionsColorClicked();
    void OnVariablesColorClicked();
    void OnUnitsColorClicked();
    void OnShapesColorClicked();
    void OnErrorsColorClicked();
    void OnFormulaBgColorClicked();
    void OnSelectionBgColorClicked();

private:
    Ui::ColorsSettingsForm *form;
    yutovo::Config& config;
};

#endif
