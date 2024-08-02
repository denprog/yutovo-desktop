#ifndef __QT_WINDOW_H__
#define __QT_WINDOW_H__

#include <QImage>
#include <QPixmap>
#include <memory>
#include <mutex>
#include <thread>
#include <yutovo_editor/window.h>
#include <yutovo_logger/logger.h>

class DocumentWidget;

using namespace yutovo;

//Qt implementation of the abstract window
class QtWindow : public QObject, public Window
{
    Q_OBJECT

public:
    QtWindow(const int width, const int height);
    ~QtWindow();

    virtual void Init(Document* _document);

    virtual void DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color, const Color bg_color);
    virtual void DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color);
    virtual void DrawRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillEllipse(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillPath(const std::list<Point>& path, const Color color);
    virtual void DrawBezierPath(const std::list<Point>& path, const Color color);
    virtual void DrawWavyLine(const int x1, const int y1, const int width, const int radius, const Color color);
    virtual void DrawImage(const int x1, const int y1, const int width, const int height, const std::vector<unsigned char>& bmp);
    virtual int GetSymbolSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline);
    virtual void PrepareSymbolsSizes(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes);

    virtual void ClearRect(const int x1, const int y1, const int width, const int height);

    virtual void ClearSurface();

    virtual void StoreRect(const Rect& rect);
    virtual void RestoreRect();

    virtual Size GetTextSize(const std::u32string& text, const StringFormatPtr format);
    virtual int GetCharPos(const std::u32string& text, const StringFormatPtr format, int pos);
    virtual int GetFontAscent(const StringFormatPtr format);
    virtual Size GetImageSize(const std::vector<unsigned char>& bmp, const int width, const int height);

    virtual void Update(const Rect& rect);

    virtual void SetViewPort(const Rect view_port);
    virtual void AddViewPort(const Rect view_port);
    virtual Rect GetViewPort(const int pos);

    virtual void Resize(uint width, uint height);

    virtual std::u32string GetString(const std::u32string& str);

    virtual Rect GetRect();

    virtual std::string Translate(ElementId id, const std::string& str);
    virtual std::u32string Translate(ElementId id, const std::u32string& str);
    
    virtual void OnCaretMoved(const EditorState _editor_state);

    virtual void OnFormatChanged(const EditorState editor_state);

    virtual void OnDocumentChanged(const bool changed);

    virtual void OnSaveResult(const uint task_id, IOResult result);
    virtual void OnLoadResult(const uint task_id, IOResult result, const int document_id);

    virtual void OnCopyResult(CopyResult result);
    virtual void OnPasteResult(PasteResult result);

    virtual void OnFormattingStarted();
    virtual void OnFormattingFinished();

    virtual void OnResizeStarted();
    virtual void OnResizeFinished();

#ifdef REMOTE_SOLVER
    virtual void OnServiceStatus(IOResult result);
#endif

public:
    void GetPixmap(QPixmap& out, const QRect& rect);

private:
    void FillCacheThread(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes);
    int GetCachedSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline);

signals:
    void DocumentUpdated(const Rect rect);
    void CaretMoved(const EditorState editor_state);
    void DocumentChanged(const bool changed);
    void SaveResult(const uint task_id, IOResult result);
    void LoadResult(const uint task_id, IOResult result);
    void ClipboardCopyResult(CopyResult result);
    void ClipboardPasteResult(PasteResult result);
    void FormatingStarted();
    void FormatingFinished();
    void ResizeStarted();
    void ResizeFinished();
    void ServiceStatus(IOResult result);

private:
    friend class MainWindow;

    Document* document;

    //last editor state
    EditorState editor_state;
    bool parent_editable = false;
    ParagraphFormat common_paragraph_format;
    bool is_string = false;
    bool is_row = false;
    StringFormat string_format;
    bool can_undo = false;
    bool can_redo = false;

    std::unique_ptr<QImage> surface;
    QPixmap pixmap;
    std::mutex pixmap_mutex;
    Rect store_rect;
    QImage store_image;

    struct SymbolSize
    {
        int height = 0;
        int font_size = 0;
        QSize symbol_size;
        int baseline = 0;
    };

    std::mutex sizes_cache_mutex;
    std::thread fill_cache_thread;
    bool stop_cache_thread = false;
    typedef std::map<std::string, std::vector<SymbolSize>> FontSymbolSizes;
    std::map<char32_t, FontSymbolSizes> sizes_cache; //symbol sizes by font and by character

    QRegion clip_region;

    Logger* logger;
};

#endif
