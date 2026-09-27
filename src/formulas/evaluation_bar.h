/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __EVALUATION_BAR_H__
#define __EVALUATION_BAR_H__

#include "only_shape_formula.h"
#include <utility>
#include <vector>

namespace yutovo
{

class Assignment;
template<typename T> class CodeParagraphsBlock;
template<typename T> class CodeParagraph;
template<typename T> class CodeRow;

//Evaluation bar with variables in subscript
class EvaluationBarSubscript : public Formula
{
public:
    EvaluationBarSubscript(Element* _parent, bool with_init = true);
    EvaluationBarSubscript(Document* _document, bool with_init = true);
    EvaluationBarSubscript(const EvaluationBarSubscript& source);

    virtual Element* Clone();

    virtual Element* Create(Element* _parent);

    static Element* FromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

    void Init();

    virtual bool AfterFromJson();

    virtual void Draw() const;
    virtual bool Remake(bool with_elements = false);

    virtual void UpdateLevel(uint8_t _level);

    virtual bool AfterInsert(bool with_undo);

    virtual bool GetFirstCaretState(CaretState& caret_state, Selection* select);
    virtual bool GetLastCaretState(CaretState& caret_state, Selection* select);

    virtual std::string ToHtml() const;
    virtual std::u32string ToText() const;
    virtual void ToParserString(ParserString& str);

    std::vector<std::u32string> GetAssignments() const;

protected:
    Shape* GetShape() const; //the "|" shape
    CodeParagraphsBlock<Assignment>* GetBlock() const; //the block of variables

private:
    CodeParagraph<Assignment>* GetParagraph(uint index) const;
    CodeRow<Assignment>* GetParagraphRow(uint index) const;

    const char32_t symbol = U'|';
    std::string symbol_str;
    static const std::string family_name;
};

}

#endif
