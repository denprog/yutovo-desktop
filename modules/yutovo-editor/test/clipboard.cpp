/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include <sstream>
#include <QBuffer>

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

TEST_F(DocumentTest, clipboard1)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == U"Text") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    clipboard_json = U"";
    clipboard_text = U"";
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextTextext</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(clipboard_text == U"ext") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard2)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.InsertString("The source of ", true);
    document.SetBold(true);
    document.SetFontFamily("Courier New");
    document.WaitTask(document.InsertString("the text ", true));
    document.SetBold(false);
    document.SetItalic(true);
    document.WaitTask(document.SetFontFamily("Times New Roman"));
    document.WaitTask(document.SetFontSize(14));
    document.WaitTask(document.InsertString("itself ", true));
    document.SetFontSize(20);
    document.WaitTask(document.SetItalic(false));
    document.WaitTask(document.InsertString("is a little mysterious.", true));
    std::this_thread::sleep_for(400ms);
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
    
    clipboard_json = U"";
    clipboard_text = U"";
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the text </strong></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:14px;\"><em>itself </em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">is a little </span>"\
                "<span style=\"font-family:'Times New Roman';font-size:20px;\">mysterious.</span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">of </span>"\
                "<span style=\"font-family:'Courier New';font-size:22px;\"><strong>the text </strong></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(clipboard_text == U"of the text ") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 2, 9)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard3)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little ", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.PasteText(std::move(clipboard_text)));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 38)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, clipboard4)
{
    Start(630);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 42, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());

    for (int i = 0; i < 5; ++i)
        document.MoveCaretWordLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the strangetext itself is a little ") << ToBasicString(document.ToText());
}

TEST_F(DocumentTest, clipboard5)
{
    Start(327);
    int width = 327;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(std::u32string(U"The <mrow> MathML element is used to group sub-expressions")));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The <mrow> </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">MathML element is </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">used to group </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">sub-expressions</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    std::this_thread::sleep_for(200ms);
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The <mrow> MathML element is used to group sub-expressions") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    width = 480;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(100ms);

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"The <mrow> MathML element is used to group sub-expressions") << ToBasicString(document.ToText());
}

//Paste with paragraph
TEST_F(DocumentTest, clipboard6)
{
    Start(412);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(U"Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\r\n"\
        "Desde el punto de vista de la informática, es un archivo."));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">medio de un documento </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">información era ingresada </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a mano.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Desde el punto de vista de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la informática, es un </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">archivo.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 2, 0, 8)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.SetFontSize(22);
    document.WaitTask(document.PasteText(std::u32string(U"Tradicionalmente, el medio de un documento era el papel y la información era ingresada a mano.\n"\
        "Desde el punto de vista de la informática, es un archivo.")));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">medio de un documento </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">información era ingresada </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a mano.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Desde el punto de vista de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la informática, es un </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">archivo.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 2, 0, 8)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Tradicionalmente, el </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">medio de un documento </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">información era ingresada </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a mano.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Desde el punto de vista de </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">la informática, es un </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">archivo.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 2, 0, 8)) << document.GetEditorState().ToString();
}

//Paste with paragraph
TEST_F(DocumentTest, clipboard7)
{
    Start(400);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(U"Paragraph1.\r\n"\
        "Paragraph2"));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph2</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(100ms);

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(U"Paragraph1.\r\n"\
        "Paragraph2.\r\n"\
        "Paragraph3"));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph2.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(100ms);

    document.SetFontSize(22);
    document.WaitTask(document.PasteText(U"Paragraph1.\n"\
        "Paragraph2.\n"\
        "Paragraph3"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph2.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.New());
    std::this_thread::sleep_for(100ms);

    document.WaitTask(document.PasteText(U"Paragraph1.\n"\
        "\n"\
        "Paragraph3"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 10)) << document.GetEditorState().ToString();
}

//Cut/Paste at the beginning of the second row
TEST_F(DocumentTest, clipboard8)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little strange") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 42})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little strange") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block
TEST_F(DocumentTest, clipboard9)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block after a text
TEST_F(DocumentTest, clipboard10)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.InsertString("Text", true);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy/Paste of different code blocks
TEST_F(DocumentTest, clipboard11)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });
    
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("56", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>56</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Copy/Paste of a code block inside text
TEST_F(DocumentTest, clipboard12)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });
    
    document.WaitTask(document.InsertString("The source of the text itself is a little strange", true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretRight(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">sou</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 3})) << document.GetEditorState().ToString();
}

//Insert a part of a formula into text
TEST_F(DocumentTest, clipboard13)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>23</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a part of a formula into text
TEST_F(DocumentTest, clipboard14)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard15)
{
    Start(500);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.InsertString("The source of the text itself is a little strange.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 3, 0, 0, 5})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard16)
{
    Start(510);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Text.", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretEnd(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.The source of the text itself is </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">a little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 5})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little strange.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 5})) << document.GetEditorState().ToString();
}

