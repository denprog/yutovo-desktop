/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, files1)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
        [&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    ASSERT_TRUE(document.IsChanged() == false);
    document.WaitTask(document.InsertString("Text", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.Save("1.yut"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    std::this_thread::sleep_for(200ms);
    document.Load("1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, files2)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
        [&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.SetFontSize(22);
    document.InsertString("The source of ", true);
    document.SetBold(true);
    document.SetFontFamily("Courier New");
    document.InsertString("the text ", true);
    document.SetBold(false);
    document.WaitTask(document.SetItalic(true));
    document.SetFontFamily("Times New Roman");
    document.SetFontSize(14);
    document.WaitTask(document.InsertString("itself ", true));
    document.SetFontSize(20);
    document.SetItalic(false);
    document.WaitTask(document.InsertString("is a little mysterious.", true));
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.IsChanged() == true);
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the text </strong></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:14px;\"><em>itself </em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">is a little </span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Save("2.yut");
    document.WaitTask(document.New());
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Load("2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the text </strong></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:14px;\"><em>itself </em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">is a little </span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 12}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, files3)
{
    Start(642);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature, "\
        "a street sign, an arrangement of buildings on a city block, or styles of clothing.", true));
    std::this_thread::sleep_for(100ms);
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(100ms);
    for (int i = 0; i < 9; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"><strong>literary</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a text is any object that can be read, </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">whether this object is a work of literature, a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">street sign, an arrangement of buildings on a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">city block, or styles of clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.SetFontFamily("Courier New"));
    std::this_thread::sleep_for(100ms);
    for (int i = 0; i < 20; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"><strong>literary</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\">a text</span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> is any object that </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">can be read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">literature, a street sign, an arrangement of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">buildings on a city block, or styles of clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.WaitTask(document.Save("3.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Load("3.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"><strong>literary</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\">a text</span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"> is any object that </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">can be read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">literature, a street sign, an arrangement of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">buildings on a city block, or styles of clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("5678", true));

    document.WaitTask(document.Save("3.yut"));
    document.WaitTask(document.New());

    document.Load("3.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1000ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, files4)
{
    Start(610);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature, "\
        "a street sign, an arrangement of buildings on a city block, or styles of clothing.", true);
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 20; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">a text is any object that can be read, whether this </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">object is a work of literature, a street sign, an </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">arrangement of buildings on a city block, or styles of </span>"
                "<span style=\"font-family:'Courier New';font-size:12px;\">clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });
    
    document.WaitTask(document.Save("4.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Load("4.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">a text is any object that can be read, whether this </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">object is a work of literature, a street sign, an </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">arrangement of buildings on a city block, or styles of </span>"
                "<span style=\"font-family:'Courier New';font-size:12px;\">clothing.</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    ParagraphFormat f;
    document.GetParagraphFormat(document.caret->GetElement()->id, f);
    ASSERT_TRUE(f.name == "Monospace") << f.name;
}

//Load and save a text file
TEST_F(DocumentTest, files5)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    auto el = document.GetElement(ElementId{0, 0, 0, 0, 0});
    ASSERT_TRUE(el->type == ElementType::STRING && el->ToText().rfind(U"Арифме́тика", 0) == 0);

    auto text = document.GetElement(ElementId{0});
    auto paragraph = document.GetElement(ElementId{0, (int)text->elements->Count() - 1});
    el = paragraph->elements->Get(0)->elements->Get(0);
    ASSERT_TRUE(el->type == ElementType::STRING && el->ToText().rfind(U"Основам арифметики", 0) == 0);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Save("files5.txt");
    document.WaitTask(document.New());
    document.Load("files5.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    el = document.GetElement(ElementId{0, 0, 0, 0, 0});
    ASSERT_TRUE(el->type == ElementType::STRING && el->ToText().rfind(U"Арифме́тика", 0) == 0);
}

//Load a broken file
TEST_F(DocumentTest, files6)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::InputStreamError);
        });

    document.Load("../../test/tests/broken_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);
}

//Load an abcent file
TEST_F(DocumentTest, files7)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::InputStreamError);
        });

    document.Load("../../test/tests/abcent.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);
}

//Check paragraph style after New
TEST_F(DocumentTest, files8)
{
    Start(600);

    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Load json from string
TEST_F(DocumentTest, files9)
{
    Start(600);

    auto json = "{\"string_formats\":[{\"id\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\",\"family\":\"Arial\",\"size\":14,\"bold\":false,"
        "\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"d7dbac2d-be4a-4895-b0a2-57ec557622a9\","
        "\"family\":\"Arial\",\"size\":30,\"bold\":true,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},"
        "{\"id\":\"0d8fb519-1505-4eb1-a282-bd7623c09a6f\",\"family\":\"Arial\",\"size\":26,\"bold\":true,\"italic\":false,\"underline\":false,"
        "\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"f3b45b0f-dd34-4758-9fd8-02ab0c0b2cb5\",\"family\":\"Arial\",\"size\":22,"
        "\"bold\":true,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295},"
        "{\"id\":\"511ee5e7-6e85-4e76-bb43-3423611c5c6a\",\"family\":\"Courier New\",\"size\":12,\"bold\":false,\"italic\":false,\"underline\":false,"
        "\"color\":4278190080,\"selection_color\":4294967295},{\"id\":\"89c69148-d1bc-4384-b009-7b752eab898c\",\"family\":\"Courier New\",\"size\":14,"
        "\"bold\":false,\"italic\":false,\"underline\":false,\"color\":4278190080,\"selection_color\":4294967295}],\"paragraph_formats\":"
        "[{\"name\":\"Text body\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,"
        "\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\"},{\"name\":\"Header 1\","
        "\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,"
        "\"spacing_after\":10,\"default_string_format\":\"d7dbac2d-be4a-4895-b0a2-57ec557622a9\"},{\"name\":\"Header 2\",\"alignment\":0,\"word_wrap\":1,"
        "\"line_spacing\":5,\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,"
        "\"default_string_format\":\"0d8fb519-1505-4eb1-a282-bd7623c09a6f\"},{\"name\":\"Header 3\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,"
        "\"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":"
        "\"f3b45b0f-dd34-4758-9fd8-02ab0c0b2cb5\"},{\"name\":\"Monospace\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,"
        "\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\"511ee5e7-6e85-4e76-bb43-3423611c5c6a\"}"
        ",{\"name\":\"Code\",\"alignment\":0,\"word_wrap\":0,\"line_spacing\":2,\"indent_before\":2,\"indent_after\":2,\"indent_first_line\":0,"
        "\"spacing_before\":2,\"spacing_after\":2,\"default_string_format\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}],\"text\":{\"id\":\"0\",\"type\":1,"
        "\"elements\":[{\"id\":\"0,0\",\"type\":2,\"elements\":[{\"id\":\"0,0,0\",\"type\":3,\"elements\":[{\"id\":\"0,0,0,0\",\"type\":4,\"elements\":"
        "\"Newton's second law of motion\",\"format_id\":\"e9fe76c1-fdcb-41b4-a64c-b5d5e84eff91\"}]}],\"format_name\":\"Text body\"},{\"id\":\"0,1\",\"type\":2,"
        "\"elements\":[{\"id\":\"0,1,0\",\"type\":3,\"elements\":[{\"id\":\"0,1,0,0\",\"type\":5,\"elements\":[{\"id\":\"0,1,0,0,0\",\"type\":6,"
        "\"elements\":[{\"id\":\"0,1,0,0,0,0\",\"type\":7,\"elements\":[{\"id\":\"0,1,0,0,0,0,0\",\"type\":27,\"elements\":[{\"id\":\"0,1,0,0,0,0,0,0\","
        "\"type\":7,\"elements\":[{\"id\":\"0,1,0,0,0,0,0,0,0\",\"type\":8,\"elements\":\"mass\",\"format_id\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}]},"
        "{\"id\":\"0,1,0,0,0,0,0,1\",\"type\":10,\"elements\":[]},{\"id\":\"0,1,0,0,0,0,0,2\",\"type\":7,\"elements\":[{\"id\":\"0,1,0,0,0,0,0,2,0\","
        "\"type\":8,\"elements\":\"1kg\",\"format_id\":\"89c69148-d1bc-4384-b009-7b752eab898c\"}]}]}]}],\"format_name\":\"Code\"}],\"code_id\":1}]}],"
        "\"format_name\":\"Text body\"}]}}";

    document.WaitTask(document.LoadJson(json, 0));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Newton's second law of motion</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>mass</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mi>1kg</mi>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Split a code string with a space
TEST_F(DocumentTest, files10)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString(" 123   4355 45 ", true));
    document.WaitTask(document.Save("files10.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Load("files10.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi>4355</mi>"\
                        "<mi>45</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Text colors
TEST_F(DocumentTest, files11)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertString("Text ", true));
    document.SetColor(Color::Red());
    document.WaitTask(document.InsertString("red ", true));
    document.SetBgColor(Color::Blue());
    document.WaitTask(document.InsertString("blue", true));
    document.WaitTask(document.Save("files11.yut"));
    document.WaitTask(document.New());
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Load("files11.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);\">red </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);bgcolor:rgba(0,0,255,255);\">blue</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 4})) << document.GetEditorState().ToString();
}

//Check is changed after undo/redo
TEST_F(DocumentTest, files12)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    ASSERT_TRUE(document.IsChanged() == false);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertString(" ", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.InsertString("s", true));
    document.WaitTask(document.InsertString("t", true));
    document.WaitTask(document.InsertString("ring", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text st") << ToBasicString(document.ToText());
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.WaitTask(document.Save("files12.yut"));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == false);

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"Text st") << ToBasicString(document.ToText());
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == false);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged() == true);
}

//Save/load with caret on a result
TEST_F(DocumentTest, files13)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.config.solve_delay = 10000;

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("5678", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);

    document.WaitTask(document.Save("files13.yut"));
    document.WaitTask(document.New());

    document.Load("files13.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToText() == U"5678=~") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Save/load a file with a language
TEST_F(DocumentTest, files14)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1+j", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.Save("files14.yut"));
    document.WaitTask(document.New());
    document.Load("files14.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"1+j=1.+1.j") << ToBasicString(document.ToText());
}

//Save/load a file with a unit definition
TEST_F(DocumentTest, files15)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("d_m", true);
    document.InsertUnit(true);
    document.WaitTask(document.InsertString("0.221m", true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    
    document.InsertString("d_m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.Save("files15.yut"));
    document.WaitTask(document.New());

    document.Load("files15.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"d_m~0.221m\n"\
        "d_m=1.d_m"
        ) << ToBasicString(document.ToText());
}

//Save/load a file with a paragraph alignment
TEST_F(DocumentTest, files16)
{
    Start(510);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true));

    document.WaitTask(document.Save("files16.yut"));
    document.WaitTask(document.New());

    document.Load("files16.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Change language and Save/load a file with a unit
TEST_F(DocumentTest, files17)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("6кг", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.Save("files17.yut"));

    document.WaitTask(document.New());
    document.Load("files17.yut");
    document.WaitLoad();
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == U"6кг=6.кг") << ToBasicString(document.ToText());
}

//Save/load a file with a sum
TEST_F(DocumentTest, files18)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertSum(true);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"i", true));

    document.WaitTask(document.Save("files18.yut"));

    document.WaitTask(document.New());
    document.Load("files18.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToText() == 
        U"sum(i=0,10,i)"
        ) << ToBasicString(document.ToText());
}

//Save/load a file with a product
TEST_F(DocumentTest, files19)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertProduct(true);
    document.InsertString(U"i", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"0", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString(U"10", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(U"i", true));

    document.WaitTask(document.Save("files19.yut"));

    document.WaitTask(document.New());
    document.Load("files19.yut");
    document.WaitLoad();
    ASSERT_TRUE(document.ToText() == 
        U"prod(i=0,10,i)"
        ) << ToBasicString(document.ToText());
}

//Check is changed
TEST_F(DocumentTest, files20)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
        [&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    ASSERT_TRUE(document.IsChanged() == false);
    document.InsertString("T", true);
    document.InsertString("e", true);
    document.InsertString("x", true);
    document.WaitTask(document.InsertString("t", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.IsChanged() == false);

    document.Undo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == false);

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.WaitTask(document.Save("files_20.yut"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == false);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == false);

    document.InsertString("N", true);
    document.InsertString("e", true);
    document.WaitTask(document.InsertString("w", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.IsChanged() == true);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Text"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.IsChanged() == false);

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.IsChanged() == true);

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.IsChanged() == false);
}

//Check is changed
TEST_F(DocumentTest, files21)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
        [&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    ASSERT_TRUE(document.IsChanged() == false);
    document.WaitTask(document.InsertString("t", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Save("1.yut"));
    std::this_thread::sleep_for(200ms);
    ASSERT_FALSE(document.IsChanged());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.IsChanged());
}

//Check gzip
TEST_F(DocumentTest, files22)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    ASSERT_TRUE(document.IsChanged() == false);
    document.WaitTask(document.InsertString("Text", true));
    std::string json;
    document.WaitTask(document.SaveJson(json, 1, true));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    std::this_thread::sleep_for(200ms);
    document.LoadJson(json, 2);
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.IsChanged() == false);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

