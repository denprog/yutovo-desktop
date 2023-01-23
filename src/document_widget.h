#ifndef __DOCUMENT_WIDGET_H__
#define __DOCUMENT_WIDGET_H__

#include <QWidget>
#include <yutovo_editor/document.h>
#include "qt_window.h"
#include "command_map.h"

using namespace yutovo;

class DocumentWidget : public QWidget
{
public:
    DocumentWidget(QWidget *parent);

    DocumentPtr CreateDocument();

    void InsertText(const std::string& str, const StringFormatPtr string_format);

    bool GetElementAtCoords(const int x, const int y, ElementId& id);

public slots:
    void OnDocumentUpdated(const Rect rect);
    void OnWindowUpdated();
    void OnCaretMoved(const EditorState editor_state);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

private:
    friend class MainWindow;
    QtWindow window;

    ShortcutsMap shortcuts_map;
    
    EditorState current_editor_state;

public:
    DocumentPtr document;
};

#endif