//Copy/Paste of paragraphs
TEST_F(DocumentTest, clipboard17)
{
    Start(340);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little strange.The source of the </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">text itself is a little strange.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 2, 0, 32})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little strange.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 15})) << document.GetEditorState().ToString();
}

//Copy/Paste between documents
TEST_F(TwoDocumentsTest, clipboard18)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    EXPECT_CALL(window_mock2, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock2, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));

    document2.WaitTask(document2.Paste(clipboard_json));
    ASSERT_TRUE(document2.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document2.ToHtml();
    ASSERT_TRUE(clipboard_text == U"Text") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document2.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document2.GetEditorState().ToString();

    clipboard_json = U"";
    clipboard_text = U"";
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));

    document2.WaitTask(document2.Paste(clipboard_json));
    ASSERT_TRUE(document2.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Textext</span>"\
            "</p>"\
        "</body>") << 
        document2.ToHtml();
    ASSERT_TRUE(clipboard_text == U"ext") << ToBasicString(clipboard_text);
    ASSERT_TRUE(document2.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document2.GetEditorState().ToString();
}

//Copy-paste rows
TEST_F(DocumentTest, clipboard19)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 14}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 14})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether In literary theory, a text"\
        " is any object that can be read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 14})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Copy-paste rows
TEST_F(DocumentTest, clipboard20)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 14}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 14})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"In literary In literary theory, a text is any object that can be read, whether theory, a text"\
        " is any object that can be read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 21})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
}

//Copy-paste rows
TEST_F(DocumentTest, clipboard21)
{
    Start(500);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 14}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 14})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.PasteText(std::move(clipboard_text)));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be In literary theory, a text is any object"\
        " that can be read, whether read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 8})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 53})) << document.GetEditorState().ToString();
}

//Copy-paste a paragraph
TEST_F(DocumentTest, clipboard22)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        "In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        "Text") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this "\
        "object is a work of literature\nText") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste a text with spaces inside a code block
TEST_F(DocumentTest, clipboard23)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.WaitTask(document.PasteText(U" 123 45"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi>45</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste an empty and wrong json
TEST_F(DocumentTest, clipboard24)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::PasteError);
        });

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    clipboard_json = U"wrong";
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste after a code block
TEST_F(DocumentTest, clipboard25)
{
    Start(600);

    document.InsertString("String", true);
    document.InsertDivision(true);
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretHome(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretRight(false));
    EditorState s = document.GetEditorState();
    ASSERT_TRUE(document.IsEditable(s.caret_state.id));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"String()/()String()/()"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"String()/()"
        ) << ToBasicString(document.ToText());
}

//Paste inside a code block
TEST_F(DocumentTest, clipboard26)
{
    Start(600);

    document.InsertString("String", true);
    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"String()/()ing()/()"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"String()/()"
        ) << ToBasicString(document.ToText());
}

//Paste a division in a division
TEST_F(DocumentTest, clipboard27)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"()/(()/())"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Paste a division in a code block
TEST_F(DocumentTest, clipboard28)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"()/()()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Paste a division in a code block
TEST_F(DocumentTest, clipboard29)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"()/()1()/()23"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 3, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"()/()123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Paste paragraphs in a code block
TEST_F(DocumentTest, clipboard30)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertPlus(true);
    document.InsertString("35", true);
    document.InsertParagraph(true);
    document.InsertString("1234", true);
    document.InsertPlus(true);
    document.InsertString("5678", true);
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretUp(true);
    document.MoveCaretUp(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"234+35\n"
        U"1234+5678\n"
        U"234+35\n"
        U"1234+5678\n"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"234+35\n"
        U"1234+5678\n"
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Paste in second row
TEST_F(DocumentTest, clipboard31)
{
    Start(280);

    document.InsertString("123", true);
    document.InsertDivision(true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.Paste(clipboard_json);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        U"123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Paste in second row
TEST_F(DocumentTest, clipboard32)
{
    Start(280);

    document.InsertString("123", true);
    document.InsertDivision(true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.Paste(clipboard_json);
    document.Paste(clipboard_json);
    document.Paste(clipboard_json);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        U"123()/()123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123()/()123()/()123()/()"
        U"123()/()"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 2})) << document.GetEditorState().ToString();
}

//Copy-paste between paragraphs
TEST_F(DocumentTest, clipboard33)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertString("that can be read", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 10})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary e read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
}

