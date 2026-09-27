/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "window.h"

namespace yutovo
{

//Window

Window::Window()
{
}

void Window::DrawFillRect(const Rect& rect, const Color color)
{
    DrawFillRect(rect.left, rect.top, rect.width, rect.height, color);
}

void Window::DrawRect(const Rect& rect, const Color color)
{
    DrawRect(rect.left, rect.top, rect.width, rect.height, color);
}

void Window::ClearRect(const Rect& rect, const Color color)
{
    ClearRect(rect.left, rect.top, rect.width, rect.height, color);
}

void Window::MoveDocument(const int left, const int top)
{
    document_point.x = left > 0 ? left : 0;
    document_point.y = top > 0 ? top : 0;
}

void Window::SetDocumentSize(const Size size)
{
    document_size = size;
}

Point Window::GetDocumentPoint()
{
    return document_point;
}

void Window::BeginDrawOutside()
{
    draw_doc = false;
}

void Window::EndDrawOutside()
{
    draw_doc = true;
}

std::string Window::Translate(ElementId id, const std::string& str)
{
    return str;
}

std::u32string Window::Translate(ElementId id, const std::u32string& str)
{
    return str;
}

int Window::ConvertToPixels(const int mm)
{
    return 0;
}

void Window::OnCaretMoved(const EditorState editor_state)
{
}

void Window::OnFormatChanged(const EditorState editor_state)
{
}

void Window::OnIdentifierChanged(const LogicalId id)
{
}

void Window::OnLanguageChanged(const yutovo_calculator::Language language)
{
}

void Window::OnDocumentChanged(const bool changed)
{
}

void Window::OnSaveResult(const uint task_id, IOResult result, const int document_id)
{
}

void Window::OnLoadResult(const uint task_id, IOResult result, const int document_id)
{
}

void Window::OnLoadInclude(const std::string& file_name, const int document_id)
{
}

void Window::OnCopyResult(CopyResult result)
{
}

void Window::OnPasteResult(PasteResult result)
{
}

void Window::OnFormattingStarted()
{
}

void Window::OnFormattingFinished()
{
}

void Window::OnResizeStarted()
{
}

void Window::OnResizeFinished()
{
}

void Window::OnServiceStatus(IOResult result)
{
}

void Window::OnIdentifiersReceived(std::string json)
{
}

void Window::OnLinkClicked(const ElementId& id, const std::u32string& url)
{
}

void Window::OnSolverAction(const std::string& json)
{
}

void Window::OnSetConfig()
{
}

#ifdef EMSCRIPTEN
int Window::Connect(const std::string& addr)
{
    return 0;
}

bool Window::Send(const int socket_id, const std::string& message)
{
    return false;
}

bool Window::Receive(const int socket_id, std::string& message)
{
    return false;
}

bool Window::Reset(const int socket_id)
{
    return false;
}

bool Window::IsOpen(const int socket_id)
{
    return false;
}

bool Window::Close(const int socket_id)
{
    return false;
}
#endif

}
