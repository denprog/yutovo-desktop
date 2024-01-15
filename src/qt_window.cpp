#include "qt_window.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPainterPath>
#include <QBuffer>
#include <QCoreApplication>
#include <QApplication>

//QtWindow

QtWindow::QtWindow(const int width, const int height) :
    surface(new QImage(width, height, QImage::Format_RGB32)),
    logger(Logger::GetInstance(".", "yutovo_desktop", true, true))
{
}

void QtWindow::Init()
{
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
    QImage image;
    QByteArray arr(reinterpret_cast<const char*>(bmp.data()), bmp.size());
    QBuffer buffer(&arr);
    buffer.open(QIODevice::ReadOnly);
    if (!image.load(&buffer, "BMP"))
        return;

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

float QtWindow::GetSymbolSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline)
{
    auto it = sizes_cache.find(symbol);
    if (it != sizes_cache.end())
    {
        std::vector<SymbolSize>& v = it->second;
        auto v_it = std::find_if(v.begin(), v.end(), 
            [&](SymbolSize& s)
            {
                return s.height == height && s.family_name == family_name;
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
        auto [_it, success] = sizes_cache.insert(std::pair<char32_t, std::vector<SymbolSize>>(symbol, std::vector<SymbolSize>()));
        it = _it;
    }

    QSize s(0, 0);
    int font_size = 1;
    auto _symbol = std::u32string(1, symbol);
    QString str = QString::fromUcs4(_symbol.c_str());
    baseline = 0;
    std::vector<SymbolSize>& v = it->second;
    while (s.height() < height)
    {
        auto v_it = std::find_if(v.begin(), v.end(), 
            [&](SymbolSize& _s)
            {
                return _s.font_size == font_size && _s.family_name == family_name;
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
        it->second.push_back(SymbolSize{s.height(), family_name, font_size, s, baseline});
        ++font_size;
    }
    size.Set(s.width(), s.height());
    return font_size - 1;
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
    
    QPainter p(surface.get());
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

void QtWindow::OnCaretMoved(const EditorState editor_state)
{
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

void QtWindow::OnLoadResult(const uint task_id, IOResult result)
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
    QApplication::setOverrideCursor(Qt::WaitCursor);
}

void QtWindow::OnFormattingFinished()
{
    QApplication::restoreOverrideCursor();
}

void QtWindow::GetPixmap(QPixmap& out, const QRect& rect)
{
    std::lock_guard<std::mutex> lock(pixmap_mutex);
    out = pixmap.copy(rect);
}
