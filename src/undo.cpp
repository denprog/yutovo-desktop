/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "undo.h"
#include "str.h"
#include "paragraph.h"
#include "row.h"
#include "text.h"
#include "image.h"
#include "link.h"
#include "formulas/code_row.h"
#include "formulas/code_paragraph.h"
#include "formulas/code_block.h"
#include "formulas/code_string.h"
#include "formulas/plus.h"
#include "formulas/minus.h"
#include "formulas/multiply.h"
#include "formulas/division.h"
#include "formulas/square_root.h"
#include "formulas/nth_root.h"
#include "formulas/power.h"
#include "formulas/equation.h"
#include "formulas/brackets.h"
#include "formulas/assignment.h"
#include "formulas/unit.h"
#include "formulas/equation.h"
#include "formulas/subscript.h"
#include "formulas/exclamation.h"
#include "formulas/and.h"
#include "formulas/or.h"
#include "formulas/xor.h"
#include "formulas/not.h"
#include "formulas/percent.h"
#include "formulas/sum.h"
#include "formulas/product.h"
#include "formulas/definite_integral.h"
#include "formulas/indefinite_integral.h"
#include "formulas/evaluation_bar.h"
#include "formulas/comma.h"
#include "formulas/graph.h"

namespace yutovo
{

//UndoElement

UndoElement::UndoElement(ElementType _type) :
    type(_type)
{
}

bool UndoElement::operator==(const UndoElement& el) const
{
    if (type != el.type || elements.size() != el.elements.size())
        return false;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (*elements[i] != *el.elements[i])
            return false;
    }
    return true;
}

bool UndoElement::operator!=(const UndoElement& el) const
{
    return !(*this == el);
}

bool UndoElement::operator==(const Element& el) const
{
    if (type != el.type || elements.size() != el.elements->Count())
        return false;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (*elements[i] != *el.elements->Get(i))
            return false;
    }
    return true;
}

bool UndoElement::operator!=(const Element& el) const
{
    return !(*this == el);
}

//UndoString

UndoString::UndoString(std::u32string _str, StringFormatPtr _format, bool _can_merge) :
    UndoElement(ElementType::STRING),
    str(_str),
    format(_format),
    can_merge(_can_merge)
{
}

bool UndoString::operator==(const UndoString& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return str == el.str && *format == *el.format;
}

bool UndoString::operator==(const Element& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return str == el.elements->ToText() && *format == *((String&)el).format;
}

bool UndoString::operator==(const String& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return str == el.elements->ToText() && *format == *el.format;
}

Element* UndoString::Restore(Document* document, Element* parent)
{
    Element* el;
    if (parent)
        el = new String(parent, str, format);
    else
        el = new String(document, str, format);
    el->can_merge = can_merge;
    return el;
}

//UndoLink

UndoLink::UndoLink(std::u32string _str, std::u32string _url, StringFormatPtr _format, bool _can_merge) : 
    UndoElement(ElementType::LINK),
    str(_str),
    url(_url),
    format(_format),
    can_merge(_can_merge)
{
}

bool UndoLink::operator==(const UndoLink& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return str == el.str && url == el.url && *format == *el.format;
}

bool UndoLink::operator==(const Element& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    Link& link = (Link&)el;
    return str == el.elements->ToText() && url == link.url && *format == *(link).format;
}

bool UndoLink::operator==(const Link& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return str == el.elements->ToText() && url == el.url && *format == *el.format;
}

Element* UndoLink::Restore(Document* document, Element* parent)
{
    Element* el;
    if (parent)
        el = new Link(parent, str, url, format);
    else
        el = new Link(document, str, url, format);
    el->can_merge = can_merge;
    return el;
}

//UndoParagraph

UndoParagraph::UndoParagraph(ParagraphFormatPtr _format, StringFormatPtr _current_string_format, const std::u32string& _marker, 
    const StringFormatPtr& _marker_format) :
    UndoElement(ElementType::PARAGRAPH),
    format(_format),
    current_string_format(_current_string_format),
    marker(_marker),
    marker_format(_marker_format)
{
}

bool UndoParagraph::operator==(const UndoParagraph& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return *format == *el.format && marker == el.marker && *marker_format == *el.marker_format;
}

bool UndoParagraph::operator==(const Paragraph& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return *format == *el.format && marker == el.marker && *marker_format == *el.marker_format;
}

Element* UndoParagraph::Restore(Document* document, Element* parent)
{
    Paragraph* p;
    if (parent)
        p = new Paragraph(parent);
    else
        p = new Paragraph(document);
    p->format = format;
    p->current_string_format = current_string_format;
    p->marker = marker;
    p->marker_format = marker_format;
    auto r = p->elements->Get(0);
    r->elements->Clear();
    for (size_t i = 0; i < elements.size(); ++i)
        r->elements->Add(ElementPtr(elements[i]->Restore(document, r.get())));
    return p;
}

