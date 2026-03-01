/*
 * Yutovo Desktop
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <QApplication>
#include <QMouseEvent>
#include "prompt_form.h"

//PromptForm

const std::map<std::string, QString> PromptForm::icons = 
{
    {"plus", ":/icons/images/algebra/plus.png"},
    {"minus", ":/icons/images/algebra/minus.png"},
    {"mul", ":/icons/images/algebra/multiply.png"},
    {"div", ":/icons/images/algebra/division.png"},
    {"power", ":/icons/images/algebra/power.png"},
    {"root", ":/icons/images/algebra/nth_root.png"},
    {"sqrt", ":/icons/images/algebra/sqrt.png"},
    {"sub", ":/icons/images/algebra/subscript.png"},
    {"sum", ":/icons/images/algebra/sum.png"},
    {"prod", ":/icons/images/algebra/product.png"}
};

PromptForm::PromptForm(QWidget* parent) : 
    QListWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setIconSize(QSize(24, 24));

    qApp->installEventFilter(this);

    setItemDelegate(new SubscriptDelegate(this));

    connect(this, &PromptForm::itemActivated, this, &PromptForm::OnPromptActivated);
}

PromptForm::~PromptForm()
{
    qApp->removeEventFilter(this);
}

void PromptForm::Fill(std::vector<std::pair<IdentifierType, std::string>>&& _prompt)
{
    prompt = _prompt;
    clear();
    for (auto& p : prompt)
    {
        QListWidgetItem* item;
        auto it = icons.find(p.second);
        if (it != icons.end())
            item = new QListWidgetItem(QIcon(it->second), "");
        else
        {
            //make subscript if it exists
            QString s = QString::fromStdString(p.second);
            s.replace("{", "<sub>");
            s.replace("}", "</sub>");
            item = new QListWidgetItem(s);
        }
        addItem(item);
    }

    //adjust height
    int rows = count();
    if (rows == 0)
        return;
    int visible_rows = qMin(rows, max_visible_rows);
    int h = sizeHintForRow(0);
    int frame = frameWidth() * 2;
    int scrollbar = (rows > max_visible_rows) ? style()->pixelMetric(QStyle::PM_ScrollBarExtent) : 0;
    setFixedHeight(visible_rows * h + frame + scrollbar);

    //adjust width
    int w = sizeHintForColumn(0) + frameWidth() * 2 + style()->pixelMetric(QStyle::PM_ScrollBarExtent);
    setFixedWidth(w);
}

void PromptForm::ActivateCurrentItem()
{
    int pos = row(currentItem());
    if (prompt.size() > pos)
        emit PromptActivated(prompt[pos].first, prompt[pos].second);
    hide();
}

bool PromptForm::eventFilter(QObject* obj, QEvent* event)
{
    if (!isVisible())
        return false;

    if (event->type() == QEvent::MouseButtonPress)
    {
        auto* t = static_cast<QMouseEvent*>(event);
        if (!this->geometry().contains(t->globalPos())) //click outside
            hide();
    }
    return false;
}

void PromptForm::OnPromptActivated(QListWidgetItem *item)
{
    ActivateCurrentItem();
}

//SubscriptDelegate

void SubscriptDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QString text = opt.text;
    opt.text.clear();

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    QRect textRect = style->subElementRect(QStyle::SE_ItemViewItemText, &opt, opt.widget);

    QTextDocument doc;
    doc.setHtml(text);

    QTextOption textOption;
    textOption.setWrapMode(QTextOption::NoWrap);
    doc.setDefaultTextOption(textOption);
    doc.setTextWidth(-1);

    painter->save();
    painter->translate(textRect.topLeft());
    doc.drawContents(painter);
    painter->restore();
}

QSize SubscriptDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTextDocument doc;
    doc.setHtml(index.data().toString());
    doc.setTextWidth(-1);
    QSizeF size = doc.size();
    return QSize(size.width() + 8, size.height() + 6);
}
