/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __TASK_H__
#define __TASK_H__

#include <vector>
#include <memory>
#include <sstream>
#include "caret_state.h"
#include "selection.h"
#include "editor_state.h"
#include "caret.h"
#include "solver.h"

#ifdef DELETE
#undef DELETE
#endif

namespace yutovo
{

class Logger;

struct Task
{
    Task(ElementPtr _text);
    Task(ElementPtr _text, const uint _id);
    virtual ~Task() = default;

    virtual bool Execute() = 0;

    void Remake(ElementId _id, bool move_into_view);

    ElementPtr text;
    Document* document;
    Window* window;
    bool undo = false; //this is an undo task
    bool with_undo = false; //this task has (will have) undo
    bool next_task = false; //execute next task for undo/redo

    Logger* logger;

    static uint next_id;
    uint id; //for syncing with undo/redo
};

typedef std::shared_ptr<Task> TaskPtr;

struct InsertElementsTask : Task
{
    InsertElementsTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo, bool _pasting, bool _replace);

    virtual bool Execute();

    ElementId element_id; //insert into this element or use id from caret state
    std::vector<ElementPtr> elements;
    LogicalEditorState before_state;
    bool pasting = false; //this is pasting from clipboard
    bool insert_mode = true; //current insert mode
    bool replace = false; //replace current element
};

struct DeleteElementsTask : Task
{
    DeleteElementsTask(ElementPtr _text, bool _left, bool _with_undo);
    DeleteElementsTask(ElementPtr _text, ElementId _element_id, bool _with_undo);

    virtual bool Execute();

    ElementId element_id; //delete elements from this element or use id from caret state
    bool left; //delete on the left or on the right
    LogicalEditorState before_state;
};

struct InsertFormulasTask : Task
{
    InsertFormulasTask(ElementPtr _text, std::vector<ElementPtr>& _elements, bool _with_undo, bool _pasting, bool _replace, int _select_pos = -1);
    InsertFormulasTask(ElementPtr _text, uint _id, std::vector<ElementPtr>& _elements, bool _with_undo);

    virtual bool Execute();

    std::vector<ElementPtr> elements;
    LogicalEditorState before_state;
    bool pasting = false; //this is pasting from clipboard
    int select_pos = -1; //insert selected elements at this position
    bool insert_mode = true; //current insert mode
    bool replace = false; //replace current element
};

struct ChangeStringFormatTask : Task
{
    ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, bool _with_undo);
    ChangeStringFormatTask(ElementPtr _text, const StringFormatPtr& _format, bool _set_family, bool _set_size, bool _set_bold, bool _set_italic, 
        bool _set_strikethrough, bool _set_underline, bool _set_subscript, bool _set_superscript, bool _set_text_color, bool _set_text_bg_color, 
        bool _with_undo);

    virtual bool Execute();

    StringFormatPtr format;

    bool set_family = true;
    bool set_size = true;
    bool set_bold = true;
    bool set_italic = true;
    bool set_underline = true;
    bool set_strikethrough = true;
    bool set_subscript = true;
    bool set_superscript = true;
    bool set_text_color = true;
    bool set_text_bg_color = true;

    LogicalEditorState before_state;
};

struct ChangeParagraphFormatTask : Task
{
    ChangeParagraphFormatTask(ElementPtr _text, ElementId _element_id, const ParagraphFormatPtr& _format, bool _with_undo);

    virtual bool Execute();

    ElementId element_id;
    ParagraphFormatPtr format;

    LogicalEditorState before_state;
};

struct MovePictureTask : Task
{
    MovePictureTask(ElementPtr _text, ElementId _id, const int _dx, const int _dy);

    virtual bool Execute();

    ElementId id;
    const int dx;
    const int dy;
};

struct ZoomPictureTask : Task
{
    ZoomPictureTask(ElementPtr _text, ElementId _id, const int _pixels);

    virtual bool Execute();

    ElementId id;
    const int pixels;
};

struct ResizeElementTask : Task
{
    ResizeElementTask(ElementPtr _text, ElementId _id, const int _dx, const int _dy);

    virtual bool Execute();

    ElementId id;
    const int dx;
    const int dy;
};

struct RedrawTask : Task
{
    RedrawTask(ElementPtr _text, const ElementId& _id, bool _move_into_view);

    virtual bool Execute();

    ElementId element_id;
    bool move_into_view;
};

struct ResizeTask : Task
{
    ResizeTask(ElementPtr _text, const uint _width, const uint _height);

    virtual bool Execute();

    uint width;
    uint height;
};

typedef std::function<void (const ElementId)> CallFuncPtr;

struct UndoTask : Task
{
    enum class UndoOperation
    {
        CHANGE = 1,
        INSERT,
        DELETE,
        CONFIG,
        FORMAT
    };

    UndoTask(ElementPtr _text, int _undo_id, ElementId _id, const int _pos, const int _delete_size, const uint task_id);
    UndoTask(ElementPtr _text, int _undo_id, ElementId _id, const int _pos, const int _size, const int _delete_size, 
        UndoOperation _undo_operation, const uint task_id);
    UndoTask(ElementPtr _text, int _undo_id, const uint task_id);
    UndoTask(ElementPtr _text, int _undo_id, const uint task_id, UndoOperation _undo_operation);

    virtual bool Execute();

    int undo_id;
    LogicalId id;
    int delete_size = 0;
    LogicalEditorState before_state;
    UndoOperation undo_operation = UndoOperation::CHANGE;
    int pos = 0;
    int size = 0;
};

