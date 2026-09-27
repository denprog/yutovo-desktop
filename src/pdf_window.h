#ifndef __PDF_WINDOW_H__
#define __PDF_WINDOW_H__

#include "window.h"
#include "hpdf.h"

namespace yutovo
{

class PdfWindow : public Window
{
public:
    PdfWindow(const Size& _page_size, bool _draw_footer);
    virtual ~PdfWindow();

    void SetPageRange(int _first_page, int _last_page);
    int GetPageCount() const;

    virtual void Init(Document* document);

    virtual void DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color, const Color bg_color, const bool transparent);
    virtual void DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color);
    virtual void DrawRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillEllipse(const int x1, const int y1, const int width, const int height, const Color color);
    virtual void DrawFillPath(const std::list<Point>& path, const Color color);
    virtual void DrawBezierPath(const std::list<Point>& path, const Color color);
    virtual void DrawWavyLine(const int x1, const int y1, const int width, const int radius, const Color color);
    virtual void DrawImage(const int x1, const int y1, const int width, const int height, const std::vector<unsigned char>& image);
    virtual int GetSymbolSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline);
    virtual void PrepareSymbolsSizes(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes);

    virtual void ClearRect(const int x1, const int y1, const int width, const int height, const Color color);

    virtual void ClearSurface();

    virtual void StoreRect(const Rect& rect);
    virtual void RestoreRect();

    virtual Size GetTextSize(const std::u32string& text, const StringFormatPtr format);
    virtual int GetCharPos(const std::u32string& text, const StringFormatPtr format, int pos);
    virtual int GetFontAscent(const StringFormatPtr format);
    virtual Size GetImageSize(const std::vector<unsigned char>& image);

    virtual void SetViewPort(const Rect _view_port);
    virtual void AddViewPort(const Rect _view_port);
    virtual Rect GetViewPort(const int pos);

    virtual void Update(const Rect& rect);

    virtual void Resize(uint width, uint height);

    virtual Rect GetRect();

    virtual void MoveDocument(const int left, const int top);

    virtual int ConvertToPixels(const int mm);

    virtual void OnSaveResult(const uint task_id, IOResult result, const int document_id);
    virtual void OnLoadResult(const uint task_id, IOResult result, const int document_id);
    virtual void OnLoadInclude(const std::string& file_name, const int document_id);

    virtual void OnPdfExportResult(const std::vector<uint8_t>& pdf, const PdfResult result) = 0;

    virtual bool GetFontPath(const StringFormatPtr format, std::string& path) = 0;

private:
    void AddPage();
    void CreatePage();
    bool IsCurrentPageInRange() const;
    HPDF_Font GetFont(const StringFormatPtr format);

private:
    struct ClipDraw
    {
        ClipDraw(const PdfWindow& _pdf_window) : 
            pdf_window(_pdf_window)
        {
            if (pdf_window.draw_doc)
            {
                HPDF_Page_GSave(pdf_window.page);
                HPDF_Page_Rectangle(pdf_window.page, pdf_window.view_port.left, pdf_window.view_port.top, pdf_window.view_port.width, 
                    pdf_window.view_port.height);
                HPDF_Page_Clip(pdf_window.page);
                HPDF_Page_EndPath(pdf_window.page);
            }
        }

        ~ClipDraw()
        {
            if (pdf_window.draw_doc)
                HPDF_Page_GRestore(pdf_window.page);
        }

        const PdfWindow& pdf_window;
    };

private:
    HPDF_Doc pdf = nullptr;
    HPDF_Page page = nullptr;
    Rect view_port;
    Size page_size;
    uint pages = 1;
    int y_top = 0;
    int y_diff = 0;
    int y_filled = 0, top_y_filled = 0, last_y_filled = 0;
    std::map<boost::uuids::uuid, std::string> fonts_map;
    std::vector<uint8_t> result;
    bool draw_footer;
    int first_page = 1;
    int last_page = 0;
};
    
}

#endif
