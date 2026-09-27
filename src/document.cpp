/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "document.h"
#include "str.h"
#include "link.h"
#include "paragraph.h"
#include "row.h"
#include "image.h"
#include "formulas/code_block.h"
#include "formulas/code_string.h"
#include "formulas/code_paragraph.h"
#include "formulas/plus.h"
#include "formulas/minus.h"
#include "formulas/multiply.h"
#include "formulas/division.h"
#include "formulas/power.h"
#include "formulas/nth_root.h"
#include "formulas/square_root.h"
#include "formulas/equation.h"
#include "formulas/brackets.h"
#include "formulas/assignment.h"
#include "formulas/unit.h"
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
#include "editor_utils.h"
#include <assert.h>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <rapidjson/istreamwrapper.h>

#ifdef _MSC_VER
#undef GetObject
#endif

namespace yutovo
{

using namespace std::chrono_literals;
using namespace std::chrono;

//Document

const std::regex Document::subscript_pattern(R"(^([^{]+?)(?:\{([^}]*)\})?$)");

Document::Document(Window* _window, Config& _config, const std::string _document_guid) :
    window(_window),
    selection(this),
    config(_config),
    file_guid(boost::uuids::to_string(boost::uuids::random_generator()())),
    document_guid(_document_guid),
    solver(this),
    undo_base(this),
    last_selection(this),
    logger(Logger::GetInstance(config.logs_path + "/yutovo-editor", "yutovo-editor", config.log_console, config.log_file))
{
    logger->SetLevel(_config.log_level);
    LOG_DEBUG("Document start");

    if (document_guid.empty())
        document_guid = boost::uuids::to_string(boost::uuids::random_generator()());

    string_formats.reset(new StringFormats());
    paragraph_formats.reset(new ParagraphFormats(string_formats, config.language));
    code_formats.reset(new CodeFormats());
    formula_formats.reset(new FormulaFormats(string_formats));

    current_code_format = code_formats->GetFormat("Calculator", 5, 5, 5, 5, 2, 2, 2, 2, 2, Color::Blue());

    current_paragraph_format = paragraph_formats->GetFormat("Text body", config.language);
    current_formula_format = formula_formats->GetFormat("Code");
    current_text_format = TextFormats::GetFormat(TextFormat::Paging::WEB_VIEW, 20, 20, 20, 20, 10, Size{0, 0});
    default_text_format = *current_text_format;

    caret.reset(new Caret(this));
    caret->SetVisible(config.caret_visible);

#ifndef DEBUG
    config.pretty_json = false;
#endif
}

Document::Document(Window* _window, Config& _config, const Document& source) :
    window(_window),
    selection(this),
    config(_config),
    file_guid(boost::uuids::to_string(boost::uuids::random_generator()())),
    document_guid(boost::uuids::to_string(boost::uuids::random_generator()())),
    solver(this),
    undo_base(this),
    last_selection(this),
    logger(Logger::GetInstance(config.logs_path + "/yutovo-editor", "yutovo-editor", config.log_console, config.log_file))
{
    logger->SetLevel(_config.log_level);
    LOG_DEBUG("Document start");

    string_formats.reset(new StringFormats());
    paragraph_formats.reset(new ParagraphFormats(string_formats, config.language));
    code_formats.reset(new CodeFormats());
    formula_formats.reset(new FormulaFormats(string_formats));

    current_code_format = code_formats->GetFormat("Calculator", 5, 5, 5, 5, 2, 2, 2, 2, 2, Color::Blue());

    current_paragraph_format = paragraph_formats->GetFormat("Text body", config.language);
    current_formula_format = formula_formats->GetFormat("Code");
    current_text_format = TextFormats::GetFormat(TextFormat::Paging::WEB_VIEW, 20, 20, 20, 20, 10, Size{0, 0});
    default_text_format = *current_text_format;

    identifiers = source.identifiers;

    caret.reset(new Caret(this));
    caret->SetVisible(config.caret_visible);

    text.reset(source.text->Clone());
    text->SetDocument(this);

#ifndef DEBUG
    config.pretty_json = false;
#endif
}

Document::~Document()
{
    exit = true;
    next_circle = true;
    if (main_loop.joinable())
        main_loop.join();
    LOG_DEBUG("Document end");
}

void Document::Start()
{
    current_code_format->border_color = config.code_block_border_color;

    window->Init(this);

    if (!text || text->IsEmpty())
        text.reset(new Text(this, current_text_format, true));
    else
        ((Text*)text.get())->SetTextFormat(*current_text_format);

    caret->MoveToDocumentBegin(nullptr);

    main_loop = std::thread(&Document::MainLoop, this);

    text->Remake(true);
    Redraw(text->id, false);

    std::tuple<char32_t, std::string, int> s1{U'(', "Arial", 300};
    std::tuple<char32_t, std::string, int> s2{U')', "Arial", 300};
    std::vector<std::tuple<char32_t, std::string, int>> s{s1, s2};
    window->PrepareSymbolsSizes(s);

    solver.SetLocale(config.language);
}

void Document::Start(const TextFormat _default_text_format)
{
    default_text_format = _default_text_format;
    current_text_format = GetDefaultTextFormat();
    Start();
}

void Document::Stop()
{
    exit = true;
    next_circle = true;
    if (main_loop.joinable())
        main_loop.join();
    LOG_DEBUG("Document stop");
}

void Document::GetConfig(Config& _config)
{
    std::unique_lock<std::recursive_mutex> lock(edit_mutex);
    _config = config;
}

uint Document::SetConfig(const Config& _config, bool with_undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SetConfigTask(text, _config, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::SetConfig(const std::string& _config, bool with_undo)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SetConfigTask(text, _config, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

void Document::MainLoop()
{
    std::vector<TaskPtr> temp_tasks;

    while (!exit)
    {
        {
            bool empty = false;
            {
                std::unique_lock<std::recursive_mutex> lock(tasks_mutex);
                empty = tasks.empty() && undos.empty() && redos.empty();
            }
            if (empty)
            {
                auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                auto next = now;
                while (!next_circle && next - now < config.caret_blink_delay * 1ms) //wait for tasks
                {
                    std::this_thread::sleep_for(10ms);
                    next = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
                }
                if (!next_circle)
                {
                    caret->Blink();
                    continue;
                }
                next_circle = false;
            }
        }

        {
            std::vector<TaskPtr> temp_undo_tasks;
            {
                std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
                if (!undos.empty())
                {
                    //execute one undo
                    if (!undo_tasks.empty())
                    {
                        //collect tasks with one id
                        TaskPtr t = undo_tasks.back();
                        uint id = t->id;
                        bool s = t->next_task;
                        bool sp = false;
                        while (id == t->id || (s && t->next_task))
                        {
                            temp_undo_tasks.push_back(t);
                            undo_tasks.pop_back();
                            if (undo_tasks.empty())
                                break;
                            sp = t->next_task;
                            t = undo_tasks.back();
                        }
                        if (!undo_tasks.empty() && sp)
                        {
                            temp_undo_tasks.push_back(t);
                            undo_tasks.pop_back();
                        }
                    }
                    undos.clear();
                }
            }
            if (!temp_undo_tasks.empty())
            {
                caret->Hide(); //caret will be shown on Redraw or caret moving
                selection.can_optimize = false;
                for (size_t i = 0; i < temp_undo_tasks.size(); ++i)
                {
                    changed_elements.clear();
                    resolve_elements.clear();
                    TaskPtr& t = temp_undo_tasks[i];
                    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
                    if (!t->Execute())
                        break;
                    if (!undo_tasks.empty())
                        last_modify_task_id = undo_tasks.back()->id;
                    else
                        last_modify_task_id = 0;
                }
                selection.can_optimize = true;
                last_insert_caret_state.id = LogicalId{};
                last_delete_caret_state.id = LogicalId{};
#ifdef DEBUG
                last_undo_executed = true;
#endif
            }
        }

        {
            std::vector<TaskPtr> temp_redo_tasks;
            {
                std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
                if (!redos.empty())
                {
                    //execute one redo: collect all redo tasks for first absent undo
                    uint last_undo_task_id = 0;
                    if (!undo_tasks.empty())
                        last_undo_task_id = undo_tasks.back()->id;
                    
                    int i = 0;
                    for (i = (int)redo_tasks.size() - 1; i >= 0; --i)
                    {
                        if (redo_tasks[i]->id == last_undo_task_id)
                            break;
                    }

                    if (++i < redo_tasks.size())
                    {
                        uint redo_task_id = redo_tasks[i]->id;
                        while (i < redo_tasks.size() && (redo_tasks[i]->id == redo_task_id || redo_tasks[i]->next_task))
                        {
                            temp_redo_tasks.push_back(redo_tasks[i++]);
                        }
                    }
                    redos.clear();
                }
            }
            if (!temp_redo_tasks.empty())
            {
                caret->Hide();
                for (size_t i = 0; i < temp_redo_tasks.size(); ++i)
                {
                    changed_elements.clear();
                    resolve_elements.clear();
                    TaskPtr& t = temp_redo_tasks[i];
                    cur_task_id = t->id;
                    cur_modify_task_id = t->id;
                    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
                    if (!t->Execute())
                        break;
                    last_modify_task_id = t->id;
                }
                caret->Show();
#ifdef DEBUG
                last_redo_executed = true;
#endif
            }
        }

        {
            std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
            for (auto it = tasks.begin(); it != tasks.end(); ++it)
                temp_tasks.push_back(*it);
            tasks.clear();
        }

        if (!temp_tasks.empty())
        {
            caret->Hide();
            //execute all the tasks
            for (size_t i = 0; i < temp_tasks.size(); ++i)
            {
                TaskPtr& t = temp_tasks[i];
                cur_task_id = t->id;
                if (t->with_undo)
                    cur_modify_task_id = t->id;
                uint last_undo_task_id = 0;
                if (!undo_tasks.empty())
                    last_undo_task_id = undo_tasks.back()->id;
                
                changed_elements.clear();
                resolve_elements.clear();

                {
                    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
                    if (t->Execute() && t->with_undo)
                    {
                        //shrink the redo vector to the size of the undo stack
                        for (int i = redo_tasks.size() - 1; i >= 0; --i)
                        {
                            if (redo_tasks[i]->id == last_undo_task_id)
                                break;
                            redo_tasks.erase(redo_tasks.begin() + i);
                        }
                        if (!undo_tasks.empty())
                            t->next_task = undo_tasks.back()->next_task;
                        redo_tasks.push_back(t);
                        last_modify_task_id = t->id;
                    }
                    last_editor_selection = selection.GetState();
                }

                {
                    std::lock_guard<std::recursive_mutex> lock(state_mutex);
                    last_editor_state = EditorState{caret->GetCaretState(), last_editor_selection};
                }

#ifdef DEBUG
                if (last_task_id > 0)
                {
                    if (last_task_id == t->id)
                        last_task_executed = true;
                }
                if (last_load_task_id == t->id)
                    last_load_executed = true;
                if (last_solver_task_id == t->id)
                    last_solver_executed = true;
#endif
                std::lock_guard<std::recursive_mutex> lock(last_tasks_mutex);
                while (last_tasks.size() > last_tasks_count)
                    last_tasks.pop_back();
                last_tasks.push_front(t->id);
            }

            caret->Show();

            temp_tasks.clear();
        }

        UpdateChanged();
    }
}

uint Document::InsertParagraph(bool with_undo)
{
    LOG_TRACE("Insert paragraph");
    CaretState c = caret->GetCaretState();
    auto el = FindParentParagraph(c.id);
    if (!el)
        return 0;
    ParagraphFormatPtr format = ((Paragraph*)el.get())->format;
    return InsertElement(new Paragraph(this, format), with_undo);
}

uint Document::InsertString(const std::string& str, bool with_undo)
{
    LOG_TRACE("Insert string: {}", str);
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new String(this, str, format), with_undo);
    return 0;
}

uint Document::InsertString(const std::u32string& str, bool with_undo)
{
    return InsertString(str, false, with_undo);
}

uint Document::InsertString(const std::u32string& str, bool paste, bool with_undo)
{
    LOG_TRACE("Insert string: {}", ToBasicString(str));
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new String(this, str, format), with_undo, paste);
    return 0;
}

uint Document::InsertString(const std::string& str, const StringFormatPtr string_format, bool with_undo)
{
    LOG_TRACE("Insert string: {}, format: {}", str, string_format->ToString());
    return InsertElement(new String(this, str, string_format), with_undo);
}

uint Document::InsertString(const std::string& str, ElementId element_id, bool with_undo)
{
    LOG_TRACE("Insert string: {}", str);
    StringFormat f;
    if (!GetStringFormat(element_id, f))
        return 0;
    StringFormatPtr string_format = string_formats->GetFormat(f);

    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SetStringTask(text, ToUtfString(str), element_id));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertString(const std::string& str, bool parse, bool with_undo)
{
    if (parse)
    {
        //parse for str{sub} - it will be subscript
        std::smatch match;
        if (std::regex_match(str, match, subscript_pattern))
        {
            std::string _str = match[1].str();
            std::string sub = match[2].matched ? match[2].str() : "";
            if (!sub.empty())
                return InsertElement(new Subscript(this, _str, sub), with_undo, false, false);
            return InsertString(_str, with_undo);
        }
    }
    return InsertString(str, with_undo);
}

uint Document::ReplaceString(const std::u32string& str, bool with_undo)
{
    LOG_TRACE("Insert string: {}", ToBasicString(str));
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
    {
        //parse for str{sub} - it will be subscript
        std::smatch match;
        auto s = ToBasicString(str);
        if (std::regex_match(s, match, subscript_pattern))
        {
            std::string _str = match[1].str();
            std::string sub = match[2].matched ? match[2].str() : "";
            if (!sub.empty())
                return InsertElement(new Subscript(this, _str, sub), with_undo, false, true);
            return InsertElement(new String(this, _str, format), with_undo, false, true);
        }
        return InsertElement(new String(this, str, format), with_undo, false, true);
    }
    return 0;
}

uint Document::InsertLink(const std::string& str, const std::string& url, bool with_undo)
{
    LOG_TRACE("Insert link: {} {}", str, url);
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new Link(this, str, url, format), with_undo);
    return 0;
}

uint Document::InsertLink(const std::u32string& str, const std::u32string& url, bool with_undo)
{
    LOG_TRACE("Insert link: {} {}", ToBasicString(str), ToBasicString(url));
    StringFormatPtr format;
    if (GetCurrentStringFormat(format))
        return InsertElement(new Link(this, str, url, format), with_undo);
    return 0;
}

uint Document::InsertElement(Element* element, bool with_undo, bool pasting, bool replace)
{
    LOG_TRACE("Insert element: {}", ToBasicString(element->ToText()));
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    return InsertElements(elements, with_undo, pasting, replace);
}

uint Document::InsertElements(std::vector<ElementPtr>& elements, bool with_undo, bool pasting, bool replace)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new InsertElementsTask(text, elements, with_undo, pasting, replace));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ReplaceElement(Element* element, bool with_undo)
{
    LOG_TRACE("Replace element: {}", ToBasicString(element->ToText()));
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    return InsertElements(elements, with_undo, false, true);
}

