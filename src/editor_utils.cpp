/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "editor_utils.h"
#include <limits>
#include <cmath>
#include "row.h"
#include "str.h"
#include "image.h"
#include "paragraph.h"
#include "link.h"
#include "element.h"
#include "formulas/code_block.h"
#include "formulas/code_paragraph.h"
#include "formulas/code_paragraphs_block.h"
#include "formulas/code_row.h"
#include "formulas/code_string.h"
#include "formulas/plus.h"
#include "formulas/minus.h"
#include "formulas/multiply.h"
#include "formulas/division.h"
#include "formulas/nth_root.h"
#include "formulas/square_root.h"
#include "formulas/result.h"
#include "formulas/power.h"
#include "formulas/equation.h"
#include "formulas/brackets.h"
#include "formulas/assignment.h"
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
#include "formulas/unit.h"
#include "formulas/comma.h"
#include "formulas/graph.h"
#include "formulas/evaluation_bar.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

namespace yutovo
{

using namespace yutovo_solver;

//Rect

int Rect::DistToPoint(const int x, const int y)
{
	int min_dist = std::numeric_limits<int>::max(), dist;
	
	//calculate distances to each side
	min_dist = DistToSegment(x, y, left, top, left, GetBottom());
	
	dist = DistToSegment(x, y, left, top, GetRight(), top);
	if (min_dist > dist)
		min_dist = dist;

	dist = DistToSegment(x, y, GetRight(), top, GetRight(), GetBottom());
	if (min_dist > dist)
		min_dist = dist;

	dist = DistToSegment(x, y, left, GetBottom(), GetRight(), GetBottom());
	if (min_dist > dist)
		min_dist = dist;
	
	return min_dist;
}

int Rect::DistToSegment(const int x, const int y, const int seg_x1, const int seg_y1, const int seg_x2, const int seg_y2)
{
	if (seg_x1 == seg_x2 && seg_y1 == seg_y2)
		return round(sqrt(pow(x - seg_x1, 2) + pow(y - seg_y1, 2)));

	double d1 = sqrt(pow(x - seg_x1, 2) + pow(y - seg_y1, 2));
	double d2 = sqrt(pow(x - seg_x2, 2) + pow(y - seg_y2, 2));
	double r = sqrt(pow(seg_x1 - seg_x2, 2) + pow(seg_y1 - seg_y2, 2));
	double p = (d1 + d2 + r) / 2;

	if (p - d1 < 0)
		d1 = p;
	if (p - d2 < 0)
		d2 = p;

	double h = 2 * sqrt(p * (p - r) * (p - d1) * (p - d2)) / r;
	double s = sqrt(pow(d1, 2) - pow(h, 2));

	if (s > r)
		return round(d2);

	s = sqrt(pow(d2, 2) - pow(h, 2));

	if (s > r)
		return round(d1);

	return round(h);
}

bool Rect::IsPointInside(const int x, const int y)
{
    return x >= left && x <= left + width && y >= top && y <= top + height;
}

bool Rect::Intersects(const Rect& rect)
{
    return !(left > rect.GetRight() || rect.left > GetRight() || top > rect.GetBottom() || rect.top > GetBottom());
}

//FormattingScope

FormattingScope::FormattingScope(Window* _window) :
    window(_window)
{
    window->OnFormattingStarted();
}

FormattingScope::~FormattingScope()
{
    window->OnFormattingFinished();
}

ElementId IdFromString(const std::string& id)
{
    ElementId res;
    std::stringstream s(id);
    for (int i; s >> i;)
    {
        res.push_back(i);
        if (s.peek() == ',')
            s.ignore();
    }
    return res;
}

LogicalId LogicalIdFromString(const std::string& id)
{
    LogicalId res;
    std::stringstream s(id);
    for (int i; s >> i;)
    {
        res.push_back(i);
        if (s.peek() == ',')
            s.ignore();
    }
    return res;
}

bool IsChild(const ElementId& parent_id, const ElementId& child_id)
{
    if (child_id.size() <= parent_id.size() || parent_id.empty())
        return false;
    for (size_t i = 0; i < parent_id.size(); ++i)
    {
        if (child_id[i] != parent_id[i])
            return false;
    }
    return true;
}

bool IsChild(const LogicalId& parent_id, const LogicalId& child_id)
{
    if (child_id.size() <= parent_id.size() || parent_id.empty())
        return false;
    for (size_t i = 0; i < parent_id.size(); ++i)
    {
        if (child_id[i] != parent_id[i])
            return false;
    }
    return true;
}

bool IsDirectChild(const ElementId& parent_id, const ElementId& child_id)
{
    if (IsChild(parent_id, child_id) && child_id.size() == parent_id.size() + 1)
        return true;
    return false;
}

ElementId GetParent(const ElementId& id)
{
    ElementId _id(id);
    _id.erase(_id.end() - 1);
    return _id;
}

LogicalId GetParent(const LogicalId& id)
{
    LogicalId _id(id);
    _id.erase(_id.end() - 1);
    return _id;
}

ElementId GetChild(const ElementId& id, uint pos)
{
    ElementId _id(id);
    _id.push_back(pos);
    return _id;
}

LogicalId GetChild(const LogicalId& id, uint pos)
{
    LogicalId _id(id);
    _id.push_back(pos);
    return _id;
}

int GetChildPos(const ElementId& id)
{
    assert(!id.empty());
    return id[id.size() - 1];
}

int GetChildPos(const LogicalId& id)
{
    assert(!id.empty());
    return id[id.size() - 1];
}

int GetChildPos(const ElementId& parent_id, const ElementId& child_id)
{
    assert(parent_id.size() < child_id.size());
    return child_id[parent_id.size()];
}

ElementId GetPrevPos(const ElementId& id)
{
    ElementId _id(id);
    --_id[_id.size() - 1];
    return _id;
}

ElementId GetWithParent(const ElementId& id, const ElementId& parent_id)
{
    ElementId _id(parent_id);
    for (int i = _id.size(); i < id.size(); ++i)
        _id.push_back(id[i]);
    return _id;
}

LogicalId GetWithParent(const LogicalId& id, const LogicalId& parent_id)
{
    LogicalId _id(parent_id);
    for (int i = _id.size(); i < id.size(); ++i)
        _id.push_back(id[i]);
    return _id;
}

ElementId GetCommonParent(const ElementId& id1, const ElementId& id2)
{
    int i = 0;
    while (id1.size() > i && id2.size() > i && id1[i] == id2[i])
        ++i;
    //return ElementId(id1.begin(), id1.begin() + i);
    return ElementId(id1, i);
}

ElementId GetCommonParent(const std::vector<ElementId>& ids)
{
    if (ids.empty())
        return {};

    int pos = 0;
    bool f = true;
    uint j = 0;
    while (f)
    {
        for (int i = 0; i < ids.size(); ++i)
        {
            const ElementId& _id = ids[i];
            if (_id.size() <= pos || _id[pos] != j)
            {
                f = false;
                break;
            }
        }
        if (!f || ids[0].size() <= ++pos)
            break;
        j = ids[0][pos];
    }

    return ElementId(ids[0], pos);
}

Element* CreateFromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc)
{
    using CreateFunc = Element*(*)(Element*, Document*, const rapidjson::Value::ConstObject&, rapidjson::Document::AllocatorType&);
    static std::map<ElementType, CreateFunc> create_elements = 
        {
            {ElementType::TEXT, &Text::FromJson},
            {ElementType::PARAGRAPH, &Paragraph::FromJson},
            {ElementType::ROW, &Row::FromJson},
            {ElementType::STRING, &String::FromJson},
            {ElementType::IMAGE, &Image::FromJson},
            {ElementType::CODE_BLOCK, &CodeBlock::FromJson},
            {ElementType::CODE_PARAGRAPH, &CodeParagraph<>::FromJson},
            {ElementType::CODE_ROW, &CodeRow<>::FromJson},
            {ElementType::CODE_STRING, &CodeString::FromJson},
            {ElementType::SHAPE, &Shape::FromJson},
            {ElementType::PLUS, &Plus::FromJson},
            {ElementType::MINUS, &Minus::FromJson},
            {ElementType::MULTIPLY, &Multiply::FromJson},
            {ElementType::DIVISION, &Division::FromJson},
            {ElementType::POWER, &Power::FromJson},
            {ElementType::SQUARE_ROOT, &SquareRoot::FromJson},
            {ElementType::NTH_ROOT, &NthRoot::FromJson},
            {ElementType::EQUATION, &Equation::FromJson},
            {ElementType::OPEN_ROUND_BRACKET, &OpenBracket::FromJson},
            {ElementType::CLOSE_ROUND_BRACKET, &CloseBracket::FromJson},
            {ElementType::OPEN_SQUARE_BRACKET, &OpenBracket::FromJson},
            {ElementType::CLOSE_SQUARE_BRACKET, &CloseBracket::FromJson},
            {ElementType::REAL_RESULT, &RealResult::FromJson},
            {ElementType::INTEGER_RESULT, &IntegerResult::FromJson},
            {ElementType::RATIONAL_RESULT, &RationalResult::FromJson},
            {ElementType::COMPLEX_RESULT, &ComplexResult::FromJson},
            {ElementType::AUTO_RESULT, &AutoResult::FromJson},
            {ElementType::ARRAY_REAL_RESULT, &ArrayRealResult::FromJson},
            {ElementType::ERROR_RESULT, &ErrorResult::FromJson},
            {ElementType::ASSIGNMENT, &Assignment::FromJson},
            {ElementType::SUBSCRIPT, &Subscript::FromJson},
            {ElementType::EXCLAMATION, &Exclamation::FromJson},
            {ElementType::AND, &And::FromJson},
            {ElementType::OR, &Or::FromJson},
            {ElementType::XOR, &Xor::FromJson},
            {ElementType::NOT, &Not::FromJson},
            {ElementType::PERCENT, &Percent::FromJson},
            {ElementType::IMAGE, &Image::FromJson},
            {ElementType::SUM, &Sum::FromJson},
            {ElementType::PRODUCT, &Product::FromJson},
            {ElementType::DEFINITE_INTEGRAL, &DefiniteIntegral::FromJson},
            {ElementType::INDEFINITE_INTEGRAL, &IndefiniteIntegral::FromJson},
            {ElementType::UNIT, &Unit::FromJson},
            {ElementType::COMMA, &Comma::FromJson},
            {ElementType::LINK, &Link::FromJson},
            {ElementType::GRAPH_LINE, &GraphLine::FromJson},
            {ElementType::CODE_PARAGRAPHS_BLOCK, &CodeParagraphsBlock<>::FromJson},
            {ElementType::CODE_ROW_ASSIGNMENT, &CodeRow<Assignment>::FromJson},
            {ElementType::CODE_PARAGRAPH_ASSIGNMENT, &CodeParagraph<Assignment>::FromJson},
            {ElementType::CODE_PARAGRAPHS_BLOCK_ASSIGNMENT, &CodeParagraphsBlock<Assignment>::FromJson},
            {ElementType::SYMBOLIC_REAL_RESULT, &SymbolicRealResult::FromJson},
            {ElementType::SYMBOLIC_RATIONAL_RESULT, &SymbolicRationalResult::FromJson},
            {ElementType::SYMBOLIC_COMPLEX_RESULT, &SymbolicComplexResult::FromJson},
            {ElementType::EVALUATION_BAR_SUBSCRIPT, &EvaluationBarSubscript::FromJson}
        };

    if (!value.HasMember("type") || !value["type"].IsInt())
        return nullptr;
    
    ElementType type = (ElementType)value["type"].GetInt();
    CreateFunc f = create_elements[type];
    if (!f)
        return nullptr;
    Element* el = f(parent, document, value, alloc);
    if (!el)
        return nullptr;
    if (value.HasMember("level") && value["level"].IsInt())
        el->level = (uint8_t)value["level"].GetInt();
    if (value.HasMember("elements"))
    {
        if (!el->elements->FromJson(document, value, alloc) || !el->AfterFromJson())
        {
            delete el;
            return nullptr;
        }
    }
    return el;
}

