#ifndef __COMMAND_MAP_H__
#define __COMMAND_MAP_H__

#include <QKeySequence>
#include <QWidget>
#include <functional>
#include <variant>
#include <yutovo_editor/document.h>

namespace yutovo
{

enum CommandContext
{
    Everywhere = 1,
    Text,
    Formula
};

struct CommandMap
{
    QKeySequence shortcut;
    QChar symbol;
    std::string command;
    CommandContext context = CommandContext::Everywhere;
};

struct CommandMapVoid : CommandMap
{
    void operator()()
    {
        func();
    }

    std::function<void ()> func;
};

struct CommandMapString : CommandMap
{
    void operator()(const std::string& param)
    {
        func(param);
    }

    std::function<void (const std::string&)> func;
};

typedef std::variant<CommandMapVoid, CommandMapString> CommandMapVariant;

class ShortcutsMap
{
public:
    ShortcutsMap() = default;

    void Init(DocumentPtr _document, QWidget* document_widget);

    bool Call(const QKeySequence& shortcut, QChar symbol, const EditorState& editor_state);

private:
    void Add(QKeySequence shortcut, std::string command, std::function<void (void)> func, CommandContext context = CommandContext::Everywhere);
    void Add(QKeySequence shortcut, QChar symbol, std::string command, std::function<void (void)> func, 
        CommandContext context = CommandContext::Everywhere);
    
    void Add(QKeySequence shortcut, std::string command, std::function<void (const std::string&)> func, 
        CommandContext context = CommandContext::Everywhere);
    void Add(QKeySequence shortcut, QChar symbol, std::string command, std::function<void (const std::string&)> func, 
        CommandContext context = CommandContext::Everywhere);

    void Add(QKeySequence shortcut, std::function<void ()> func);

private:
    DocumentPtr document;
    std::vector<CommandMapVariant> command_maps;
};

}

#endif