uint Document::DeleteElements(bool left, bool with_undo)
{
    LOG_TRACE("Delete elements: {}", left);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new DeleteElementsTask(text, left, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ClearElements(ElementId element_id, bool with_undo)
{
    LOG_TRACE("Clear elements: {}", ElementIdToString(element_id));
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new DeleteElementsTask(text, element_id, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertCode(bool next_code_id, bool with_undo)
{
    LOG_TRACE("Insert code: {}", next_code_id);
    if (next_code_id)
        ++cur_code_id;
    return InsertFormula(new CodeBlock(this, cur_code_id, true, true), with_undo);
}

uint Document::InsertCodeString(const std::string& str, bool with_undo)
{
    LOG_TRACE("Insert code string: {}", str);
    FormulaFormatPtr format;
    if (GetCurrentFormulaFormat(format))
        return InsertFormula(new CodeString(this, str, format->string_format), with_undo);
    return 0;
}

uint Document::InsertPlus(bool with_undo, bool replace)
{
    LOG_TRACE("Insert plus");
    return InsertFormula(new Plus(this), with_undo, false, replace);
}

uint Document::InsertMinus(bool with_undo, bool replace)
{
    LOG_TRACE("Insert minus");
    return InsertFormula(new Minus(this), with_undo, false, replace);
}

uint Document::InsertMultiply(bool with_undo, bool replace)
{
    LOG_TRACE("Insert multiply");
    return InsertFormula(new Multiply(this), with_undo, false, replace);
}

uint Document::InsertDivision(bool with_undo, bool replace)
{
    LOG_TRACE("Insert division");
    return InsertFormula(new Division(this), with_undo, false, replace);
}

uint Document::InsertPower(bool with_undo, bool replace)
{
    LOG_TRACE("Insert power");
    return InsertFormula(new Power(this), with_undo, false, replace);
}

uint Document::InsertNthRoot(bool with_undo, bool replace)
{
    LOG_TRACE("Insert nth root");
    return InsertFormula(new NthRoot(this), with_undo, false, replace);
}

uint Document::InsertSquareRoot(bool with_undo, bool replace)
{
    LOG_TRACE("Insert square root");
    return InsertFormula(new SquareRoot(this), with_undo, false, replace);
}

uint Document::InsertEquation(yutovo_solver::ResultType result_type, bool with_undo)
{
    LOG_TRACE("Insert equation");
    return InsertFormula(new Equation(this, result_type), with_undo);
}

uint Document::InsertOpenRoundBracket(bool with_undo)
{
    LOG_TRACE("Insert open round bracket");
    return InsertFormula(new OpenBracket(this, ElementType::OPEN_ROUND_BRACKET), with_undo);
}

uint Document::InsertCloseRoundBracket(bool with_undo)
{
    LOG_TRACE("Insert close round bracket");
    return InsertFormula(new CloseBracket(this, ElementType::CLOSE_ROUND_BRACKET), with_undo);
}

uint Document::InsertOpenSquareBracket(bool with_undo)
{
    LOG_TRACE("Insert open square bracket");
    return InsertFormula(new OpenBracket(this, ElementType::OPEN_SQUARE_BRACKET), with_undo);
}

uint Document::InsertCloseSquareBracket(bool with_undo)
{
    LOG_TRACE("Insert close square bracket");
    return InsertFormula(new CloseBracket(this, ElementType::CLOSE_SQUARE_BRACKET), with_undo);
}

uint Document::InsertAssignment(bool with_undo)
{
    LOG_TRACE("Insert assignment");
    return InsertFormula(new Assignment(this), with_undo);
}

uint Document::InsertUnit(bool with_undo)
{
    LOG_TRACE("Insert unit");
    return InsertFormula(new Unit(this), with_undo);
}

uint Document::InsertSubscript(bool with_undo, bool replace)
{
    LOG_TRACE("Insert subscript");
    return InsertFormula(new Subscript(this), with_undo, false, replace);
}

uint Document::InsertExclamation(bool with_undo)
{
    LOG_TRACE("Insert exclamation");
    return InsertFormula(new Exclamation(this), with_undo);
}

uint Document::InsertAnd(bool with_undo)
{
    LOG_TRACE("Insert and");
    return InsertFormula(new And(this), with_undo);
}

uint Document::InsertOr(bool with_undo)
{
    LOG_TRACE("Insert or");
    return InsertFormula(new Or(this), with_undo);
}

uint Document::InsertXor(bool with_undo)
{
    LOG_TRACE("Insert xor");
    return InsertFormula(new Xor(this), with_undo);
}

uint Document::InsertNot(bool with_undo)
{
    LOG_TRACE("Insert not");
    return InsertFormula(new Not(this), with_undo);
}

uint Document::InsertPercent(bool with_undo)
{
    LOG_TRACE("Insert percent");
    return InsertFormula(new Percent(this), with_undo);
}

uint Document::InsertSum(bool with_undo, bool replace)
{
    LOG_TRACE("Insert sum");
    return InsertFormula(new Sum(this), with_undo, false, replace);
}

uint Document::InsertProduct(bool with_undo, bool replace)
{
    LOG_TRACE("Insert product");
    return InsertFormula(new Product(this), with_undo, false, replace);
}

uint Document::InsertDefiniteIntegral(bool with_undo, bool replace)
{
    LOG_TRACE("Insert definite integral");
    return InsertFormula(new DefiniteIntegral(this), with_undo, false, replace);
}

uint Document::InsertIndefiniteIntegral(bool with_undo, bool replace)
{
    LOG_TRACE("Insert indefinite integral");
    return InsertFormula(new IndefiniteIntegral(this), with_undo, false, replace);
}

uint Document::InsertDerivative(bool with_undo, bool replace)
{
    return InsertDerivative(U"d", 1, with_undo, replace);
}

uint Document::InsertDerivative(const std::u32string& symbol, const int order, bool with_undo, bool replace)
{
    LOG_TRACE("Insert derivative");

    auto int_to_str = 
        [](int value) -> std::u32string
        {
            if (value == 0)
                return U"0";
            bool negative = value < 0;
            int n = negative ? -value : value;
            std::u32string result;
            while (n > 0)
            {
                result = std::u32string(1, char32_t(U'0' + n % 10)) + result;
                n /= 10;
            }
            return negative ? (U"-" + result) : result;
        };

    Division* div = new Division(this);
    CodeRow<>* num = div->GetNumeratorRow();
    CodeRow<>* den = div->GetDenominatorRow();

    if (order == 1)
    {
        ElementPtr d(new CodeString(num, symbol));
        d->can_merge = false;
        div->AddNumerator(d);
    }
    else
    {
        Power* p = new Power(num);
        CodeRow<>* base_row = p->GetBaseRow();
        CodeRow<>* exp_row = p->GetExponentRow();
        base_row->elements->Clear();
        exp_row->elements->Clear();

        ElementPtr d(new CodeString(base_row, symbol));
        d->can_merge = false;
        base_row->elements->Add(d);

        ElementPtr n(new CodeString(exp_row, int_to_str(order)));
        n->can_merge = false;
        exp_row->elements->Add(n);

        ElementPtr pp(p);
        div->AddNumerator(pp);
    }

    ElementPtr func(new CodeString(num, U""));
    func->can_merge = false;
    div->AddNumerator(func);

    if (order == 1)
    {
        ElementPtr dd(new CodeString(den, symbol));
        dd->can_merge = false;
        div->AddDenomerator(dd);

        ElementPtr var(new CodeString(den, U""));
        var->can_merge = false;
        div->AddDenomerator(var);
    }
    else
    {
        Power* p = new Power(den);
        CodeRow<>* base_row = p->GetBaseRow();
        CodeRow<>* exp_row = p->GetExponentRow();
        base_row->elements->Clear();
        exp_row->elements->Clear();

        ElementPtr dd(new CodeString(base_row, symbol));
        dd->can_merge = false;
        base_row->elements->Add(dd);

        ElementPtr var(new CodeString(base_row, U""));
        var->can_merge = false;
        base_row->elements->Add(var);

        ElementPtr n(new CodeString(exp_row, int_to_str(order)));
        n->can_merge = false;
        exp_row->elements->Add(n);

        ElementPtr pp(p);
        div->AddDenomerator(pp);
    }

    return InsertFormula(div, with_undo, false, replace);
}

uint Document::InsertEvaluationBarSubscript(bool with_undo, bool replace)
{
    LOG_TRACE("Insert evaluation bar");
    return InsertFormula(new EvaluationBarSubscript(this), with_undo, false, replace);
}

uint Document::InsertDerivativeAtPoint(bool with_undo, bool replace)
{
    LOG_TRACE("Insert derivative at point");
    InsertEvaluationBarSubscript(with_undo);
    MoveCaretHome(false);
    return InsertDerivative(U"d", 1, with_undo, replace);
}

uint Document::InsertImage(const std::string& image_base64, bool with_undo, bool pasting)
{
    LOG_TRACE("Insert image");
    return InsertElement(new Image(this, image_base64), with_undo, pasting);
}

uint Document::InsertImage(const std::vector<unsigned char>& image, bool with_undo, bool pasting)
{
    LOG_TRACE("Insert image");
    return InsertElement(new Image(this, image), with_undo, pasting);
}

uint Document::InsertComma(bool with_undo)
{
    LOG_TRACE("Insert comma");
    return InsertFormula(new Comma(this), with_undo);
}

uint Document::InsertRoundBrackets(bool with_undo)
{
    LOG_TRACE("Insert round brackets");
    std::vector<ElementPtr> els;
    els.emplace_back(new OpenBracket(this, ElementType::OPEN_ROUND_BRACKET));
    els.emplace_back(new CloseBracket(this, ElementType::CLOSE_ROUND_BRACKET));
    uint r = InsertFormulas(els, with_undo, false, false, false, 1);
    if (r > 0)
        MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertSquareBrackets(bool with_undo)
{
    LOG_TRACE("Insert square brackets");
    std::vector<ElementPtr> els;
    els.emplace_back(new OpenBracket(this, ElementType::OPEN_SQUARE_BRACKET));
    els.emplace_back(new CloseBracket(this, ElementType::CLOSE_SQUARE_BRACKET));
    uint r = InsertFormulas(els, with_undo, false, false, false, 1);
    if (r > 0)
        MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertFunction(const std::string& name, bool with_undo)
{
    LOG_TRACE("Insert function: {}", name);
    FormulaFormatPtr format;
    if (!GetCurrentFormulaFormat(format))
        return 0;
    std::vector<ElementPtr> els;
    els.emplace_back(new CodeString(this, name, format->string_format));
    els.emplace_back(new OpenBracket(this, ElementType::OPEN_ROUND_BRACKET));
    els.emplace_back(new CloseBracket(this, ElementType::CLOSE_ROUND_BRACKET));
    uint r = InsertFormulas(els, with_undo, false, false, false, 2);
    if (r > 0)
        MoveCaretLeft(false, true);
    return r;
}

uint Document::InsertSubscriptFunction(const std::string& name, bool with_undo)
{
    LOG_TRACE("Insert subscript function: {}", name);
    InsertCodeString(name, with_undo);
    return InsertFormula(new Subscript(this), with_undo, true);
}

uint Document::InsertGraph(bool with_undo)
{
    LOG_TRACE("Insert graph");
    return InsertFormula(new GraphLine(this), with_undo, true);
}

uint Document::InsertFormula(Element* element, bool with_undo, bool with_last_task_id, bool replace)
{
    LOG_TRACE("Insert formula: {}", ToBasicString(element->ToText()));
    std::vector<ElementPtr> elements;
    elements.emplace_back(element);
    if (current_formula_format)
        SetCurrentStringFormat(current_formula_format->string_format);
    return InsertFormulas(elements, with_undo, with_last_task_id, false, replace, -1);
}

uint Document::InsertFormulas(std::vector<ElementPtr>& elements, bool with_undo, bool with_last_task_id, bool pasting, bool replace, int select_pos)
{
    LOG_TRACE("Insert formulas");
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        if (with_last_task_id)
            tasks.emplace_back(new InsertFormulasTask(text, last_task_id, elements, with_undo));
        else
            tasks.emplace_back(new InsertFormulasTask(text, elements, with_undo, pasting, replace, select_pos));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::InsertUnit(const yutovo_calculator::Unit& unit, bool list_identifiers)
{
    LOG_TRACE("Insert unit");
    FormulaFormatPtr f = formula_formats->GetFormat("Code");
    StringFormatPtr string_format = f->string_format;

    ElementPtr numerator(new CodeRow<>(this));
    numerator->elements->Clear();
    ElementPtr denomerator;
    for (auto& u : unit.unit)
    {
        auto s = ToBasicString(u.first);
        if (u.second > 0)
        {
            if (numerator->elements->Count() > 0)
                numerator->AddElement(ElementPtr(new Multiply(this)));
            if (u.second == 1)
                numerator->AddElement(CodeStringPtr(new CodeString(numerator.get(), s, string_format)));
            else
            {
                PowerPtr p(new Power(this));
                p->AddBase(CodeStringPtr(new CodeString(p.get(), s)));
                p->AddExponent(CodeStringPtr(new CodeString(p.get(), std::to_string(u.second), string_format)));
                numerator->AddElement(p);
            }
        }
        else
        {
            if (!denomerator)
            {
                denomerator.reset(new CodeRow<>(this));
                denomerator->elements->Clear();
            }
            if (denomerator->elements->Count() > 0)
                denomerator->AddElement(ElementPtr(new Multiply(this)));
            if (u.second == -1)
                denomerator->AddElement(CodeStringPtr(new CodeString(denomerator.get(), s, string_format)));
            else
            {
                PowerPtr p(new Power(this));
                p->AddBase(CodeStringPtr(new CodeString(p.get(), s)));
                p->AddExponent(CodeStringPtr(new CodeString(p.get(), std::to_string(-u.second), string_format)));
                denomerator->AddElement(p);
            }
        }
    }

    auto* row = new CodeRow<>(this);

    if (denomerator)
    {
        auto* d = new Division(this);
        if (numerator->elements->Count() == 0)
            numerator->AddElement(CodeStringPtr(new CodeString(d, U"1")));
        d->AddNumerator(numerator);
        d->AddDenomerator(denomerator);
        row->AddElement(ElementPtr(d));
    }
    else
    {
        for (int i = 0; i < numerator->elements->Count(); ++i)
            row->AddElement(numerator->elements->Get(i));
    }

    auto* code = new CodeBlock(this, 1, true, list_identifiers);
    code->elements->Clear();
    code->elements->Add(ElementPtr(row));
    return InsertFormula(code, false);
}

uint Document::ChangeStringFormat(const std::string family, const uint size, const bool bold, const bool italic, const bool underline, const bool strikethrough, 
    const bool subscript, const bool superscript, Color text_color, Color text_bg_color, bool with_undo)
{
    LOG_TRACE("Change string format: family={}, size={}, bold={}, italic={}, underline={}, text_color={}, text_bg_color={}", 
        family, size, bold, italic, underline, text_color.ToString(), text_bg_color.ToString());
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return ChangeStringFormat(string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, subscript, superscript, 
        text_color, text_bg_color, Color::Blue()), with_undo);
}

uint Document::ChangeStringFormat(const StringFormatPtr format, bool set_family, bool set_size, bool set_bold, bool set_italic, bool set_underline, 
    bool set_strikethrough, bool set_subscript, bool set_superscript, bool set_text_color, bool set_text_bg_color, bool with_undo)
{
    LOG_TRACE("Change string format: format={}, set_family={}, set_size={}, set_bold={}, set_italic={}, set_underline={}, set_strikethrough={}, "\
        "set_subscript={}, set_superscript={}, set_text_color={}, set_text_bg_color={}", 
        format->ToString(), set_family, set_size, set_bold, set_italic, set_underline, set_strikethrough, set_subscript, set_superscript, 
        set_text_color, set_text_bg_color);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangeStringFormatTask(text, format, set_family, set_size, set_bold, set_italic, set_underline, set_strikethrough, 
            set_subscript, set_superscript, set_text_color, set_text_bg_color, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeStringFormat(const StringFormatPtr format, bool with_undo)
{
    LOG_TRACE("Change string format: format={}", format->ToString());
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ChangeStringFormatTask(text, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const ParagraphFormatPtr format, bool with_undo)
{
    LOG_TRACE("Change paragraph format: format={}", format->ToString());
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        CaretState c = caret->GetCaretState();
        tasks.emplace_back(new ChangeParagraphFormatTask(text, c.id, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const std::string name, bool with_undo)
{
    LOG_TRACE("Change paragraph format: name={}", name);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        auto format = paragraph_formats->GetFormat(name, config.language);
        if (!format)
            return 0;
        CaretState c = caret->GetCaretState();
        tasks.emplace_back(new ChangeParagraphFormatTask(text, c.id, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::ChangeParagraphFormat(const ParagraphFormat::Alignment alignment, bool with_undo)
{
    LOG_TRACE("Change paragraph format: alignment={}", (int)alignment);
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        CaretState c = caret->GetCaretState();
        auto el = FindParentParagraph(c.id);
        if (!el)
            return 0;
        ParagraphFormatPtr format = ((Paragraph*)el.get())->format;
        format = paragraph_formats->GetFormat(format->name, alignment, format->word_wrap, format->line_spacing, format->indent_before, 
            format->indent_after, format->indent_first_line, format->spacing_before, format->spacing_after, format->default_string_format, 
            config.language);
        tasks.emplace_back(new ChangeParagraphFormatTask(text, c.id, format, with_undo));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

bool Document::StoreUndo(const ElementId& _id)
{
    if (_id.size() == 1)
        return StoreUndo(_id, 0, text->elements->Count());

    int undo_id = undo_base.Store(_id);
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, _id, 0, 0, cur_task_id)));
    RestrictUndo();
    return true;
}

bool Document::StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size)
{
    int undo_id;
    ElementId _id;
    if (IsRow(parent_id))
    {
        auto p = GetParent(parent_id);
        undo_id = undo_base.Store(p->id, 0, p->elements->Count());
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->id, 0, p->elements->Count(), cur_task_id)));
    }
    else
    {
        undo_id = undo_base.Store(parent_id, pos, size);
        _id = yutovo::GetChild(parent_id, pos);
    }
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, _id, pos, delete_size, cur_task_id)));
    RestrictUndo();
    return true;
}

bool Document::StoreUndo(const ElementId& parent_id, const int pos, const int size, const int delete_size, UndoTask::UndoOperation undo_operation)
{
    int undo_id;
    auto p = GetParent(parent_id);
    if (p && (p->type == ElementType::EQUATION || p->type == ElementType::ASSIGNMENT))
    {
        int _pos = yutovo::GetChildPos(p->id);
        undo_id = undo_base.Store(p->parent->id, _pos, 1);
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->parent->id, _pos, 1, 0, UndoTask::UndoOperation::CHANGE, cur_task_id)));
    }
    else if (IsRow(parent_id))
    {
        undo_id = undo_base.Store(p->id, 0, p->elements->Count());
    }
    else if (IsParagraph(parent_id))
    {
        int _pos = p->elements->GetChildPos(parent_id);
        undo_id = undo_base.Store(p->id, _pos, 1);
        if (undo_id < 0)
            return false;
        undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, p->id, _pos, 1, 0, UndoTask::UndoOperation::CHANGE, cur_task_id)));
    }
    else
    {
        undo_id = undo_base.Store(parent_id, pos, size);
    }
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, parent_id, pos, size, delete_size, undo_operation, cur_task_id)));
    RestrictUndo();
    return true;
}

