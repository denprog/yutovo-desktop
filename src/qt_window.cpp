#include "qt_window.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPainterPath>
#include <QBuffer>
#include <QCoreApplication>
#include <QApplication>
#include <yutovo_editor/document.h>

//QtWindow

using namespace std::chrono_literals;
using namespace std::chrono;

QtWindow::QtWindow(const int width, const int height) :
    surface(new QImage(width, height, QImage::Format_RGB32)),
    logger(Logger::GetInstance(".", "yutovo_desktop", true, true))
{
}

QtWindow::~QtWindow()
{
    stop_cache_thread = true;
    if (fill_cache_thread.joinable())
        fill_cache_thread.join();
}

void QtWindow::Init(Document* _document)
{
    document = _document;
}

void QtWindow::DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color, const Color bg_color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    
    QFont font(format->family.c_str(), format->size);
    font.setItalic(format->italic);
    font.setBold(format->bold);
    font.setUnderline(format->underline);
    font.setStrikeOut(format->strikethrough);
    p.setPen(QColor::fromRgba(color.ToInt()));
    p.setBackgroundMode(Qt::OpaqueMode);
    p.setBackground(QBrush(QColor::fromRgba(bg_color.ToInt())));
    p.setFont(font);
    if (draw_doc)
        p.setClipRegion(clip_region);
    p.drawText(QRect(rect.left - document_point.x, rect.top - document_point.y, rect.width, rect.height), text.c_str());
    p.end();
}

void QtWindow::DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setPen(QColor::fromRgba(color.ToInt()));
    if (draw_doc)
        p.setClipRegion(clip_region);
    p.drawLine(x1 - document_point.x, y1 - document_point.y, x2 - document_point.x, y2 - document_point.y);
    p.end();
}

void QtWindow::DrawRect(const int x1, const int y1, const int width, const int height, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setPen(QColor::fromRgba(color.ToInt()));
    if (draw_doc)
    {
        p.setClipRegion(clip_region);
        p.drawRect(x1 - document_point.x, y1 - document_point.y, width, height);
    }
    else
        p.drawRect(x1, y1, width, height);
    p.end();
}

void QtWindow::DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    if (draw_doc)
    {
        p.setClipRegion(clip_region);
        p.fillRect(x1 - document_point.x, y1 - document_point.y, width, height, QColor::fromRgba(color.ToInt()));
    }
    else
        p.fillRect(x1, y1, width, height, QColor::fromRgba(color.ToInt()));
    p.end();
}

void QtWindow::DrawFillEllipse(const int x1, const int y1, const int width, const int height, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setBrush(QColor::fromRgba(color.ToInt()));
    if (draw_doc)
    {
        p.setClipRegion(clip_region);
        p.drawEllipse(QPoint(x1 - document_point.x, y1 - document_point.y), width, height);
    }
    else
        p.drawEllipse(QPoint(x1, y1), width, height);
    p.end();
}

void QtWindow::DrawFillPath(const std::list<Point>& path, const Color color)
{
    QVector<QPointF> points;
    for (const Point& p : path)
    {
        if (draw_doc)
            points.push_back(QPointF(p.x - document_point.x, p.y - document_point.y));
        else
            points.push_back(QPointF(p.x, p.y));
    }
    QPolygonF polygon(points);

    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QColor::fromRgba(color.ToInt()));
    p.setBrush(QColor::fromRgba(color.ToInt()));
    p.setClipRegion(clip_region);
    QPainterPath _path;
    _path.addPolygon(polygon);
    p.drawPath(_path);
    p.end();
}

void QtWindow::DrawBezierPath(const std::list<Point>& path, const Color color)
{
    QVector<QPointF> points;
    for (const Point& p : path)
    {
        if (draw_doc)
            points.push_back(QPointF(p.x - document_point.x, p.y - document_point.y));
        else
            points.push_back(QPointF(p.x, p.y));
    }

    if (points.size() < 4)
        return;

    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QColor::fromRgba(color.ToInt()));
    p.setBrush(QColor::fromRgba(color.ToInt()));
    p.setClipRegion(clip_region);
    QPainterPath _path;
    _path.moveTo(points[0]);
    for (int i = 1; i < points.size(); i += 3)
    {
        if (i + 3 > points.size())
            break;
        _path.cubicTo(points[i], points[i + 1], points[i + 2]);
    }
    p.drawPath(_path);
    p.end();
}