//UndoImage

UndoImage::UndoImage(const std::vector<unsigned char>& _picture) :
    UndoElement(ElementType::IMAGE),
    picture(_picture)
{
}

bool UndoImage::operator==(const UndoImage& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return picture == el.picture;
}

bool UndoImage::operator==(const Image& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return picture == el.picture;
}

Element* UndoImage::Restore(Document* document, Element* parent)
{
    Image* p;
    if (parent)
        p = new Image(parent, picture);
    else
        p = new Image(document, picture);
    return p;
}

//UndoFormula

UndoFormula::UndoFormula(ElementType _type, FormulaFormatPtr _formula_format) :
    UndoElement(_type),
    formula_format(_formula_format)
{
}

bool UndoFormula::operator==(const UndoFormula& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return *formula_format == *el.formula_format;
}

bool UndoFormula::operator==(const Formula& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return *formula_format == *el.formula_format;
}

Element* UndoFormula::Restore(Document* document, Element* parent)
{
    Formula* el = nullptr;
    switch (type)
    {
    case ElementType::PLUS:
        el = parent ? new Plus(parent) : new Plus(document);
        break;
    case ElementType::MINUS:
        el = parent ? new Minus(parent) : new Minus(document);
        break;
    case ElementType::MULTIPLY:
        el = parent ? new Multiply(parent) : new Multiply(document);
        break;
    case ElementType::POWER:
    case ElementType::DIVISION:
    case ElementType::NTH_ROOT:
    case ElementType::SUBSCRIPT:
    case ElementType::ASSIGNMENT:
    case ElementType::UNIT:
        {
            assert(elements.size() == 2);
            switch (type)
            {
            case ElementType::POWER:
                el = parent ? new Power(parent) : new Power(document);
                break;
            case ElementType::DIVISION:
                el = parent ? new Division(parent) : new Division(document);
                break;
            case ElementType::NTH_ROOT:
                el = parent ? new NthRoot(parent) : new NthRoot(document);
                break;
            case ElementType::SUBSCRIPT:
                el = parent ? new Subscript(parent) : new Subscript(document);
                break;
            case ElementType::ASSIGNMENT:
                el = parent ? new Assignment(parent) : new Assignment(document);
                break;
            case ElementType::UNIT:
                el = parent ? new Unit(parent) : new Unit(document);
                break;
            default:
                assert(false);
            }
            ElementPtr first(elements[0]->Restore(document, el));
            ElementPtr last(elements[1]->Restore(document, el));
            el->elements->Get(0)->elements->ReplaceAll(*first->elements);
            el->elements->Get(2)->elements->ReplaceAll(*last->elements);
        }
        break;
    case ElementType::SQUARE_ROOT:
        {
            el = parent ? new SquareRoot(parent) : new SquareRoot(document);
            ElementPtr last(elements[0]->Restore(document, el));
            el->elements->Get(1)->elements->ReplaceAll(*last->elements);
        }
        break;
    case ElementType::OPEN_ROUND_BRACKET:
        el = parent ? new OpenBracket(parent, ElementType::OPEN_ROUND_BRACKET) : new OpenBracket(document, ElementType::OPEN_ROUND_BRACKET);
        break;
    case ElementType::CLOSE_ROUND_BRACKET:
        el = parent ? new CloseBracket(parent, ElementType::CLOSE_ROUND_BRACKET) : new CloseBracket(document, ElementType::CLOSE_ROUND_BRACKET);
        break;
    case ElementType::OPEN_SQUARE_BRACKET:
        el = parent ? new OpenBracket(parent, ElementType::OPEN_SQUARE_BRACKET) : new OpenBracket(document, ElementType::OPEN_SQUARE_BRACKET);
        break;
    case ElementType::CLOSE_SQUARE_BRACKET:
        el = parent ? new CloseBracket(parent, ElementType::CLOSE_SQUARE_BRACKET) : new CloseBracket(document, ElementType::CLOSE_SQUARE_BRACKET);
        break;
    case ElementType::EXCLAMATION:
        el = parent ? new Exclamation(parent) : new Exclamation(document);
        break;
    case ElementType::AND:
        el = parent ? new And(parent) : new And(document);
        break;
    case ElementType::OR:
        el = parent ? new Or(parent) : new Or(document);
        break;
    case ElementType::XOR:
        el = parent ? new Xor(parent) : new Xor(document);
        break;
    case ElementType::NOT:
        el = parent ? new Not(parent) : new Not(document);
        break;
    case ElementType::PERCENT:
        el = parent ? new Percent(parent) : new Percent(document);
        break;
    case ElementType::COMMA:
        el = parent ? new Comma(parent) : new Comma(document);
        break;
    case ElementType::SHAPE:
        el = parent ? new Shape(parent) : new Shape(document);
        break;
    case ElementType::SUM:
    case ElementType::PRODUCT:
        {
            if (type == ElementType::SUM)
                el = parent ? new Sum(parent) : new Sum(document);
            else
                el = parent ? new Product(parent) : new Product(document);
            assert(elements.size() == 3);
            ElementPtr lower(elements[0]->Restore(document, el));
            ElementPtr upper(elements[1]->Restore(document, el));
            ElementPtr right(elements[2]->Restore(document, el));
            el->elements->Get(0)->elements->ReplaceAll(*lower->elements);
            el->elements->Get(2)->elements->ReplaceAll(*upper->elements);
            el->elements->Get(3)->elements->ReplaceAll(*right->elements);
        }
        break;
    case ElementType::DEFINITE_INTEGRAL:
        {
            el = parent ? new DefiniteIntegral(parent) : new DefiniteIntegral(document);
            assert(elements.size() == 4);
            ElementPtr lower(elements[0]->Restore(document, el));
            ElementPtr upper(elements[1]->Restore(document, el));
            ElementPtr expression(elements[2]->Restore(document, el));
            ElementPtr var(elements[3]->Restore(document, el));
            el->elements->Get(0)->elements->ReplaceAll(*lower->elements);
            el->elements->Get(2)->elements->ReplaceAll(*upper->elements);
            el->elements->Get(3)->elements->ReplaceAll(*expression->elements);
            el->elements->Get(5)->elements->ReplaceAll(*var->elements);
        }
        break;
    case ElementType::INDEFINITE_INTEGRAL:
        {
            el = parent ? new IndefiniteIntegral(parent) : new IndefiniteIntegral(document);
            assert(elements.size() == 2);
            ElementPtr expression(elements[0]->Restore(document, el));
            ElementPtr var(elements[1]->Restore(document, el));
            el->elements->Get(1)->elements->ReplaceAll(*expression->elements);
            el->elements->Get(3)->elements->ReplaceAll(*var->elements);
        }
        break;
    case ElementType::EVALUATION_BAR_SUBSCRIPT:
        {
            el = parent ? new EvaluationBarSubscript(parent) : new EvaluationBarSubscript(document);
            assert(elements.size() == 2);
            ElementPtr shape_el(elements[0]->Restore(document, el));
            ElementPtr block(elements[1]->Restore(document, el));
            el->elements->Get(0)->elements->ReplaceAll(*shape_el->elements);
            el->elements->Get(1)->elements->ReplaceAll(*block->elements);
        }
        break;
    default:
        assert(false);
    }

    el->formula_format = formula_format;

    return el;
}

