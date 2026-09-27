/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __EDITOR_UTILS_H__
#define __EDITOR_UTILS_H__

#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <boost/locale.hpp>
#include <yutovo-calculator/parser_exception.h>
#include <yutovo-calculator/math_helper.h>
#include <yutovo-solver/types.h>
#ifdef min
#undef min
#endif
#include <boost/uuid/uuid.hpp>
#include <rapidjson/document.h>

typedef unsigned int uint;

namespace yutovo
{

using namespace yutovo_calculator;

class Window;

typedef std::shared_ptr<Window> WindowPtr;

enum class ElementType
{
    NONE = 0,
    TEXT,
    PARAGRAPH,
    ROW,
    STRING,
    CODE_BLOCK,
    CODE_PARAGRAPH,
    CODE_ROW,
    CODE_STRING,
    CODE_COLUMN,
    SHAPE,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVISION,
    POWER,
    SQUARE_ROOT,
    NTH_ROOT,
    EQUATION,
    OPEN_ROUND_BRACKET,
    CLOSE_ROUND_BRACKET,
    REAL_RESULT,
    INTEGER_RESULT,
    RATIONAL_RESULT,
    COMPLEX_RESULT,
    AUTO_RESULT,
    ERROR_RESULT,
    ASSIGNMENT,
    SUBSCRIPT,
    EXCLAMATION,
    AND,
    OR,
    XOR,
    PERCENT,
    IMAGE,
    SUM,
    PRODUCT,
    UNIT,
    COMMA,
    LINK,
    OPEN_SQUARE_BRACKET,
    CLOSE_SQUARE_BRACKET,
    ARRAY_REAL_RESULT,
    GRAPH_LINE,
    CODE_PARAGRAPHS_BLOCK,
    SYMBOLIC_REAL_RESULT,
    SYMBOLIC_RATIONAL_RESULT,
    SYMBOLIC_COMPLEX_RESULT,
    NOT,
    DEFINITE_INTEGRAL,
    INDEFINITE_INTEGRAL,
    CODE_ROW_ASSIGNMENT,
    CODE_PARAGRAPH_ASSIGNMENT,
    CODE_PARAGRAPHS_BLOCK_ASSIGNMENT,
    EVALUATION_BAR_SUBSCRIPT
};

enum class IdentifierType
{
    NONE = 0,
    VARIABLE,
    FUNCTION,
    UNIT,
    STRING,
    OPERATION
};

enum class MouseHoldType
{
    NONE = 0,
    RESIZING,
    MOVING,
    PLOT_FORMAT_DIALOG
};

class Document;
class Element;

using ElementId = yutovo_calculator::ElementId;
using LogicalId = yutovo_calculator::LogicalId;

struct Point
{
    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return !operator==(other);
    }

    void Set(const int _x, const int _y)
    {
        x = _x;
        y = _y;
    }

    bool IsNull() const
    {
        return x == 0 && y == 0;
    }
    
    int x = 0;
    int y = 0;
};

struct Size
{
    Size() = default;

    Size(const int _width, const int _height) : 
        width(_width),
        height(_height)
    {
    }

    Size(const Size& source, const float scale) : 
        width(std::round(source.width * scale)),
        height(std::round(source.height * scale))
    {
    }

    bool operator==(const Size& other) const
    {
        return width == other.width && height == other.height;
    }

    bool operator!=(const Size& other) const
    {
        return !operator==(other);
    }

    void Set(const int _width, const int _height)
    {
        width = _width;
        height = _height;
    }

    void Set(const Size& source, const float scale)
    {
        width = std::round(source.width * scale);
        height = std::round(source.height * scale);
    }

    int width = 0;
    int height = 0;
};

struct Rect
{
    Rect() = default;
    
    Rect(int _left, int _top, int _width, int _height) :
        left(_left),
        top(_top),
        width(_width),
        height(_height)
    {
    }