void QtWindow::DrawWavyLine(const int x1, const int y1, const int width, const int radius, const Color color)
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QColor::fromRgba(color.ToInt()));
    int x = x1;
    while (x <= x1 + width)
    {
        p.drawArc(QRect(x, y1, radius * 2, radius * 2), 0, -180 * 16);
        x += radius * 2;
        if (x >= x1 + width)
            break;
        p.drawArc(QRect(x, y1, radius * 2, radius * 2), 0, 180 * 16);
        x += radius * 2;
    }
    p.end();
}

void QtWindow::DrawImage(const int x1, const int y1, const int width, const int height, const std::vector<unsigned char>& bmp)
{
    QImage image(&bmp[0], width, height, QImage::Format_ARGB32);

    QPainter p;
    if (!p.begin(surface.get()))
        return;
    if (draw_doc)
    {
        p.setClipRegion(clip_region);
        p.drawImage(QRect{x1 - document_point.x, y1 - document_point.y, width, height}, image);
    }
    else
    {
        p.drawImage(QRect{x1, y1, width, height}, image);
    }
    p.end();
}

int QtWindow::GetSymbolSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline)
{
    std::lock_guard<std::mutex> lock(sizes_cache_mutex);
    return GetCachedSize(symbol, height, family_name, size, baseline);
}

void QtWindow::PrepareSymbolsSizes(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes)
{
    stop_cache_thread = true;
    if (fill_cache_thread.joinable())
        fill_cache_thread.join();
    stop_cache_thread = false;
    fill_cache_thread = std::thread(&QtWindow::FillCacheThread, this, symbols_sizes);
}

void QtWindow::ClearRect(const int x1, const int y1, const int width, const int height)
{
    DrawFillRect(x1, y1, width, height, Color::White());
}

void QtWindow::ClearSurface()
{
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    QRect rect = surface->rect();
    p.fillRect(rect, QColor::fromRgb(255, 255, 255));
    p.end();
}

void QtWindow::StoreRect(const Rect& rect)
{
    Rect r(rect);
    r.left -= document_point.x;
    r.top -= document_point.y;
    store_image = surface->copy(r.left, r.top, r.width, r.height);
    store_rect = r;
}

void QtWindow::RestoreRect()
{
    if (store_rect.IsEmpty())
        return;
    
    QPainter p;
    if (!p.begin(surface.get()))
        return;
    p.drawImage(QPoint(store_rect.left, store_rect.top), store_image);
    p.end();

    emit DocumentUpdated(store_rect);
}

Size QtWindow::GetTextSize(const std::u32string& text, const StringFormatPtr format)
{
    QFont font(format->family.c_str(), format->size);
    font.setBold(format->bold);
    font.setItalic(format->italic);
    font.setUnderline(format->underline);
    font.setStrikeOut(format->strikethrough);
    QFontMetrics m(font);
    QString str = QString::fromUcs4(text.c_str());
    QSize s = m.size(Qt::TextSingleLine, str);
    int cx = m.horizontalAdvance(str);
    return Size{cx > s.width() ? cx : s.width(), s.height()};
}

int QtWindow::GetCharPos(const std::u32string& text, const StringFormatPtr format, int pos)
{
    QFont font(format->family.c_str(), format->size);
    font.setBold(format->bold);
    font.setItalic(format->italic);
    font.setUnderline(format->underline);
    font.setStrikeOut(format->strikethrough);
    QFontMetrics m(font);
    QString str = QString::fromUcs4(text.c_str());
    if (str.length() == pos)
        return m.horizontalAdvance(str, pos);
    int p1 = m.horizontalAdvance(str, pos + 1);
    int p2 = m.horizontalAdvance(str.mid(pos), 1);
    return p1 - p2;
}

int QtWindow::GetFontAscent(const StringFormatPtr format)
{
    QFont font(format->family.c_str(), format->size);
    font.setBold(format->bold);
    font.setItalic(format->italic);
    font.setUnderline(format->underline);
    font.setStrikeOut(format->strikethrough);
    QFontMetrics m(font);
    return m.ascent();
}

Size QtWindow::GetImageSize(const std::vector<unsigned char>& bmp, const int width, const int height)
{
    QImage image(&bmp[0], width, height, QImage::Format_ARGB32);
    return Size{image.width(), image.height()};
}

