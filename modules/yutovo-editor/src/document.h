/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <thread>
#include <mutex>
#include <vector>
#include <stack>
#include <memory>
#include <map>
#include <regex>
#include "window.h"
#include "caret.h"
#include "selection.h"
#include "text.h"
#include "task.h"
#include "solver.h"
#include "editor_utils.h"
#include "editor_state.h"
#include "config.h"
#include "undo.h"
#include <yutovo-logger/logger.h>
#include <yutovo-solver/types.h>
#include <yutovo-calculator/math_helper.h>

namespace yutovo
{

class Document
{
public:
    Document(Window* _window, Config& _config, const std::string _document_guid = "");
    Document(Window* _window, Config& _config, const Document& source);
    ~Document();

    void Start();
    void Start(const TextFormat _default_text_format);
    void Stop();

    void GetConfig(Config& _config);
    uint SetConfig(const Config& _config, bool with_undo);
    uint SetConfig(const std::string& _config, bool with_undo);

    uint InsertParagraph(bool with_undo);
    uint InsertString(const std::string& str, bool with_undo);
    uint InsertString(const std::u32string& str, bool with_undo);
    uint InsertString(const std::u32string& str, bool paste, bool with_undo);
    uint InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo);
    uint InsertString(const std::string& str, ElementId element_id, bool with_undo);
    uint InsertString(const std::string& str, bool parse, bool with_undo);
    uint ReplaceString(const std::u32string& str, bool with_undo);

    uint InsertLink(const std::string& str, const std::string& url, bool with_undo);
    uint InsertLink(const std::u32string& str, const std::u32string& url, bool with_undo);

    uint InsertElement(Element* element, bool with_undo, bool pasting = false, bool replace = false);
    uint InsertElements(std::vector<ElementPtr>& elements, bool with_undo, bool pasting = false, bool replace = false);

    uint ReplaceElement(Element* element, bool with_undo);

    uint DeleteElements(bool left, bool with_undo);
    uint ClearElements(ElementId element_id, bool with_undo);

    uint InsertCode(bool next_code_id, bool with_undo);
    uint InsertCodeString(const std::string& str, bool with_undo);
    uint InsertPlus(bool with_undo, bool replace = false);
    uint InsertMinus(bool with_undo, bool replace = false);
    uint InsertMultiply(bool with_undo, bool replace = false);
    uint InsertDivision(bool with_undo, bool replace = false);
    uint InsertPower(bool with_undo, bool replace = false);
    uint InsertNthRoot(bool with_undo, bool replace = false);
    uint InsertSquareRoot(bool with_undo, bool replace = false);
    uint InsertEquation(yutovo_solver::ResultType result_type, bool with_undo);
    uint InsertOpenRoundBracket(bool with_undo);
    uint InsertCloseRoundBracket(bool with_undo);
    uint InsertOpenSquareBracket(bool with_undo);
    uint InsertCloseSquareBracket(bool with_undo);
    uint InsertAssignment(bool with_undo);
    uint InsertUnit(bool with_undo);
    uint InsertSubscript(bool with_undo, bool replace = false);
    uint InsertExclamation(bool with_undo);
    uint InsertAnd(bool with_undo);
    uint InsertOr(bool with_undo);
    uint InsertXor(bool with_undo);
    uint InsertNot(bool with_undo);
    uint InsertPercent(bool with_undo);
    uint InsertSum(bool with_undo, bool replace = false);
    uint InsertProduct(bool with_undo, bool replace = false);
    uint InsertDefiniteIntegral(bool with_undo, bool replace = false);
    uint InsertIndefiniteIntegral(bool with_undo, bool replace = false);
    uint InsertDerivative(bool with_undo, bool replace = false);
    uint InsertDerivative(const std::u32string& symbol, const int order, bool with_undo, bool replace = false);
    uint InsertEvaluationBarSubscript(bool with_undo, bool replace = false);
    uint InsertDerivativeAtPoint(bool with_undo, bool replace = false);
    uint InsertImage(const std::string& image_base64, bool with_undo, bool pasting);
    uint InsertImage(const std::vector<unsigned char>& image, bool with_undo, bool pasting);
    uint InsertComma(bool with_undo);

    uint InsertRoundBrackets(bool with_undo);
    uint InsertSquareBrackets(bool with_undo);
    
    uint InsertFunction(const std::string& name, bool with_undo);
    uint InsertSubscriptFunction(const std::string& name, bool with_undo);