std::string ErrorCodeToString(const yutovo_solver::ErrorCode error_code)
{
    static std::map<yutovo_solver::ErrorCode, std::string> error_code_str = 
        {
            {ErrorCode::OK, "Ok"},
            {ErrorCode::UNKNOWN_COMMAND, "Unknown command"},
            {ErrorCode::JSON_ERROR, "Json error"},
            {ErrorCode::NO_FIELD_ERROR, "No field"},
            {ErrorCode::FIELD_ERROR, "Field error"},
            {ErrorCode::SOLVER_ERROR, "Solver error"},
            {ErrorCode::PARSER_ERROR, "Parser error"},
            {ErrorCode::OPERATION_ERROR, "Operation error"},
            {ErrorCode::SOLVER_RESTARTED_ERROR, "Solver restarted"},
            {ErrorCode::TIMEOUT_ERROR, "Solver timeout"},
            {ErrorCode::NO_RESULT, "No result"}
        };
    return error_code_str[error_code];
}

std::string ErrorCodeToString(const yutovo_calculator::ParserExceptionCode parser_error_code)
{
    using namespace yutovo_calculator;
    static std::map<ParserExceptionCode, std::string> error_code_str = 
        {
            {ParserExceptionCode::None, "Ok"},
            {ParserExceptionCode::SyntaxError, "Syntax error"},
            {ParserExceptionCode::WrongArgumentsCount, "Wrong arguments count"},
            {ParserExceptionCode::UnknownIdentifier, "Unknown identifier"},
            {ParserExceptionCode::ExpressionExpected, "Expression expected"},
            {ParserExceptionCode::IncorrectIdentifier, "Incorrect identifier"},
            {ParserExceptionCode::DivisionByZero, "Division by zero"},
            {ParserExceptionCode::Overflow, "Overflow"},
            {ParserExceptionCode::ArgumentIsOver, "Argument is over"},
            {ParserExceptionCode::NotImplemented, "Function is not implemented"},
            {ParserExceptionCode::SerializationError, "Data conversion error"},
            {ParserExceptionCode::ConversionDoesNotFit, "Conversion does not fit"},
            {ParserExceptionCode::IncorrectComparison, "Incorrect comparasion"},
            {ParserExceptionCode::IncorrectOperation, "Incorrect operation"},
            {ParserExceptionCode::UnitsAreIncompatible, "Units are incompatible"},
            {ParserExceptionCode::CannotCastToUnit, "Cannot cast to unit"},
            {ParserExceptionCode::UnknownLanguage, "Unknown language"},
            {ParserExceptionCode::TimeExceed, "Solving time exceeded"},
            {ParserExceptionCode::Break, "Solving has interrupted"}
        };
    return error_code_str[parser_error_code];
}