//UndoCodeRow

template<typename T>
UndoCodeRow<T>::UndoCodeRow() :
    UndoElement(ElementType::CODE_ROW)
{
}

template<typename T>
Element* UndoCodeRow<T>::Restore(Document* document, Element* parent)
{
    CodeRow<T>* r = (parent && parent->parent) ? new CodeRow<T>(parent, true) : new CodeRow<T>(document);
    r->elements->Clear();
    for (size_t i = 0; i < elements.size(); ++i)
        r->elements->Add(ElementPtr(elements[i]->Restore(document, r)));
    return r;
}

template<>
UndoCodeRow<Assignment>::UndoCodeRow() :
    UndoElement(ElementType::CODE_ROW_ASSIGNMENT)
{
}

//UndoCodeParagraph

template<typename T>
UndoCodeParagraph<T>::UndoCodeParagraph(ParagraphFormatPtr _format, StringFormatPtr _current_string_format, const std::u32string& _marker,
    const StringFormatPtr& _marker_format) :
    UndoParagraph(_format, _current_string_format, _marker, _marker_format)
{
    type = ElementType::CODE_PARAGRAPH;
}

template<typename T>
Element* UndoCodeParagraph<T>::Restore(Document* document, Element* parent)
{
    CodeParagraph<T>* p = parent ? new CodeParagraph<T>(parent, true) : new CodeParagraph<T>(document, true);
    p->format = format;
    p->current_string_format = current_string_format;
    p->marker = marker;
    p->marker_format = marker_format;
    p->elements->Clear();
    for (size_t i = 0; i < elements.size(); ++i)
        p->elements->Add(ElementPtr(elements[i]->Restore(document, p)));
    return p;
}