//Copy-paste an image
TEST_F(DocumentTest, clipboard34)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, true, true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Paste an image
TEST_F(DocumentTest, clipboard35)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    document.InsertString("Text", true);

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.PasteImage(data);
    document.WaitTask(document.InsertString("String", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 6})) << document.GetEditorState().ToString();
}

//Paste images one by one
TEST_F(DocumentTest, clipboard36)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.PasteImage(data);
    document.WaitTask(document.PasteImage(data));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.PasteImage(data));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste of code blocks
TEST_F(DocumentTest, clipboard37)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretRight(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("55", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Paste a rational result
TEST_F(DocumentTest, clipboard38)
{
    Start(600);

    document.GetConfig(config);
    config.rational_result.fraction_form = FractionForm::Improper;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.InsertString("34", true);
    document.InsertDivision(true);
    document.InsertString("567", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::RATIONAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)"\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)\n"\
        U"(69775)/(567)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123+(34)/(567)=(69775)/(567)\n"\
        U"(69775)/(567)"
        ) << ToBasicString(document.ToText());
}

//Paste an auto result
TEST_F(DocumentTest, clipboard39)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123=123."\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"123."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"123."
        ) << ToBasicString(document.ToText());
}

//Paste a complex result
TEST_F(DocumentTest, clipboard40)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertMinus(true);
    document.InsertString("1.", true);
    document.InsertPlus(true);
    document.InsertString("3.i", true);
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i\n"\
        U"-1.+3.i"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"-1.+3.i=-1.+3.i\n"\
        U"-1.+3.i"
        ) << ToBasicString(document.ToText());
}

//Paste a real result
TEST_F(DocumentTest, clipboard41)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("arcsin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("1", true);
    document.InsertCloseRoundBracket(true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 10; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U"1.571(rad)"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U"1.571(rad"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"arcsin(1)=1.571(rad)\n"\
        U"1.571(rad)"
        ) << ToBasicString(document.ToText());
}

//Paste a real result
TEST_F(DocumentTest, clipboard42)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertString("2.3", true);
    document.WaitTask(document.InsertEquation(ResultType::REAL, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3\n"\
        U"2.3"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"2.3=2.3\n"\
        U"2.3"
        ) << ToBasicString(document.ToText());
}

//Paste an integer result
TEST_F(DocumentTest, clipboard43)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = true;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)"\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 9; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)\n"\
        U"2345(dec)"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345(dec)\n"\
        U"2345(dec)"
        ) << ToBasicString(document.ToText());
}

//Paste an integer result
TEST_F(DocumentTest, clipboard44)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.integer_result.show_notation = false;
    document.WaitTask(document.SetConfig(config, true));

    document.InsertCode(false, true);
    document.InsertString("2345", true);
    document.WaitTask(document.InsertEquation(ResultType::INTEGER, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345"\
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretRight(false));
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345\n"\
        U"2345"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345\n"\
        U""
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"2345=2345\n"\
        U"2345"
        ) << ToBasicString(document.ToText());
}

//Paste rows from a code block outside of it
TEST_F(DocumentTest, clipboard45)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("5678", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("789", true));

    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"5678\n"\
        U"789\n"\
        U"123=123.\n"\
        U"5678\n"
        ) << ToBasicString(document.ToText());
}

//Paste rows from a code block outside of it
TEST_F(DocumentTest, clipboard46)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("5678", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("789", true));

    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"123\n"\
        U"5678=5678.\n"\
        U"789\n"\
        U"123\n"\
        U"5678=5678.\n"
        ) << ToBasicString(document.ToText());
}

//Undo of cutting a code block
TEST_F(DocumentTest, clipboard47)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretEnd(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == U"123=123.") << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
}

//Undo of copying after a code block
TEST_F(DocumentTest, clipboard48)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"123Text"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123\n"\
        "Text"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123Text"
        ) << ToBasicString(document.ToText());
}


//Undo of pasting paragraphs
TEST_F(DocumentTest, clipboard49)
{
    Start(745);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, "\
        "комплексные числа) и его свойства.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));

    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.\n"\
        U"\n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.\n"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.\n"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.\n"\
        U"\n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.\n"
        ) << ToBasicString(document.ToText());
}

//Undo of pasting a row
TEST_F(DocumentTest, clipboard50)
{
    Start(745);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós \n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós \n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."
        ) << ToBasicString(document.ToText());
}

//Undo of pasting a paragraph after a code block
TEST_F(DocumentTest, clipboard51)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretHome(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"123Text"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"123\n"\
        "Text"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"123Text"
        ) << ToBasicString(document.ToText());
}