void QtWindow::Update(const Rect& rect)
{
    {
        std::lock_guard<std::mutex> lock(pixmap_mutex);
        pixmap.convertFromImage(*surface);
    }

    Rect r(rect);
    r.left -= document_point.x;
    r.top -= document_point.y;
    emit DocumentUpdated(r);
}

void QtWindow::SetViewPort(const Rect view_port)
{
    clip_region = QRegion(QRect(view_port.left, view_port.top, view_port.width, view_port.height));
}

void QtWindow::AddViewPort(const Rect view_port)
{
}

Rect QtWindow::GetViewPort(const int pos)
{
    QRegion::const_iterator iter;
    int p = 0;
    for (iter = clip_region.begin(); p < pos && iter != clip_region.end(); ++p && ++iter);
    const QRect& rect = *iter;
    return Rect{rect.left(), rect.top(), rect.width(), rect.height()};
}

void QtWindow::Resize(uint width, uint height)
{
    surface.reset(new QImage(width, height, QImage::Format_RGB32));
}

std::u32string QtWindow::GetString(const std::u32string& str)
{
    return tr(QString::fromUcs4(str.c_str()).toUtf8().data()).toStdU32String();
}

Rect QtWindow::GetRect()
{
    QRect rect = surface->rect();
    return Rect{rect.left(), rect.top(), rect.width(), rect.height()};
}

std::string QtWindow::Translate(ElementId id, const std::string& str)
{
    return QCoreApplication::translate("Solver", str.c_str()).toStdString();
}

std::u32string QtWindow::Translate(ElementId id, const std::u32string& str)
{
    return QCoreApplication::translate("Solver", ToBasicString(str).c_str()).toStdU32String();
}

void QtWindow::OnCaretMoved(const EditorState _editor_state)
{
    if (!document)
        return;
    
    editor_state = _editor_state;
    const CaretState& c = editor_state.caret_state;
    if (c.id.empty())
        return;
    const SelectionState& s = editor_state.selection_state;

    parent_editable = document->IsEditable(yutovo::GetParent(c.id));

    //find common paragraph format
    document->GetParagraphFormat(c.id, common_paragraph_format);
    for (auto& state : s.state)
    {
        ParagraphFormat p;
        if (document->GetParagraphFormat(c.id, p))
        {
            if (p.name != common_paragraph_format.name)
            {
                common_paragraph_format.name = "";
                break;
            }
        }
    }

    ElementId _id = GetParent(c.id);
    is_string = document->IsString(document->GetElement(_id));
    is_row = document->IsRow(document->GetElement(_id));

    //find common string format
    if (c.id.empty() || c.id.size() == 1)
        return;
    if (!is_string && !is_row)
    {
        string_format.Reset();
    }
    else if (!s.IsEmpty())
    {
        bool set_family = true, set_size = true, set_bold = true, set_italic = true, set_underline = true, set_strikethrough = true;
        for (auto& state : s.state)
        {
            for (int i = state.start; i < state.start + state.size; ++i)
            {
                ElementId _id = GetChild(state.id, i);
                StringFormat f;
                if (document->GetStringFormat(_id, f))
                {
                    if (set_family)
                    {
                        if (string_format.family == "")
                            string_format.family = f.family;
                        else if (string_format.family != f.family)
                        {
                            string_format.family = "";
                            set_family = false;
                        }
                    }
                    if (set_size)
                    {
                        if (string_format.size == 0)
                            string_format.size = f.size;
                        else if (string_format.size != f.size)
                        {
                            string_format.size = 0;
                            set_size = false;
                        }
                    }
                    if (set_bold)
                    {
                        if (string_format.bold == true && f.bold == false)
                        {
                            string_format.bold = false;
                            set_bold = false;
                        }
                        else
                            string_format.bold = f.bold;
                    }
                    if (set_italic)
                    {
                        if (string_format.italic == true && f.italic == false)
                        {
                            string_format.italic = false;
                            set_italic = false;
                        }
                        else
                            string_format.italic = f.italic;
                    }
                    if (set_underline)
                    {
                        if (string_format.underline == true && f.underline == false)
                        {
                            string_format.underline = false;
                            set_underline = false;
                        }
                        else
                            string_format.underline = f.underline;
                    }
                    if (set_strikethrough)
                    {
                        if (string_format.strikethrough == true && f.strikethrough == false)
                        {
                            string_format.strikethrough = false;
                            set_strikethrough = false;
                        }
                        else
                            string_format.strikethrough = f.strikethrough;
                    }
                }
            }
        }
    }
    else
    {
        document->GetStringFormat(_id, string_format);
    }

    can_undo = document->CanUndo();
    can_redo = document->CanRedo();

    emit CaretMoved(editor_state);
}