template<>
UndoCodeParagraph<Assignment>::UndoCodeParagraph(ParagraphFormatPtr _format, StringFormatPtr _current_string_format, const std::u32string& _marker,
    const StringFormatPtr& _marker_format) :
    UndoParagraph(_format, _current_string_format, _marker, _marker_format)
{
    type = ElementType::CODE_PARAGRAPH_ASSIGNMENT;
}

//UndoCodeBlock

UndoCodeBlock::UndoCodeBlock(uint _code_id, CodeFormatPtr _code_format, ParagraphFormatPtr _paragraph_format, FormulaFormatPtr _formula_format) :
    UndoElement(ElementType::CODE_BLOCK),
    code_id(_code_id),
    code_format(_code_format),
    paragraph_format(_paragraph_format),
    formula_format(_formula_format)
{
}

bool UndoCodeBlock::operator==(const UndoCodeBlock& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return *code_format == *el.code_format && *paragraph_format == *el.paragraph_format && *formula_format == *el.formula_format;
}

bool UndoCodeBlock::operator==(const CodeBlock& el) const
{
    if (!UndoElement::operator==(el))
        return false;
    return *code_format == *el.code_format && *paragraph_format == *el.paragraph_format && *formula_format == *el.formula_format;
}

Element* UndoCodeBlock::Restore(Document* document, Element* parent)
{
    CodeBlock* c = parent ? new CodeBlock(parent, code_id, true, false) : new CodeBlock(document, code_id, true, false);
    c->code_format = code_format;
    c->paragraph_format = paragraph_format;
    c->formula_format = formula_format;
    c->elements->Clear();
    for (size_t i = 0; i < elements.size(); ++i)
        c->elements->Add(ElementPtr(elements[i]->Restore(document, c)));
    return c;
}

//UndoCodeParagraphsBlock

template<typename T>
UndoCodeParagraphsBlock<T>::UndoCodeParagraphsBlock(ParagraphFormatPtr _paragraph_format, FormulaFormatPtr _formula_format) :
    UndoElement(ElementType::CODE_PARAGRAPHS_BLOCK),
    paragraph_format(_paragraph_format),
    formula_format(_formula_format)
{
}

template<typename T>
Element* UndoCodeParagraphsBlock<T>::Restore(Document* document, Element* parent)
{
    CodeParagraphsBlock<T>* c = parent ? new CodeParagraphsBlock<T>(parent, false) : new CodeParagraphsBlock<T>(document, false);
    c->paragraph_format = paragraph_format;
    c->formula_format = formula_format;
    c->elements->Clear();
    for (size_t i = 0; i < elements.size(); ++i)
        c->elements->Add(ElementPtr(elements[i]->Restore(document, c)));
    return c;
}

template<>
UndoCodeParagraphsBlock<Assignment>::UndoCodeParagraphsBlock(ParagraphFormatPtr _paragraph_format, FormulaFormatPtr _formula_format) :
    UndoElement(ElementType::CODE_PARAGRAPHS_BLOCK_ASSIGNMENT),
    paragraph_format(_paragraph_format),
    formula_format(_formula_format)
{
}

//Explicit instantiations for undo templates
template struct UndoCodeRow<void>;
template struct UndoCodeRow<Assignment>;
template struct UndoCodeParagraph<void>;
template struct UndoCodeParagraph<Assignment>;
template struct UndoCodeParagraphsBlock<void>;
template struct UndoCodeParagraphsBlock<Assignment>;

//UndoCodeString

UndoCodeString::UndoCodeString(std::u32string _str, StringFormatPtr _format, bool _can_merge) :
    UndoString(_str, _format, _can_merge)
{
}

Element* UndoCodeString::Restore(Document* document, Element* parent)
{
    Element* el = parent ? new CodeString(parent, str, format) : new CodeString(document, str, format);
    el->can_merge = can_merge;
    return el;
}

//UndoEquation

UndoEquation::UndoEquation(Equation* equation) :
    UndoFormula(ElementType::EQUATION, equation->formula_format),
    result_type(equation->result_type)
{
    if (equation->elements->Count() != 3)
        return;
    auto el = equation->elements->Get(2)->elements->Get(0);
    auto result = el.get();
    switch (result_type)
    {
    case ResultType::REAL:
        config = ((RealResult*)result)->config;
        return;
	case ResultType::INTEGER:
        config = ((IntegerResult*)result)->config;
        return;
	case ResultType::RATIONAL:
        config = ((RationalResult*)result)->config;
        return;
	case ResultType::COMPLEX:
        config = ((ComplexResult*)result)->config;
        return;
    case ResultType::ARRAY_REAL:
        config = ((ArrayRealResult*)result)->config;
        return;
    case ResultType::AUTO:
        config = ((AutoResult*)result)->config;
        return;
    case ResultType::SYMBOLIC_REAL:
        config = ((SymbolicRealResult*)result)->config;
        return;
    case ResultType::SYMBOLIC_RATIONAL:
        config = ((SymbolicRationalResult*)result)->config;
        return;
    case ResultType::SYMBOLIC_COMPLEX:
        config = ((SymbolicComplexResult*)result)->config;
        return;
    default:
        assert(false);
    }
}