//Copy paste a paragraph before a code block
TEST_F(DocumentTest, clipboard52)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Text", true));
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertString("TTT", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123\n"\
        U"TTT"
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Text\n"\
        U"123\n"\
        U"TTT"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123\n"\
        U"TTT"
        ) << ToBasicString(document.ToText());
}

//Undo of pasting paragraphs
TEST_F(DocumentTest, clipboard53)
{
    Start(1000);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertString("TTT", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, \n"\
        U"TTT"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        U"изучающий числа, их отношения и свойства.\n"\
        U"123\n"\
        U"TTT"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, \n"\
        U"TTT"
        ) << ToBasicString(document.ToText());
}

//Copy paste paragraphs with a code block
TEST_F(DocumentTest, clipboard54)
{
    Start(610);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertString("TTT", true);
    document.InsertParagraph(true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretToDocumentEnd(false));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        U"изучающий числа, их отношения и свойства.\n"\
        U"123\n"\
        U"TTT\n"
        U"их отношения и свойства.\n"\
        U"123\n"\
        U"TTT\n"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        U"изучающий числа, их отношения и свойства.\n"\
        U"123\n"\
        U"TTT\n"
        ) << ToBasicString(document.ToText());
}

TEST_F(DocumentTest, clipboard55)
{
    Start(490);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertString("that can be read", true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Inxt</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Paste a formula above a formula
TEST_F(DocumentTest, clipboard56)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123=123."\
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.InsertParagraph(true);
    document.MoveCaretUp(false);

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"123=123."\
        ) << ToBasicString(document.ToText());
}

//Paste a formula above a formula
TEST_F(DocumentTest, clipboard57)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertParagraph(true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"123=123."\
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(false));

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123=123.\n"\
        U"123=123."\
        ) << ToBasicString(document.ToText());
}

//Paste paragraphs after a result
TEST_F(DocumentTest, clipboard58)
{
    Start(600);

    document.InsertString("Paragraph1 text", true);
    document.InsertParagraph(true);
    document.InsertString("Paragraph2 text", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.SelectAll();
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"Paragraph1 text\n"\
        U"Paragraph2 text\n"\
        U"123=123.Paragraph1text\n"\
        U"Paragraph2text\n"\
        U"123=123."\
        ) << ToBasicString(document.ToText());
}

//Paste paragraphs after a result
TEST_F(DocumentTest, clipboard59)
{
    Start(600);

    document.InsertString("Paragraph1", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("Paragraph2", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretToDocumentEnd(false);
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("Paragraph3", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("55", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretRight(true));
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    ASSERT_TRUE(ToBasicString(clipboard_text) == 
        "Paragraph2\n"\
        "123=123.\n"\
        ) << ToBasicString(clipboard_text);

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"Paragraph1\n"\
        U"Paragraph2\n"\
        U"123=123.\n"\
        U"Paragraph3\n"\
        U"55=55.Paragraph2\n"\
        U"123=123.\n"\
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"Paragraph1\n"\
        U"Paragraph2\n"\
        U"123=123.\n"\
        U"Paragraph3\n"\
        U"55=55."\
        ) << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>123.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>55</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>55.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                            "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>123</mi>"\
                            "</mrow>"\
                            "<mo>=</mo>"\
                            "<mrow>"\
                                "<mrow>"\
                                    "<mi>123.</mi>"\
                                "</mrow>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>123.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>55.</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Copy paste paragraphs with a code block
TEST_F(DocumentTest, clipboard60)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretToDocumentEnd(false);
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("55", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    ASSERT_TRUE(clipboard_text == 
        U"arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"123=123.\n"\
        ) << ToBasicString(clipboard_text);

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"123=123.\n"\
        U"55=55.arithmós«число»)—разделматематики,изучающийчисла,ихотношенияисвойства.\n"\
        U"123=123.\n"\
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"123=123.\n"\
        U"55=55."\
        ) << ToBasicString(document.ToText());
}

//Copy paste paragraphs with a code block
TEST_F(DocumentTest, clipboard61)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("55", true));
    document.WaitTask(document.MoveCaretHome(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.Paste(clipboard_json));

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.Paste(clipboard_json));

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.Paste(clipboard_json));

    document.InsertParagraph(true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("67", true));
    document.WaitTask(document.MoveCaretHome(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.MoveCaretDown(true));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>67123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>67</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                            "<mo>+</mo>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>67</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Cut-paste empty paragraphs
TEST_F(DocumentTest, clipboard62)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Text1", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Text2", true));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text1\n"\
        U"Text2"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text1\n"\
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text1\n"\
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Cut-paste text with empty paragraphs
TEST_F(DocumentTest, clipboard63)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.WaitTask(document.InsertString("Text1", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Text2", true));
    ASSERT_TRUE(document.ToText() == 
        U"Text1\n"\
        U"\n"\
        U"Text2"
        ) << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Text1\n"\
        U"\n"\
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Text1\n"\
        U"\n"\
        U"Text2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Cut-paste text with empty paragraphs
TEST_F(DocumentTest, clipboard64)
{
    Start(500);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a "
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 35})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a little strange.\n"\
        U"\n"\
        U"Text."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a "
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 35})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a little strange.\n"\
        U"\n"\
        U"Text."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();
}

