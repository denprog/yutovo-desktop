#ifndef __QT_PDF_WINDOW_H__
#define __QT_PDF_WINDOW_H__

#include <QObject>
#include <yutovo-editor/pdf_window.h>

using namespace yutovo;

class QtPdfWindow : public QObject, public PdfWindow
{
    Q_OBJECT

public:
    QtPdfWindow(const Size& _page_size);

    virtual std::u32string Translate(ElementId id, const std::u32string& str);

    virtual void OnPdfExportResult(const std::vector<uint8_t>& pdf, const PdfResult result);

    virtual bool GetFontPath(const StringFormatPtr format, std::string& path);

signals:
    void PdfExportResult(const std::vector<uint8_t>& pdf, const PdfResult result);

private:
    QString ResolveFontPath(const StringFormatPtr format);
#ifdef _WIN32
    std::wstring FindFontFile(const std::wstring& family);
#endif

    const QString base_dir = "./fonts/";
    static std::map<std::string, QString> font_files;
};

#endif