bool UndoEquation::operator==(const UndoEquation& el) const
{
    if (!UndoFormula::operator==(el) || result_type != el.result_type)
        return false;
    if (config.has_value() != el.config.has_value())
        return false;
    if (!config.has_value())
        return true;
    
    switch (result_type)
    {
    case ResultType::AUTO:
        return std::any_cast<Config::AutoResultConfig>(config) == std::any_cast<Config::AutoResultConfig>(el.config);
    case ResultType::REAL:
        return std::any_cast<Config::RealResultConfig>(config) == std::any_cast<Config::RealResultConfig>(el.config);
    case ResultType::INTEGER:
        return std::any_cast<Config::IntegerResultConfig>(config) == std::any_cast<Config::IntegerResultConfig>(el.config);
    case ResultType::RATIONAL:
        return std::any_cast<Config::RationalResultConfig>(config) == std::any_cast<Config::RationalResultConfig>(el.config);
    case ResultType::COMPLEX:
        return std::any_cast<Config::ComplexResultConfig>(config) == std::any_cast<Config::ComplexResultConfig>(el.config);
    case ResultType::ARRAY_REAL:
        return std::any_cast<Config::ArrayRealResultConfig>(config) == std::any_cast<Config::ArrayRealResultConfig>(el.config);
    case ResultType::SYMBOLIC_REAL:
        return std::any_cast<Config::RealResultConfig>(config) == std::any_cast<Config::RealResultConfig>(el.config);
    case ResultType::SYMBOLIC_RATIONAL:
        return std::any_cast<Config::RationalResultConfig>(config) == std::any_cast<Config::RationalResultConfig>(el.config);
    case ResultType::SYMBOLIC_COMPLEX:
        return std::any_cast<Config::ComplexResultConfig>(config) == std::any_cast<Config::ComplexResultConfig>(el.config);
    default:
        assert(false);
    }
    return false;
}

Element* UndoEquation::Restore(Document* document, Element* parent)
{
    Equation* el = parent ? new Equation(parent, result_type) : new Equation(document, result_type);
    el->formula_format = formula_format;
    ElementPtr first(elements[0]->Restore(document, el));
    el->elements->Get(0)->elements->ReplaceAll(*first->elements);

    if (!config.has_value())
        return el;
    
    ElementPtr last;
    switch (result_type)
    {
    case ResultType::AUTO:
        el->SetResult(std::any_cast<Config::AutoResultConfig>(config));
        break;
    case ResultType::REAL:
        el->SetResult(std::any_cast<Config::RealResultConfig>(config));
        break;
    case ResultType::INTEGER:
        el->SetResult(std::any_cast<Config::IntegerResultConfig>(config));
        break;
    case ResultType::RATIONAL:
        el->SetResult(std::any_cast<Config::RationalResultConfig>(config));
        break;
    case ResultType::COMPLEX:
        el->SetResult(std::any_cast<Config::ComplexResultConfig>(config));
        break;
    case ResultType::ARRAY_REAL:
        el->SetResult(std::any_cast<Config::ArrayRealResultConfig>(config));
        break;
    case ResultType::SYMBOLIC_REAL:
        el->SetSymbolicRealResult(std::any_cast<Config::RealResultConfig>(config));
        break;
    case ResultType::SYMBOLIC_RATIONAL:
        el->SetSymbolicRationalResult(std::any_cast<Config::RationalResultConfig>(config));
        break;
    case ResultType::SYMBOLIC_COMPLEX:
        el->SetSymbolicComplexResult(std::any_cast<Config::ComplexResultConfig>(config));
        break;
    default:
        assert(false);
    }
    return el;
}

//UndoResult

UndoResult::UndoResult(ElementType _type) :
    UndoElement(_type)
{
}