//Check format after load
TEST_F(TwoDocumentsTest, files23)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, OnLoadResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.Save("files23.yut"));
    std::this_thread::sleep_for(200ms);

    document2.WaitTask(document2.InsertCode(false, true));
    document2.Load("files23.yut");
    document2.WaitLoad();
    std::this_thread::sleep_for(400ms);

    document2.WaitTask(document2.MoveCaretToDocumentBegin(false));
    StringFormatPtr f;
    document2.GetCurrentStringFormat(f);
    ASSERT_TRUE(f->family == "Arial");
    StringFormat format;
    document2.GetStringFormat(ElementId{0, 0, 0}, format);
    ASSERT_TRUE(format.family == "Arial");
}

//Save file with square brackets
TEST_F(DocumentTest, files24)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertOpenSquareBracket(true);
    document.WaitTask(document.InsertCloseSquareBracket(true));
    document.WaitTask(document.Save("files24.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.Load("files24.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mo>[</mo>"\
                        "<mo>]</mo>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Check fonts after load
TEST_F(DocumentTest, files25)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPower(true);
    document.InsertString("4", true);
    document.InsertPower(true);
    document.InsertString("5", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("6", true));
    ASSERT_TRUE(document.ToText() == 
        U"pow(123,pow(4,(5)/(6)))"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.Save("files25.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.Load("files25.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(400ms);

    StringFormat format;
    auto el = document.FindByString({0}, U"4");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 12) << format.size;

    el = document.FindByString({0}, U"5");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 10) << format.size;

    el = document.FindByString({0}, U"6");
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 10) << format.size;
}