void QtWindow::OnFormatChanged(const EditorState editor_state)
{
    emit CaretMoved(editor_state);
}

void QtWindow::OnSaveResult(const uint task_id, IOResult result)
{
    emit SaveResult(task_id, result);
}

void QtWindow::OnLoadResult(const uint task_id, IOResult result, const int document_id)
{
    emit LoadResult(task_id, result);
}

void QtWindow::OnCopyResult(CopyResult result)
{
    emit ClipboardCopyResult(result);
}

void QtWindow::OnPasteResult(PasteResult result)
{
    emit ClipboardPasteResult(result);
}

void QtWindow::OnFormattingStarted()
{
    emit FormatingStarted();
}

void QtWindow::OnFormattingFinished()
{
    emit FormatingFinished();
}

void QtWindow::OnResizeStarted()
{
    emit ResizeStarted();
}

void QtWindow::OnResizeFinished()
{
    emit ResizeFinished();
}

void QtWindow::GetPixmap(QPixmap& out, const QRect& rect)
{
    std::lock_guard<std::mutex> lock(pixmap_mutex);
    out = pixmap.copy(rect);
}

void QtWindow::FillCacheThread(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes)
{
    Size size;
    int baseline = 0;
    for (auto& s : symbols_sizes)
    {
        int height = 1;
        char32_t symbol = std::get<0>(s);
        std::string family = std::get<1>(s);
        int max_height = std::get<2>(s);
        while (!stop_cache_thread && height < max_height)
        {
            std::this_thread::sleep_for(10ms); //this thread has low priority
            std::lock_guard<std::mutex> lock(sizes_cache_mutex);
            GetCachedSize(symbol, height, family, size, baseline);
            ++height;
        }
    }
}

int QtWindow::GetCachedSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline)
{
    //firstly search the cache
    FontSymbolSizes::iterator s_it;
    auto it = sizes_cache.find(symbol);
    if (it != sizes_cache.end())
    {
        s_it = it->second.find(family_name);
        if (s_it == it->second.end())
        {
            auto [_it, success] = it->second.insert(std::pair<std::string, std::vector<SymbolSize>>(family_name, std::vector<SymbolSize>()));
            s_it = _it;
        }

        std::vector<SymbolSize>& v = s_it->second;
        auto v_it = std::find_if(v.begin(), v.end(), 
            [&](SymbolSize& s)
            {
                return s.height == height;
            });
        if (v_it != v.end())
        {
            size.Set(v_it->symbol_size.width(), v_it->symbol_size.height());
            baseline = v_it->baseline;
            return v_it->font_size;
        }
    }
    else
    {
        FontSymbolSizes f;
        f.insert(std::pair<std::string, std::vector<SymbolSize>>(family_name, std::vector<SymbolSize>()));
        auto [_it, success] = sizes_cache.insert(std::pair<char32_t, FontSymbolSizes>(symbol, f));
        it = _it;
        s_it = it->second.find(family_name);
    }

    //add sizes in the cache from zero up to the height
    QSize s(0, 0);
    int font_size = 1;
    auto _symbol = std::u32string(1, symbol);
    QString str = QString::fromUcs4(_symbol.c_str());
    baseline = 0;
    std::vector<SymbolSize>& v = s_it->second;
    while (s.height() < height)
    {
        //skip the already present items
        auto v_it = std::find_if(v.begin(), v.end(), 
            [&](SymbolSize& _s)
            {
                return _s.font_size == font_size;
            });
        if (v_it != v.end())
        {
            s = v_it->symbol_size;
            baseline = v_it->baseline;
            ++font_size;
            continue;
        }

        QFont font(family_name.c_str(), font_size);
        QFontMetrics m(font);
        s = m.size(Qt::TextSingleLine, str);
        baseline = m.ascent();
        s_it->second.push_back(SymbolSize{s.height(), font_size, s, baseline});
        ++font_size;
    }
    size.Set(s.width(), s.height());
    return font_size - 1;
}