bool Document::StoreUndo(const Config& config)
{
    int undo_id = undo_base.Store(config);
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, cur_task_id)));
    RestrictUndo();
    return true;
}

bool Document::StoreUndo(const TextFormat& format)
{
    int undo_id = undo_base.Store(format);
    if (undo_id < 0)
        return false;
    undo_tasks.push_back(TaskPtr(new UndoTask(text, undo_id, cur_task_id, UndoTask::UndoOperation::FORMAT)));
    RestrictUndo();
    return true;
}

bool Document::RestoreUndo(const int undo_id, std::vector<ElementPtr>& elements)
{
    return undo_base.Restore(undo_id, elements);
}

bool Document::RestoreUndo(const int undo_id, Config& config)
{
    RestrictUndo();
    return undo_base.Restore(undo_id, config);
}

bool Document::RestoreUndo(const int undo_id, TextFormat& format)
{
    RestrictUndo();
    return undo_base.Restore(undo_id, format);
}

void Document::RollbackUndo()
{
    //remove last undo tasks with one id
    if (undo_tasks.empty())
        return;
    TaskPtr t = undo_tasks.back();
    uint id = t->id;
    while (id == t->id)
    {
        undo_tasks.pop_back();
        if (undo_tasks.empty())
            break;
        t = undo_tasks.back();
    }
}

size_t Document::GetUndoSize()
{
    return undo_tasks.size();
}

void Document::ResetTasks()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.clear();
    undo_tasks.clear();
    redo_tasks.clear();
    changed = false;
}

uint Document::SetIncludeDocuments(const std::vector<std::string>& files)
{
    Config c = config;
    c.include_documents.documents.clear();
    for (auto& f : files)
        c.include_documents.documents.push_back(Config::IncludeDocument{f});
    return SetConfig(c, true);
}

ElementPtr Document::GetElement(const ElementId& _id)
{
    if (_id.empty())
        return nullptr;
    if (_id.size() == 1)
        return text;
    ElementPtr el = text->elements->Get(_id[1]);
    for (uint i = 2; i < _id.size(); ++i)
    {
        if (!el || el->elements->Count() < _id[i])
            return nullptr;
        if (IsString(el))
        {
            if (_id.size() == i + 1)
                return el;
            return nullptr;
        }
        el = el->elements->Get(_id[i]);
    }
    return el;
}

ElementPtr Document::GetLogicalElement(const LogicalId& _id)
{
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    if (elements.empty())
        return nullptr;
    return elements[0];
}

void Document::GetElements(const LogicalId& _id, std::vector<ElementPtr>& elements)
{
    if (_id.empty())
        return;
    if (_id.size() == 1)
    {
        elements.push_back(text);
        return;
    }
    ElementPtr el = text->elements->Get(_id[1]);
    if (!el)
        return;
    if (_id.size() == 2)
    {
        elements.push_back(el);
        return;
    }
    
    auto find_in_paragraph = 
        [_id](ElementPtr el, int pos)
        {
            ElementPtr res;
            LogicalId part_id(_id, pos);

            for (size_t i = 0; i < el->elements->Count(); ++i)
            {
                auto row = el->elements->Get(i);
                if (row->elements->Count() == 0)
                    continue;
                if (row->elements->Get(0)->logical_id == part_id)
                {
                    res = row->elements->Get(0);
                    break;
                }
                if (row->elements->Get(row->elements->Count() - 1)->logical_id[2] >= part_id[2])
                {
                    for (size_t j = 1; j < row->elements->Count(); ++j)
                    {
                        auto ch = row->elements->Get(j);
                        if (ch->logical_id == part_id)
                        {
                            res = ch;
                            break;
                        }
                    }
                    break;
                }
            }
            return res;
        };
    
    el = find_in_paragraph(el, 3);
    if (!el)
        return;
    
    assert(!IsParagraph(el));

    for (uint i = 3; i < _id.size(); ++i)
    {
        if (!el || el->elements->Count() < _id[i])
            return;
        if (_id[i] > 0 && _id[i] == el->elements->Count())
        {
            auto ch = el->elements->Get(_id[i - 1]);
            if (!ch || !ch->HasLastCaretState())
                return;
            el = el->elements->Get(_id[i - 1]);
        }
        else
            el = el->elements->Get(_id[i]);
        if (el && el->type == ElementType::PARAGRAPH)
        {
            el = find_in_paragraph(el, i);
            ++i;
        }
    }

    if (!el)
        return;
    elements.push_back(el);

    int pos = GetChildPos(el->id);
    bool found = true;
    auto p = el->parent;

    if (IsParagraph(p->parent->id))
    {
        for (int i = pos - 1; i >= 0; --i) //look for the same logical ids backward
        {
            auto ch = p->elements->Get(i);
            if (ch->logical_id != el->logical_id)
            {
                found = false;
                break;
            }
            elements.insert(elements.begin(), ch);
        }
        if (found)
        {
            int p_pos = GetChildPos(p->id);
            for (int i = p_pos - 1; i >= 0; --i)
            {
                auto r = p->parent->elements->Get(i);
                for (int j = r->elements->Count() - 1; j >= 0; --j)
                {
                    auto ch = r->elements->Get(j);
                    if (ch->logical_id != el->logical_id)
                    {
                        found = false;
                        break;
                    }
                    elements.insert(elements.begin(), ch);
                }
                if (!found)
                    break;
            }
        }
    }

    pos = GetChildPos(el->id) + 1;
    found = true;
    if (IsParagraph(p->parent->id))
    {
        for (int i = pos; i < p->elements->Count(); ++i) //look for the same logical ids forward
        {
            auto ch = p->elements->Get(i);
            if (ch->logical_id != el->logical_id)
            {
                found = false;
                break;
            }
            elements.push_back(ch);
        }
        if (!found)
            return;

        int p_pos = GetChildPos(p->id);
        for (int i = p_pos + 1; i < p->parent->elements->Count(); ++i)
        {
            auto r = p->parent->elements->Get(i);
            for (int j = 0; j < r->elements->Count(); ++j)
            {
                auto ch = r->elements->Get(j);
                if (ch->logical_id != el->logical_id)
                {
                    found = false;
                    break;
                }
                elements.push_back(ch);
            }
            if (!found)
                break;
        }
    }
    else
    {
        while (pos + 1 < p->elements->Count())
        {
            auto ch = p->elements->Get(pos + 1);
            if (ch->logical_id != el->logical_id)
                break;
            elements.push_back(ch);
        }
    }
}