//Check of concatenating strings when saving file
TEST_F(DocumentTest, files26)
{
    Start(400);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
        [&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.compressed_file = false;
    document.WaitTask(document.InsertString("Автоматический пересчет выражений помогает пользователям наблюдать за результатами вычислений "\
        "при редактировании формул.", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Save("files26.yut"));
    std::this_thread::sleep_for(200ms);

    std::ifstream file("files26.yut");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    ASSERT_TRUE(content.find("Автоматический пересчет выражений помогает пользователям наблюдать за результатами вычислений "\
        "при редактировании формул.") != std::string::npos) << content;
}

//Check of concatenating strings when saving file
TEST_F(DocumentTest, files27)
{
    Start(400);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly(
        [&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.compressed_file = false;
    document.InsertCode(false, true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertString("Автоматический пересчет выражений помогает пользователям наблюдать за результатами вычислений "\
        "при редактировании формул.", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Save("files27.yut"));
    std::this_thread::sleep_for(200ms);

    std::ifstream file("files27.yut");
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    ASSERT_TRUE(content.find("Автоматический пересчет выражений помогает пользователям наблюдать за результатами вычислений "\
        "при редактировании формул.") != std::string::npos) << content;
}

//Check include file
TEST_F(IncludeDocumentsTest, include_files1)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("var", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("var", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var=5."\
        ) << ToBasicString(document.ToText());
}

//Check include files
TEST_F(IncludeDocumentsTest, include_files2)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.IsChanged() == true);
    document.InsertCode(false, true);
    document.InsertString("var2", true);
    document.InsertAssignment(true);
    document.InsertString("var1", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("var2", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var2=var1+2\n"\
        U"var2=7."\
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include2.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include2.yut")}));
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("var1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("var2", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var1=5.\n"\
        U"var2=7."\
        ) << ToBasicString(document.ToText());
}

//Check of changing include file
TEST_F(IncludeDocumentsTest, include_files3)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.InsertCode(false, true);
    document.InsertString("var2", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("6", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include2.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.IsChanged() == true);
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var1=5."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include2.yut")}));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.IsChanged() == true);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("var1", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("var2", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var1=Unknown identifier\n"\
        U"var2=6."\
        ) << ToBasicString(document.ToText());
}

//Check of circle include files
TEST_F(IncludeDocumentsTest, include_files4)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    std::filesystem::remove("include4_1.yut");
    std::filesystem::remove("include4_2.yut");

    //include1.yut
    document.WaitTask(document.New());
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    document.SetIncludeDocuments(std::vector{std::string("include4_2.yut")});
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Save("include4_1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);
    std::this_thread::sleep_for(1s);

    //include2.yut
    document.WaitTask(document.Save("include4_2.yut"));
    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include4_1.yut")}));
    ASSERT_TRUE(document.IsChanged() == true);
    std::this_thread::sleep_for(4s);
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var1=Unknown identifier"
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include4_2.yut"));

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    document2.Load("include4_2.yut");
    document2.WaitLoad();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document2.IsChanged() == false);
    ASSERT_TRUE(document2.ToText() == 
        U"var1=Unknown identifier"
        ) << ToBasicString(document2.ToText());
}

