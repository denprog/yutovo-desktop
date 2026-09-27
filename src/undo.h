/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __UNDO_H__
#define __UNDO_H__

#include <any>
#include <map>
#include "caret_state.h"
#include "style.h"
#include "config.h"
#include "element.h"

namespace yutovo
{

struct UndoElement;
class Element;
class String;
class Link;
class Paragraph;
class Image;
class Text;
template<typename T> class CodeRow;
template<typename T> class CodeParagraph;
template<typename T> class CodeParagraphsBlock;
class Formula;
class CodeBlock;
class Equation;
class GraphLine;
class Assignment;

typedef std::shared_ptr<UndoElement> UndoElementPtr;

struct UndoElement
{
    UndoElement(ElementType _type);

    virtual bool operator==(const UndoElement& el) const;
    virtual bool operator!=(const UndoElement& el) const;
    virtual bool operator==(const Element& el) const;
    virtual bool operator!=(const Element& el) const;

    virtual Element* Restore(Document* document, Element* parent) = 0;

    ElementType type;
    std::vector<UndoElementPtr> elements;
};

struct UndoString : UndoElement
{
    UndoString(std::u32string _str, StringFormatPtr _format, bool _can_merge);

    virtual bool operator==(const UndoString& el) const;
    virtual bool operator==(const Element& el) const;
    virtual bool operator==(const String& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    std::u32string str;
    StringFormatPtr format;
    bool can_merge;
};

struct UndoLink : UndoElement
{
    UndoLink(std::u32string _str, std::u32string _url, StringFormatPtr _format, bool _can_merge);

    virtual bool operator==(const UndoLink& el) const;
    virtual bool operator==(const Element& el) const;
    virtual bool operator==(const Link& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    std::u32string str;
    std::u32string url;
    StringFormatPtr format;
    bool can_merge;
};

struct UndoParagraph : UndoElement
{
    UndoParagraph(ParagraphFormatPtr _format, StringFormatPtr _current_string_format, const std::u32string& _marker, const StringFormatPtr& _marker_format);

    virtual bool operator==(const UndoParagraph& el) const;
    virtual bool operator==(const Paragraph& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    ParagraphFormatPtr format;
    StringFormatPtr current_string_format;
    std::u32string marker;
    StringFormatPtr marker_format;
};

struct UndoImage : UndoElement
{
    UndoImage(const std::vector<unsigned char>& _picture);

    virtual bool operator==(const UndoImage& el) const;
    virtual bool operator==(const Image& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    std::vector<unsigned char> picture;
};

struct UndoFormula : UndoElement
{
    UndoFormula(ElementType _type, FormulaFormatPtr _formula_format);

    virtual bool operator==(const UndoFormula& el) const;
    virtual bool operator==(const Formula& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    FormulaFormatPtr formula_format;
};

template<typename T = void>
struct UndoCodeRow : UndoElement
{
    UndoCodeRow();

    virtual bool operator==(const UndoCodeRow<T>& el) const
    {
        return UndoElement::operator==(el);
    }

    virtual bool operator==(const CodeRow<T>& el) const
    {
        return UndoElement::operator==(el);
    }

    virtual Element* Restore(Document* document, Element* parent);
};

template<typename T = void>
struct UndoCodeParagraph : UndoParagraph
{
    UndoCodeParagraph(ParagraphFormatPtr _format, StringFormatPtr _current_string_format, const std::u32string& _marker,
        const StringFormatPtr& _marker_format);

    virtual bool operator==(const UndoCodeParagraph<T>& el) const
    {
        if (!UndoElement::operator==(el))
            return false;
        return *format == *el.format && marker == el.marker && *marker_format == *el.marker_format;
    }

    virtual bool operator==(const CodeParagraph<T>& el) const
    {
        if (!UndoElement::operator==(el))
            return false;
        return *format == *el.format && marker == el.marker && *marker_format == *el.marker_format;
    }