    bool operator==(const Rect& compare) const
    {
        return left == compare.left && top == compare.top && width == compare.width && height == compare.height;
    }

    bool operator!=(const Rect& compare) const
    {
        return !operator==(compare);
    }

    Point GetTopLeft()
    {
        return Point{left, top};
    }

    int GetRight() const
    {
        return left + width;
    }

    int GetBottom() const
    {
        return top + height;
    }

    void SetRect(int _left, int _top, int _right, int _bottom)
    {
        left = _left;
        top = _top;
        width = _right - _left;
        height = _bottom - _top;
    }

    void Move(int _left, int _top)
    {
        left = _left;
        top = _top;
    }

    void SetSize(int _width, int _height)
    {
        width = _width;
        height = _height;
    }

    void Reset()
    {
        left = 0;
        top = 0;
        width = 0;
        height = 0;
    }

    bool IsEmpty()
    {
        return width == 0 && height == 0;
    }
    
    int DistToPoint(const int x, const int y);
    int DistToSegment(const int x, const int y, const int seg_x1, const int seg_y1, const int seg_x2, const int seg_y2);
    bool IsPointInside(const int x, const int y);
    bool Intersects(const Rect& rect);

    int left = 0;
    int top = 0;
    int width = 0;
    int height = 0;
};

struct Color
{
    bool operator==(const Color& compare) const
    {
        return a == compare.a && r == compare.r && g == compare.g && b == compare.b;
    }

    bool operator!=(const Color& compare) const
    {
        return !(*this == compare);
    }

    uint32_t ToInt() const
    {
        return (a << 24) + (r << 16) + (g << 8) + b;
    }

    std::string ToString() const
    {
        return "rgba(" + std::to_string(r) + "," + std::to_string(g) + "," + std::to_string(b) + "," + std::to_string(a) + ")";
    }

    std::string ToHex() const
    {
        std::stringstream s;
        s << "#";
        s << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)r << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)g << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)b;
        return s.str();
    }

    std::string ToHexBGR() const
    {
        std::stringstream s;
        s << "#";
        s << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)b << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)g << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)r;
        return s.str();
    }

    std::string ToBGR() const
    {
        std::stringstream s;
        s << "x";
        s << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)b << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)g << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)r;
        return s.str();
    }

    std::string ToRGB() const
    {
        std::stringstream s;
        s << "x";
        s << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)r << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)g << 
            std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << (int)b;
        return s.str();
    }

    static Color FromInt(uint32_t c)
    {
        uint8_t a = (c & 0xff000000) >> 24;
        uint8_t r = (c & 0x00ff0000) >> 16;
        uint8_t g = (c & 0x0000ff00) >> 8;
        uint8_t b = (c & 0x000000ff);
        return Color{a, r, g, b};
    }

    static Color FromHex(std::string c)
    {
        if ((c.size() != 9 && c.size() != 7) || c[0] != '#')
            return Color{};
        c.erase(0, 1);
        try
        {
            const unsigned long val = stoul(c, nullptr, 16);
            if (c.size() == 6)
                return Color{0xff, uint8_t((val >> 16) & 0xff), uint8_t((val >> 8) & 0xff), uint8_t((val) & 0xff)};
            return Color{uint8_t((val >> 24) & 0xff), uint8_t((val >> 16) & 0xff), uint8_t((val >> 8) & 0xff), uint8_t((val) & 0xff)};
        }
        catch (const std::exception&)
        {
            return Color{};
        }
    }

    static Color Red()
    {
        return Color{0xff, 0xff, 0, 0};
    }

    static Color Green()
    {
        return Color{0xff, 0, 0xff, 0};
    }

    static Color Blue()
    {
        return Color{0xff, 0, 0, 0xff};
    }

    static Color Yellow()
    {
        return Color{0xff, 0xff, 0xff, 0x0};
    }

    static Color Magenta()
    {
        return Color{0xff, 0xff, 0x0, 0xff};
    }

    static Color Cian()
    {
        return Color{0xff, 0x0, 0xff, 0xff};
    }

    static Color Black()
    {
        return Color{0xff, 0x0, 0x0, 0x0};
    }

    static Color White()
    {
        return Color{0xff, 0xff, 0xff, 0xff};
    }

    uint8_t a = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};

