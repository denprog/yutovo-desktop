#include "settings_dialog.h"
#include "system_settings_form.h"
#include "interface_settings_form.h"
#include "ui_settings_dialog.h"

//SettingsDialog

SettingsDialog::SettingsDialog(yutovo::Config& _config, QHash<QString, QVariant>& _settings) :
    form(new Ui::SettingsDialog()),
    config(_config),
    settings(_settings)
{
    form->setupUi(this);

    form->settings_tree->setColumnCount(1);

    QTreeWidgetItem *basic_item = new QTreeWidgetItem(form->settings_tree);
    basic_item->setText(0, tr("Basic"));
    QTreeWidgetItem *item = new QTreeWidgetItem(basic_item);
    item->setText(0, tr("System"));
    item = new QTreeWidgetItem(basic_item);
    item->setText(0, tr("Documents"));
    item = new QTreeWidgetItem(basic_item);
    item->setText(0, tr("Interface"));

    QTreeWidgetItem *formula_item = new QTreeWidgetItem(form->settings_tree);
    formula_item->setText(0, tr("Formula"));
    item = new QTreeWidgetItem(formula_item);
    item->setText(0, tr("Font"));
    item = new QTreeWidgetItem(formula_item);
    item->setText(0, tr("Colors"));
    item = new QTreeWidgetItem(formula_item);
    item->setText(0, tr("Cursor"));
    item = new QTreeWidgetItem(formula_item);
    item->setText(0, tr("Clipboard"));

    QTreeWidgetItem *math_item = new QTreeWidgetItem(form->settings_tree);
    math_item->setText(0, tr("Math"));
    item = new QTreeWidgetItem(math_item);
    item->setText(0, tr("Computation"));
    item = new QTreeWidgetItem(math_item);
    item->setText(0, tr("Result"));
    item = new QTreeWidgetItem(math_item);
    item->setText(0, tr("Include"));

    form->settings_tree->expandAll();

    connect(form->settings_tree, &QTreeWidget::itemActivated, this, &SettingsDialog::OnSettingsTreeItemActivated);
}

void SettingsDialog::OnSettingsTreeItemActivated(QTreeWidgetItem *item, int column)
{
    QLayoutItem* _item;
    while ((_item = form->settings_page_layout->takeAt(0)) != nullptr)
    {
        delete _item->widget();
        delete _item;
    }
    
    if (item->text(0) == tr("Basic"))
    {
        QList<QTreeWidgetItem*> items = form->settings_tree->findItems(tr("System"), Qt::MatchExactly | Qt::MatchRecursive);
        form->settings_tree->setCurrentItem(*items.begin());
        form->settings_page_layout->addWidget(new SystemSettingsForm());
    }
    else if (item->text(0) == tr("System"))
    {
        form->settings_page_layout->addWidget(new SystemSettingsForm());
    }
    else if (item->text(0) == tr("Interface"))
    {
        form->settings_page_layout->addWidget(new InterfaceSettingsForm(settings));
    }
}
