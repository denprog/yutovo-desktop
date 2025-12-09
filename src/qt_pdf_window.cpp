#include "qt_pdf_window.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <fontconfig/fontconfig.h>
#endif

//QtPdfWindow

QtPdfWindow::QtPdfWindow(const Size& _page_size) : 
    PdfWindow(_page_size)
{
}

void QtPdfWindow::OnPdfExportResult(const std::vector<uint8_t>& pdf, const PdfResult result)
{
    emit PdfExportResult(pdf, result);
}

bool QtPdfWindow::GetFontPath(const StringFormatPtr format, std::string& path)
{
    QString p = ResolveFontPath(format);
    if (p.isEmpty())
        return false;
    path = p.toUtf8().data();
    return true;
}

QString QtPdfWindow::ResolveFontPath(const StringFormatPtr format)
{
#ifdef _WIN32
    WCHAR filePath[MAX_PATH];
    DWORD size = MAX_PATH;
    BOOL ok = GetFontResourceInfoW((LPCWSTR)family.toStdWString().c_str(), &size, filePath, GFRI_FONTFILENAME);
    if (ok)
        return QString::fromWCharArray(filePath);
    return {};
#else
    FcInit();

    FcPattern* pat = FcPatternCreate();
    FcPatternAddString(pat, FC_FAMILY, reinterpret_cast<const FcChar8*>(format->family.c_str()));
    FcPatternAddInteger(pat, FC_WEIGHT, format->bold ? FC_WEIGHT_BOLD : FC_WEIGHT_REGULAR);
    FcPatternAddInteger(pat, FC_SLANT, format->italic ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
    FcConfigSubstitute(nullptr, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult result;
    FcPattern* match = FcFontMatch(nullptr, pat, &result);
    QString path;
    if (match)
    {
        FcChar8* file = nullptr;
        if (FcPatternGetString(match, FC_FILE, 0, &file) == FcResultMatch)
            path = QString::fromUtf8(reinterpret_cast<char*>(file));
        FcPatternDestroy(match);
    }
    FcPatternDestroy(pat);
    return path;
#endif
}
