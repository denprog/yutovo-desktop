/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include "mock.h"
#include "style.h"
#include "pdf_window.h"
#include "formulas/graph.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

//A little pdf with text
TEST_F(PdfTest, pdf1)
{
    Start(600, {200, 200}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf1.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });

    document.InsertString("The source of the text itself is a little strange.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Строка", true));

    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();

    std::this_thread::sleep_for(2s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf1.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    poppler::rectf r = page->page_rect();
    ASSERT_TRUE(r.width() == 200 && r.height() == 200);
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "The source of the text\nitself is a little strange.\nСтрока\n\f") << text;

    std::vector<poppler::text_box> boxes = page->text_list();
    b = boxes[0].text().to_utf8();
    std::string t(b.begin(), b.end());
    r = boxes[0].bbox();
    ASSERT_TRUE(t == "The" && r.x() == 30 && std::fabs(r.y() - 19.33) < 0.1 && std::fabs(r.width() - 24.11) < 0.1 && std::fabs(r.height() - 15.6) < 0.1)
        << r.x() << " " << r.y() << " " << r.width() << " " << r.height();
    b = boxes[1].text().to_utf8();
    t = std::string(b.begin(), b.end());
    r = boxes[1].bbox();
    ASSERT_TRUE(t == "source" && std::fabs(r.x() - 57.98) < 0.1 && std::fabs(r.y() - 19.33) < 0.1 && 
        std::fabs(r.width() - 42.014) < 0.1 && std::fabs(r.height() - 15.6) < 0.1) 
        << r.x() << " " << r.y() << " " << r.width() << " " << r.height();;
}

//Pdf with solvings
TEST_F(PdfTest, pdf2)
{
    Start(600, {600, 400}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf2.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("2км", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    std::this_thread::sleep_for(600ms);
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2км=2.км"
        ) << ToBasicString(document.ToText());

    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();
    std::this_thread::sleep_for(1s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf2.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "2км = 2. км\n\f") << text;
}

//Pdf with graph
TEST_F(PdfTest, pdf3)
{
    Start(600, {600, 400}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf3.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });

    document.WaitTask(document.InsertGraph(true));
    auto el = document.FindByType(ElementId{0}, ElementType::GRAPH_LINE);
    GraphLine* graph = (GraphLine*)el.get();
    graph->Resize(-200, -200);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("2", true));
    document.MoveCaretRight(false);

    document.InsertMinus(true);
    document.WaitTask(document.InsertString("4", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("x", true));
    document.MoveCaretRight(false);

    document.WaitTask(document.InsertString("4", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);

    document.MoveCaretToDocumentEnd(false);
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("String", true));

    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.caret->SetVisible(false);
    pdf_document.Start();
    std::this_thread::sleep_for(1s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf3.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "          2\n\n\n\n\n                           String\n█ sin ( x )\n\n\n\n\n        -2\n              -4   x   4\n\f") << text;
}

//Pdf with formulas
TEST_F(PdfTest, pdf4)
{
    Start(600, {595, 842}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf4.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });

    document.InsertDivision(true);
    document.InsertString("123", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("55", true);
    document.InsertPlus(true);
    document.InsertString("45.789", true);
    document.MoveCaretRight(false);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("567890.88", true));

    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();
    std::this_thread::sleep_for(1s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf4.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "   123\n              · 567 890.88\n55 + 45.789\n\f") << text;
}

//Two pages pdf
TEST_F(PdfTest, pdf5)
{
    Start(600, {200, 220}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf5.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });
    
    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и "
        "вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true));
    
    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();
    std::this_thread::sleep_for(2s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf5.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 2);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text.substr(0, 16) == "Tradicionalmente") << text.substr(0, 16);

    page = std::unique_ptr<poppler::page>(pdf->create_page(1));
    b = page->text().to_utf8();
    text = std::string(b.begin(), b.end());
    ASSERT_TRUE(text == "практическая\nпотребность в счёте\nи вычислениях,\nсвязанных с\nзадачами учёта при\nцентрализации\nсельского хозяйства.\n\f") << text;
}