ElementPtr Document::GetParent(const ElementId& _id)
{
    if (_id.size() == 1 || _id.empty())
        return nullptr;
    if (_id.size() == 2)
        return text;
    ElementPtr el = text->elements->Get(_id[1]);
    for (uint i = 2; i < _id.size() - 1; ++i)
    {
        if (!el || el->elements->Count() <= _id[i])
            return nullptr;
        el = el->elements->Get(_id[i]);
    }
    return el;
}

ElementPtr Document::GetLogicalParent(const LogicalId& _id)
{
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    if (elements.empty())
        return nullptr;
    return GetElement(elements[0]->parent->id);
}

bool Document::GetElementAtCoords(const int x, const int y, const int margin, ElementId& id)
{
    bool r = false;
    if (edit_mutex.try_lock())
    {
        r = text->GetElementAtCoords(x, y, margin, id);
        edit_mutex.unlock();
    }
    return r;
}

bool Document::GetElementRect(const ElementId id, Rect& rect)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto _el = GetElement(id);
    if (!_el)
        return false;
    rect = _el->GetAbsoluteRect();
    return true;
}

bool Document::GetCaretRect(Rect& rect)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (!caret->GetElement())
        return false;
    auto _el = GetElement(caret->GetElement()->id);
    if (!_el)
        return false;
    rect = _el->GetAbsoluteRect(_el->GetCaretRect(caret->GetPos()));
    return true;
}

LogicalId Document::GetLogicalId(const ElementId& _id)
{
    if (_id.size() <= 2)
    {
        LogicalId id;
        std::copy(_id.begin(), _id.end(), std::back_inserter(id));
        return id;
    }
    return GetLogicalId(yutovo::GetParent(_id), yutovo::GetChildPos(_id));
}

LogicalId Document::GetLogicalId(const ElementId& _id, const int pos)
{
    LogicalId res;
    auto el = GetElement(_id);
    if (IsString(el))
    {
        int _pos = pos;
        auto r = el->parent;
        auto p = r->parent;
        int r_p = GetChildPos(el->id);
        for (int i = r_p - 1; i >= 0; --i)
        {
            auto ch = r->elements->Get(i);
            if (ch->logical_id == el->logical_id)
                _pos += ch->elements->Count();
            else
                break;
        }
        int p_p = GetChildPos(p->id, r->id);
        for (int i = p_p - 1; i >= 0; --i)
        {
            auto r = p->elements->Get(i);
            for (int j = r->elements->Count() - 1; j >= 0; --j)
            {
                auto ch = r->elements->Get(j);
                if (ch->logical_id == el->logical_id)
                    _pos += ch->elements->Count();
                else
                {
                    res = el->logical_id;
                    res.push_back(_pos);
                    return res;
                }
            }
        }
        res = el->logical_id;
        res.push_back(_pos);
        return res;
    }
    else if (el->type == ElementType::PARAGRAPH)
    {
        return el->logical_id;
    }

    if (pos > 0 && pos == el->elements->Count())
    {
        res = GetElement(GetChild(_id, pos - 1))->logical_id;
        ++res[res.size() - 1];
        return res;
    }
    return GetElement(GetChild(_id, pos))->logical_id;
}

ElementId Document::GetElementId(const LogicalId& _id, bool& last_pos)
{
    return GetElementId(yutovo::GetParent(_id), yutovo::GetChildPos(_id), last_pos);
}

ElementId Document::GetElementId(const LogicalId& _id, const int pos, bool& last_pos)
{
    ElementId res;
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    int p = 0;
    for (size_t i = 0; i < elements.size(); ++i)
    {
        auto& _el = elements[i];
        res = _el->id;
        if (_el->type == ElementType::PARAGRAPH)
        {
            LogicalId last_id;
            for (size_t j = 0; j < _el->elements->Count(); ++j)
            {
                auto r = _el->elements->Get(j);
                for (int k = 0; k < r->elements->Count(); ++k)
                {
                    auto ch = r->elements->Get(k);
                    if (last_id.empty())
                        last_id = ch->logical_id;
                    if (ch->logical_id != last_id)
                    {
                        last_id = ch->logical_id;
                        ++p;
                    }
                    if (pos - p <= r->elements->Count())
                    {
                        res = r->id;
                        res.push_back(pos - p);
                        if (pos - p == r->elements->Count())
                            last_pos = true;
                        else
                            p += r->elements->Count() - k - 1;
                        break;
                    }
                    else if (r->elements->Count() == k + 1 && _el->elements->Count() == j + 1 && GetChildPos(ch->logical_id) == pos - 1)
                    {
                        res = r->id;
                        res.push_back(r->elements->Count());
                        last_pos = true;
                        break;
                    }
                }
            }
        }
        else if (pos - p <= _el->elements->Count())
        {
            res.push_back(pos - p);
            break;
        }
        p += _el->elements->Count();
    }
    return res;
}

ElementId Document::GetParentId(const ElementId& id, const ElementType type)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = FindParent(id, type);
    if (!el)
        return ElementId{};
    return el->id;
}

ElementPtr Document::FindElementOrParent(const ElementId& id, const ElementType type)
{
    ElementPtr el = GetElement(id);
    if (el && el->type == type)
        return el;
    return FindParent(id, type);
}

ElementPtr Document::FindParent(const ElementId& id, const ElementType type)
{
    ElementPtr el = GetParent(id);
    while (el && el->type != type)
        el = GetParent(el->id);
    return el;
}

ElementId Document::FindCurrentParentByType(const ElementType type)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = caret->GetElement();
    if (!el)
        return ElementId{};
    auto p = FindParent(el->id, type);
    return p ? p->id : ElementId{};
}

ElementPtr Document::FindParentParagraph(const ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (el && (el->type == ElementType::PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH_ASSIGNMENT))
        return el;
    el = GetParent(id);
    while (el && el->type != ElementType::PARAGRAPH && el->type != ElementType::CODE_PARAGRAPH && el->type != ElementType::CODE_PARAGRAPH_ASSIGNMENT)
        el = GetParent(el->id);
    return el;
}

ElementPtr Document::FindParentRow(const ElementId& id)
{
    ElementPtr el = GetElement(id);
    if (el && (el->type == ElementType::ROW || el->type == ElementType::CODE_ROW || el->type == ElementType::CODE_ROW_ASSIGNMENT))
        return el;
    el = GetParent(id);
    while (el && el->type != ElementType::ROW && el->type != ElementType::CODE_ROW && el->type != ElementType::CODE_ROW_ASSIGNMENT)
        el = GetParent(el->id);
    return el;
}

uint Document::FindCodeBlock(const ElementId& id)
{
    auto el = FindParent(id, ElementType::CODE_BLOCK);
    if (!el)
        return 0;
    CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
    return c->code_id;
}

ElementPtr Document::FindByString(const ElementId& start_id, const std::u32string& str)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (start_id.empty() || str.empty())
        return nullptr;
    ElementPtr el = GetElement(start_id);
    if (!el)
        return nullptr;
    
    ElementPtr res = nullptr;
    if (IsString(el))
    {
        if (el->ToText() == str)
            return el;
    }
    else
    {
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            res = FindByString(el->elements->Get(i)->id, str);
            if (res)
                break;
        }
    }
    return res;
}

ElementPtr Document::FindByType(const ElementId& start_id, const ElementType type)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (start_id.empty())
        return nullptr;
    ElementPtr el = GetElement(start_id);
    if (!el)
        return nullptr;
    
    ElementPtr res = nullptr;
    if (el->type == type)
        return el;
    else if (IsString(el))
    {
        return nullptr;
    }
    else
    {
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            res = FindByType(el->elements->Get(i)->id, type);
            if (res)
                break;
        }
    }
    return res;
}

bool Document::GetLink(const ElementId& id, std::u32string& str, std::u32string& url)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el || el->type != ElementType::LINK)
        return false;
    Link* link = (Link*)el.get();
    str = link->ToText();
    url = link->url;
    return true;
}

Rect Document::GetCaretRect(const CaretState& caret_state)
{
    ElementPtr el = GetParent(caret_state.id);
    return el->GetAbsoluteRect(el->GetCaretRect(caret_state.GetPos()));
}

bool Document::GetCurrentStringFormat(StringFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        format = current_string_format;
        return true;
    }
    return false;
}

void Document::SetCurrentStringFormat(StringFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    current_string_format = format;
}

bool Document::GetCurrentParagraphFormat(ParagraphFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_paragraph_format)
    {
        format = current_paragraph_format;
        return true;
    }
    return false;
}

uint Document::SetCurrentParagraphFormat(const std::string& name, bool with_undo)
{
    LOG_TRACE("Set current paragraph format: {}", name);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    current_paragraph_format = paragraph_formats->GetFormat(name, config.language);
    if (current_paragraph_format)
        return ChangeParagraphFormat(current_paragraph_format, with_undo);
    return 0;
}

bool Document::GetCurrentFormulaFormat(FormulaFormatPtr& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_formula_format)
    {
        format = current_formula_format;
        return true;
    }
    return false;
}

void Document::SetCurrentFormulaFormat(const std::string& name)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    current_formula_format = formula_formats->GetFormat("Code");
}

void Document::ChangeCurrentFormulaFormat(const StringFormatPtr& string_format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    current_formula_format = formula_formats->GetFormat(current_formula_format->name, string_format, current_formula_format->inter_spacing, 
        current_formula_format->left_margin, current_formula_format->top_margin, current_formula_format->right_margin, 
        current_formula_format->bottom_margin, current_formula_format->color, current_formula_format->bg_color, current_formula_format->bg_selection_color);
}

bool Document::GetGraphFormat(const ElementId& id, GraphFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el || el->type != ElementType::GRAPH_LINE)
        return false;
    format = ((GraphLine*)el.get())->format;
    return true;
}

bool Document::GetGraphImage(const ElementId& id, std::vector<unsigned char>& png)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el || el->type != ElementType::GRAPH_LINE)
        return false;
    std::string image_base64;
    ((GraphLine*)el.get())->GetImage(image_base64);
    png = Base64Decode(image_base64);
    return !png.empty();
}

uint Document::SetGraphFormat(const ElementId& id, const GraphFormat& format, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    std::function<bool()> func = 
        [id, format, with_undo, this]()
        {
            ElementPtr el = GetElement(id);
            if (!el || el->type != ElementType::GRAPH_LINE)
                return false;
            if (with_undo)
                StoreUndo(el->id);
            ((GraphLine*)el.get())->format = format;
            return true;
        };
    tasks.emplace_back(new SetFormatTask(text, id, func, with_undo));
    last_task_id = tasks.back()->id;
    next_circle = true;
    return last_task_id;
}

bool Document::GetPlotFormat(const ElementId& id, PlotFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el || el->type != ElementType::GRAPH_LINE)
        return false;
    ((GraphLine*)el.get())->GetPlotFormat(format);
    return true;
}

uint Document::SetPlotFormat(const ElementId& id, const PlotFormat& format, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    std::function<bool()> func = 
        [id, format, with_undo, this]()
        {
            ElementPtr el = GetElement(id);
            if (!el || el->type != ElementType::GRAPH_LINE)
                return false;
            if (with_undo)
                StoreUndo(el->id);
            ((GraphLine*)el.get())->SetPlotFormat(format);
            return true;
        };
    tasks.emplace_back(new SetFormatTask(text, id, func, with_undo));
    last_task_id = tasks.back()->id;
    next_circle = true;
    return last_task_id;
}

bool Document::GetTextFormat(TextFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    format = *((Text*)text.get())->format;
    return true;
}