std::string AngleMeasureToString(const AngleMeasure angle_measure)
{
    switch (angle_measure)
    {
    case AngleMeasure::Radian:
        return "rad";
    case AngleMeasure::Degree:
        return "deg";
    case AngleMeasure::Grad:
        return "grad";
    case AngleMeasure::None:
        return "";
    }
    return "";
}

std::string NotationToString(const Notation notation)
{
    switch (notation)
    {
    case Notation::Binary:
        return "bin";
    case Notation::Octal:
        return "oct";
    case Notation::Decimal:
        return "dec";
    case Notation::Hexadecimal:
        return "hex";
    case Notation::None:
        return "";
    }
    return "";
}

std::string ResultTypeToString(const yutovo_solver::ResultType result_type)
{
    switch (result_type)
    {
	case yutovo_solver::ResultType::NONE:
        return "None";
	case yutovo_solver::ResultType::REAL:
        return "Real";
	case yutovo_solver::ResultType::INTEGER:
        return "Integer";
	case yutovo_solver::ResultType::RATIONAL:
        return "Rational";
	case yutovo_solver::ResultType::COMPLEX:
        return "Complex";
	case yutovo_solver::ResultType::ARRAY_REAL:
        return "ArrayReal";
	case yutovo_solver::ResultType::AUTO:
        return "Auto";
	case yutovo_solver::ResultType::SYMBOLIC_REAL:
        return "SymbolicReal";
	case yutovo_solver::ResultType::SYMBOLIC_RATIONAL:
        return "SymbolicRational";
	case yutovo_solver::ResultType::SYMBOLIC_COMPLEX:
        return "SymbolicComplex";
    }
    return "";
}