    virtual Element* Restore(Document* document, Element* parent);
};

struct UndoCodeBlock : UndoElement
{
    UndoCodeBlock(uint _code_id, CodeFormatPtr _code_format, ParagraphFormatPtr _paragraph_format, FormulaFormatPtr _formula_format);

    virtual bool operator==(const UndoCodeBlock& el) const;
    virtual bool operator==(const CodeBlock& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    uint code_id;
    CodeFormatPtr code_format;
    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

template<typename T = void>
struct UndoCodeParagraphsBlock : UndoElement
{
    UndoCodeParagraphsBlock(ParagraphFormatPtr _paragraph_format, FormulaFormatPtr _formula_format);

    virtual bool operator==(const UndoCodeParagraphsBlock<T>& el) const
    {
        if (!UndoElement::operator==(el))
            return false;
        return *paragraph_format == *el.paragraph_format && *formula_format == *el.formula_format;
    }

    virtual bool operator==(const CodeParagraphsBlock<T>& el) const
    {
        if (!UndoElement::operator==(el))
            return false;
        return *paragraph_format == *el.paragraph_format && *formula_format == *el.formula_format;
    }

    virtual Element* Restore(Document* document, Element* parent);

    ParagraphFormatPtr paragraph_format;
    FormulaFormatPtr formula_format;
};

struct UndoCodeString : UndoString
{
    UndoCodeString(std::u32string _str, StringFormatPtr _format, bool _can_merge);

    virtual Element* Restore(Document* document, Element* parent);
};

struct UndoEquation : UndoFormula
{
    UndoEquation(Equation* equation);

    virtual bool operator==(const UndoEquation& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    yutovo_solver::ResultType result_type;
    std::any config;
};

struct UndoResult : UndoElement
{
    UndoResult(ElementType _type);

    virtual Element* Restore(Document* document, Element* parent);
};

struct UndoGraphLine : UndoFormula
{
    UndoGraphLine(GraphLine* graph);

    virtual bool operator==(const UndoGraphLine& el) const;

    virtual Element* Restore(Document* document, Element* parent);

    GraphFormat format;
};

class ConfigElement : public Element
{
public:
    ConfigElement(const Config& _config);

    virtual Element* Clone()
    {
        return nullptr;
    }

    virtual Element* Create(Element* parent)
    {
        return nullptr;
    }

    Config config;
};

struct UndoConfig : UndoElement
{
    UndoConfig(const Config& _config);

    virtual Element* Restore(Document* document, Element* parent);

    Config config;
};

class FormatElement : public Element
{
public:
    FormatElement(const TextFormat& _format);

    virtual Element* Clone()
    {
        return nullptr;
    }

    virtual Element* Create(Element* parent)
    {
        return nullptr;
    }

    TextFormat format;
};

struct UndoFormat : UndoElement
{
    UndoFormat(const TextFormat& _format);

    virtual Element* Restore(Document* document, Element* parent);

    TextFormat format;
};

class UndoBase
{
public:
    UndoBase(Document* _document);

    int Store(const ElementId& id);
    int Store(const ElementId& parent_id, const int pos, const int size);
    int Store(const Config& config);
    int Store(const TextFormat& format);
    bool Restore(int undo_id, std::vector<ElementPtr>& elements);
    bool Restore(int undo_id, Config& config);
    bool Restore(int undo_id, TextFormat& format);

private:
    int Store(const int undo_id, const LogicalId& id);
    UndoElementPtr StoreElement(const LogicalId id, ElementPtr el);

private:
    Document* document;

    int next_undo_id = 1;

    struct UndoItem
    {
        LogicalId id;
        int refs = 0;
        std::vector<UndoElementPtr> undo_elements; //all saved variants of this element
    };

    std::vector<UndoItem> undo_store; //cached elements are stored here
    std::map<int, std::vector<UndoElementPtr>> undo_items; //undo items by undo_id
};

}

#endif