//Check functions in include file
TEST_F(IncludeDocumentsTest, include_files5)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("func", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("func", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("5", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"func(5)=5."\
        ) << ToBasicString(document.ToText());
}

//Check units in include file
TEST_F(IncludeDocumentsTest, include_files6)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d_m", true);
    document.InsertUnit(true);
    document.WaitTask(document.InsertString("10m", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("d_m", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d_m=1.d_m"
        ) << ToBasicString(document.ToText());
}

//Check include files
TEST_F(IncludeDocumentsTest, include_files7)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.InsertString("10", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document.InsertCode(false, true);
    document.InsertString("var2", true);
    document.InsertAssignment(true);
    document.InsertString("var1", true);
    document.InsertPlus(true);
    document.InsertString("2", true);
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var2=var1+2"\
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include2.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    document.InsertCode(false, true);
    document.InsertString("func", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("x", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include3.yut"));
    std::this_thread::sleep_for(200ms);

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include2.yut"), std::string("include3.yut")}));
    std::this_thread::sleep_for(2s);

    document2.InsertCode(false, true);
    document2.InsertString("var1", true);
    document2.WaitTask(document2.InsertEquation(ResultType::RATIONAL, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(200ms);
    document2.WaitTask(document2.MoveCaretEnd(false));
    document2.WaitTask(document2.InsertParagraph(true));

    document2.InsertString("var2", true);
    document2.WaitTask(document2.InsertEquation(ResultType::RATIONAL, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(200ms);
    document2.WaitTask(document2.MoveCaretEnd(false));
    document2.WaitTask(document2.InsertParagraph(true));

    document2.InsertString("func", true);
    document2.InsertOpenRoundBracket(true);
    document2.InsertString("var2", true);
    document2.InsertCloseRoundBracket(true);
    document2.WaitTask(document2.InsertEquation(ResultType::RATIONAL, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"var1=3(1)/(3)\n"\
        U"var2=5(1)/(3)\n"\
        U"func(var2)=5(1)/(3)"\
        ) << ToBasicString(document2.ToText());
}

//Chain of 3 include files
TEST_F(IncludeDocumentsTest, include_files8)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.InsertString("10", true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("3", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    //include2.yut
    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(1s);
    document.WaitTask(document.Save("include2.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);

    //include3.yut
    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include2.yut")}));
    std::this_thread::sleep_for(1s);
    document.WaitTask(document.Save("include3.yut"));
    std::this_thread::sleep_for(200ms);

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include3.yut")}));
    std::this_thread::sleep_for(4s);
    document2.InsertCode(false, true);
    document2.InsertString("var1", true);
    document2.WaitTask(document2.InsertEquation(ResultType::RATIONAL, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"var1=3(1)/(3)"\
        ) << ToBasicString(document2.ToText());
}

//Load include file from a directory
TEST_F(IncludeDocumentsTest, include_files9)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.InsertString("10", true);
    document.WaitSolver();
    document.WaitTask(document.Save("test/include9.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);
    std::this_thread::sleep_for(200ms);

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document2.InsertCode(false, true);
    document2.WaitTask(document2.Save("test/include_files9.yut"));
    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include9.yut")}));
    std::this_thread::sleep_for(2s);
    document2.InsertString("var1", true);
    document2.WaitTask(document2.InsertEquation(ResultType::AUTO, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"var1=10."\
        ) << ToBasicString(document2.ToText());
}

//Load a file with an include file
TEST_F(IncludeDocumentsTest, include_files10)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    ASSERT_TRUE(document.IsChanged() == false);
    std::this_thread::sleep_for(200ms);

    //include_files10.yut
    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document.InsertCode(false, true);
    document.InsertString("var1", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    document.WaitTask(document.Save("include_files10.yut"));
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"var1=5."
        ) << ToBasicString(document.ToText());

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    document2.Load("include_files10.yut");
    document2.WaitLoad();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document2.IsChanged() == false);
    ASSERT_TRUE(document2.ToText() == 
        U"var1=5."
        ) << ToBasicString(document2.ToText());
}

//Use a variable from an include file
TEST_F(IncludeDocumentsTest, include_files11)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("var", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("S", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("var", true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"var=5\n"\
        U"S=var"
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document2.WaitTask(document2.InsertParagraph(true)); //the next code block is "below" the code block from the include file
    document2.InsertCode(false, true);
    document2.InsertString("var", true);
    document2.InsertAssignment(true);
    document2.WaitTask(document2.InsertString("55", true));
    document2.WaitTask(document2.InsertParagraph(true));
    document2.InsertString("S", true);
    document2.WaitTask(document2.InsertEquation(ResultType::AUTO, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document2.ToText() == 
        U"\n"\
        U"var=55\n"\
        U"S=55."
        ) << ToBasicString(document2.ToText());

    document2.MoveCaretUp(false);
    document2.MoveCaretEnd(false);
    document2.WaitTask(document2.MoveCaretLeft(false));
    document2.WaitTask(document2.InsertString("7", true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"\n"\
        U"var=557\n"\
        U"S=557."
        ) << ToBasicString(document2.ToText());
}

//Use a function from an include file
TEST_F(IncludeDocumentsTest, include_files12)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("func", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("x", true));

    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("var", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("func", true));
    document.InsertOpenRoundBracket(true);
    document.InsertString("2", true);
    document.WaitTask(document.InsertCloseRoundBracket(true));
    document.WaitSolver();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"func(x)=x\n"\
        U"var=func(2)"
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);

    document2.WaitTask(document2.InsertParagraph(true)); //the next code block is "below" the code block from the include file
    document2.InsertCode(false, true);
    document2.InsertString("func", true);
    document2.InsertOpenRoundBracket(true);
    document2.InsertString("x", true);
    document2.InsertCloseRoundBracket(true);
    document2.InsertAssignment(true);
    document2.InsertString("x", true);
    document2.InsertMultiply(true);
    document2.WaitTask(document2.InsertString("2", true));

    document2.WaitTask(document2.InsertParagraph(true));
    document2.InsertString("var", true);
    document2.WaitTask(document2.InsertEquation(ResultType::AUTO, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"\n"\
        U"func(x)=x*2\n"\
        U"var=4."
        ) << ToBasicString(document2.ToText());

    document2.MoveCaretUp(false);
    document2.MoveCaretEnd(false);
    document2.WaitTask(document2.MoveCaretLeft(false));
    document2.WaitTask(document2.InsertString("2", true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"\n"\
        U"func(x)=x*22\n"\
        U"var=44."
        ) << ToBasicString(document2.ToText());
}

//Change include file to incorrect name
TEST_F(IncludeDocumentsTest, include_files13)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("123", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"v=123"
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document2.InsertCode(false, true);
    document2.InsertString("v", true);
    document2.WaitTask(document2.InsertEquation(ResultType::REAL, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"v=123."
        ) << ToBasicString(document2.ToText());

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include234.yut")}));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"v=Unknown identifier"
        ) << ToBasicString(document2.ToText());
}

//Remove include file
TEST_F(IncludeDocumentsTest, include_files14)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    //include1.yut
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("123", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"v=123"
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.Save("include1.yut"));
    std::this_thread::sleep_for(200ms);

    document2.WaitTask(document2.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    std::this_thread::sleep_for(2s);
    document2.InsertCode(false, true);
    document2.InsertString("v", true);
    document2.WaitTask(document2.InsertEquation(ResultType::REAL, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"v=123."
        ) << ToBasicString(document2.ToText());

    document2.WaitTask(document2.SetIncludeDocuments(std::vector<std::string>()));
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.ToText() == 
        U"v=Unknown identifier"
        ) << ToBasicString(document2.ToText());
}

//Load include file from a json string
TEST_F(IncludeDocumentsTest, include_files15)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            ASSERT_TRUE(file_name == "include1.yut");
            std::string json = "{\"file_guid\":\"6dfd2a78-44f9-4dfa-af67-620f0e36f8d9\",\"config\":{\"language\":1,\"use_tabs\":true,\"tab_spaces\":4,\
            \"code_block_border_color\":-16776961,\"numbers_color\":-16776961,\"variables_color\":-16770754,\"functions_color\":-43776,\"units_color\"\
            :-16755456,\"shapes_color\":-16777216,\"error_marks_color\":-65536,\"formula_bg_color\":-1,\"bg_selection_color\":-16776961,\"real_result\"\
            :{\"precision\":3,\"exp\":10,\"default_angle_measure\":0,\"result_angle_measure\":0,\"show_angle_measure\":true},\"integer_result\":\
            {\"result_notation\":2,\"default_notation\":2,\"show_notation\":true},\"rational_result\":{\"fraction_form\":0},\"complex_result\":\
            {\"precision\":3,\"exp\":10,\"default_angle_measure\":0,\"result_angle_measure\":0,\"show_angle_measure\":true,\"form\":0,\"max_count\":10},\
            \"auto_result\":{\"result_auto_advance\":true,\"results_order\":[1,2,3,4,6],\"real_config\":{\"precision\":3,\"exp\":10,\"default_angle_measure\":\
            0,\"result_angle_measure\":0,\"show_angle_measure\":true},\"integer_config\":{\"result_notation\":2,\"default_notation\":2,\"show_notation\":true},\
            \"rational_config\":{\"fraction_form\":0},\"complex_config\":{\"precision\":3,\"exp\":10,\"default_angle_measure\":0,\"result_angle_measure\":0,\
            \"show_angle_measure\":true,\"form\":0,\"max_count\":10},\"array_real_config\":{\"precision\":3,\"exp\":10,\"default_angle_measure\":0,\
            \"result_angle_measure\":0,\"show_angle_measure\":true}},\"include_documents\":[]},\"string_formats\":[{\"id\":\
            \"0ddc5d06-7053-484c-ac3b-6a901ebaf535\",\"family\":\"Arial\",\"size\":14,\"bold\":false,\"italic\":false,\"underline\":false,\"strikethrough\":\
            false,\"subscript\":false,\"superscript\":false,\"text_color\":4278190080,\"text_bg_color\":4294967295,\"text_bg_selection_color\":4278190335},\
            {\"id\":\"b2bfc560-4091-4c68-9bfe-6b9fa0291659\",\"family\":\"Arial\",\"size\":30,\"bold\":true,\"italic\":false,\"underline\":false,\
            \"strikethrough\":false,\"subscript\":false,\"superscript\":false,\"text_color\":4278190080,\"text_bg_color\":4294967295,\
            \"text_bg_selection_color\":4278190335},{\"id\":\"370fa0b2-4e8d-4230-8799-98e5e893a158\",\"family\":\"Arial\",\"size\":26,\
            \"bold\":true,\"italic\":false,\"underline\":false,\"strikethrough\":false,\"subscript\":false,\"superscript\":false,\"text_color\":\
            4278190080,\"text_bg_color\":4294967295,\"text_bg_selection_color\":4278190335},{\"id\":\"fd3be7d1-f3c6-4863-af26-724fe4811ffa\",\
            \"family\":\"Arial\",\"size\":22,\"bold\":true,\"italic\":false,\"underline\":false,\"strikethrough\":false,\"subscript\":false,\
            \"superscript\":false,\"text_color\":4278190080,\"text_bg_color\":4294967295,\"text_bg_selection_color\":4278190335},{\"id\":\
            \"0dbf1b76-9154-4091-9c9f-59c11efe2f75\",\"family\":\"Arial\",\"size\":16,\"bold\":true,\"italic\":false,\"underline\":false,\
            \"strikethrough\":false,\"subscript\":false,\"superscript\":false,\"text_color\":4278190080,\"text_bg_color\":4294967295,\
            \"text_bg_selection_color\":4278190335},{\"id\":\"efdfa0bf-6741-41be-9c2c-d9b654a60413\",\"family\":\"Arial\",\"size\":14,\
            \"bold\":false,\"italic\":true,\"underline\":false,\"strikethrough\":false,\"subscript\":false,\"superscript\":false,\"text_color\":\
            4278190080,\"text_bg_color\":4294967295,\"text_bg_selection_color\":4278190335},{\"id\":\"c1f538ec-1719-44c8-be9f-12e26721c45c\",\
            \"family\":\"Courier New\",\"size\":12,\"bold\":false,\"italic\":false,\"underline\":false,\"strikethrough\":false,\"subscript\":\
            false,\"superscript\":false,\"text_color\":4278190080,\"text_bg_color\":4294967295,\"text_bg_selection_color\":4278190335},\
            {\"id\":\"3416bf54-af4e-4884-aa1b-dd75a81c283b\",\"family\":\"FreeMono\",\"size\":14,\"bold\":false,\"italic\":false,\"underline\":\
            false,\"strikethrough\":false,\"subscript\":false,\"superscript\":false,\"text_color\":4278190080,\"text_bg_color\":4294967295,\
            \"text_bg_selection_color\":4278190335}],\"paragraph_formats\":[{\"name\":\"Text body\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\
            \"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"0ddc5d06-7053-484c-ac3b-6a901ebaf535\"},{\"name\":\"Header 1\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\
            \"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"b2bfc560-4091-4c68-9bfe-6b9fa0291659\"},{\"name\":\"Header 2\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\
            \"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"370fa0b2-4e8d-4230-8799-98e5e893a158\"},{\"name\":\"Header 3\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\
            \"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"fd3be7d1-f3c6-4863-af26-724fe4811ffa\"},{\"name\":\"Header 4\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\
            \"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"0dbf1b76-9154-4091-9c9f-59c11efe2f75\"},{\"name\":\"Example\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\"indent_before\":10,\
            \"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"efdfa0bf-6741-41be-9c2c-d9b654a60413\"},{\"name\":\"Monospace\",\"alignment\":0,\"word_wrap\":1,\"line_spacing\":5,\
            \"indent_before\":10,\"indent_after\":10,\"indent_first_line\":0,\"spacing_before\":10,\"spacing_after\":10,\"default_string_format\":\
            \"c1f538ec-1719-44c8-be9f-12e26721c45c\"},{\"name\":\"Code\",\"alignment\":0,\"word_wrap\":0,\"line_spacing\":2,\"indent_before\":2,\
            \"indent_after\":2,\"indent_first_line\":0,\"spacing_before\":2,\"spacing_after\":2,\"default_string_format\":\
            \"3416bf54-af4e-4884-aa1b-dd75a81c283b\"}],\"text\":{\"id\":\"0\",\"type\":1,\"elements\":[{\"id\":\"0,0\",\"type\":2,\
            \"elements\":[{\"id\":\"0,0,0\",\"type\":3,\"elements\":[{\"id\":\"0,0,0,0\",\"type\":5,\"elements\":[{\"id\":\"0,0,0,0,0\",\
            \"type\":6,\"elements\":[{\"id\":\"0,0,0,0,0,0\",\"type\":7,\"elements\":[{\"id\":\"0,0,0,0,0,0,0\",\"type\":27,\"elements\":\
            [{\"id\":\"0,0,0,0,0,0,0,0\",\"type\":7,\"elements\":[{\"id\":\"0,0,0,0,0,0,0,0,0\",\"type\":8,\"elements\":\"v\",\"format_id\":\
            \"3416bf54-af4e-4884-aa1b-dd75a81c283b\",\"can_merge\":true}]},{\"id\":\"0,0,0,0,0,0,0,1\",\"type\":10,\"elements\":[]},{\"id\":\
            \"0,0,0,0,0,0,0,2\",\"type\":7,\"elements\":[{\"id\":\"0,0,0,0,0,0,0,2,0\",\"type\":8,\"elements\":\"123\",\"format_id\":\
            \"3416bf54-af4e-4884-aa1b-dd75a81c283b\",\"can_merge\":true}]}],\"auto_solve\":true}]}],\"format_name\":\"Code\",\"format_alignment\":0}],\
            \"code_id\":1}]}],\"format_name\":\"Text body\",\"format_alignment\":0}]},\"caret\":{\"id\":\"0,0,0,0,0,0,2,0,3\"},\"selection\":[]}";
            
            document.LoadJsonInclude(json, document_id);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.WaitTask(document.SetIncludeDocuments(std::vector{std::string("include1.yut")}));
    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"v=123."
        ) << ToBasicString(document.ToText());
}

//Reload document that includes another document
TEST_F(IncludeDocumentsTest, include_files16)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.Load("../../test/tests/include_files16_2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(6s);
    ASSERT_TRUE(document.ToText() == 
        U"t=234\n"\
        U"d=234."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.ReSolve(ElementId{}));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"t=234\n"\
        U"d=234."
        ) << ToBasicString(document.ToText());
}

//Include document with an array
TEST_F(IncludeDocumentsTest, include_files17)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });
        
    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.Load("../../test/tests/include_files17_2.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(6s);
    ASSERT_TRUE(document.ToText() == 
        U"t=1234\n"\
        U"d=2468."
        ) << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"t=12345\n"\
        U"d=24690."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.ReSolve(ElementId{}));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"t=12345\n"\
        U"d=24690."
        ) << ToBasicString(document.ToText());
}

//Include document with a list and a string variable
TEST_F(IncludeDocumentsTest, include_files18)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });
        
    EXPECT_CALL(window_mock, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.Load("../../test/tests/include_files18_2.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(6s);
    ASSERT_TRUE(document.ToText() == 
        U"проводник=\"алюминий\"\n"\
        U"длина=1м\n"\
        U"сечение=1pow(мм,2)\n"\
        U"сопротивление=27.мОм"\
        ) << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretWordRight(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"проводник=\"люминий\"\n"\
        U"длина=1м\n"\
        U"сечение=1pow(мм,2)\n"\
        U"сопротивление=Unknown identifier"\
        ) << ToBasicString(document.ToText());
    
    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"проводник=\"алюминий\"\n"\
        U"длина=1м\n"\
        U"сечение=1pow(мм,2)\n"\
        U"сопротивление=27.мОм"\
        ) << ToBasicString(document.ToText());
}

//Delete at the beginning of the document with an include file
TEST_F(IncludeDocumentsTest, include_files19)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });
        
    document.Load("../../test/tests/include_files18_2.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(6s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0, 0, 0, 0, 0, 9}, 
        ElementSelectionState{ElementId{0, 3, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == 
        U"проводник=\"алюминий\"\n"\
        U"длина=1м\n"\
        U"сечение=1pow(мм,2)\n"\
        U"сопротивление=27.мОм"\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.CanUndo());
}

//Chain of two include files
TEST_F(IncludeDocumentsTest, include_files20)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
        });

    document.Load("../../test/tests/include_files20_3.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(8s);
    ASSERT_TRUE(document.ToText() == 
        U"include files\n"\
        U"v=24.\n"\
        U"func(55)=110."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 6, 0, 0, 1, 0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 6, 0, 0, 1, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretUp(false);
    document.InsertParagraph(true);
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("func", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("t", true);
    document.InsertCloseRoundBracket(true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("t", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"include files\n"\
        U"func(t)=t\n"\
        U"v=12.\n"\
        U"func(55)=55."
        ) << ToBasicString(document.ToText());
}

//Check save state
TEST_F(IncludeDocumentsTest, include_files21)
{
    Start(600);

    EXPECT_CALL(window_mock2, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document2.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock2, OnLoadResult).WillRepeatedly(
        [&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, Translate).WillRepeatedly(
        [&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("v", true);
    document.InsertAssignment(true);
    document.WaitTask(document.InsertString("5", true));
    document.WaitSolver();
    document.WaitTask(document.Save("include_files21_1.yut"));
    std::this_thread::sleep_for(200ms);

    document2.InsertCode(false, true);
    document2.InsertString("v", true);
    document2.WaitTask(document2.InsertEquation(ResultType::AUTO, true));
    document2.WaitSolver();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document2.IsChanged() == true);

    yutovo::Config c;
    document2.GetConfig(c);
    c.include_documents.documents.emplace_back(yutovo::Config::IncludeDocument{std::string("./include_files21_1.yut")});
    document2.WaitTask(document2.SetConfig(c, true));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document2.IsChanged() == true);

    document2.WaitTask(document2.ReSolve(ElementId{}));
    document2.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document2.ToText() == 
        U"v=5."
        ) << ToBasicString(document2.ToText());
    ASSERT_TRUE(document2.IsChanged() == true);
}

//Check font after load
TEST_F(IncludeDocumentsTest, include_files22)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly(
        [&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    document.Load("../../test/tests/include_files22_2.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(4s);
    ASSERT_TRUE(document.ToText() == 
        U"var=12."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0,0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 1, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    auto el = document.FindByString({0, 1, 0, 0}, U"var");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.family == "FreeMono") << format.family;
    ASSERT_TRUE(format.size == 14);
}

}
