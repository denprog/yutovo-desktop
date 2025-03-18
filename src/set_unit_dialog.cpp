#include "set_unit_dialog.h"
#include "ui_set_unit_dialog.h"
#include <yutovo_editor/editor_utils.h>
#include <QPainter>

//SetUnitDialog

SetUnitDialog::SetUnitDialog(std::vector<yutovo_calculator::Unit>& cast_units, yutovo::Config& _config) :
    QDialog(nullptr),
    window(1, 1, _config),
    ui(new Ui::SetUnitDialog),
    units_delegate(new UnitsDelegate(this))
{
    ui->setupUi(this);

    connect(ui->systems, &QListWidget::currentItemChanged, this, &SetUnitDialog::OnCurrentSystemChanged);
    connect(this, &SetUnitDialog::UnitsItemsReady, this, &SetUnitDialog::OnUnitsItemsReady);
    connect(ui->units, &QListWidget::itemDoubleClicked, this, &SetUnitDialog::OnUnitsItemDoubleClicked);
    connect(this, &QDialog::accepted, this, &SetUnitDialog::OnAccepted);

    //sort the cast units by those systems
    for (const Unit& unit : cast_units)
    {
        if (unit.system == U"")
            system_units[U"SI"].push_back(unit);
        else
            system_units[unit.system].push_back(unit);
    }

    for (auto& s : system_units)
        ui->systems->addItem(yutovo::ToBasicString(s.first).c_str());
    ui->systems->setCurrentRow(0);

    ui->units->setItemDelegate(units_delegate.get());
}

SetUnitDialog::~SetUnitDialog()
{
    stop_fill_thread = true;
    if (fill_thread.joinable())
        fill_thread.join();

    delete ui;
}

void SetUnitDialog::FillUnits()
{
    auto it = system_units.find(current_system);
    if (it == system_units.end())
        return;
    
    stop_fill_thread = false;

    DocumentPtr document;
    Config config;
    document.reset(new Document(&window, config));

    document->GetConfig(config);
    config.with_border = false;
    config.caret_visible = false;
    config.formula_border = false;
    document->Start();
    document->SetConfig(config, false);

    std::vector<Unit>& units = it->second;
    for (size_t i = 0; i < units.size(); ++i)
    {
        if (stop_fill_thread)
            break;
        
        Unit& unit = units[i];
        document->Resize(1, 1);
        document->MoveCaretToDocumentBegin(false);
        document->WaitTask(document->DeleteElements(false, false));
        document->WaitTask(document->InsertUnit(unit));
        ElementPtr text = document->GetElement({0});
        document->Resize(text->rect.width, text->rect.height);
        document->WaitTask(document->Redraw(ElementId{0}, false));

        QPixmap pixmap;
        window.GetPixmap(pixmap, QRect(0, 0, text->rect.width, text->rect.height));

        {
            std::lock_guard<std::mutex> lock(units_items_mutex);
            units_items[i] = pixmap;
        }

        emit UnitsItemsReady();
    }
}

void SetUnitDialog::OnCurrentSystemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        return;
    
    stop_fill_thread = true;
    if (fill_thread.joinable())
        fill_thread.join();

    {
        std::lock_guard<std::mutex> lock(units_items_mutex);
        ui->units->clear();
        units_items.clear();
    }

    current_system = current->text().toStdU32String();

    fill_thread = std::thread(&SetUnitDialog::FillUnits, this);
}

void SetUnitDialog::OnUnitsItemDoubleClicked(QListWidgetItem *item)
{
    accept();
}

void SetUnitDialog::OnUnitsItemsReady()
{
    std::lock_guard<std::mutex> lock(units_items_mutex);
    for (int i = ui->units->count(); i < units_items.size(); ++i)
        ui->units->addItem(new QListWidgetItem());
}

void SetUnitDialog::OnAccepted()
{
    int i = ui->units->currentRow();
    if (i >= 0)
        value = system_units[current_system][i];
}

//UnitsDelegate

UnitsDelegate::UnitsDelegate(SetUnitDialog* _dialog) :
    dialog(_dialog)
{
}

void UnitsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, dialog->palette().highlight().color());
    std::lock_guard<std::mutex> lock(dialog->units_items_mutex);
    QPixmap& p = dialog->units_items[index.row()];
    painter->drawPixmap(option.rect.x(), option.rect.y(), p);
}

QSize UnitsDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    std::lock_guard<std::mutex> lock(dialog->units_items_mutex);
    return dialog->units_items[index.row()].size();
}
