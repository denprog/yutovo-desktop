#ifndef __SYSTEM_SETTINGS_FORM_H__
#define __SYSTEM_SETTINGS_FORM_H__

#include <QWidget>

namespace Ui
{
class SystemSettingsForm;
};

class SystemSettingsForm : public QWidget
{
public:
    SystemSettingsForm();
    ~SystemSettingsForm();

private:
    Ui::SystemSettingsForm* form = nullptr;
};

#endif