    uint InsertGraph(bool with_undo);

    uint InsertFormula(Element* element, bool with_undo, bool with_last_task_id = false, bool replace = false);
    uint InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool with_last_task_id, bool pasting, bool replace, int select_pos);

    uint InsertUnit(const yutovo_calculator::Unit& unit, bool list_identifiers = true);

    uint ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, const bool strikethrough, 
        const bool subscript, const bool superscript, Color text_color, Color text_bg_color, bool with_undo);
    uint ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
        bool set_strikethrough, bool set_subscript, bool set_superscript, bool set_text_color, bool set_text_bg_color, bool with_undo);
    uint ChangeStringFormat(const StringFormatPtr format, bool with_undo);

    uint ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo);
    uint ChangeParagraphFormat(const std::string name, bool with_undo);
    uint ChangeParagraphFormat(const ParagraphFormat::Alignment alignment, bool with_undo);

    bool StoreUndo(const ElementId& _id);
    bool StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size = 0);
    bool StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size, UndoTask::UndoOperation undo_operation);
    bool StoreUndo(const Config& config);
    bool StoreUndo(const TextFormat& format);
    bool RestoreUndo(const int undo_id, std::vector<ElementPtr>& elements);
    bool RestoreUndo(const int undo_id, Config& config);
    bool RestoreUndo(const int undo_id, TextFormat& format);
    void RollbackUndo();
    size_t GetUndoSize();

    void ResetTasks();

    uint SetIncludeDocuments(const std::vector<std::string>& files);

    ElementPtr GetElement(const ElementId& _id);
    ElementPtr GetLogicalElement(const LogicalId& _id);
    void GetElements(const LogicalId& _id, std::vector<ElementPtr>& elements);
    ElementPtr GetParent(const ElementId& _id);
    ElementPtr GetLogicalParent(const LogicalId& _id);
    bool GetElementAtCoords(const int x, const int y, const int margin, ElementId& id);
    bool GetElementRect(const ElementId id, Rect& rect);
    bool GetCaretRect(Rect& rect);

    LogicalId GetLogicalId(const ElementId& _id);
    LogicalId GetLogicalId(const ElementId& _id, const int pos);
    ElementId GetElementId(const LogicalId& _id, bool& last_pos);
    ElementId GetElementId(const LogicalId& _id, const int pos, bool& last_pos);
    ElementId GetParentId(const ElementId& id, const ElementType type);

    ElementPtr FindElementOrParent(const ElementId& id, const ElementType type);
    ElementPtr FindParent(const ElementId& id, const ElementType type);
    ElementId FindCurrentParentByType(const ElementType type);
    ElementPtr FindParentParagraph(const ElementId& id);
    ElementPtr FindParentRow(const ElementId& id);
    uint FindCodeBlock(const ElementId& id);
    ElementPtr FindByString(const ElementId& start_id, const std::u32string& str);
    ElementPtr FindByType(const ElementId& start_id, const ElementType type);

    bool GetLink(const ElementId& id, std::u32string& str, std::u32string& url);

    Rect GetCaretRect(const CaretState& caret_state);

    bool GetCurrentStringFormat(StringFormatPtr& format);
    void SetCurrentStringFormat(StringFormatPtr& format);

    bool GetCurrentParagraphFormat(ParagraphFormatPtr& format);
    uint SetCurrentParagraphFormat(const std::string& name, bool with_undo = true);

    bool GetCurrentFormulaFormat(FormulaFormatPtr& format);
    void SetCurrentFormulaFormat(const std::string& name);
    void ChangeCurrentFormulaFormat(const StringFormatPtr& string_format);

    ElementType GetCurrentElementType();
    ElementType GetElementType(const ElementId& id);
    bool IsEditable(const ElementId& id);
    bool IsEmpty();
    bool IsResizable(const ElementId& id);

    bool IsString(ElementPtr el);
    bool IsString(ElementId id);
    bool IsRow(ElementPtr el);
    bool IsRow(ElementId id);
    bool IsParagraph(ElementPtr el);
    bool IsParagraph(ElementId id);
    bool IsFormula(ElementPtr el);

    bool GetStringFormat(const ElementId id, StringFormat& format);
    bool GetParagraphFormat(const ElementId id, ParagraphFormat& format);

    uint MoveCaret(MoveCaretTask::MoveCaretDir dir, bool select, bool with_last_task_id = false, bool move_into_view = true);
    uint MoveCaretLeft(bool select, bool with_last_task_id = false);
    uint MoveCaretRight(bool select, bool with_last_task_id = false);
    uint MoveCaretUp(bool select);
    uint MoveCaretDown(bool select);
    uint MoveCaretHome(bool select);
    uint MoveCaretEnd(bool select);
    uint MoveCaretPageUp(bool select);
    uint MoveCaretPageDown(bool select);
    uint MoveCaretWordLeft(bool select);
    uint MoveCaretWordRight(bool select);
    uint MoveCaretToDocumentBegin(bool select);
    uint MoveCaretToDocumentEnd(bool select);
    uint MoveCaretToDocumentEnd(bool select, bool move_into_view);
    uint MoveCaret(const int x, const int y, bool click = false);
    uint SelectAll();
    uint Select(const int start_x, const int start_y, const int end_x, const int end_y);
    uint SelectOut();

    void SetCaretVisible(bool visible);

    void CaretMoved();

    bool MouseLButtonDown(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id);
    bool MouseLButtonUp(const int x, const int y);
    bool MouseMove(const int x, const int y);
    bool MouseWheel(const int x, const int y, const Point pixel_delta, const Point angle_delta);

    void Undo();
    void Redo();

    bool CanUndo();
    bool CanRedo();

    void SetChanged(bool _changed);
    bool IsChanged();

    uint Resize(uint width, uint height);

    uint Redraw(const ElementId& id, bool move_into_view);
    void Redraw();

    bool WillRedraw(const ElementId& id, bool move_into_view);
    bool WillResize();

    uint New();
    uint Save(const std::string& filename);
    uint SaveJson(std::string& json, const int document_id, const bool gzip);
    uint Load(const std::string& filename);
    uint LoadInclude(const std::string& filename);
    uint LoadJson(const std::string& json_doc, const int document_id);
    uint LoadJsonInclude(const std::string& json_doc, const int document_id);
    void ClearIncludes();
    void AddInclude(const std::string& filename, const int document_id);
    void LoadNextInclude();

    uint Copy(std::u32string& out_json, std::u32string& out_text);
    uint Paste(std::u32string& in_json);
    uint PasteText(std::u32string&& str);
    uint PasteImage(const std::vector<unsigned char>& image);
    uint PasteImage(const std::string& image_base64);
    uint Cut(std::u32string& out_json, std::u32string& out_text);

    std::string ToHtml();
    std::u32string ToText();
    std::u32string ToText(const ElementId& id);

    TextFormatPtr GetDefaultTextFormat();
    StringFormatPtr GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
        bool subscript = false, bool superscript = false);
    StringFormatPtr GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
        bool subscript, bool superscript, Color text_color, Color text_bg_color);
    StringFormatPtr GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
        bool subscript, bool superscript, Color text_color, Color text_bg_color, Color text_bg_selection_color);
    StringFormatPtr GetStringFormat(const boost::uuids::uuid& id);
    void SaveStringFormats(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc);
    void SaveStringFormats(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc, const std::vector<ElementPtr>& elements);
    bool LoadStringFormats(const rapidjson::Value::ConstArray& value, rapidjson::Document::AllocatorType& alloc);

    bool GetGraphFormat(const ElementId& id, GraphFormat& format);
    uint SetGraphFormat(const ElementId& id, const GraphFormat& format, bool with_undo);
    bool GetGraphImage(const ElementId& id, std::vector<unsigned char>& png);
    bool GetPlotFormat(const ElementId& id, PlotFormat& format);
    uint SetPlotFormat(const ElementId& id, const PlotFormat& format, bool with_undo);
    bool GetTextFormat(TextFormat& format);
    uint SetTextFormat(const TextFormat& format, bool with_undo);
    
    void UpdateFormats();

    uint SetFontFamily(const std::string& family);
    uint SetFontSize(const uint size);
    uint SetBold(const bool enabled);
    uint SetItalic(const bool enabled);
    uint SetUnderline(const bool enabled);
    uint SetStrikethrough(const bool enabled);
    uint SetSubscript(const bool enabled);
    uint SetSuperscript(const bool enabled);
    uint SetColor(const Color color);
    uint SetBgColor(const Color color);

    void SetInsertMode(const bool enabled);
    void SwitchInsertMode();
    bool GetInsertMode();

    ElementPtr CreateParagraph(const ElementId& id);

    EditorState GetEditorState();
    EditorState MakeEditorState();
    LogicalEditorState GetLogicalEditorState();
    void SetEditorState(EditorState& state);
    void SetEditorState(LogicalEditorState& state);

    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::AutoResultConfig& auto_config, bool include_document, 
        std::u32string& expression, const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::RealResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::IntegerResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::RationalResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::ComplexResultConfig& config, bool include_document, 
        const std::u32string& expression, const uint delay);
    void Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::ArrayRealResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void SolveSymbolicReal(const LogicalId& _id, const std::string& guid, uint code_id, Config::RealResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void SolveSymbolicRational(const LogicalId& _id, const std::string& guid, uint code_id, Config::RationalResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void SolveSymbolicComplex(const LogicalId& _id, const std::string& guid, uint code_id, Config::ComplexResultConfig& config, bool include_document,
        const std::u32string& expression, const uint delay);
    void BreakSolving(const LogicalId& _id, const std::string& guid, uint code_id, bool wait = true);

    void SetIdentifier(const LogicalId& _id, const std::string& guid, uint code_id, Config::AutoResultConfig& config, bool include_document, 
        const std::u32string& identifier, const std::u32string& expression, const uint delay);
    void RemoveIdentifier(const LogicalId& _id, uint code_id, const std::u32string& identifier, const uint delay);
    void RemoveUserIdentifiers();
    void ClearExport();

    void RemoveSolver(uint code_id);

    ResultType GetResultType(ElementId _id);
    uint SetResultType(ElementId _id, ResultType result_type, bool with_undo);

    int GetPrecision(ElementId _id);
    uint SetPrecision(ElementId _id, uint precision, bool with_undo);

    int GetExp(ElementId _id);
    uint SetExp(ElementId _id, uint exp, bool with_undo);

    AngleMeasure GetDefaultAngleMeasure(ElementId _id);
    AngleMeasure GetResultAngleMeasure(ElementId _id);
    uint SetAngleMeasure(ElementId _id, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo);

    Notation GetResultNotation(ElementId _id);
    Notation GetDefaultNotation(ElementId _id);
    uint SetNotation(ElementId _id, Notation default_notation, Notation result_notation, bool with_undo);

    FractionForm GetFractionForm(ElementId _id);
    uint SetFractionForm(ElementId _id, FractionForm fraction_form, bool with_undo);

    ComplexForm GetComplexForm(ElementId _id);
    uint SetComplexForm(ElementId _id, ComplexForm form, bool with_undo);

    bool HasUnit(ElementId _id);
    void GetCastUnits(ElementId _id, std::vector<yutovo_calculator::Unit>& cast_units);
    uint SetUnit(ElementId _id, yutovo_calculator::Unit& unit, bool with_undo);

    uint ReSolve(const ElementId& _id);
    void ReSolve(const LogicalId& _id);
    uint ReSolveDependencies(const LogicalId& after_id, const std::u32string& identifier);
    uint ReSolveErrors();
    uint PutResult(const Result& result);
    void AddResolveElement(const LogicalId& _id);
    void AddChangedElement(const ElementId& _id);
    void GetSolverGuid(std::string& guid);
    uint SetLocale(const yutovo_calculator::Language language, bool with_undo);
    void ListIdentifiers(const uint code_id);

    void UpdateSolveId(const std::string& guid, const LogicalId& new_id);

    bool IsVisible(ElementId _id);
    ElementId GetFirstVisibleParagraph();
    ElementId GetFirstVisibleRow(ElementId paragraph_id);

    void AddErrorMark(const ElementId& _id, int start, int size);
    void RemoveErrorMarks(const ElementId& parent_id, Dependencies* dependencies = nullptr);
    bool HasErrorMark(const ElementId& _id, int& start, int& size);
    bool HasErrorMarks(const ElementId& _id);

    void SetIdentifiers(const uint code_id, const std::vector<std::string>& variables, const std::vector<std::string>& functions, 
        const std::vector<std::string>& operations, const std::vector<std::string>& units, const std::vector<std::string>& strings);
    IdentifierType FindIdentifier(const uint code_id, const std::string& str);
    void GetIdentifiers(const uint code_id, const std::string& left, std::vector<std::pair<IdentifierType, std::string>>& res);
    void GetPrompt(std::vector<std::pair<IdentifierType, std::string>>& res);

    bool WaitTask(uint task_id, uint64_t timeout = 0, uint64_t circle_delay = 1);

private:
    void MainLoop();

    void RestrictUndo();

    void UpdateChanged();

    int GetCharWidth(const StringFormatPtr& format, const char32_t symbol);

#ifdef TEST
public:
    void WaitMainLoop();
    void WaitUndo();
    void WaitRedo();
    void WaitLoad();
    void WaitSolver();
#endif

#ifdef TEST
public:
#else
private:
#endif
    int last_task_id = -1;
    int last_load_task_id = -1;
    bool last_task_executed = false;
    bool last_load_executed = false;
    bool last_undo_executed = false;
    bool last_redo_executed = false;

    int last_solver_task_id = -1;
    bool last_solver_executed = false;

    std::deque<uint> last_tasks;
    const int last_tasks_count = 100;

    int cur_modify_task_id = 0;
    int last_modify_task_id = 0;
    LogicalCaretState last_insert_caret_state, last_delete_caret_state;

    bool compressed_file = true;

private:
    friend class MoveCaretTask;
    friend class SetEditorStateTask;
    friend class GetEditorStateTask;
    friend class ResolveTask;
    friend class NewTask;
    friend class LoadTask;
    friend class SaveTask;
    friend class RedrawTask;
    friend class SetConfigTask;
    friend class InsertElementsTask;
    friend class DeleteElementsTask;
    friend class InsertFormulasTask;
    friend class Caret;
    friend class Element;
    friend class Elements;
    friend class Assignment;
    friend class String;
    friend class StringElements;
    friend class CodeString;

    void UpdateCaretView();
    void UpdateLastSelection();

private:
    std::recursive_mutex tasks_mutex;
    std::recursive_mutex solver_tasks_mutex;
    std::recursive_mutex edit_mutex;
    std::recursive_mutex state_mutex;
    std::recursive_mutex last_tasks_mutex;

public:
    Window* window;

    ParagraphFormatsPtr paragraph_formats;
    CodeFormatsPtr code_formats;
    FormulaFormatsPtr formula_formats;

    TextFormatPtr current_text_format;
    TextFormat default_text_format;

    StringFormatsPtr string_formats;

private:
    StringFormatPtr current_string_format;
    ParagraphFormatPtr current_paragraph_format;
    CodeFormatPtr current_code_format;
    FormulaFormatPtr current_formula_format;

    ElementPtr text;

    int include_pos = -1;

    std::map<StringFormatPtr, std::map<char32_t, int>> char_widths;

public:
    CaretPtr caret;
    Selection selection;

    bool pasting = false;
    bool saving = false;
    bool editing = false;

    uint cur_code_id = 1;

    Config config;

    std::vector<ElementId> changed_elements;
    std::vector<LogicalId> resolve_elements;

    std::map<std::string, LogicalId> solve_ids;
    std::recursive_mutex solve_ids_mutex;

    std::atomic_bool break_remake = false;

    std::atomic<bool> changed{false};
    int save_task_id = 0;

    std::string file_guid; //unique document id

    std::string document_guid; //for identifing include documents

    std::string path;

    bool insert_mode = true; //or replace mode

private:
    std::queue<std::pair<int, std::string>> include_documents;
    std::vector<std::string> include_file_guids; //for checking circle includes

    std::list<TaskPtr> tasks;
    std::deque<TaskPtr> undo_tasks;
    std::deque<TaskPtr> redo_tasks;
    std::vector<bool> undos; //requieres for undo
    std::vector<bool> redos; //requieres for redo

    std::recursive_mutex identifiers_mutex;
    std::map<uint, Identifiers> identifiers; //by code_id

    Solver solver;

    bool exit = false;

    std::atomic<bool> next_circle{false};
    std::thread main_loop;

    uint cur_task_id = 0;

    ElementId cur_visible_row; //any row which is visible

    UndoBase undo_base;

    EditorState last_editor_state;

    Selection last_selection;

    SelectionState last_editor_selection;

    bool last_changed = false;

    ElementId mouse_capture_id;

    EditorState last_caret_state;

    LogicalEditorState include_editor_state;

    ElementId caret_hilight_id;

    Point last_mouse_pos{0, 0};

    enum class ResizeDir
    {
        None = 0,
        HorizontalLeft,
        HorizontalRight,
        VerticalLeft,
        VerticalRight,
        BothTopLeft,
        BothTopRight,
        BothBottomLeft,
        BothBottomRight
    };

    ResizeDir resize_dir = ResizeDir::None;

    bool moving_element = false;

    static const std::regex subscript_pattern;

    Logger* logger;
};

typedef std::shared_ptr<Document> DocumentPtr;

}

#endif