uint Document::SetTextFormat(const TextFormat& format, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    std::function<bool()> func = 
        [format, with_undo, this]()
        {
            if (with_undo)
            {
                TextFormat f;
                GetTextFormat(f);
                StoreUndo(f);
            }
            ((Text*)text.get())->SetTextFormat(format);
            return true;
        };
    tasks.emplace_back(new SetFormatTask(text, text->id, func, with_undo));
    last_task_id = tasks.back()->id;
    next_circle = true;
    return last_task_id;
}

void Document::UpdateFormats()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    CaretState c = caret->GetCaretState();
    ElementPtr el = GetParent(c.id);
    if (!el)
    {
        current_string_format.reset();
        return;
    }
    StringFormat f;
    if (GetStringFormat(c.id, f))
    {
        current_string_format = string_formats->GetFormat(f);
        window->OnFormatChanged(MakeEditorState());
        if (FindCodeBlock(c.id) != 0)
            ChangeCurrentFormulaFormat(current_string_format);
    }
    ParagraphFormat p;
    if (GetParagraphFormat(c.id, p))
    {
        auto f = paragraph_formats->GetFormat(p.name, config.language);
        if (f)
            current_paragraph_format = f;
    }
}

uint Document::SetFontFamily(const std::string& family)
{
    LOG_TRACE("Set font family: {}", family);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        if (!selection.IsEmpty())
        {
            return ChangeStringFormat(string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, 
                current_string_format->subscript, current_string_format->superscript, current_string_format->text_color, 
                current_string_format->text_bg_color, current_string_format->text_bg_selection_color), true);
        }
        else
        {
            current_string_format = string_formats->GetFormat(family, current_string_format->size, current_string_format->bold, 
                current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, 
                current_string_format->subscript, current_string_format->superscript, current_string_format->text_color, 
                current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        }
    }
    return 0;
}

uint Document::SetFontSize(const uint size)
{
    LOG_TRACE("Set font size: {}", size);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, size, current_string_format->bold, current_string_format->italic, 
            current_string_format->underline, current_string_format->strikethrough, current_string_format->subscript, current_string_format->superscript, 
            current_string_format->text_color, current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, true, false, false, false, false, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetBold(const bool enabled)
{
    LOG_TRACE("Set bold: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, enabled, current_string_format->italic, 
            current_string_format->underline, current_string_format->strikethrough, current_string_format->subscript, current_string_format->superscript, 
            current_string_format->text_color, current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, true, false, false, false, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetItalic(const bool enabled)
{
    LOG_TRACE("Set italic: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, enabled, 
            current_string_format->underline, current_string_format->strikethrough, current_string_format->subscript, current_string_format->superscript, 
            current_string_format->text_color, current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, true, false, false, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetUnderline(const bool enabled)
{
    LOG_TRACE("Set underline: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, enabled, current_string_format->strikethrough, current_string_format->subscript, 
            current_string_format->superscript, current_string_format->text_color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, true, false, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetStrikethrough(const bool enabled)
{
    LOG_TRACE("Set strikethrough: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, enabled, current_string_format->subscript, current_string_format->superscript, 
            current_string_format->text_color, current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, true, false, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetSubscript(const bool enabled)
{
    LOG_TRACE("Set subscript: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, enabled, 
            enabled ? false : current_string_format->superscript, 
            current_string_format->text_color, current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, false, true, false, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetSuperscript(const bool enabled)
{
    LOG_TRACE("Set superscript: {}", enabled);
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, 
            enabled ? false : current_string_format->subscript, enabled, 
            current_string_format->text_color, current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, false, false, true, false, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetColor(const Color color)
{
    LOG_TRACE("Set color: {}", color.ToString());
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, 
            current_string_format->subscript, current_string_format->superscript, color, 
            current_string_format->text_bg_color, current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, false, false, false, true, false, true);
        else
            current_string_format = f;
    }
    return 0;
}

uint Document::SetBgColor(const Color color)
{
    LOG_TRACE("Set background bold: {}", color.ToString());
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    if (current_string_format)
    {
        auto f = string_formats->GetFormat(current_string_format->family, current_string_format->size, current_string_format->bold, 
            current_string_format->italic, current_string_format->underline, current_string_format->strikethrough, 
            current_string_format->subscript, current_string_format->superscript, current_string_format->text_color, color, 
            current_string_format->text_bg_selection_color);
        if (!selection.IsEmpty())
            return ChangeStringFormat(f, false, false, false, false, false, false, false, false, false, true, true);
        else
            current_string_format = f;
    }
    return 0;
}

void Document::SetInsertMode(const bool enabled)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    insert_mode = enabled;
}

void Document::SwitchInsertMode()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    insert_mode = !insert_mode;
    caret->Blink();
}

bool Document::GetInsertMode()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return insert_mode;
}

ElementPtr Document::CreateParagraph(const ElementId& id)
{
    if (FindElementOrParent(id, ElementType::CODE_BLOCK))
        return ElementPtr(new CodeParagraph<>(this, true));
    return ElementPtr(new Paragraph(this, true));
}

ElementType Document::GetCurrentElementType()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = caret->GetElement();
    if (!el)
        return ElementType::NONE;
    return el->type;
}

ElementType Document::GetElementType(const ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
        return ElementType::NONE;
    return el->type;
}

bool Document::IsEditable(const ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
    {
        //check it's the last caret position
        auto p = GetParent(id);
        if (!p || p->elements->Count() == 0)
            return false;
        if (id[id.size() - 1] == p->elements->Count() && p->editable)
            return true;
        return false;
    }
    return el->editable;
}

bool Document::IsEmpty()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return text->IsEmpty();
}

bool Document::IsResizable(const ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
        return false;
    return el->can_resize;
}

bool Document::IsString(ElementPtr el)
{
    return el && (el->type == ElementType::STRING || el->type == ElementType::CODE_STRING || el->type == ElementType::LINK);
}

bool Document::IsString(ElementId id)
{
    return IsString(GetElement(id));
}

bool Document::IsRow(ElementPtr el)
{
    return el && (el->type == ElementType::ROW || el->type == ElementType::CODE_ROW || el->type == ElementType::CODE_ROW_ASSIGNMENT);
}

bool Document::IsRow(ElementId id)
{
    return IsRow(GetElement(id));
}

bool Document::IsParagraph(ElementPtr el)
{
    return el && (el->type == ElementType::PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH || el->type == ElementType::CODE_PARAGRAPH_ASSIGNMENT);
}

bool Document::IsParagraph(ElementId id)
{
    return IsParagraph(GetElement(id));
}

bool Document::IsFormula(ElementPtr el)
{
    return dynamic_cast<Formula*>(el.get());
}

bool Document::GetStringFormat(const ElementId id, StringFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    ElementPtr p = GetParent(id);
    if (el && (el->type == ElementType::STRING || el->type == ElementType::CODE_STRING || el->type == ElementType::LINK))
    {
        auto f = el->GetStringFormat();
        format = *f;
        return true;
    }
    else if (p && (p->type == ElementType::STRING || p->type == ElementType::CODE_STRING))
    {
        auto f = p->GetStringFormat();
        format = *f;
        return true;
    }
    else if (IsParagraph(el))
    {
        //check if the paragraph has strings with only one format
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            auto row = el->elements->Get(i);
            for (int j = 0; j < row->elements->Count(); ++j)
            {
                auto ch = row->elements->Get(j);
                if (!IsString(ch))
                    return false;
                auto f = ch->GetStringFormat();
                if (i != 0 && j != 0)
                {
                    if (*f != format)
                        return false;
                }
                format = *f;
            }
        }
        return true;
    }
    else if (IsRow(el))
    {
        //check if the row has strings with only one format
        for (int i = 0; i < el->elements->Count(); ++i)
        {
            auto ch = el->elements->Get(i);
            if (!IsString(ch))
            {
                auto f = el->GetStringFormat();
                if (!f)
                    return false;
                format = *f;
                break;
            }
            auto f = ch->GetStringFormat();
            if (i != 0)
            {
                if (*f != format)
                    return false;
            }
            format = *f;
        }
        return true;
    }

    el = FindParentRow(id);
    if (el)
    {
        CaretState c;
        if (el->GetLastCaretState(c, nullptr) && c.id == id)
        {
            //find previous string format
            for (int i = el->elements->Count() - 1; i >= 0; --i)
            {
                auto s = el->elements->Get(i);
                if (s->type == ElementType::STRING || s->type == ElementType::CODE_STRING)
                {
                    auto f = s->GetStringFormat();
                    format = *f;
                    return true;
                }
            }
        }
        else if (el->GetFirstCaretState(c, nullptr) && c.id == id)
        {
            //find next string format
            for (int i = 0; i < el->elements->Count(); ++i)
            {
                auto s = el->elements->Get(i);
                if (s->type == ElementType::STRING || s->type == ElementType::CODE_STRING)
                {
                    auto f = s->GetStringFormat();
                    format = *f;
                    return true;
                }
            }
        }

        auto f = el->GetStringFormat();
        format = *f;
        return true;
    }
    return false;
}

bool Document::GetParagraphFormat(const ElementId id, ParagraphFormat& format)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetParent(id);
    if (!el)
        return false;
    ParagraphFormatPtr p = el->GetParagraphFormat();
    if (!p)
        return false;
    format = *p;
    return true;
}

uint Document::MoveCaret(MoveCaretTask::MoveCaretDir dir, bool select, bool with_last_task_id, bool move_into_view)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        MoveCaretTask* t;
        if (with_last_task_id)
            t = new MoveCaretTask(text, caret, dir, true, select, last_task_id);
        else
            t = new MoveCaretTask(text, caret, dir, true, select);
        t->move_into_view = move_into_view;
        tasks.emplace_back(t);
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::MoveCaretLeft(bool select, bool with_last_task_id)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::LEFT, select, with_last_task_id);
}

uint Document::MoveCaretRight(bool select, bool with_last_task_id)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::RIGHT, select, with_last_task_id);
}

uint Document::MoveCaretUp(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::UP, select);
}

uint Document::MoveCaretDown(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOWN, select);
}

uint Document::MoveCaretHome(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::HOME, select);
}

uint Document::MoveCaretEnd(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::END, select);
}

uint Document::MoveCaretPageUp(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::PAGE_UP, select);
}

uint Document::MoveCaretPageDown(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::PAGE_DOWN, select);
}

uint Document::MoveCaretWordLeft(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::WORD_LEFT, select);
}

uint Document::MoveCaretWordRight(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::WORD_RIGHT, select);
}

uint Document::MoveCaretToDocumentBegin(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_BEGIN, select);
}

uint Document::MoveCaretToDocumentEnd(bool select)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_END, select);
}

uint Document::MoveCaretToDocumentEnd(bool select, bool move_into_view)
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::DOCUMENT_END, select, false, move_into_view);
}

uint Document::MoveCaret(const int x, const int y, bool click)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, Point{x, y}, 
            click ? MoveCaretTask::MoveCaretDir::CLICK_LINK : MoveCaretTask::MoveCaretDir::POINT));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::SelectAll()
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::SELECT_ALL, true, false, false);
}

uint Document::Select(const int start_x, const int start_y, const int end_x, const int end_y)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, Point{start_x, start_y}, Point{end_x, end_y}));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::SelectOut()
{
    return MoveCaret(MoveCaretTask::MoveCaretDir::SELECT_OUT, true);
}

void Document::SetCaretVisible(bool visible)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MoveCaretTask(text, caret, MoveCaretTask::MoveCaretDir::NONE, visible));
    }
    next_circle = true;
}

void Document::CaretMoved()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    caret_hilight_id.clear();

    ElementPtr el = GetElement(last_caret_state.caret_state.id);
    if (!el && last_caret_state.caret_state.last_pos)
        el = GetParent(last_caret_state.caret_state.id);
    if (el)
    {
        Element* p = el->parent;
        while (p && !p->has_caret_hilight)
            p = p->parent;
        if (p)
            Redraw(p->parent->id, false);

        p = el->parent;
        while (p && !p->has_frame_hilight)
            p = p->parent;
        if (p)
            Redraw(p->parent->id, false);
    }

    last_caret_state = MakeEditorState();
    el = GetElement(last_caret_state.caret_state.id);
    if (!el && last_caret_state.caret_state.last_pos)
        el = GetParent(last_caret_state.caret_state.id);
    if (el)
    {
        Element* p = el->parent;
        while (p && !p->has_caret_hilight)
            p = p->parent;
        if (p)
        {
            caret_hilight_id = p->id;
            Redraw(p->id, false);
        }

        p = el->parent;
        while (p && !p->has_frame_hilight)
            p = p->parent;
        if (p)
            Redraw(p->parent->id, false);
    }

    window->OnCaretMoved(last_caret_state);
}