bool IsLess(const ElementId& id1, const ElementId& id2)
{
	for (size_t i = 0, j = 0; i < id1.size() && j < id2.size(); ++i, ++j)
	{
		if (id1[i] > id2[j])
			return false;
		if (id1[i] < id2[j])
			return true;
	}
	return id1.size() < id2.size();
}

void FlipImageVertically(std::vector<unsigned char>& rgba, const int width, const int height)
{
    unsigned r = width * 4;
    for (unsigned y = 0; y < height / 2; ++y)
    {
        unsigned char* row_top = rgba.data() + y * r;
        unsigned char* row_bottom = rgba.data() + (height - 1 - y) * r;
        for (unsigned x = 0; x < r; ++x)
            std::swap(row_top[x], row_bottom[x]);
    }   
}

bool IsPng(const std::vector<unsigned char>& image)
{
    if (image.size() < 8)
        return false;

    const unsigned char png_signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    for (int i = 0; i < 8; ++i)
    {
        if (image[i] != png_signature[i])
            return false;
    }
    return true;    
}

void RgbaToPng(const std::vector<unsigned char>& rgba, const int width, const int height, std::vector<unsigned char>& png)
{
    stbi_write_png_to_func(
        [](void* context, void* data, int size)
        {
            auto* v = static_cast<std::vector<unsigned char>*>(context);
            v->insert(v->end(), (unsigned char*)data, (unsigned char*)data + size);
        },
        &png, width, height, 4, rgba.data(), width * 4);
}