//Cut-paste text with empty paragraphs
TEST_F(DocumentTest, clipboard65)
{
    Start(500);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little strange.", true));
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));

    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 8},
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a little "
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a little strange.\n"\
        U"\n"\
        U"Text."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a little "
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"The source of the text itself is a little strange.\n"\
        U"\n"\
        U"Text."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 8},
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();
}

//Cut-paste paragraphs with empty ones
TEST_F(DocumentTest, clipboard66)
{
    Start(600);

    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("that can be read", true));
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 27}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 27, 13},
        ElementSelectionState{ElementId{0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 2, 0, 0}, 0, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    ASSERT_TRUE(clipboard_text == 
        U"is any object\n"\
        U"\n"\
        U"that "\
        ) << ToBasicString(clipboard_text);
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 27})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 27})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 27}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 27, 13},
        ElementSelectionState{ElementId{0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 2, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Cut-paste paragraphs with empty ones
TEST_F(DocumentTest, clipboard67)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("that can be read", true));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Cut-paste paragraphs with empty ones
TEST_F(DocumentTest, clipboard68)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("that can be read", true));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Cut-paste paragraphs with empty ones
TEST_F(DocumentTest, clipboard69)
{
    Start(600);

    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("that can be read", true));

    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 16}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 16})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 16}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();
}

//Cut-paste paragraphs with empty ones
TEST_F(DocumentTest, clipboard70)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("that can be read", true));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Cut-paste paragraphs with empty ones
TEST_F(DocumentTest, clipboard71)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.", 
        true);
    
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 3}, 
        ElementSelectionState{ElementId{0}, 0, 3}, 
        ElementSelectionState{ElementId{0, 3, 0, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"дметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"дметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 3}, 
        ElementSelectionState{ElementId{0}, 0, 3}, 
        ElementSelectionState{ElementId{0, 3, 0, 0}, 0, 3})) << document.GetEditorState().ToString();
}

//Copy-paste paragraphs into an element
TEST_F(DocumentTest, clipboard72)
{
    Start(600);

    document.InsertString("Предметом арифметики является", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.InsertParagraph(true);

    document.MoveCaretUp(true);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"\n"\
        U"Text\n"
        U"\n"\
        U"String\n"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 2, 3})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арText</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ифметики является</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
}

//Copy-paste paragraphs into an element of a different font
TEST_F(DocumentTest, clipboard73)
{
    Start(600);

    document.InsertString("Предметом арифметики является", true);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(200ms);

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.InsertParagraph(true);

    document.MoveCaretUp(true);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"\n"\
        U"Text\n"
        U"\n"\
        U"String\n"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 2, 3})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>ар</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>ифметики </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">является</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>арифметики </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">является</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2})) << document.GetEditorState().ToString();
}

//Copy-paste a code block and a string
TEST_F(DocumentTest, clipboard74)
{
    Start(600);

    document.InsertString("Предметом арифметики является", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.PasteText(U"123");
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("String", true));
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123StringПредметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">П</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Stringредметом арифметики является</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste a code block and a string
TEST_F(DocumentTest, clipboard75)
{
    Start(600);

    document.InsertString("Предметом арифметики является", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.PasteText(U"123");
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("String", true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 1, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 1}, 0, 3})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">StrПредметом арифметики является</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">П</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Strредметом арифметики является</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste a code block and a string
TEST_F(DocumentTest, clipboard76)
{
    Start(600);

    document.InsertString("Предметом арифметики является", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.PasteText(U"123");
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("String", true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 1, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 1}, 0, 3})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Str</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является</span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 29})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretDown(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Предметом арифметики является\n"
        U"123String"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 29})) << document.GetEditorState().ToString();
}

//Cut-paste rows of different paragraphs
TEST_F(DocumentTest, clipboard77)
{
    Start(800);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, "\
        "комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 65})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — рациональные, вещественные, комплексные числа) и его свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 81})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 65}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();
}