bool Document::MouseLButtonDown(const int x, const int y, MouseHoldType& hold_type, ElementId& hold_id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementId id;
    int m = config.resize_margin_width;
    if (GetElementAtCoords(x, y, m, id))
    {
        if (IsResizable(id))
        {
            Rect rect;
            if (GetElementRect(id, rect))
            {
                if (x <= rect.left + m && y <= rect.top + m)
                    resize_dir = ResizeDir::BothTopLeft;
                else if (x <= rect.left + m && y >= rect.GetBottom() - m)
                    resize_dir = ResizeDir::BothBottomLeft;
                else if (x >= rect.GetRight() - m && y <= rect.top + m)
                    resize_dir = ResizeDir::BothTopRight;
                else if (x >= rect.GetRight() - m && y >= rect.GetBottom() - m)
                    resize_dir = ResizeDir::BothBottomRight;
                else if (x <= rect.left + m)
                    resize_dir = ResizeDir::HorizontalLeft;
                else if (x <= rect.GetRight() + m && x >= rect.GetRight() - m)
                    resize_dir = ResizeDir::HorizontalRight;
                else if (y <= rect.top + m)
                    resize_dir = ResizeDir::VerticalLeft;
                else if (y <= rect.GetBottom() + m && y >= rect.GetBottom() - m)
                    resize_dir = ResizeDir::VerticalRight;
                mouse_capture_id = id;
                last_mouse_pos.Set(x, y);
                hold_type = MouseHoldType::RESIZING;
                hold_id = id;
                return true;
            }
        }
    }

    resize_dir = ResizeDir::None;

    if (GetElementAtCoords(x, y, 0, id))
    {
        ElementPtr el = GetElement(id);
        if (el)
        {
            if (el->MouseLButtonHold(x, y, hold_type, hold_id))
                return true;
            if (el->can_move_picture)
            {
                mouse_capture_id = id;
                last_mouse_pos.Set(x, y);
                hold_type = MouseHoldType::MOVING;
                hold_id = id;
                return true;
            }
        }
    }
    return false;
}

bool Document::MouseLButtonUp(const int x, const int y)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    resize_dir = ResizeDir::None;
    mouse_capture_id = ElementId{};
    return false;
}

bool Document::MouseMove(const int x, const int y)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(mouse_capture_id);
    if (!el)
        return false;

    if (resize_dir != ResizeDir::None)
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        switch (resize_dir)
        {
        case ResizeDir::HorizontalLeft:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, last_mouse_pos.x - x, 0));
            break;
        case ResizeDir::HorizontalRight:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, x - last_mouse_pos.x, 0));
            break;
        case ResizeDir::VerticalLeft:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, 0, last_mouse_pos.y - y));
            break;
        case ResizeDir::VerticalRight:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, 0, y - last_mouse_pos.y));
            break;
        case ResizeDir::BothTopLeft:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, last_mouse_pos.x - x, last_mouse_pos.y - y));
            break;
        case ResizeDir::BothTopRight:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, x - last_mouse_pos.x, last_mouse_pos.y - y));
            break;
        case ResizeDir::BothBottomLeft:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, last_mouse_pos.x - x, y - last_mouse_pos.y));
            break;
        case ResizeDir::BothBottomRight:
            tasks.emplace_back(new ResizeElementTask(text, mouse_capture_id, x - last_mouse_pos.x, y - last_mouse_pos.y));
            break;
        default:
            return false;
        }
        last_mouse_pos.Set(x, y);
        last_task_id = tasks.back()->id;
        next_circle = true;
        return true;
    }

    if (el->can_move_picture)
    {
        if (abs(x - last_mouse_pos.x) < 10 && abs(y - last_mouse_pos.y) < 10)
            return true;
        
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new MovePictureTask(text, mouse_capture_id, x - last_mouse_pos.x, y - last_mouse_pos.y));
        last_mouse_pos.Set(x, y);
        last_task_id = tasks.back()->id;
        next_circle = true;
        return true;
    }

    return false;
}

bool Document::MouseWheel(const int x, const int y, const Point pixel_delta, const Point angle_delta)
{
    ElementId id;
    {
        std::lock_guard<std::recursive_mutex> lock(edit_mutex);
        ElementPtr el = GetElement(mouse_capture_id);
        if (el)
            return false;
        if (!GetElementAtCoords(x, y, 0, id))
            return false;
        el = GetElement(id);
        if (!el || !el->can_move_picture)
            return false;
    }
    
    if (!pixel_delta.IsNull())
    {
        if (pixel_delta.x != 0 || pixel_delta.y != 0)
        {
            std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
            tasks.emplace_back(new ZoomPictureTask(text, id, abs(pixel_delta.x) > abs(pixel_delta.y) ? pixel_delta.x : pixel_delta.y));
            last_task_id = tasks.back()->id;
            next_circle = true;
            return true;
        }
    }
    else if (!angle_delta.IsNull())
    {
        if (angle_delta.x != 0 || angle_delta.y != 0)
        {
            std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
            tasks.emplace_back(new ZoomPictureTask(text, id, abs(angle_delta.x) > abs(angle_delta.y) ? angle_delta.x : angle_delta.y));
            last_task_id = tasks.back()->id;
            next_circle = true;
            return true;
        }
    }
    return false;
}

void Document::Undo()
{
    if (!CanUndo())
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        undos.push_back(true);
    }
    next_circle = true;
}

void Document::Redo()
{
    if (!CanRedo())
        return;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        redos.push_back(true);
    }
    next_circle = true;
}

bool Document::CanUndo()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    return !undo_tasks.empty();
}

bool Document::CanRedo()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    uint last_undo_task_id = 0;
    if (!undo_tasks.empty())
        last_undo_task_id = undo_tasks.back()->id;
    
    int i = 0;
    for (i = (int)redo_tasks.size() - 1; i >= 0; --i)
    {
        if (redo_tasks[i]->id == last_undo_task_id)
            break;
    }

    if (++i < redo_tasks.size())
    {
        uint redo_task_id = redo_tasks[i]->id;
        if (i < redo_tasks.size() && redo_tasks[i]->id == redo_task_id)
            return true;
    }
    return false;
}

void Document::SetChanged(bool _changed)
{
    if (changed != _changed)
    {
        save_task_id = last_modify_task_id;
        UpdateChanged();
    }
}

bool Document::IsChanged()
{
    return changed;
}

uint Document::Resize(uint width, uint height)
{
    break_remake = true;
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new ResizeTask(text, width, height));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Redraw(const ElementId& id, bool move_into_view)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    if (!tasks.empty())
    {
        if (!WillRedraw(id, move_into_view))
        {
            TaskPtr last = tasks.back();
            RedrawTask* t = dynamic_cast<RedrawTask*>(last.get());
            if (!t || t->element_id != id)
                tasks.emplace_back(new RedrawTask(text, id, move_into_view));
            else if (!t->move_into_view)
                t->move_into_view = move_into_view;
        }
    }
    else
    {
        tasks.emplace_back(new RedrawTask(text, id, move_into_view));
    }
    next_circle = true;
    return tasks.back()->id;
}

void Document::Redraw()
{
    Redraw(text->id, false);
}

bool Document::WillRedraw(const ElementId& id, bool move_into_view)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    int i = 0;
    for (auto it = tasks.rbegin(); it != tasks.rend() && i < 10; ++it, ++i)
    {
        TaskPtr& t = *it;
        RedrawTask* redraw_task = dynamic_cast<RedrawTask*>(t.get());
        if (redraw_task && IsChild(redraw_task->element_id, id))
        {
            if (!redraw_task->move_into_view)
                redraw_task->move_into_view = move_into_view;
            return true;
        }
    }
    return false;
}

bool Document::WillResize()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    for (auto it = tasks.rbegin(); it != tasks.rend(); ++it)
    {
        TaskPtr& t = *it;
        if (dynamic_cast<ResizeTask*>(t.get()))
            return true;
    }
    return false;
}

uint Document::New()
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new NewTask(text));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Save(const std::string& filename)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SaveTask(text, filename));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::SaveJson(std::string& json, const int document_id, const bool gzip)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new SaveTask(text, &json, document_id, gzip));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Load(const std::string& filename)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new LoadTask(text, filename, false));
        last_load_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_load_task_id;
}

uint Document::LoadInclude(const std::string& filename)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new LoadTask(text, filename, true));
        last_load_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_load_task_id;
}

uint Document::LoadJson(const std::string& json_doc, const int document_id)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new LoadTask(text, json_doc, document_id, false));
        last_load_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_load_task_id;
}

uint Document::LoadJsonInclude(const std::string& json_doc, const int document_id)
{
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new LoadTask(text, json_doc, document_id, true));
        last_load_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_load_task_id;
}

void Document::ClearIncludes()
{
    std::queue<std::pair<int, std::string>> c;
    std::swap(include_documents, c);
    include_file_guids.clear();
}

void Document::AddInclude(const std::string& filename, const int document_id)
{
    include_documents.push({document_id, filename});
}

void Document::LoadNextInclude()
{
    if (include_documents.empty())
    {
        if (!include_editor_state.IsEmpty())
        {
            std::vector<ElementPtr> els;
            GetElements(yutovo::GetParent(include_editor_state.caret_state.id), els);
            if (!els.empty() && els[0]->visible)
            {
                SetEditorState(include_editor_state);
                include_editor_state.caret_state.id = LogicalId{};
            }
        }
        return;
    }
    auto& p = include_documents.front();
    window->OnLoadInclude(p.second, p.first);
    include_documents.pop();
}

uint Document::Copy(std::u32string& out_json, std::u32string& out_text)
{
    LOG_TRACE("Copy: out_json={}, out_text={}", ToBasicString(out_json), ToBasicString(out_text));
    {
        out_json = U"";
        out_text = U"";
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new CopyTask(text, out_json, out_text, false));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

uint Document::Paste(std::u32string& in_json)
{
    LOG_TRACE("Paste: in_json={}", ToBasicString(in_json));
    StringFormatsPtr _string_formats;
    rapidjson::Document doc;
    auto str = ToBasicString(in_json);
    if (doc.Parse<0>(str.c_str()).HasParseError())
    {
        window->OnPasteResult(PasteResult::PasteError);
        return 0;
    }

    if (doc.HasMember("string_formats") && doc["string_formats"].IsArray())
    {
        //load string formats
        std::lock_guard<std::recursive_mutex> lock(edit_mutex);
        string_formats->FromJson(((const rapidjson::Value&)doc["string_formats"]).GetArray(), doc.GetAllocator());
    }

    if (!doc.HasMember("copy") || !doc["copy"].IsArray())
    {
        window->OnPasteResult(PasteResult::PasteError);
        return 0;
    }

    //load elements
    std::vector<ElementPtr> elements;
    rapidjson::Value::Array arr = doc["copy"].GetArray();
    for (rapidjson::SizeType i = 0; i < arr.Size(); ++i)
    {
        if (!arr[i].IsObject())
            return 0;
        rapidjson::Value::Object value = arr[i].GetObject();
        ElementPtr el(CreateFromJson(nullptr, this, (rapidjson::Value::ConstObject&)value, doc.GetAllocator()));
        if (!el)
            return 0;
        elements.push_back(el);
    }

    if (elements.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return last_task_id;
    }

    bool only_formulas = true;
    bool only_paragraphs = true;
    for (auto& el : elements)
    {
        if (!el->IsFormula())
            only_formulas = false;
        if (!IsParagraph(el))
            only_paragraphs = false;
    }

    if (only_formulas)
        InsertFormulas(elements, true, false, true, false, -1);
    else if (only_paragraphs)
    {
        //compound them in text to workout them in one iteration
        ElementPtr t(new Text(this, current_text_format, false));
        for (auto& el : elements)
            t->elements->Add(el);
        elements.clear();
        elements.push_back(t);
        InsertElements(elements, true, true);
    }
    else
        InsertElements(elements, true, true);
    
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::PasteText(std::u32string&& str)
{
    LOG_TRACE("Paste text: {}", ToBasicString(str));
    if (str.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertString(str, true, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::PasteImage(const std::vector<unsigned char>& image)
{
    LOG_TRACE("Paste image");
    if (image.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertImage(image, true, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::PasteImage(const std::string& image_base64)
{
    LOG_TRACE("Paste image");
    if (image_base64.empty())
    {
        window->OnPasteResult(PasteResult::EmptyBuffer);
        return 0;
    }

    InsertImage(image_base64, true, true);
    window->OnPasteResult(PasteResult::Success);
    return last_task_id;
}

uint Document::Cut(std::u32string& out_json, std::u32string& out_text)
{
    LOG_TRACE("Cut: out_json={}, out_text={}", ToBasicString(out_json), ToBasicString(out_text));
    {
        std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
        tasks.emplace_back(new CopyTask(text, out_json, out_text, true));
        last_task_id = tasks.back()->id;
    }
    next_circle = true;
    return last_task_id;
}

std::string Document::ToHtml()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return text->ToHtml();
}

std::u32string Document::ToText()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return text->ToText();
}

std::u32string Document::ToText(const ElementId& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    ElementPtr el = GetElement(id);
    if (!el)
        return U"";
    return el->ToText();
}

TextFormatPtr Document::GetDefaultTextFormat()
{
    return TextFormats::GetFormat(default_text_format.paging, default_text_format.left_indent, default_text_format.top_indent, 
        default_text_format.right_indent, default_text_format.bottom_indent, default_text_format.paragraph_spacing, default_text_format.size);
}

StringFormatPtr Document::GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
    bool subscript, bool superscript)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, subscript, superscript, Color::Black(), Color::White(), Color::Blue());
}

StringFormatPtr Document::GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
    bool subscript, bool superscript, Color text_color, Color text_bg_color)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, subscript, superscript, text_color, text_bg_color, Color::Blue());
}