Element* UndoResult::Restore(Document* document, Element* parent)
{
    ResultRow* el = nullptr;
    assert(elements.size() == 1);
    switch (type)
    {
    case ElementType::REAL_RESULT:
        el = parent ? new RealResult(parent) : new RealResult(document);
        break;
    case ElementType::INTEGER_RESULT:
        el = parent ? new IntegerResult(parent) : new IntegerResult(document);
        break;
    case ElementType::RATIONAL_RESULT:
        el = parent ? new RationalResult(parent) : new RationalResult(document);
        break;
    case ElementType::COMPLEX_RESULT:
        el = parent ? new ComplexResult(parent) : new ComplexResult(document);
        break;
    case ElementType::ARRAY_REAL_RESULT:
        el = parent ? new ArrayRealResult(parent) : new ArrayRealResult(document);
        break;
    case ElementType::AUTO_RESULT:
        el = parent ? new AutoResult(parent) : new AutoResult(document);
        break;
    case ElementType::ERROR_RESULT:
        el = parent ? new ErrorResult(parent, Error{}) : new ErrorResult(document);
        break;
    default:
        assert(false);
    }

    ElementPtr first(elements[0]->Restore(document, el));
    el->elements->Add(first);
    return el;
}

//UndoGraphLine

UndoGraphLine::UndoGraphLine(GraphLine* graph) :
    UndoFormula(ElementType::GRAPH_LINE, graph->formula_format),
    format(graph->format)
{
}

bool UndoGraphLine::operator==(const UndoGraphLine& el) const
{
    return UndoFormula::operator==(el);
}

Element* UndoGraphLine::Restore(Document* document, Element* parent)
{
    GraphLine* el = parent ? new GraphLine(parent) : new GraphLine(document);
    el->format = format;
    el->formula_format = formula_format;
    for (int i = 0; i < elements.size(); ++i)
    {
        ElementPtr ch(elements[i]->Restore(document, el));
        el->elements->Get(i)->elements->ReplaceAll(*ch->elements);
    }
    return el;
}

//ConfigElement

ConfigElement::ConfigElement(const Config& _config) : 
    Element((Element*)nullptr),
    config(_config)
{
}

//UndoConfig

UndoConfig::UndoConfig(const Config& _config) : 
    UndoElement(ElementType::NONE),
    config(_config)
{
}

Element* UndoConfig::Restore(Document* document, Element* parent)
{
    return new ConfigElement(config);
}

//FormatElement

FormatElement::FormatElement(const TextFormat& _format) : 
    Element((Element*)nullptr),
    format(_format)
{
}

//UndoFormat

UndoFormat::UndoFormat(const TextFormat& _format) :
    UndoElement(ElementType::NONE),
    format(_format)
{
}

Element* UndoFormat::Restore(Document* document, Element* parent)
{
    return new FormatElement(format);
}

//UndoBase

UndoBase::UndoBase(Document* _document) :
    document(_document)
{
}

int UndoBase::Store(const ElementId& id)
{
    std::vector<LogicalId> ids;
    auto el = document->GetElement(id);
    if (el->type == ElementType::ROW)
    {
        el = document->GetParent(el->id);
    }
    ids.push_back(el->logical_id);
    for (auto& _id : ids)
    {
        if (!Store(next_undo_id, _id))
            return -1;
    }
    return next_undo_id++;
}

int UndoBase::Store(const ElementId& parent_id, const int pos, const int size)
{
    std::vector<LogicalId> ids;
    for (int i = pos; i < pos + size; ++i)
    {
        auto el = document->GetElement(GetChild(parent_id, i));
        if (ids.empty() || ids[i - pos - 1] != el->logical_id)
        {
            if (el->logical_id.empty())
            {
                for (int j = 0; j < el->elements->Count(); ++j)
                {
                    LogicalId _id = el->elements->Get(j)->logical_id;
                    if (ids.empty() || ids[ids.size() - 1] != _id)
                        ids.push_back(_id);
                }
            }
            else
                ids.push_back(el->logical_id);
        }
    }
    for (auto& id : ids)
    {
        if (!Store(next_undo_id, id))
            return -1;
    }
    return next_undo_id++;
}

int UndoBase::Store(const Config& config)
{
    UndoElementPtr undo_element(new UndoConfig(config));
    UndoItem item{LogicalId{}, 1, std::vector{undo_element}};
    undo_items[next_undo_id].push_back(undo_element);
    return next_undo_id++;
}

int UndoBase::Store(const TextFormat& format)
{
    UndoElementPtr undo_element(new UndoFormat(format));
    UndoItem item{LogicalId{}, 1, std::vector{undo_element}};
    undo_items[next_undo_id].push_back(undo_element);
    return next_undo_id++;
}

