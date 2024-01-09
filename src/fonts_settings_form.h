#ifndef FONTS_SETTINGS_FORM_H
#define FONTS_SETTINGS_FORM_H

#include <QWidget>
#include <QSettings>
#include <yutovo_editor/config.h>

namespace Ui
{
class FontsSettingsForm;
}

using namespace yutovo;

class FontsSettingsForm : public QWidget
{
    Q_OBJECT

public:
    explicit FontsSettingsForm(yutovo::Config& _config, QWidget *parent = nullptr);
    ~FontsSettingsForm();

private:
    Ui::FontsSettingsForm *form;
    yutovo::Config& config;
};

#endif