StringFormatPtr Document::GetStringFormat(const std::string& family, uint size, bool bold, bool italic, bool underline, bool strikethrough, 
    bool subscript, bool superscript, Color text_color, Color text_bg_color, Color text_bg_selection_color)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(family, size, bold, italic, underline, strikethrough, subscript, superscript, text_color, text_bg_color, 
        text_bg_selection_color);
}

StringFormatPtr Document::GetStringFormat(const boost::uuids::uuid& id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->GetFormat(id);
}

void Document::SaveStringFormats(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    string_formats->ToJson(value, alloc);
}

void Document::SaveStringFormats(rapidjson::Value& value, rapidjson::Document::AllocatorType& alloc, const std::vector<ElementPtr>& elements)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    string_formats->ToJson(value, alloc, elements);
}

bool Document::LoadStringFormats(const rapidjson::Value::ConstArray& value, rapidjson::Document::AllocatorType& alloc)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return string_formats->FromJson(value, alloc);
}

EditorState Document::GetEditorState()
{
    std::lock_guard<std::recursive_mutex> lock(state_mutex);
    return last_editor_state;
}

EditorState Document::MakeEditorState()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return EditorState{caret->GetCaretState(), selection.GetState()};
}

LogicalEditorState Document::GetLogicalEditorState()
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    return {caret->GetLogicalCaretState(), selection.GetLogicalState()};
}

void Document::SetEditorState(EditorState& state)
{
    selection.Set(state.selection_state);
    caret->SetState(state.caret_state);
}

void Document::SetEditorState(LogicalEditorState& state)
{
    selection.Set(state.selection_state);
    caret->SetState(state.caret_state);
}

void Document::Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::AutoResultConfig& config, bool include_document, 
    std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.Solve(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::RealResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.Solve(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::IntegerResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.Solve(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::RationalResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.Solve(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::ComplexResultConfig& config, bool include_document, 
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.Solve(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::Solve(const LogicalId& _id, const std::string& guid, uint code_id, Config::ArrayRealResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.Solve(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::SolveSymbolicReal(const LogicalId& _id, const std::string& guid, uint code_id, Config::RealResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.SolveSymbolicReal(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::SolveSymbolicRational(const LogicalId& _id, const std::string& guid, uint code_id, Config::RationalResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.SolveSymbolicRational(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::SolveSymbolicComplex(const LogicalId& _id, const std::string& guid, uint code_id, Config::ComplexResultConfig& config, bool include_document,
    const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.SolveSymbolicComplex(_id, guid, code_id, config, include_document, expression + U";", delay);
}

void Document::BreakSolving(const LogicalId& _id, const std::string& guid, uint code_id, bool wait)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        auto it = solve_ids.find(guid);
        if (it != solve_ids.end())
            solve_ids.erase(it);
    }
    solver.BreakSolving(_id, guid, code_id, wait);
}

void Document::SetIdentifier(const LogicalId& _id, const std::string& guid, uint code_id, Config::AutoResultConfig& config, bool include_document, 
    const std::u32string& identifier, const std::u32string& expression, const uint delay)
{
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        solve_ids[guid] = _id;
    }
    solver.SetIdentifier(_id, guid, code_id, config, include_document, identifier, expression + U";", delay);
}

void Document::RemoveIdentifier(const LogicalId& _id, uint code_id, const std::u32string& identifier, const uint delay)
{
    solver.RemoveIdentifier(_id, code_id, identifier, delay);
}

void Document::RemoveUserIdentifiers()
{
    solver.RemoveUserIdentifiers();
}

void Document::ClearExport()
{
    solver.ClearExport();
}

void Document::RemoveSolver(uint code_id)
{
    solver.RemoveSolver(code_id);
}

ResultType Document::GetResultType(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (el->elements->Count() == 0)
        return ResultType::NONE;
    switch (el->elements->Get(0)->type)
    {
    case ElementType::AUTO_RESULT:
    case ElementType::ERROR_RESULT:
        return ResultType::AUTO;
    case ElementType::REAL_RESULT:
        return ResultType::REAL;
    case ElementType::INTEGER_RESULT:
        return ResultType::INTEGER;
    case ElementType::RATIONAL_RESULT:
        return ResultType::RATIONAL;
    case ElementType::COMPLEX_RESULT:
        return ResultType::COMPLEX;
    case ElementType::ARRAY_REAL_RESULT:
        return ResultType::ARRAY_REAL;
    case ElementType::SYMBOLIC_REAL_RESULT:
        return ResultType::SYMBOLIC_REAL;
    case ElementType::SYMBOLIC_RATIONAL_RESULT:
        return ResultType::SYMBOLIC_RATIONAL;
    case ElementType::SYMBOLIC_COMPLEX_RESULT:
        return ResultType::SYMBOLIC_COMPLEX;
    default:
        return ResultType::NONE;
    }
}

uint Document::SetResultType(ElementId _id, ResultType result_type, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultTypeTask(text, _id, result_type, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

int Document::GetPrecision(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return 0;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.precision;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.precision;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.precision;
    SymbolicRealResult* s_r = dynamic_cast<SymbolicRealResult*>(el.get());
    if (s_r)
        return s_r->config.precision;
    SymbolicComplexResult* sc_r = dynamic_cast<SymbolicComplexResult*>(el.get());
    if (sc_r)
        return sc_r->config.precision;
    return -1;
}

uint Document::SetPrecision(ElementId _id, uint precision, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, precision, -1, AngleMeasure::None, AngleMeasure::None, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

int Document::GetExp(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return 0;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.exp;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.exp;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.exp;
    return -1;
}

uint Document::SetExp(ElementId _id, uint exp, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, -1, exp, AngleMeasure::None, AngleMeasure::None, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

AngleMeasure Document::GetDefaultAngleMeasure(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return AngleMeasure::None;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.default_angle_measure;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.default_angle_measure;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.default_angle_measure;
    return AngleMeasure::None;
}

AngleMeasure Document::GetResultAngleMeasure(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return AngleMeasure::None;
    RealResult* r = dynamic_cast<RealResult*>(el.get());
    if (r)
        return r->config.result_angle_measure;
    AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
    if (a_r)
        return a_r->config.real_result.result_angle_measure;
    ComplexResult* c_r = dynamic_cast<ComplexResult*>(el.get());
    if (c_r)
        return c_r->config.result_angle_measure;
    return AngleMeasure::None;
}

uint Document::SetAngleMeasure(ElementId _id, AngleMeasure default_angle_measure, AngleMeasure result_angle_measure, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, -1, -1, default_angle_measure, result_angle_measure, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

Notation Document::GetResultNotation(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return Notation::None;
    IntegerResult* r = dynamic_cast<IntegerResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.integer_result.result_notation;
        return Notation::None;
    }
    return r->config.result_notation;
}

Notation Document::GetDefaultNotation(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return Notation::None;
    IntegerResult* r = dynamic_cast<IntegerResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.integer_result.default_notation;
        return Notation::None;
    }
    return r->config.default_notation;
}

uint Document::SetNotation(ElementId _id, Notation default_notation, Notation result_notation, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, default_notation, result_notation, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

FractionForm Document::GetFractionForm(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return FractionForm::None;
    RationalResult* r = dynamic_cast<RationalResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.rational_result.fraction_form;
        return FractionForm::None;
    }
    return r->config.fraction_form;
}

uint Document::SetFractionForm(ElementId _id, FractionForm fraction_form, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, fraction_form, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

ComplexForm Document::GetComplexForm(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return ComplexForm::None;
    ComplexResult* r = dynamic_cast<ComplexResult*>(el.get());
    if (!r)
    {
        AutoResult* a_r = dynamic_cast<AutoResult*>(el.get());
        if (a_r)
            return a_r->config.complex_result.form;
        return ComplexForm::None;
    }
    return r->config.form;
}

uint Document::SetComplexForm(ElementId _id, ComplexForm form, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, form, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

bool Document::HasUnit(ElementId _id)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return false;
    
    if (el->type == ElementType::AUTO_RESULT)
    {
        AutoResult* r = dynamic_cast<AutoResult*>(el.get());
        std::vector<yutovo_calculator::Unit> cast_units;
        r->GetCastUnits(cast_units);
        return !cast_units.empty();
    }
    else if (el->type == ElementType::REAL_RESULT)
    {
        RealResult* r = dynamic_cast<RealResult*>(el.get());
        return !r->cast_units.empty();
    }
    else if (el->type == ElementType::RATIONAL_RESULT)
    {
        RationalResult* r = dynamic_cast<RationalResult*>(el.get());
        return !r->cast_units.empty();
    }
    return false;
}

void Document::GetCastUnits(ElementId _id, std::vector<yutovo_calculator::Unit>& cast_units)
{
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    auto el = GetElement(_id);
    if (!el)
        return;
    if (el->type == ElementType::AUTO_RESULT)
    {
        AutoResult* r = dynamic_cast<AutoResult*>(el.get());
        r->GetCastUnits(cast_units);
    }
    else if (el->type == ElementType::REAL_RESULT)
    {
        RealResult* r = dynamic_cast<RealResult*>(el.get());
        cast_units = r->cast_units;
    }
    else if (el->type == ElementType::RATIONAL_RESULT)
    {
        RationalResult* r = dynamic_cast<RationalResult*>(el.get());
        cast_units = r->cast_units;
    }
}

uint Document::SetUnit(ElementId _id, yutovo_calculator::Unit& unit, bool with_undo)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new SetResultParamsTask(text, _id, unit, with_undo));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

uint Document::ReSolve(const ElementId& _id)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResolveTask(text, _id));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

void Document::ReSolve(const LogicalId& _id)
{
    std::vector<ElementPtr> elements;
    GetElements(_id, elements);
    for (auto el : elements)
        ReSolve(el->id);
}

uint Document::ReSolveDependencies(const LogicalId& after_id, const std::u32string& identifier)
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResolveDependenciesTask(text, after_id, ToBasicString(identifier)));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

uint Document::ReSolveErrors()
{
    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    tasks.emplace_back(new ResolveErrorsTask(text));
    last_task_id = tasks.back()->id;
    return last_task_id;
}

uint Document::PutResult(const Result& result)
{
    std::lock_guard<std::recursive_mutex> lock1(edit_mutex);

    LogicalId solve_id;
    {
        std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
        auto it = solve_ids.find(result.guid);
        if (it == solve_ids.end())
            return 0;
        solve_id = it->second;
    }

    std::lock_guard<std::recursive_mutex> lock2(tasks_mutex);
    tasks.emplace_back(new ResultTask(text, solve_id, result));

#ifdef DEBUG
    if (result.error.error_code != yutovo_solver::ErrorCode::SOLVER_RESTARTED_ERROR || result.error.error_code == yutovo_solver::ErrorCode::PARSER_ERROR)
        last_solver_task_id = tasks.back()->id;
#endif
    last_task_id = tasks.back()->id;
    return last_task_id;
}

void Document::AddResolveElement(const LogicalId& _id)
{
    if (std::find(resolve_elements.begin(), resolve_elements.end(), _id) == resolve_elements.end())
        resolve_elements.push_back(_id);
}

void Document::AddChangedElement(const ElementId& _id)
{
    if (std::find(changed_elements.begin(), changed_elements.end(), _id) == changed_elements.end())
        changed_elements.push_back(_id);
}

void Document::GetSolverGuid(std::string& guid)
{
    guid = solver.solver_guid;
}

uint Document::SetLocale(const yutovo_calculator::Language language, bool with_undo)
{
    Config c = config;
    c.language = language;
    return SetConfig(c, with_undo);
}

void Document::ListIdentifiers(const uint code_id)
{
    solver.ListIdentifiers(code_id);
}

void Document::UpdateSolveId(const std::string& guid, const LogicalId& new_id)
{
    std::lock_guard<std::recursive_mutex> lock(solve_ids_mutex);
    auto it = solve_ids.find(guid);
    if (it != solve_ids.end())
        it->second = new_id;
}

bool Document::IsVisible(ElementId _id)
{
    auto el = GetElement(_id);
    if (!el)
        return false;
    if (el->type == ElementType::TEXT)
        return true;
    
    Rect w = window->GetRect();
    w.left += window->document_point.x;
    w.top += window->document_point.y;
    Rect r = el->GetAbsoluteRect();
    if (config.draw_whole) //draw multiply pages
        return !(r.left > w.GetRight() || w.left > r.GetRight() || w.top > r.GetBottom());
    return r.Intersects(w);
}

ElementId Document::GetFirstVisibleParagraph()
{
    ElementId res;
    if (!cur_visible_row.empty())
    {
        auto p = GetParent(cur_visible_row);
        int pos = GetChildPos(p->id);
        if (IsVisible(p->id))
        {
            //the most simple variant - go above and find first invisible paragraph
            res = p->id;
            while (--pos >= 0)
            {
                auto el = p->parent->elements->Get(pos);
                if (!IsVisible(el->id))
                    break;
                res = el->id;
            }
            return res;
        }

        //try to find a visible paragraph above
        for (int i = 0; i < 10 && pos >= 0; ++i, --pos)
        {
            auto el = p->parent->elements->Get(pos);
            if (IsVisible(el->id))
            {
                res = el->id;
                while (--pos >= 0)
                {
                    auto el = p->parent->elements->Get(pos);
                    if (!IsVisible(el->id))
                        break;
                    res = el->id;
                }
                return res;
            }
        }

        //try to find a visible paragraph below
        for (int i = 0; i < 10 && pos >= 0; ++i, ++pos)
        {
            auto el = p->parent->elements->Get(pos);
            if (IsVisible(el->id))
                return el->id;
        }
    }

    cur_visible_row.clear();

    //the most comprehensive variant: find in the whole document
    return text->elements->FindUpper(window->document_point.y);
}

ElementId Document::GetFirstVisibleRow(ElementId paragraph_id)
{
    ElementId res;
    auto el = GetElement(paragraph_id);
    cur_visible_row = el->elements->FindUpper(window->document_point.y);
    return cur_visible_row;
}

void Document::AddErrorMark(const ElementId& _id, int start, int size)
{
    auto el = GetElement(_id);
    if (!el)
        return;
    for (int i = start; i < start + size; ++i)
        el->elements->Get(i)->error_mark = true;
    
    //if all the child elements have error mark - set error mark to the parent
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        if (!el->elements->Get(i)->error_mark)
            return;
    }
    for (int i = 0; i < el->elements->Count(); ++i)
        el->elements->Get(i)->error_mark = false;
    if (el->parent)
        el->parent->elements->Get(yutovo::GetChildPos(el->id))->error_mark = true;
}

void Document::RemoveErrorMarks(const ElementId& parent_id, Dependencies* dependencies)
{
    auto el = GetElement(parent_id);
    if (!el)
        return;
    el->error_mark = false;

    if (dependencies) //remove error marks in the dependencies
    {
        std::vector<ElementId> elements;
        auto c = FindElementOrParent(parent_id, ElementType::CODE_BLOCK);
        c->GetElementsAbove(parent_id, ElementType::ASSIGNMENT, elements);
        for (auto& _id : elements)
        {
            auto _el = GetElement(_id);
            Assignment* s = dynamic_cast<Assignment*>(_el.get());
            for (auto& _d : *dependencies)
            {
                if (s->Depends(_d))
                    RemoveErrorMarks(s->id);
            }
        }

        std::vector<ElementId> code_blocks;
        text->GetElementsAbove(c->id, ElementType::CODE_BLOCK, code_blocks); //find all code blocks above
        for (ElementId code_id : code_blocks)
        {
            auto _el = GetElement(code_id);
            auto* code = dynamic_cast<CodeBlock*>(_el.get());
            elements.clear();
            code->GetElements(ElementType::ASSIGNMENT, elements);
            for (auto& _id : elements)
            {
                auto _el = GetElement(_id);
                Assignment* s = dynamic_cast<Assignment*>(_el.get());
                for (auto& _d : *dependencies)
                {
                    if (s->Depends(_d))
                        RemoveErrorMarks(s->id);
                }
            }
        }
    }

    if (!IsString(el))
    {
        for (int i = 0; i < el->elements->Count(); ++i)
            RemoveErrorMarks(el->elements->Get(i)->id);
    }
}

bool Document::HasErrorMark(const ElementId& _id, int& start, int& size)
{
    auto el = GetElement(_id);
    if (!el)
        return false;
    if (el->error_mark)
    {
        start = 0;
        size = el->elements->Count();
        return true;
    }
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        auto ch = el->elements->Get(i);
        if (ch->error_mark)
        {
            start = i;
            size = 1;
            for (int j = i + 1; j < el->elements->Count(); ++j)
            {
                if (el->elements->Get(j)->error_mark)
                    ++size;
            }
            return true;
        }
    }
    return false;
}

bool Document::HasErrorMarks(const ElementId& _id)
{
    int start, size;
    if (HasErrorMark(_id, start, size))
        return true;
    auto el = GetElement(_id);
    if (!el || IsString(el))
        return false;
    for (int i = 0; i < el->elements->Count(); ++i)
    {
        if (HasErrorMarks(el->elements->Get(i)->id))
            return true;
    }
    return false;
}

void Document::SetIdentifiers(const uint code_id, const std::vector<std::string>& variables, const std::vector<std::string>& functions, 
    const std::vector<std::string>& operations, const std::vector<std::string>& units, const std::vector<std::string>& strings)
{
    {
        std::lock_guard<std::recursive_mutex> lock(identifiers_mutex);
        identifiers[code_id] = Identifiers{variables, functions, units, operations, strings};
    }
    
    std::vector<ElementId> code_blocks;
    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    text->GetElements(ElementType::CODE_BLOCK, code_blocks); //find all code blocks
    for (ElementId _id : code_blocks)
    {
        auto el = GetElement(_id);
        CodeBlock* c = dynamic_cast<CodeBlock*>(el.get());
        if (c && c->code_id == code_id)
            Redraw(_id, false);
    }
}

IdentifierType Document::FindIdentifier(const uint code_id, const std::string& str)
{
    std::lock_guard<std::recursive_mutex> lock(identifiers_mutex);
    auto it = identifiers.find(code_id);
    if (it == identifiers.end())
        return IdentifierType::NONE;
    Identifiers& id = it->second;
    if (std::find(id.variables.begin(), id.variables.end(), str) != id.variables.end())
        return IdentifierType::VARIABLE;
    if (std::find(id.functions.begin(), id.functions.end(), str) != id.functions.end())
        return IdentifierType::FUNCTION;
    if (std::find(id.units.begin(), id.units.end(), str) != id.units.end())
        return IdentifierType::UNIT;
    return IdentifierType::NONE;
}

void Document::GetIdentifiers(const uint code_id, const std::string& left, std::vector<std::pair<IdentifierType, std::string>>& res)
{
    std::lock_guard<std::recursive_mutex> lock(identifiers_mutex);
    auto it = identifiers.find(code_id);
    if (it == identifiers.end())
        return;

    Identifiers& ids = it->second;
    for (auto& var : ids.variables)
    {
        if (var.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::VARIABLE, var));
    }
    for (auto& func : ids.functions)
    {
        if (func.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::FUNCTION, func));
    }
    for (auto& op : ids.operations)
    {
        if (op.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::OPERATION, op));
    }
    for (auto& str : ids.strings)
    {
        if (str.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::STRING, str));
    }
    for (auto& unit : ids.units)
    {
        if (unit.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::UNIT, unit));
    }
}

void Document::GetPrompt(std::vector<std::pair<IdentifierType, std::string>>& res)
{
    uint code_id = 0;
    std::string left;
    {
        std::lock_guard<std::recursive_mutex> lock(edit_mutex);
        auto el = caret->GetElement();
        if (!el)
            return;
        code_id = FindCodeBlock(el->id);
        if (code_id == 0)
            return;
        el = caret->GetElement();
        if (!el || !IsString(el->id))
            return;
        left = ToBasicString(el->ToText().substr(0, caret->GetPos()));
    }

    if (left.empty())
        return;
    
    std::lock_guard<std::recursive_mutex> lock(identifiers_mutex);
    auto it = identifiers.find(code_id);
    if (it == identifiers.end())
        return;

    if (left[0] == '\"')
        left = left.substr(1);

    std::smatch match;
    Identifiers& ids = it->second;
    for (auto& op : ids.operations)
    {
        if (op.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::OPERATION, op));
    }
    for (auto& var : ids.variables)
    {
        if (var.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::VARIABLE, var));
        if (std::regex_match(var, match, subscript_pattern))
        {
            std::string subscript = match[2].matched ? match[2].str() : "";
            if (!subscript.empty())
            {
                if (subscript.find(left, 0) == 0 && std::find(res.begin(), res.end(), std::make_pair(IdentifierType::VARIABLE, subscript)) == res.end())
                    res.push_back(std::make_pair(IdentifierType::VARIABLE, subscript));
            }
        }
    }
    for (auto& func : ids.functions)
    {
        if (func.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::FUNCTION, func));
    }
    for (auto& unit : ids.units)
    {
        if (unit.rfind(left, 0) == 0)
            res.push_back(std::make_pair(IdentifierType::UNIT, unit));
        if (std::regex_match(unit, match, subscript_pattern))
        {
            std::string subscript = match[2].matched ? match[2].str() : "";
            if (!subscript.empty())
            {
                if (subscript.find(left, 0) == 0 && std::find(res.begin(), res.end(), std::make_pair(IdentifierType::UNIT, subscript)) == res.end())
                    res.push_back(std::make_pair(IdentifierType::UNIT, subscript));
            }
        }
    }
    for (auto& str : ids.strings)
    {
        if (str.rfind(left, 0) == 0 && std::find(res.begin(), res.end(), std::make_pair(IdentifierType::VARIABLE, str)) == res.end())
            res.push_back(std::make_pair(IdentifierType::STRING, str));
    }
}

