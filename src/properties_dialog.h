#ifndef __PROPERTIES_DIALOG_H__
#define __PROPERTIES_DIALOG_H__

#include <QDialog>
#include <QTreeWidget>
#include <QSettings>
#include <yutovo_editor/config.h>

namespace Ui
{
class PropertiesDialog;
};

class PropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    PropertiesDialog(yutovo::Config& _config);
    ~PropertiesDialog();

private:
    Ui::PropertiesDialog* form = nullptr;

public:
    yutovo::Config& config; //copy of config
};

#endif
