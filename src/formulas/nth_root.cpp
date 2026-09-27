/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "nth_root.h"
#include "code_row.h"
#include "../str.h"
#include "../document.h"

namespace yutovo
{

//NthRoot

NthRoot::NthRoot(Element* _parent, bool with_init) :
    MiddleShapeFormula(_parent, with_init)
{
    type = ElementType::NTH_ROOT;
    UpdateLevel(level);
}

NthRoot::NthRoot(Document* _document, bool with_init) :
    MiddleShapeFormula(_document, with_init)
{
    type = ElementType::NTH_ROOT;
    UpdateLevel(level);
}

NthRoot::NthRoot(const NthRoot& source) :
    MiddleShapeFormula(source)
{
}

Element* NthRoot::Clone()
{
    return new NthRoot(*this);
}

Element* NthRoot::Create(Element* _parent)
{
    return new NthRoot(_parent);
}

Element* NthRoot::FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    if (parent)
        return new NthRoot(parent, false);
    return new NthRoot(document, false);
}

void NthRoot::Draw() const
{
    GetShape()->draw_func = 
        [&](const Rect& r)
        {
            std::list<Point> path;
            path.push_back(Point{(int)lround(r.left + r.width * 0.722), r.GetBottom()});
            path.push_back(Point{(int)lround(r.left + r.width * 0.281), (int)lround(r.top + r.height * 0.495)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.061), (int)lround(r.top + r.height * 0.544)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.075), (int)lround(r.top + r.height * 0.510)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.343), (int)lround(r.top + r.height * 0.429)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.703), (int)lround(r.top + r.height * 0.869)});
            path.push_back(Point{(int)lround(r.left + r.width * 0.934), r.top});
            path.push_back(Point{r.GetRight() + GetLast()->rect.width, r.top + 1});
            path.push_back(Point{r.GetRight() + GetLast()->rect.width, (int)lround(r.top + r.height * 0.001 + 1)});
            path.push_back(Point{r.GetRight(), (int)lround(r.top + r.height * 0.001 + 1)});
            window->DrawFillPath(path, document->selection.IsSelected(id) ? document->config.formula_bg_color : document->config.shapes_color);
        };
    
    if (document->selection.IsSelected(id))
    {
        Rect abs_rect = GetAbsoluteRect();
        window->DrawFillRect(abs_rect.left, abs_rect.top, abs_rect.width, abs_rect.height, document->config.bg_selection_color);
    }

    Formula::Draw();
}

bool NthRoot::Remake(bool with_elements)
{
    UpdateLevel(level);
    
    bool changed = MiddleShapeFormula::Remake(with_elements);

    //recalc shape rect
    int left_up = GetFirst()->baseline;
    int left_down = GetFirst()->rect.height - left_up;
    int bottom = 0;
    if (left_up > left_down)
        bottom = left_up * 2 + ROOT_Y_OFFSET * 2;
    else
        bottom = left_down * 2 + ROOT_Y_OFFSET * 2;
    GetShape()->rect.SetRect(0, 0, round(GetLast()->rect.height * 5 / 11), std::max(GetLast()->rect.height + ROOT_Y_OFFSET * 2, bottom));

    //recalc items' rects
    int x_offset = ROOT_X_LEFT_OFFSET;
    GetFirst()->rect.Move(x_offset, (GetShape()->rect.height - GetFirst()->rect.height) / 2 + 4);
    x_offset += GetFirst()->rect.width;
    GetShape()->rect.Move(x_offset, 4);
    x_offset += GetShape()->rect.width + ROOT_X_RIGHT_OFFSET;
    int y_offset = 0;
    if (GetShape()->rect.height > GetLast()->rect.height)
        y_offset = ROOT_Y_OFFSET + round((GetShape()->rect.height - GetLast()->rect.height) / 2);
    else
        y_offset = ROOT_Y_OFFSET + GetShape()->rect.height * 0.01;
    GetLast()->rect.Move(x_offset, y_offset + 4);
    baseline = GetLast()->rect.top + GetLast()->baseline;

    UpdateRect();

    if (rect != last_rect)
    {
        last_rect = rect;
        return true;
    }
    return changed;
}

void NthRoot::UpdateLevel(uint8_t _level)
{
    MiddleShapeFormula::UpdateLevel(_level);
    if (_level >= MAX_LEVEL)
        return;
    if (GetFirst())
        GetFirst()->UpdateLevel(_level + 1);
}

bool NthRoot::UseSelection()
{
    return true;
}

std::string NthRoot::ToHtml() const
{
    std::string s = "<mroot>";
    s += GetLast()->ToHtml();
    s += GetFirst()->ToHtml();
    s += "</mroot>";
    return s;
}

std::u32string NthRoot::ToText() const
{
    if (!GetFirst() || !GetLast())
        return U"";
    return U"root(" + GetLast()->ToText() + U"," + GetFirst()->ToText() + U")";
}

void NthRoot::ToParserString(ParserString& str)
{
    str.Add(id, U"root(");
    GetLast()->ToParserString(str);
    str.Add(id, U",");
    GetFirst()->ToParserString(str);
    str.Add(id, U")");
}

}
