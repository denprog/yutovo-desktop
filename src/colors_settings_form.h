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

private:
    Ui::ColorsSettingsForm *form;
    yutovo::Config& config;
};

#endif