bool UndoBase::Restore(int undo_id, std::vector<ElementPtr>& elements)
{
    auto it = undo_items.find(undo_id);
    if (it == undo_items.end())
        return false;
    
    std::function<void (ElementPtr)> update_fields = 
        [&](ElementPtr el)
        {
            el->document = document;
            el->window = document->window;
            el->caret = document->caret;
            el->selection = &document->selection;
            el->elements->caret = document->caret;
            el->elements->selection = &document->selection;
            if (!document->IsString(el))
            {
                for (int i = 0; i < el->elements->Count(); ++i)
                    update_fields(el->elements->Get(i));
            }
        };

    for (UndoElementPtr& el : it->second)
    {
        ElementPtr _el(el->Restore(document, nullptr));
        update_fields(_el);
        elements.push_back(_el);
    }
    return true;
}

bool UndoBase::Restore(int undo_id, Config& config)
{
    auto it = undo_items.find(undo_id);
    if (it == undo_items.end())
        return false;
    if (it->second.empty())
        return false;
    ElementPtr _el(it->second[0]->Restore(document, nullptr));
    config = ((ConfigElement*)_el.get())->config;
    return true;
}

bool UndoBase::Restore(int undo_id, TextFormat& format)
{
    auto it = undo_items.find(undo_id);
    if (it == undo_items.end())
        return false;
    if (it->second.empty())
        return false;
    ElementPtr _el(it->second[0]->Restore(document, nullptr));
    format = ((FormatElement*)_el.get())->format;
    return true;
}

int UndoBase::Store(const int undo_id, const LogicalId& id)
{
    std::vector<ElementPtr> elements;
    document->GetElements(id, elements);
    if (elements.empty())
        return false;
    
    for (ElementPtr el : elements)
    {
        UndoElementPtr undo_el = StoreElement(id, el);
        if (!undo_el)
            return false;
        undo_items[undo_id].push_back(undo_el);
    }
    return true;
}

