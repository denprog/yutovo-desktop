#ifndef __SETTINGS_DIALOG_H__
#define __SETTINGS_DIALOG_H__

#include <QDialog>
#include <QTreeWidget>
#include <QSettings>
#include <yutovo-editor/config.h>

namespace Ui
{
class SettingsDialog;
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(yutovo::Config& _config, QHash<QString, QVariant>& _settings);

private slots:
    void OnSettingsTreeItemActivated(QTreeWidgetItem *item, int column);

private:
    Ui::SettingsDialog* form = nullptr;

public:
    yutovo::Config& config; //copy of config
    QHash<QString, QVariant>& settings; //copy of settings
};

#endif