//Cut-paste rows of different paragraphs
TEST_F(DocumentTest, clipboard78)
{
    Start(800);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, "\
        "комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));

    document.WaitTask(document.Cut(clipboard_json, clipboard_text));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 65})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — рациональные, вещественные, комплексные числа) и его свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 81})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 65}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 2}, 0, 1})) << document.GetEditorState().ToString();
}

//Copy-paste paragraphs inside a code block
TEST_F(DocumentTest, clipboard79)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("55", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("6789", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>6789</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>6789</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>6789</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>55</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>6789</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Cut-paste a code block before a string
TEST_F(DocumentTest, clipboard80)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("String", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("A", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">AString</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"String"\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Cut-paste rows with a code block
TEST_F(DocumentTest, clipboard81)
{
    Start(472);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 6}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 6, 43}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 53})) << document.GetEditorState().ToString();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of literature</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 2, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 53})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of literature</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 53}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Paste paragraphs from outside
TEST_F(DocumentTest, clipboard82)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString(U"Sample", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString(U"123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.SetCurrentParagraphFormat("Example"));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.MoveCaretUp(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    document.WaitTask(document.InsertString(U"String", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Sample</em></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 6})) << document.GetEditorState().ToString();
    ParagraphFormat f;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, f));
    ASSERT_TRUE(f.name == "Example");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 3, 0, 0, 0}, f));
    ASSERT_TRUE(f.name == "Text body");
}

//Paste from subscript
TEST_F(DocumentTest, clipboard83)
{
    Start(600);

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString(U"Sample", true);
    document.InsertSubscript(true);
    document.WaitTask(document.InsertString(U"index", true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.InsertParagraph(true);
    document.InsertString(U"string", true);
    document.WaitTask(document.InsertSubscript(true));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Sample{index}\n" \
        U"string{index}"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Sample{index}\n" \
        U"string{}"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
}

//Paste from result after loading
TEST_F(TwoDocumentsTest, clipboard84)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123=123."
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.Save("clipboard84.yut"));
    std::this_thread::sleep_for(200ms);

    document2.Load("clipboard84.yut");
    document2.WaitLoad();
    document2.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document2.IsChanged() == false);

    document2.MoveCaretRight(false);
    document2.WaitTask(document2.MoveCaretEnd(true));
    document2.WaitTask(document2.Copy(clipboard_json, clipboard_text));
    document2.WaitTask(document2.MoveCaretToDocumentEnd(false));
    document2.WaitTask(document2.InsertParagraph(true));
    std::this_thread::sleep_for(200ms);
    document2.WaitTask(document2.Paste(clipboard_json));
    ASSERT_TRUE(document2.ToText() == 
        U"123=123.\n"\
        U"123."
        ) << ToBasicString(document2.ToText());
}

//Paste from result of another document
TEST_F(TwoDocumentsTest, clipboard85)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToText() == 
        U"123=123."
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.Save("clipboard85.yut"));
    std::this_thread::sleep_for(200ms);

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document2.WaitTask(document2.Paste(clipboard_json));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document2.ToText() == 
        U"123."
        ) << ToBasicString(document2.ToText());
}

