#ifndef __DOCUMENT_WIDGET_H__
#define __DOCUMENT_WIDGET_H__

#include <QWidget>
#include <yutovo_editor/document.h>
#include "qt_window.h"
#include "command_map.h"

using namespace yutovo;

class DocumentWidget : public QWidget
{
    Q_OBJECT

public:
    DocumentWidget(QWidget *parent);

    DocumentPtr CreateDocument();

    void InsertText(const std::string& str, const StringFormatPtr string_format);

    bool GetElementAtCoords(const int x, const int y, ElementId& id);

public slots:
    void OnDocumentUpdated(const Rect rect);
    void OnCaretMoved(const EditorState editor_state);

signals:
    void WheelVertical(const int value);
    void WheelHorizontal(const int value);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent* event);

private:
    friend class DocumentWindow;
    friend class MainWindow;

    QtWindow window;

    ShortcutsMap shortcuts_map;
    
    EditorState current_editor_state;

    DocumentPtr document;

    uint caret_moving_task_id = 0;
};

#endif
