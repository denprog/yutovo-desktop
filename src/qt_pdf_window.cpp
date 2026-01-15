#include "qt_pdf_window.h"
#include <QCoreApplication>
#ifdef _WIN32
#include <Windows.h>
#else
#include <fontconfig/fontconfig.h>
#endif

#ifdef _WIN32
#include <wingdi.h>
#ifndef GFRI_FONTFILENAME
#define GFRI_FONTFILENAME 4
#endif
extern "C" BOOL WINAPI GetFontResourceInfoW(LPCWSTR lpszFilename, LPDWORD cbBuffer, LPVOID lpvBuffer, DWORD dwQueryType);
#endif

//QtPdfWindow

std::map<std::string, QString> QtPdfWindow::font_files =
{
    {"Arial", "Arial.ttf"},
    {"Courier New", "Courier_New.ttf"},
    {"Courier", "Courier_New.ttf"},
    {"Times New Roman", "Times_New_Roman.ttf"},
    {"FreeMono", "FreeMono.ttf"},
    {"DejaVu Serif", "DejaVuSerif.ttf"}
};

QtPdfWindow::QtPdfWindow(const Size& _page_size) :
    PdfWindow(_page_size, true)
{
}

std::u32string QtPdfWindow::Translate(ElementId id, const std::u32string& str)
{
    return QCoreApplication::translate("MainWindow", yutovo::ToBasicString(str).c_str()).toStdU32String();
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
    auto it = font_files.find(format->family);
    if (it != font_files.end())
        return base_dir + it->second;

    std::wstring family;
    WCHAR file_path[MAX_PATH];
    DWORD size = MAX_PATH * sizeof(WCHAR);
    family = std::wstring(format->family.begin(), format->family.end());
    std::wstring path = FindFontFile(family);
    BOOL ok = GetFontResourceInfoW(path.c_str(), &size, file_path, GFRI_FONTFILENAME);
    if (ok)
        return QString::fromWCharArray(file_path);
    DWORD err = GetLastError();
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

#ifdef _WIN32
std::wstring QtPdfWindow::FindFontFile(const std::wstring& family)
{
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return {};

    DWORD index = 0;
    WCHAR name[256];
    BYTE data[512];
    DWORD nameSize, dataSize, type;
    std::wstring result;

    while (true)
    {
        nameSize = sizeof(name) / sizeof(WCHAR);
        dataSize = sizeof(data);

        if (RegEnumValueW(hKey, index, name, &nameSize, nullptr, &type, data, &dataSize) != ERROR_SUCCESS)
            break;

        std::wstring keyName(name);
        if (keyName.rfind(family, 0) == 0)
        {
            std::wstring fileName(reinterpret_cast<WCHAR*>(data));
            result = L"C:\\Windows\\Fonts\\" + fileName;
            break;
        }

        index++;
    }

    RegCloseKey(hKey);
    return result;
}
#endif