//Paste from another document, save and load
TEST_F(TwoDocumentsTest, clipboard86)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock2, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock2, OnPasteResult).WillOnce([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("Bold string", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBold(true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document2.InsertString("Italic string", true);
    document2.MoveCaretToDocumentBegin(false);
    document2.WaitTask(document2.MoveCaretWordRight(true));
    document2.WaitTask(document2.SetItalic(true));
    document2.WaitTask(document2.SetFontSize(24));
    document2.MoveCaretToDocumentEnd(false);
    document2.WaitTask(document2.InsertParagraph(true));
    document2.WaitTask(document2.Paste(clipboard_json));

    document2.WaitTask(document2.Save("clipboard86.yut"));
    std::this_thread::sleep_for(200ms);

    document2.Load("clipboard86.yut");
    document2.WaitLoad();
    ASSERT_TRUE(document2.IsChanged() == false);
    ASSERT_TRUE(document2.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><em>Italic </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">string</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>Bold </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">string</span>"\
            "</p>"\
        "</body>") << 
        document2.ToHtml();
}

//Paste from an exponent
TEST_F(DocumentTest, clipboard87)
{
    Start(600);

    EXPECT_CALL(window_mock, GetTextSize).WillRepeatedly([&](const std::u32string& text, const StringFormatPtr format)
        {
            return GetTextSizeMock(text, format);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillOnce([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString("2", true);
    document.InsertPower(true);
    document.WaitTask(document.InsertString("multi string", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>multi</mi>"\
                                "<mi>string</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    document.WaitTask(document.MoveCaretHome(true));
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertPlus(true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertPower(true));

    for (int i = 0; i < 9; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    document.WaitTask(document.DeleteElements(false, true));

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>multi</mi>"\
                                "<mi>string</mi>"\
                            "</mrow>"\
                        "</msup>"\
                        "<mo>+</mo>"\
                        "<msup>"\
                            "<mrow>"\
                                "<mi>4</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>multi</mi>"\
                                "<mi>string</mi>"\
                            "</mrow>"\
                        "</msup>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Paste an image before a code block with variables
TEST_F(DocumentTest, clipboard88)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.Load("../../test/tests/crash_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertParagraph(true));

    QImage test_image("../../test/tests/crash_1.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, true, true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.DeleteElements(false, true));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml().find("data:image/png;base64") != std::string::npos) << document.ToHtml();

    for (int i = 0; i < 10; ++i)
    {
        document.Undo();
        document.WaitUndo();
        std::this_thread::sleep_for(200ms);
        document.WaitTask(document.Paste(clipboard_json));
        ASSERT_TRUE(document.ToHtml().find("data:image/png;base64") != std::string::npos) << document.ToHtml();
    }
}

//Paste an image before a code block with variables
TEST_F(DocumentTest, clipboard89)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.Load("../../test/tests/crash_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertParagraph(true));

    QImage test_image("../../test/tests/crash_1.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertImage(data, true, true));
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.DeleteElements(false, true));

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml().find("data:image/png;base64") != std::string::npos) << document.ToHtml();

    for (int i = 0; i < 10; ++i)
    {
        document.Undo();
        document.WaitUndo();
        document.WaitTask(document.Paste(clipboard_json));
        ASSERT_TRUE(document.ToHtml().find("data:image/png;base64") != std::string::npos) << document.ToHtml();
    }

    for (int i = 0; i < 10; ++i)
    {
        document.Undo();
        document.WaitUndo();
        std::this_thread::sleep_for(200ms);
        document.WaitTask(document.Paste(clipboard_json));
        ASSERT_TRUE(document.ToHtml().find("data:image/png;base64") != std::string::npos) << document.ToHtml();
        std::this_thread::sleep_for(500ms);
    }
}

//Copy/Paste between documents with different styles
TEST_F(TwoDocumentsTest, clipboard90)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillOnce([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock2, OnPasteResult).WillOnce([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertString("Example style", true);
    document.InsertParagraph(true);
    document.InsertString("Text body style", true);
    document.InsertParagraph(true);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.SetCurrentParagraphFormat("Example"));

    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document2.InsertString("ExampleParagraph", true);
    document2.InsertParagraph(true);
    document2.InsertString("TextBodyParagraph", true);
    document2.WaitTask(document2.MoveCaretToDocumentBegin(false));
    document2.WaitTask(document2.SetCurrentParagraphFormat("Example"));

    document2.WaitTask(document2.MoveCaretHome(false));
    document2.InsertParagraph(true);
    document2.WaitTask(document2.MoveCaretToDocumentBegin(false));
    document2.WaitTask(document2.Paste(clipboard_json));
    ParagraphFormat f;
    ASSERT_TRUE(document2.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, f));
    ASSERT_TRUE(f.name == "Example");
    ASSERT_TRUE(document2.GetParagraphFormat(ElementId{0, 3, 0, 0, 0}, f));
    ASSERT_TRUE(f.name == "Example");
}

//Replace mode
TEST_F(DocumentTest, clipboard91)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertCode(false, true);
    document.InsertString("55555", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));

    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("String", true);
    document.WaitTask(document.MoveCaretHome(false));
    document.SwitchInsertMode();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55555</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>55555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">tring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Replace mode
TEST_F(DocumentTest, clipboard92)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("12345", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("67", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.SwitchInsertMode();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi>2345</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12345</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, clipboard93)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("12345", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("67", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.SwitchInsertMode();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1</mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                        "<mi>345</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12345</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, clipboard94)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("12345", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertParagraph(true);
    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("67", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.SwitchInsertMode();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1234</mi>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12345</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, clipboard95)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertString("5", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("67", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretEnd(false);
    document.SwitchInsertMode();

    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi>5</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>67</mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Select out of a code block downward
TEST_F(DocumentTest, clipboard96)
{
    Start(600);

    document.Load("../../test/tests/clipboard96.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1s);

    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"12+34j\n"\
        U"sqrt(j)\n"\
        U"pow(j,2)\n"\
        U"root(j+1,3)\n"\
        U"Действительная, мнимая часть числа; модуль:\n"\
        U"12+34j\n"\
        U"sqrt(j)\n"\
        U"pow(j,2)\n"\
        U"root(j+1,3)\n"\
        U"Д"
        ) << ToBasicString(document.ToText());
}

//Select out of a code block upward
TEST_F(DocumentTest, clipboard97)
{
    Start(600);

    document.Load("../../test/tests/clipboard97.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(1s);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"String\n"\
        U"12+34j\n"\
        U"sqrt(j)\n"\
        U"pow(j,2)\n"\
        U"root(j+1,3)\n"\
        U"Действительная, мнимая часть числа; модуль:\n"\
        U"tring\n"\
        U"12+34j\n"\
        U"sqrt(j)\n"\
        U"pow(j,2)\n"\
        U"root(j+1,3)\n"\
        ) << ToBasicString(document.ToText());
}

//Paste from an exponent
TEST_F(DocumentTest, clipboard98)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("sin", true);
    document.InsertOpenRoundBracket(true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertCloseRoundBracket(true);

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretLeft(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.Paste(clipboard_json));

    ASSERT_TRUE(document.ToText() == 
        U"sin(pow(x,(2)/(3)))\n"\
        U"(2)/(3)"
        ) << ToBasicString(document.ToText());

    auto el = document.FindByString({0, 0, 0, 0, 1}, U"2");
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(el->id, format));
    ASSERT_TRUE(format.size == 14) << format.size;
}

//Paste a text after selection
TEST_F(DocumentTest, clipboard99)
{
    Start(2000);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.Load("../../test/tests/Conta de usuário.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.PasteText(U"Após o login bem-sucedido, o usuário tem acesso às seguintes opções, que estão localizadas "\
        "na caixa de diálogo que se abre ao clicar em \"Entrar\""));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Após o login bem-sucedido, o usuário tem acesso às seguintes opções, que estão localizadas "\
        U"na caixa de diálogo que se abre ao clicar em \"Entrar\":\n"\
        U"• sair da conta;\n"\
        U"• alterando o nome que foi inserido durante o registro (não o login - ele não pode ser alterado);\n"\
        U"• alterando o email;\n"\
        U"• alterando a senha.\n"\
        U"\n"\
        U"<- Documentos de usuário"
        ) << ToBasicString(document.ToText());
}

//Paste a long string
TEST_F(DocumentTest, clipboard100)
{
    Start(2000);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    document.WaitTask(document.Load("../../test/tests/clipboard100.txt"));
    document.WaitTask(document.SetConfig(document.config, false));

    const std::u32string sentence = U"Após o login bem-sucedido, o usuário tem acesso às seguintes opções, que estão localizadas na caixa "\
        "de diálogo que se abre ao clicar em \"Entrar\"";
    const int repeat_count = 40;

    std::u32string expected_text;
    std::u32string code_sentence;
    for (char32_t c : sentence)
    {
        if (c != U' ')
            code_sentence.push_back(c);
    }
    for (int i = 0; i < repeat_count; ++i)
    {
        expected_text += sentence;
        if (i + 1 != repeat_count)
            expected_text += U' ';
    }
    ASSERT_TRUE(document.ToText() == expected_text) << ToBasicString(document.ToText());

    document.WaitTask(document.SelectAll());
    document.WaitTask(document.Cut(clipboard_json, clipboard_text));
    document.WaitTask(document.SetConfig(document.config, false));

    document.WaitTask(document.InsertCode(false, true));
    if (!document.WaitTask(document.Paste(clipboard_json), 10000))
        ASSERT_TRUE(false);
    document.WaitTask(document.SetConfig(document.config, false));

    std::u32string expected_code;
    for (int i = 0; i < repeat_count; ++i)
        expected_code += code_sentence;
    ASSERT_TRUE(document.ToText() == expected_code) << ToBasicString(document.ToText());
}

//Paste into an empty symbol
TEST_F(DocumentTest, clipboard101)
{
    Start(600);

    EXPECT_CALL(window_mock, OnCopyResult).WillRepeatedly([&](CopyResult result)
        {
            ASSERT_TRUE(result == CopyResult::Success);
        });

    EXPECT_CALL(window_mock, OnPasteResult).WillRepeatedly([&](PasteResult result)
        {
            ASSERT_TRUE(result == PasteResult::Success);
        });

    document.InsertCode(false, true);
    document.InsertString(U"123", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));

    document.WaitTask(document.InsertDerivative(true));
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi>123</mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi></mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"
                    "<mrow>"
                        "<mfrac>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi></mi>"
                            "</mrow>"
                            "<mrow>"
                                "<mi>d</mi>"
                                "<mi></mi>"
                            "</mrow>"
                        "</mfrac>"
                    "</mrow>"
                "</math>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

}