bool PngToRgba(const std::vector<unsigned char>& png, std::vector<unsigned char>& rgba, int& width, int& height)
{
    rgba.clear();

    int channels;
    unsigned char* pixels = stbi_load_from_memory(png.data(), static_cast<int>(png.size()), &width, &height, &channels, 4);
    if (!pixels)
        return false;

    size_t size = static_cast<size_t>(width) * height * 4;
    rgba.assign(pixels, pixels + size);

    stbi_image_free(pixels);

    return true;    
}

std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string Base64Encode(const std::vector<unsigned char>& picture)
{
    std::string res;
    uint32_t val = 0;
    int valb = -6;
    for (uchar c : picture)
    {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0)
        {
            res.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6)
        res.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (res.size() % 4)
        res.push_back('=');
    return res;
}

std::vector<unsigned char> Base64Decode(const std::string& base64)
{
    std::vector<int> t(256, -1);
    for (int i = 0; i < 64; ++i)
        t[base64_chars[i]] = i;

    std::vector<unsigned char> res;
    uint32_t val = 0;
    int valb = -8;
    for (uchar c : base64)
    {
        if (t[c] == -1)
            break;
        val = (val << 6) + t[c];
        valb += 6;
        if (valb >= 0)
        {
            res.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return res;
}

}
