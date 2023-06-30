#ifndef __QT_WINDOW_H__
#define __QT_WINDOW_H__

#include <QImage>
#include <QPixmap>
#include <memory>
#include <mutex>
#include <yutovo_editor/window.h>

class DocumentWidget;

using namespace yutovo;

//Qt implementation of the abstract window
class QtWindow : public QObject, public Window
{
    Q_OBJECT

public:
    QtWindow(const int width, const int height);

    virtual void Init();

    virtual void DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color);
    virtual void DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color);
    virtual void DrawRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillEllipse(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillPath(const std::list<Point>& path, const Color color);
    virtual void DrawBezierPath(const std::list<Point>& path, const Color color);
    virtual void DrawWavyLine(const int x1, const int y1, const int width, const int radius, const Color color);

    virtual void ClearRect(const int x1, const int y1, const int width, const int height);

    virtual void ClearSurface();

    virtual void StoreRect(const Rect& rect);
    virtual void RestoreRect();

    virtual Size GetTextSize(const std::u32string& text, const StringFormatPtr format);
    virtual int GetCharPos(const std::u32string& text, const StringFormatPtr format, int pos);
    virtual int GetFontAscent(const StringFormatPtr format);

    virtual void Update(const Rect& rect);

    virtual void SetViewPort(const Rect view_port);
    virtual void AddViewPort(const Rect view_port);
    virtual Rect GetViewPort(const int pos);

    virtual void Resize(uint width, uint height);

    virtual std::u32string GetString(const std::u32string& str);

    //virtual void OnElementDrawn(const ElementId id);

    virtual Rect GetRect();

    virtual void SetDocumentSize(const Size size);

    virtual void OnCaretMoved(const EditorState editor_state);

    virtual void OnFormatChanged(const EditorState editor_state);

    virtual void OnSaveResult(const uint task_id, IOResult result);
    virtual void OnLoadResult(const uint task_id, IOResult result);

    virtual void OnCopyResult(CopyResult result);
    virtual void OnPasteResult(PasteResult result);

public:
    void GetPixmap(QPixmap& out, const QRect& rect);

signals:
    void DocumentUpdated(const Rect rect);
    void WindowUpdated();
    void CaretMoved(const EditorState editor_state);
    void SaveResult(const uint task_id, IOResult result);
    void LoadResult(const uint task_id, IOResult result);
    void ClipboardCopyResult(CopyResult result);
    void ClipboardPasteResult(PasteResult result);
    //void ElementDrawn(const ElementId id);
    //void DocumentRedrawn();

private:
    std::unique_ptr<QImage> surface;
    QPixmap pixmap;
    std::mutex pixmap_mutex;
    Rect store_rect;
    QImage store_image;

    QRegion clip_region;
};

#endif