struct FormattingScope
{
    FormattingScope(Window* _window);
    ~FormattingScope();

    Window* window;
};

struct Identifiers
{
    std::vector<std::string> variables;
    std::vector<std::string> functions;
    std::vector<std::string> units;
    std::vector<std::string> operations;
    std::vector<std::string> strings;
};

bool IsChild(const ElementId& parent_id, const ElementId& child_id);
bool IsChild(const LogicalId& parent_id, const LogicalId& child_id);
bool IsDirectChild(const ElementId& parent_id, const ElementId& child_id);

ElementId GetParent(const ElementId& id);
LogicalId GetParent(const LogicalId& id);
ElementId GetChild(const ElementId& id, uint pos);
LogicalId GetChild(const LogicalId& id, uint pos);
int GetChildPos(const ElementId& id);
int GetChildPos(const LogicalId& id);
int GetChildPos(const ElementId& parent_id, const ElementId& child_id);
ElementId GetPrevPos(const ElementId& id);

ElementId GetWithParent(const ElementId& id, const ElementId& parent_id);
LogicalId GetWithParent(const LogicalId& id, const LogicalId& parent_id);

ElementId GetCommonParent(const ElementId& id1, const ElementId& id2);
ElementId GetCommonParent(const std::vector<ElementId>& ids);

Element* CreateFromJson(Element* parent, Document* document, const rapidjson::Value::ConstObject& value, rapidjson::Document::AllocatorType& alloc);

std::string ErrorCodeToString(const yutovo_solver::ErrorCode error_code);
std::string ErrorCodeToString(const yutovo_calculator::ParserExceptionCode parser_error_code);

std::string AngleMeasureToString(const AngleMeasure angle_measure);
std::string NotationToString(const Notation notation);
std::string ResultTypeToString(const yutovo_solver::ResultType result_type);

struct ErrorMark
{
    LogicalId id;
    int start = 0;
    int size = 0;
};

bool IsLess(const ElementId& id1, const ElementId& id2);

void FlipImageVertically(std::vector<unsigned char>& rgba, const int width, const int height);

bool IsPng(const std::vector<unsigned char>& image);

void RgbaToPng(const std::vector<unsigned char>& rgba, const int width, const int height, std::vector<unsigned char>& png);
bool PngToRgba(const std::vector<unsigned char>& png, std::vector<unsigned char>& rgba, int& width, int& height);

extern std::string base64_chars;

std::string Base64Encode(const std::vector<unsigned char>& picture);
std::vector<unsigned char> Base64Decode(const std::string& base64);

#define LOG_TRACE(...) \
    if (logger->GetLevel() <= LogLevel::LEVEL_TRACE) \
        logger->Trace(__VA_ARGS__);

#define LOG_DEBUG(...) \
    if (logger->GetLevel() <= LogLevel::LEVEL_DEBUG) \
        logger->Debug(__VA_ARGS__);

#define LOG_INFO(...) \
    if (logger->GetLevel() <= LogLevel::LEVEL_INFO) \
        logger->Info(__VA_ARGS__);

#define LOG_WARNING(...) \
    if (logger->GetLevel() <= LogLevel::LEVEL_WARNING) \
        logger->Warning(__VA_ARGS__);

#define LOG_ERROR(...) \
    if (logger->GetLevel() <= LogLevel::LEVEL_ERROR) \
        logger->Error(__VA_ARGS__);

#define LOG_CRITICAL(...) \
    if (logger->GetLevel() <= LogLevel::LEVEL_CRITICAL) \
        logger->Critical(__VA_ARGS__);
}

#endif