struct MoveCaretTask : Task
{
    enum class MoveCaretDir
    {
        NONE = 0,
        POINT,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        HOME,
        END,
        WORD_LEFT,
        WORD_RIGHT,
        PAGE_UP,
        PAGE_DOWN,
        DOCUMENT_BEGIN,
        DOCUMENT_END,
        SELECT_ALL,
        SELECT_TO_POINT,
        SELECT_OUT,
        CLICK_LINK
    };

    MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible, bool _select);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, MoveCaretDir _dir, bool _visible, bool _select, uint _task_id);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, Point _point, MoveCaretDir _dir);
    MoveCaretTask(ElementPtr _text, CaretPtr _caret, Point _start, Point _end);

    virtual bool Execute();

    Document* document;
    CaretPtr caret;
    MoveCaretDir dir = MoveCaretDir::NONE;
    Point point{-1, -1};
    Point end_point{-1, -1};
    bool visible = false;
    bool select = false;
    bool move_into_view = true;
};

struct NewTask : Task
{
    NewTask(ElementPtr _text);

    virtual bool Execute();
};

class Text;
class Page;
class Paragraph;
class Row;
class String;
class StringElements;

struct SaveTask : Task
{
    SaveTask(ElementPtr _text, const std::string _filename);
    SaveTask(ElementPtr _text, std::string* _json_str, const int _document_id, const bool _gzip);

    virtual bool Execute();

    bool CompressGzip(const std::string& input, std::string& output);

    std::string filename;
    std::string* json_str = nullptr;
    const int document_id = 0;
    const bool gzip = false;
};

struct LoadTask : Task
{
    LoadTask(ElementPtr _text, const std::string& _filename, const bool _include);
    LoadTask(ElementPtr _text, const std::string& _json_str, const int _document_id, const bool _include);

    virtual bool Execute();

    bool LoadJson(rapidjson::Document& doc);
    bool CheckIncludeDocument(const std::string& guid);
    bool DecompressGzip(std::istream& in, std::string& out);

    std::string filename;
    std::string json_str;
    const int document_id = 0;
    bool include = false;
};

struct CopyTask : Task
{
    CopyTask(ElementPtr _text, std::u32string& _out_json, std::u32string& _out_text, bool _cut);

    virtual bool Execute();

    std::u32string& out_json;
    std::u32string& out_text;
    bool cut;
};

struct ResultTask : Task
{
    ResultTask(ElementPtr _text, LogicalId _id, Result _result);

    virtual bool Execute();

    LogicalId id;
    Result result;
};

struct ResolveTask : Task
{
    ResolveTask(ElementPtr _text, ElementId _id);

    virtual bool Execute();

    ElementId id;
};

struct ResolveDependenciesTask : Task
{
    ResolveDependenciesTask(ElementPtr _text, LogicalId _after_id, const std::string& _identifier);

    virtual bool Execute();

    LogicalId after_id;
    std::string identifier;
};

struct ResolveErrorsTask : Task
{
    ResolveErrorsTask(ElementPtr _text);

    virtual bool Execute();
};

struct SetResultTypeTask : Task
{
    SetResultTypeTask(ElementPtr _text, ElementId _id, ResultType _result_type, bool _with_undo);

    virtual bool Execute();

    ElementId id;
    ResultType result_type;
    EditorState before_state;
};

struct SetResultParamsTask : Task
{
    SetResultParamsTask(ElementPtr _text, ElementId _id, Notation _default_notation, Notation _result_notation, bool _with_undo);
    SetResultParamsTask(ElementPtr _text, ElementId _id, FractionForm _fraction_form, bool _with_undo);
    SetResultParamsTask(ElementPtr _text, ElementId _id, uint _precision, uint _exp, AngleMeasure _default_angle_measure, 
        AngleMeasure _result_angle_measure, bool _with_undo);
    SetResultParamsTask(ElementPtr _text, ElementId _id, ComplexForm _complex_form, bool _with_undo);
    SetResultParamsTask(ElementPtr _text, ElementId _id, yutovo_calculator::Unit _unit, bool _with_undo);

    virtual bool Execute();

    ElementId id;

    int precision = -1;
    int exp = -1;
    AngleMeasure default_angle_measure = AngleMeasure::None;
    AngleMeasure result_angle_measure = AngleMeasure::None;
    Notation default_notation = Notation::None;
    Notation result_notation = Notation::None;
    FractionForm fraction_form = FractionForm::None;
    ComplexForm complex_form = ComplexForm::None;
    yutovo_calculator::Unit unit;

    EditorState before_state;
};

struct SetStringTask : Task
{
    SetStringTask(ElementPtr _text, const std::u32string& _str, ElementId _element_id);

    virtual bool Execute();

    std::u32string str;
    ElementId element_id;
};

struct SetConfigTask : Task
{
    SetConfigTask(ElementPtr _text, const Config& _config, bool _with_undo);
    SetConfigTask(ElementPtr _text, const std::string& _config_str, bool _with_undo);

    virtual bool Execute();

    Config config;
    std::string config_str;
};

struct SetFormatTask : Task
{
    SetFormatTask(ElementPtr _text, const ElementId& _id, std::function<bool()> _func, bool _with_undo);

    virtual bool Execute();

    const ElementId id;
    std::function<bool()> func;
};

}

#endif