UndoElementPtr UndoBase::StoreElement(const LogicalId id, ElementPtr el)
{
    auto undo_store_it = std::find_if(undo_store.begin(), undo_store.end(), 
        [id](UndoItem& item)
        {
            return item.id == id;
        });
    if (undo_store_it != undo_store.end())
    {
        UndoItem& item = *undo_store_it;
        auto _it = std::find_if(item.undo_elements.begin(), item.undo_elements.end(), 
            [el](UndoElementPtr& undo_element)
            {
                return *undo_element == *el;
            });
        if (_it != item.undo_elements.end())
        {
            item.refs++;
            return *_it; //this element is already saved
        }
    }

    auto store_element = 
        [this](ElementPtr el, UndoElementPtr& undo_element)
        {
            UndoElementPtr undo_ch = StoreElement(el->logical_id, el);
            if (!undo_ch)
                return false;
            undo_element->elements.push_back(undo_ch);
            return true;
        };

    //add a new undo item
    UndoElementPtr undo_element;
    switch (el->type)
    {
    case ElementType::STRING:
        undo_element.reset(new UndoString(el->ToText(), ((String*)el.get())->format, el->can_merge));
        break;
    case ElementType::LINK:
        undo_element.reset(new UndoLink(el->ToText(), ((Link*)el.get())->url, ((String*)el.get())->format, el->can_merge));
        break;
    case ElementType::PARAGRAPH:
        {
            Paragraph* p = (Paragraph*)el.get();
            undo_element.reset(new UndoParagraph(p->format, p->current_string_format, p->marker, p->marker_format));
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr row = el->elements->Get(i);
                for (int j = 0; j < row->elements->Count(); ++j)
                {
                    auto ch = row->elements->Get(j);
                    UndoElementPtr undo_ch = StoreElement(ch->logical_id, ch);
                    if (!undo_ch)
                        return nullptr;
                    undo_element->elements.push_back(undo_ch);
                }
            }
        }
        break;
    case ElementType::IMAGE:
        {
            Image* _el = (Image*)el.get();
            undo_element.reset(new UndoImage(_el->picture));
        }
        break;
    case ElementType::CODE_BLOCK:
        {
            CodeBlock* c = (CodeBlock*)el.get();
            undo_element.reset(new UndoCodeBlock(c->code_id, c->code_format, c->paragraph_format, c->formula_format));
            for (int i = 0; i < c->elements->Count(); ++i)
            {
                ElementPtr ch = c->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(ch->logical_id, ch);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_ROW:
        {
            CodeRow<>* r = (CodeRow<>*)el.get();
            undo_element.reset(new UndoCodeRow<>());
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr ch = el->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(ch->logical_id, ch);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_ROW_ASSIGNMENT:
        {
            CodeRow<Assignment>* r = (CodeRow<Assignment>*)el.get();
            undo_element.reset(new UndoCodeRow<Assignment>());
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr ch = el->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(ch->logical_id, ch);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_PARAGRAPH:
        {
            CodeParagraph<>* p = (CodeParagraph<>*)el.get();
            undo_element.reset(new UndoCodeParagraph<>(p->format, p->current_string_format, p->marker, p->marker_format));
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr row = el->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(row->logical_id, row);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_PARAGRAPH_ASSIGNMENT:
        {
            CodeParagraph<Assignment>* p = (CodeParagraph<Assignment>*)el.get();
            undo_element.reset(new UndoCodeParagraph<Assignment>(p->format, p->current_string_format, p->marker, p->marker_format));
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr row = el->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(row->logical_id, row);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_PARAGRAPHS_BLOCK:
        {
            CodeParagraphsBlock<>* c = (CodeParagraphsBlock<>*)el.get();
            undo_element.reset(new UndoCodeParagraphsBlock<>(c->paragraph_format, c->formula_format));
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr ch = el->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(ch->logical_id, ch);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_PARAGRAPHS_BLOCK_ASSIGNMENT:
        {
            CodeParagraphsBlock<Assignment>* c = (CodeParagraphsBlock<Assignment>*)el.get();
            undo_element.reset(new UndoCodeParagraphsBlock<Assignment>(c->paragraph_format, c->formula_format));
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                ElementPtr ch = el->elements->Get(i);
                UndoElementPtr undo_ch = StoreElement(ch->logical_id, ch);
                if (!undo_ch)
                    return nullptr;
                undo_element->elements.push_back(undo_ch);
            }
        }
        break;
    case ElementType::CODE_STRING:
        undo_element.reset(new UndoCodeString(el->ToText(), ((CodeString*)el.get())->format, el->can_merge));
        break;
    case ElementType::SHAPE:
    case ElementType::PLUS:
    case ElementType::MINUS:
    case ElementType::MULTIPLY:
    case ElementType::EXCLAMATION:
    case ElementType::AND:
    case ElementType::OR:
    case ElementType::XOR:
    case ElementType::NOT:
    case ElementType::PERCENT:
    case ElementType::OPEN_ROUND_BRACKET:
    case ElementType::CLOSE_ROUND_BRACKET:
    case ElementType::OPEN_SQUARE_BRACKET:
    case ElementType::CLOSE_SQUARE_BRACKET:
    case ElementType::COMMA:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        break;
    case ElementType::SQUARE_ROOT:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        if (!store_element(el->elements->Get(1), undo_element))
            return nullptr;
        break;
    case ElementType::POWER:
    case ElementType::NTH_ROOT:
    case ElementType::DIVISION:
    case ElementType::SUBSCRIPT:
    case ElementType::ASSIGNMENT:
    case ElementType::UNIT:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        if (!store_element(el->elements->Get(0), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(2), undo_element))
            return nullptr;
        break;
    case ElementType::GRAPH_LINE:
        undo_element.reset(new UndoGraphLine(((GraphLine*)el.get())));
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            if (!store_element(el->elements->Get(i), undo_element))
                return nullptr;
        }
        break;
    case ElementType::EQUATION:
        {
            Equation* _el((Equation*)el.get());
            undo_element.reset(new UndoEquation(_el));
            if (!store_element(el->elements->Get(0), undo_element))
                return nullptr;
        }
        break;
    case ElementType::SUM:
    case ElementType::PRODUCT:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        if (!store_element(el->elements->Get(0), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(2), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(3), undo_element))
            return nullptr;
        break;
    case ElementType::DEFINITE_INTEGRAL:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        if (!store_element(el->elements->Get(0), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(2), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(3), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(5), undo_element))
            return nullptr;
        break;
    case ElementType::INDEFINITE_INTEGRAL:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        if (!store_element(el->elements->Get(1), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(3), undo_element))
            return nullptr;
        break;
    case ElementType::EVALUATION_BAR_SUBSCRIPT:
        undo_element.reset(new UndoFormula(el->type, ((Formula*)el.get())->formula_format));
        if (!store_element(el->elements->Get(0), undo_element))
            return nullptr;
        if (!store_element(el->elements->Get(1), undo_element))
            return nullptr;
        break;
    case ElementType::REAL_RESULT:
    case ElementType::INTEGER_RESULT:
    case ElementType::RATIONAL_RESULT:
    case ElementType::COMPLEX_RESULT:
    case ElementType::ARRAY_REAL_RESULT:
    case ElementType::AUTO_RESULT:
    case ElementType::ERROR_RESULT:
        undo_element.reset(new UndoResult(el->type));
        if (!store_element(el->elements->Get(0), undo_element))
            return nullptr;
        break;
    default:
        return nullptr;
    }

    if (!undo_element)
        return nullptr;
    
    UndoItem item{id, 1, std::vector{undo_element}};
    undo_store.emplace_back(item);
    return undo_element;
}

}
