/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <string>
#include <vector>
#include <list>
#include "style.h"
#include "editor_state.h"
#include "result_codes.h"

namespace yutovo
{

class Document;

//Base class for output windows
class Window
{
public:
    Window();

    virtual void Init(Document* document) = 0;

    virtual void DrawText(const std::string& text, const StringFormatPtr format, const Rect& rect, const Color color, const Color bg_color, bool transparent) = 0;
    virtual void DrawLine(const int x1, const int y1, const int x2, const int y2, const Color color) = 0;
    void DrawRect(const Rect& rect, const Color color);
    virtual void DrawRect(const int x1, const int y1, const int width, const int height, const Color color) = 0;
    void DrawFillRect(const Rect& rect, const Color color);
    virtual void DrawFillRect(const int x1, const int y1, const int width, const int height, const Color color) = 0;
    virtual void DrawFillEllipse(const int x1, const int y1, const int width, const int height, const Color color) = 0;
    virtual void DrawFillPath(const std::list<Point>& path, const Color color) = 0;
    virtual void DrawBezierPath(const std::list<Point>& path, const Color color) = 0;
    virtual void DrawWavyLine(const int x1, const int y1, const int width, const int radius, const Color color) = 0;
    virtual void DrawImage(const int x1, const int y1, const int width, const int height, const std::vector<unsigned char>& image) = 0;
    virtual int GetSymbolSize(const char32_t symbol, const int height, const std::string& family_name, Size& size, int& baseline) = 0;
    virtual void PrepareSymbolsSizes(const std::vector<std::tuple<char32_t, std::string, int>>& symbols_sizes) = 0;

    void ClearRect(const Rect& rect, const Color color);
    virtual void ClearRect(const int x1, const int y1, const int width, const int height, const Color color) = 0;

    virtual void ClearSurface() = 0;

    virtual void StoreRect(const Rect& rect) = 0;
    virtual void RestoreRect() = 0;

    virtual Size GetTextSize(const std::u32string& text, const StringFormatPtr format) = 0;
    virtual int GetCharPos(const std::u32string& text, const StringFormatPtr format, int pos) = 0;
    virtual int GetFontAscent(const StringFormatPtr format) = 0;
    virtual Size GetImageSize(const std::vector<unsigned char>& image) = 0;

    virtual void SetViewPort(const Rect view_port) = 0;
    virtual void AddViewPort(const Rect view_port) = 0;
    virtual Rect GetViewPort(const int pos) = 0;

    virtual void Update(const Rect& rect) = 0;

    virtual void Resize(uint width, uint height) = 0;

    virtual Rect GetRect() = 0;

    virtual void MoveDocument(const int left, const int top);
    Point GetDocumentPoint();
    virtual void SetDocumentSize(const Size size);

    void BeginDrawOutside();
    void EndDrawOutside();

    virtual std::string Translate(ElementId id, const std::string& str);
    virtual std::u32string Translate(ElementId id, const std::u32string& str);

    virtual int ConvertToPixels(const int mm);

    virtual void OnCaretMoved(const EditorState editor_state);

    virtual void OnFormatChanged(const EditorState editor_state);

    virtual void OnIdentifierChanged(const LogicalId id);

    virtual void OnLanguageChanged(const yutovo_calculator::Language language);

    virtual void OnDocumentChanged(const bool changed);

    virtual void OnSaveResult(const uint task_id, IOResult result, const int document_id);
    virtual void OnLoadResult(const uint task_id, IOResult result, const int document_id);
    virtual void OnLoadInclude(const std::string& file_name, const int document_id);

    virtual void OnCopyResult(CopyResult result);
    virtual void OnPasteResult(PasteResult result);

    virtual void OnFormattingStarted();
    virtual void OnFormattingFinished();

    virtual void OnResizeStarted();
    virtual void OnResizeFinished();

    virtual void OnServiceStatus(IOResult result);

    virtual void OnIdentifiersReceived(std::string json);

    virtual void OnLinkClicked(const ElementId& id, const std::u32string& url);

    virtual void OnSolverAction(const std::string& json);

    virtual void OnSetConfig();

#ifdef EMSCRIPTEN
    virtual int Connect(const std::string& addr);
    virtual bool Send(const int socket_id, const std::string& message);
    virtual bool Receive(const int socket_id, std::string& message);
    virtual bool Reset(const int socket_id);
    virtual bool IsOpen(const int socket_id);
    virtual bool Close(const int socket_id);
#endif

public:
    Point document_point;
    Size document_size;

protected:
    bool draw_doc = false; //drawing inside the clipping regions
};

}

#endif
