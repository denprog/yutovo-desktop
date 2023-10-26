#ifndef __SYSTEM_SETTINGS_FORM_H__
#define __SYSTEM_SETTINGS_FORM_H__

#include <QWidget>
#include <yutovo_editor/config.h>

namespace Ui
{
class SystemSettingsForm;
};

class SystemSettingsForm : public QWidget
{
public:
    SystemSettingsForm(yutovo::Config& _config, QWidget* parent = nullptr);
    ~SystemSettingsForm();

private:
    Ui::SystemSettingsForm* form = nullptr;
    yutovo::Config& config;
};

#endif