bool Document::WaitTask(uint task_id, uint64_t timeout, uint64_t circle_delay)
{
    if (task_id == 0)
        return true;
    auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    auto cur_time = now;
    while (cur_time - now <= timeout * 1ms)
    {
        {
            std::lock_guard<std::recursive_mutex> lock(last_tasks_mutex);
            if (std::find(last_tasks.begin(), last_tasks.end(), task_id) != last_tasks.end())
                return true;
        }

        std::this_thread::sleep_for(circle_delay * 1ms);

        if (timeout > 0)
            cur_time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }
    return false;
}

void Document::RestrictUndo()
{
    if (undo_tasks.empty())
        return;
    
    uint groups_count = 0;
    uint group_id = 0;
    auto it = undo_tasks.end() - 1;
    for (; it != undo_tasks.begin(); --it) //count groups
    {
        auto& t = *it;
        if (t->id != group_id)
        {
            if (++groups_count > config.undo_size - 1)
                break;
        }
        group_id = t->id;
    }
    if (it == undo_tasks.begin())
        return;
    
    //restrict the queues
    auto d = std::distance(undo_tasks.begin(), it);
    undo_tasks.erase(undo_tasks.begin(), it);
    if (redo_tasks.size() > d)
        redo_tasks.erase(redo_tasks.begin(), redo_tasks.begin() + d);
    save_task_id = -1;
}

void Document::UpdateChanged()
{
    if (save_task_id == 0 && undo_tasks.empty())
        changed = false;
    else
        changed = (last_modify_task_id != save_task_id);
    if (last_changed != changed)
    {
        window->OnDocumentChanged(changed);
        last_changed = changed;
    }
}

int Document::GetCharWidth(const StringFormatPtr& format, const char32_t symbol)
{
    auto& widths = char_widths[format];
    auto w_it = widths.find(symbol);
    if (w_it != widths.end())
        return w_it->second;
    std::u32string s(1, symbol);
    int w = window->GetTextSize(s, format).width;
    widths[symbol] = w;
    return w;
}

#ifdef TEST
void Document::WaitMainLoop()
{
    while (!last_task_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_task_id = 0;
    last_task_executed = false;
}

void Document::WaitUndo()
{
    while (!last_undo_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_undo_executed = false;
}

void Document::WaitRedo()
{
    while (!last_redo_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    last_redo_executed = false;
}

void Document::WaitLoad()
{
    while (!last_load_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    std::lock_guard<std::recursive_mutex> lock(tasks_mutex);
    last_load_task_id = 0;
    last_load_executed = false;
}

void Document::WaitSolver()
{
    while (!last_solver_executed)
    {
        std::this_thread::sleep_for(10ms);
    }

    std::lock_guard<std::recursive_mutex> lock(edit_mutex);
    last_solver_task_id = 0;
    last_solver_executed = false;
}
#endif

void Document::UpdateCaretView()
{
    Element* element = caret->GetElement();
    if (!element)
        return;
    Rect r = element->GetAbsoluteRect(element->GetCaretRect(caret->GetPos()));
    Rect view_port = window->GetViewPort(0);
    Point p = window->GetDocumentPoint();
    
    //move view port in the view if the caret is outside of it
    if (r.left < p.x + view_port.left)
    {
        caret->Hide(); //caret will be shown on Redraw
        window->MoveDocument(r.left - view_port.left - 1, p.y);
        Redraw(text->id, false);
    }
    else if (r.width < view_port.width && r.GetRight() > view_port.GetRight() + p.x)
    {
        caret->Hide();
        window->MoveDocument(r.GetRight() - view_port.GetRight(), p.y);
        Redraw(text->id, false);
    }

    if (r.top < p.y + view_port.top)
    {
        caret->Hide();
        window->MoveDocument(p.x, r.top - view_port.top - 1);
        Redraw(text->id, false);
    }
    else if (r.height < view_port.height && r.GetBottom() > view_port.GetBottom() + p.y)
    {
        caret->Hide();
        window->MoveDocument(p.x, r.GetBottom() - view_port.GetBottom());
        Redraw(text->id, false);
    }
}

void Document::UpdateLastSelection()
{
    if (selection != last_selection)
    {
        for (auto& s : selection.selection)
            Redraw(s.element->id, false);
        for (auto& s : last_selection.selection)
            Redraw(s.element->id, false);
        last_selection = selection;
    }
}

}
