#include "properties_dialog.h"
#include "result_settings_form.h"
#include "ui_properties_dialog.h"

//PropertiesDialog

PropertiesDialog::PropertiesDialog(yutovo::Config& _config) :
    form(new Ui::PropertiesDialog()),
    config(_config)
{
    form->setupUi(this);

    form->results_layout->addWidget(new ResultSettingsForm(config));

    form->language->setCurrentIndex((int)config.language - 1);

    setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    
    connect(form->includes, &QListWidget::itemChanged, this, &PropertiesDialog::IncludesItemChanged);
    connect(form->move_file_up, &QAbstractButton::clicked, this, &PropertiesDialog::OnMoveFileUp);
    connect(form->move_file_down, &QAbstractButton::clicked, this, &PropertiesDialog::OnMoveFileDown);

    FillIncludes();
}

void PropertiesDialog::accept()
{
    config.language = (yutovo_calculator::Language)(form->language->currentIndex() + 1);

    config.include_documents.documents.clear();
    for (int i = 0; i < form->includes->count(); ++i)
    {
        QListWidgetItem* item = form->includes->item(i);
        if (item->text() == "")
            continue;
        config.include_documents.documents.emplace_back(yutovo::Config::IncludeDocument{item->text().toUtf8().data(), item->checkState() == Qt::Checked});
    }

    QDialog::accept();
}

void PropertiesDialog::IncludesItemChanged(QListWidgetItem *item)
{
    int r = form->includes->row(item);
    if (item->text() != "" && r == form->includes->count() - 1)
    {
        //add empty item
        QListWidgetItem *item = new QListWidgetItem("");
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        form->includes->addItem(item);
    }
    else if (item->text() == "" && form->includes->count() > 1 && r < form->includes->count() - 1)
    {
        //delete empty item
        form->includes->removeItemWidget(item);
        delete item;
    }
}

void PropertiesDialog::OnMoveFileUp()
{
    int r = form->includes->currentRow();
    if (r == 0)
        return;
    auto* cur = form->includes->takeItem(r);
    form->includes->insertItem(r - 1, cur);
    form->includes->setCurrentRow(r - 1);
}

void PropertiesDialog::OnMoveFileDown()
{
    int r = form->includes->currentRow();
    if (r >= form->includes->count() - 2)
        return;
    auto* cur = form->includes->takeItem(r);
    form->includes->insertItem(r + 1, cur);
    form->includes->setCurrentRow(r + 1);
}

void PropertiesDialog::FillIncludes()
{
    form->includes->clear();
    for (auto& inc : config.include_documents.documents)
    {
        QListWidgetItem *item = new QListWidgetItem(inc.file_name.c_str());
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        item->setCheckState(inc.enabled ? Qt::Checked : Qt::Unchecked);
        form->includes->addItem(item);
    }

    //add empty item
    QListWidgetItem *item = new QListWidgetItem("");
    item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    form->includes->addItem(item);
}