//Different fonts
TEST_F(PdfTest, pdf6)
{
    Start(600, {600, 400}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf6.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false, false, false), true);
    document.InsertString("Underline", document.GetStringFormat("Courier New", 14, false, false, true, false, false, false), true);
    document.InsertString("Strikethrough", document.GetStringFormat("Sans Serif", 12, false, false, false, true, false, false), true);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("DejaVu Sans", 22, true, false, false, false, false, false), true));
    
    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();
    std::this_thread::sleep_for(2s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf6.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "TextItalicUnderlineStrikethroughBold\n\f") << text;

    std::vector<poppler::text_box> text_list = page->text_list(1);
    ASSERT_TRUE(text_list.size() == 5);

    {
        poppler::text_box& t = text_list[0];
        ASSERT_TRUE(t.get_font_name().find("Arial") != std::string::npos) << t.get_font_name();
        ASSERT_TRUE(t.get_font_size() == 24) << t.get_font_size();
    }

    {
        poppler::text_box& t = text_list[1];
        ASSERT_TRUE(t.get_font_name().find("TimesNewRoman") && t.get_font_name().find("Italic")) << t.get_font_name();
        ASSERT_TRUE(t.get_font_size() == 18) << t.get_font_size();
    }

    {
        poppler::text_box& t = text_list[4];
        ASSERT_TRUE(t.get_font_name().find("DejaVuSans") && t.get_font_name().find("Bold")) << t.get_font_name();
        ASSERT_TRUE(t.get_font_size() == 22) << t.get_font_size();
    }
}

//Check margins
TEST_F(PdfTest, pdf7)
{
    Start(600, {200, 200}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf7.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });
    
    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и "
        "вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true));
    
    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    TextFormat f{TextFormat::Paging::WEB_VIEW, 10, 10, 10, 10, 10, {200, 200}};

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start(f);
    std::this_thread::sleep_for(2s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf7.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 2);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "Tradicionalmente, el\nmedio de un documento\nera el papel y la\ninformación\nПричиной\nвозникновения\nарифметики стала\n"
        "практическая\n\f") << text;

    page = std::unique_ptr<poppler::page>(pdf->create_page(1));
    b = page->text().to_utf8();
    text = std::string(b.begin(), b.end());
    ASSERT_TRUE(text == "потребность в счёте и\nвычислениях,\nсвязанных с задачами\nучёта при\nцентрализации\nсельского хозяйства.\n\f") << text;
}

//Check footer
TEST_F(PdfTest, pdf8)
{
    Start(600, {595, 842}, true);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf8.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });
    
    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и "
        "вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true));

    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();
    std::this_thread::sleep_for(2s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf8.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "  Tradicionalmente, el medio de un documento era el papel y la información\n"
        "  Причиной возникновения арифметики стала практическая потребность в счёте и\n"
        "  вычислениях, связанных с задачами учёта при централизации сельского\n"
        "  хозяйства.\n"
        "\n"
        "\n"
        "\n"
        "\n"
        "This document was created with Yutovo.\n\f") << text;
}

//Check of functions
TEST_F(PdfTest, pdf9)
{
    Start(600, {595, 842}, false);

    EXPECT_CALL(*pdf_window_mock, OnPdfExportResult).WillOnce([&](const std::vector<uint8_t>& pdf, const PdfResult result)
        {
            ASSERT_TRUE(result == PdfResult::Success);
            std::ofstream file("pdf9.pdf", std::ios::binary);
            file.write(reinterpret_cast<const char*>(pdf.data()), pdf.size());
        });

    document.InsertCode(false, true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("2", true);
    document.InsertCloseRoundBracket(true);
    document.InsertPlus(true);
    document.InsertString("cos", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("5", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    std::this_thread::sleep_for(2s);

    config.with_border = false;
    config.code_block_border = false;
    config.caret_visible = false;
    config.hilight_caret_element = false;
    config.draw_whole = true;
    config.pdf = true;

    Document pdf_document(pdf_window_mock.get(), config, document);
    pdf_document.Start();
    std::this_thread::sleep_for(1s);

    std::unique_ptr<poppler::document> pdf(poppler::document::load_from_file("pdf9.pdf"));
    ASSERT_TRUE(pdf);
    ASSERT_TRUE(pdf->pages() == 1);
    std::unique_ptr<poppler::page> page(pdf->create_page(0));
    auto b = page->text().to_utf8();
    std::string text(b.begin(), b.end());
    ASSERT_TRUE(text == "sin ( 2 ) + cos ( 5 )\n\f") << text;
}

}
