/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include <QPainter>

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, strings1)
{
    Start(600);

    document.WaitTask(document.SetFontSize(22));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("T", true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">T</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("e", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Te</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.InsertString("x", true);
    document.InsertString("t", true);
    document.WaitTask(document.InsertString("Text", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TextText</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">TextText</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, strings2)
{
    Start(600);

    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Tex</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("Str", true));
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();
    document.InsertString("i", true);
    document.InsertString("n", true);
    document.WaitTask(document.InsertString("g", true));
    ASSERT_TRUE(document.ToText() == U"TextString") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:22px;\">Text</span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"TextStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"TextString") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("Str", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"TextStringStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 13)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, strings3)
{
    Start(600);

    document.InsertString("Str", true);
    document.InsertString("i", true);
    document.InsertString("n", true);
    document.WaitTask(document.InsertString("g", true));
    document.WaitTask(document.DeleteElements(true, true));
    document.WaitTask(document.DeleteElements(true, true));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"String") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 6)) << document.GetEditorState().ToString();
}

//Insert tabulation
TEST_F(DocumentTest, strings4)
{
    Start(600);

    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"\t") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("tab", true));
    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"tab\t") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"tab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.InsertString(U"    ", true);
    document.WaitTask(document.InsertString("string", true));
    ASSERT_TRUE(document.ToText() == U"tab    string") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 13)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"tab    ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"tab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

//Insert tabulation as spaces
TEST_F(DocumentTest, strings5)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.use_tabs = false;
    document.SetConfig(config, true);

    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"    ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("tab", true));
    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"tab ") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"tab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.InsertString("\t", true);
    document.WaitTask(document.InsertString("string", true));
    ASSERT_TRUE(document.ToText() == U"tab string") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"tab") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

//Insert tabulation
TEST_F(DocumentTest, strings6)
{
    Start(600);

    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"\t123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Insert tabulation
TEST_F(DocumentTest, strings7)
{
    Start(600);

    document.InsertString("123", true);
    document.InsertString("\t", true);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"\t123\t") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"123\t") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"\t123\t") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, strings8)
{
    Start(600);

    document.SetInsertMode(false);
    document.WaitTask(document.InsertString("123", true));
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"523") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"523") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.SetInsertMode(true);
    document.WaitTask(document.InsertString("7", true));
    ASSERT_TRUE(document.ToText() == U"5723") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, strings9)
{
    Start(600);

    document.SetInsertMode(false);
    document.WaitTask(document.InsertString("12344", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertString("5", true));
    ASSERT_TRUE(document.ToText() == U"5344") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.InsertString("8", true));
    ASSERT_TRUE(document.ToText() == U"5844") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"5344") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"12344") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"5344") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"5844") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.SetInsertMode(true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.InsertString("7", true));
    ASSERT_TRUE(document.ToText() == U"744") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.InsertString("String", true));
    ASSERT_TRUE(document.ToText() == U"744String") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"744") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(DocumentTest, strings10)
{
    Start(600);

    document.InsertString("12345", true);
    document.SwitchInsertMode();
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("6", true));
    ASSERT_TRUE(document.ToText() == U"12365") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.SwitchInsertMode();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"12345") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"12365") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Check caret positions in a string with a tab
TEST_F(DocumentTest, strings11)
{
    Start(600);

    document.InsertString("\t", true);
    document.WaitTask(document.InsertString("5", true));
    Rect r1, r2, r3;
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.GetCaretRect(r1);
    document.WaitTask(document.MoveCaretRight(false));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r2.left > r1.left);
    r3 = r2;

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString("1", true));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r2.left > r1.left);

    document.WaitTask(document.MoveCaretRight(false));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r2 == r3);

    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("2", true));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r2.left > r1.left);

    document.WaitTask(document.InsertString("34", true));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r2.left > r3.left);

    document.WaitTask(document.MoveCaretRight(false));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r2.left > r3.left);
}

//Check caret positions in a string with tab as spaces
TEST_F(DocumentTest, strings12)
{
    Start(600);

    document.InsertString("123", true);
    document.WaitTask(document.SetBold(true));
    document.InsertString("45", true);
    document.WaitTask(document.SetBold(false));
    document.WaitTask(document.InsertString("678", true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("\t", true));
    Rect r1, r2;
    document.GetCaretRect(r1);

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.InsertString(".", true));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.MoveCaretLeft(false));
    document.GetCaretRect(r2);
    ASSERT_TRUE(r1 == r2);
}

//Check spaces count in a tab
TEST_F(DocumentTest, strings13)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.use_tabs = false;
    config.tab_spaces = 4;
    document.SetConfig(config, true);

    document.WaitTask(document.InsertString("5", true));
    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"5   ") << ToBasicString(document.ToText());
    document.WaitTask(document.InsertString("\t", true));
    ASSERT_TRUE(document.ToText() == U"5       ") << ToBasicString(document.ToText());
    document.WaitTask(document.InsertString("7", true));
    ASSERT_TRUE(document.ToText() == U"5       7") << ToBasicString(document.ToText());
}

TEST_F(DocumentTest, selections1)
{
    Start(600);

    document.WaitTask(document.InsertString("TestString", true));
    ASSERT_TRUE(document.ToText() == U"TestString") << ToBasicString(document.ToText());

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"TestStrin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Test") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"Test") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TestStr") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 4, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TestStrin") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7, 7, 2)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"TestString") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 9, 9, 1)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections2)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("Test", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bo</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 2)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();
    document.MoveCaretLeft(false);
    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2, 0, 2, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>B</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">st</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2, 0, 2, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, selections3)
{
    Start(600);

    document.InsertString("Normal", document.GetStringFormat("Arial", 16, false, false, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    for (int i = 0; i < 9; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 2},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Norm</span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretHome(false);
    for (int i = 0; i < 6; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:16px;\">Normal</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    for (int i = 0; i < 20; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

//Selection of rows
TEST_F(DocumentTest, selections4)
{
    Start(610);

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 12, 49}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 12, 49}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 12, 49}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, selections5)
{
    Start(600);

    document.InsertString("1234", true);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.SelectAll());
    document.WaitTask(document.InsertString("55", true));
    ASSERT_TRUE(document.ToText() == U"55") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Select all with rows
TEST_F(DocumentTest, selections6)
{
    Start(370);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 41},
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Select all with rows
TEST_F(DocumentTest, selections7)
{
    Start(370);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 38},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 38},
        ElementSelectionState{ElementId{0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts1)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Test", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

TEST_F(DocumentTest, inserts2)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Test", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertString("X", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Test</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">TeX</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertString("h", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Teh</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>ld</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts3)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts4)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    for (int i = 0; i < 4; ++i)
        document.DeleteElements(true, true);
    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts5)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    for (int i = 0; i < 2; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">xt</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">xt</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts6)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, inserts7)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertString("Bold", document.GetStringFormat("Arial", 24, true, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.InsertString("T", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertString("d", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">TText</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Boldd</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Bold</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">TText</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Boldd</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();
}

//Insert a string wider than the screen and add chars
TEST_F(DocumentTest, inserts8)
{
    Start(300);

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_isa", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString(" ", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString("l", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">l</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Insert a string wider than the screen
TEST_F(DocumentTest, inserts9)
{
    Start(300);

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_isa", true));
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

TEST_F(DocumentTest, fonts1)
{
    Start(600);

    document.SetFontFamily("Courier New");
    document.SetFontSize(12);
    document.WaitTask(document.InsertString("C", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">C</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();

    document.InsertString("o", true);
    document.InsertString("u", true);
    document.InsertString("r", true);
    document.InsertString("i", true);
    document.InsertString("e", true);
    document.WaitTask(document.InsertString("r", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 15; ++i)
    {
        document.Redo();
        std::this_thread::sleep_for(10ms);
    }
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.SetFontFamily("Times New Roman");
    document.SetFontSize(22);
    document.SetBold(true);
    document.SetItalic(true);
    document.SetUnderline(true);
    document.WaitTask(document.InsertString(" New", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
        document.Redo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Courier</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;text-decoration: underline;\"><strong><em> New</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts2)
{
    Start(600);

    document.WaitTask(document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, false, false, false, Color::Black(), Color::White(), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Tex</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;\">t</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3, 3, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;text-decoration: underline;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong><em>Text</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts3)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Courier New", 14, false, true, false, false), true);
    for (int i = 0; i < 3; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Te</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><strong>xt</strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\"><strong><em>Ita</em></strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 2, 2, 2, 1, 0, 3)) << document.GetEditorState().ToString();
}

//Insert text, code and text with the same font as previous text
TEST_F(DocumentTest, fonts4)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("Text", true);
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("T", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">T</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 1)) << document.GetEditorState().ToString();
}

//Insert code and text with the same font as next text
TEST_F(DocumentTest, fonts5)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertString("T", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">T</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 1)) << document.GetEditorState().ToString();
}

//Check font
TEST_F(DocumentTest, fonts6)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 4}, format));
    ASSERT_TRUE(format.size == 22);
}

TEST_F(DocumentTest, fonts7)
{
    Start(600);

    document.InsertString("Italic", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Italic</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts8)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.InsertString("Italic", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Italic</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, fonts9)
{
    Start(600);

    document.InsertString("TextItalic", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">TextIta</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Te</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">xtIta</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>lic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Check font of a selected string
TEST_F(DocumentTest, fonts10)
{
    Start(600);

    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);

    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);

    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
}

//Set font attributes
TEST_F(DocumentTest, fonts11)
{
    Start(600);

    document.InsertString("TestText", true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">TestTe</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>xt</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.bold);

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Test</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Te</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong><em>xt</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 2}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">Test</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><em>Te</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong><em>xt</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0}, format));
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 2}, format));
    ASSERT_TRUE(format.bold);
    ASSERT_TRUE(format.italic);
    ASSERT_TRUE(format.underline);
}

//Set font attributes
TEST_F(DocumentTest, fonts12)
{
    Start(600);

    document.InsertString("123456789", true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetUnderline(true));

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetItalic(true));

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetBold(true));

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>1234</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong><em>567</em></strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong><em>89</em></strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1234</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>567</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><em>89</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts13)
{
    Start(600);

    document.InsertString("In literary theory, a text is any object that can be read", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>In </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literary theory, a text is any object that can be read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>In </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literary theory, a text is any object that can be read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be read</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts14)
{
    Start(940);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.MoveCaretDown(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">математики, изучающий числа, их отношения и свойства. Предметом </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>арифметики </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">является </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">математики, изучающий числа, их отношения и свойства. Предметом арифметики является </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 64}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 64, 11})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts15)
{
    Start(956);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.MoveCaretDown(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">математики, изучающий числа, их отношения и свойства. Предметом </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>арифметики является </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>понятие числа (натуральные, целые, рациональные, вещественные, к</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">омплексные числа) и его </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">математики, изучающий числа, их отношения и свойства. Предметом арифметики является </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 64}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 64, 20},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 64})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts16)
{
    Start(720);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.MoveCaretDown(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>отношения и </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>свойства. Предметом арифметики является понятие числа </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>(натуральные, целые, рациональные, вещественные, </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 49}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 50, 12},
        ElementSelectionState{ElementId{0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 0, 3, 0}, 0, 49})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts17)
{
    Start(720);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.MoveCaretDown(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>отношения и </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>свойства. Предметом арифметики является понятие числа </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>(натуральные, целые, рациональные, вещественные, комплексные </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>числа) и его свойства.</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 4, 0, 22}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 50, 12},
        ElementSelectionState{ElementId{0, 0}, 2, 3})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts18)
{
    Start(700);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>«число») — раздел математики, изучающий числа, их отношения и </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>свойства. Предметом арифметики является понятие числа </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 54}, 
        ElementSelectionState{ElementId{0, 0}, 0, 3})) << document.GetEditorState().ToString();
}

//Delete 3 rows
TEST_F(DocumentTest, fonts19)
{
    Start(280);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetItalic(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>In literary theory, a text is </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>any object that can be </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>read, whether this object </em></span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">is a work of literature</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object </span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">is a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 26}, 
        ElementSelectionState{ElementId{0, 0}, 0, 3})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts20)
{
    Start(728);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа (натуральные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">целые, рациональные, вещественные, комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>«число») — раздел математики, изучающий числа, их отношения и </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>свойства. Предметом арифметики является понятие числа </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>(натуральные, целые, рациональные, вещественные, комплексные </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>числа) и его свойства.</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 4, 0, 22}, 
        ElementSelectionState{ElementId{0, 0}, 1, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа (натуральные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">целые, рациональные, вещественные, комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 69}, 
        ElementSelectionState{ElementId{0, 0}, 1, 3})) << document.GetEditorState().ToString();
    
    document.Redo();
    document.WaitRedo();
    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа (натуральные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">целые, рациональные, вещественные, комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 69}, 
        ElementSelectionState{ElementId{0, 0}, 1, 3})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts21)
{
    Start(720);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    std::this_thread::sleep_for(200ms);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>«число») — раздел математики, изучающий числа, их отношения и </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>свойства. Предметом арифметики является понятие числа </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>(натуральные, целые, рациональные, вещественные, комплексные </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 4, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetBold(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">«число») — раздел математики, изучающий числа, их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">свойства. Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, комплексные </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 4, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 1, 3})) << document.GetEditorState().ToString();
}

//Text colors
TEST_F(DocumentTest, fonts22)
{
    Start(280);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetColor(Color::Red()));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literary theory, a text is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object </span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">is a work of literature</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBgColor(Color::Green()));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literary </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;bgcolor:rgba(0,255,0,255);\">theory</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">, a text is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object </span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">is a work of literature</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;color:rgba(255,0,0,255);\">In </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literary theory, a text is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object </span>"
                "<span style=\"font-family:'Arial';font-size:14px;\">is a work of literature</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 15}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 9, 6})) << document.GetEditorState().ToString();
}

//Text colors
TEST_F(DocumentTest, fonts23)
{
    Start(600);

    document.WaitTask(document.InsertString("Text ", true));
    document.SetColor(Color::Red());
    document.WaitTask(document.InsertString("red ", true));
    document.SetBgColor(Color::Blue());
    document.WaitTask(document.InsertString("blue", true));
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

//Change format, resize, undo and redo
TEST_F(DocumentTest, fonts24)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, false, false, false, Color::Black(), Color::White(), true));

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.WaitTask(document.MoveCaretToDocumentBegin(false));

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 61}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 61, 11})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la </span>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;\">información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Set font attributes
TEST_F(DocumentTest, fonts25)
{
    Start(600);

    document.WaitTask(document.InsertString("123456789", true));
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetStrikethrough(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123456</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: line-through;\">789</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123456789</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 6, 3})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.SetStrikethrough(true);
    document.WaitTask(document.InsertString("strike", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: line-through;\">strike</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123456789</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123456789</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Set/unset font attributes
TEST_F(DocumentTest, fonts26)
{
    Start(610);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.\nПредметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetFontFamily("Courier New"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:14px;\">Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    std::this_thread::sleep_for(200ms);
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 44}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();
}

//Set the same font family
TEST_F(DocumentTest, fonts27)
{
    Start(610);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.\nПредметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Save("fonts27.yut"));
    document.WaitTask(document.SetFontFamily("Arial"));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();
    ASSERT_FALSE(document.IsChanged());

    document.WaitTask(document.MoveCaretDown(false));
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
}

//Set the same font family in two paragraphs
TEST_F(DocumentTest, fonts28)
{
    Start(610);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.\nПредметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetItalic(true));
    document.MoveCaretDown(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>отношения </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Предметом </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>арифметики </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretWordLeft(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretDown(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em> отношения и свойства.</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Предметом арифметики </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 21}, 
        ElementSelectionState{ElementId{0, 0, 2}, 1, 1}, 
        ElementSelectionState{ElementId{0, 1, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Set the same font family in two paragraphs
TEST_F(DocumentTest, fonts29)
{
    Start(610);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.\nПредметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.WaitTask(document.SetItalic(true));
    document.MoveCaretHome(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>свойства.</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Предметом </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>свойства.</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Предметом ари</em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">фметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 13}, 
        ElementSelectionState{ElementId{0, 0, 2}, 1, 1}, 
        ElementSelectionState{ElementId{0, 1, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>свойства.</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Предметом </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">арифметики является понятие числа </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(натуральные, целые, рациональные, вещественные, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">комплексные числа) и его свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 2}, 1, 1}, 
        ElementSelectionState{ElementId{0, 1, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 1}, 0, 3})) << document.GetEditorState().ToString();
}

//Set the same font family in the paragraph
TEST_F(DocumentTest, fonts30)
{
    Start(610);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetItalic(true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetBold(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong><em>Арифме́тика </em></strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>(др.-греч. ἀριθμητική, arithmētikḗ — от </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>ἀριθμός, arithmós «число») — раздел математики, </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>изучающий числа, их отношения и свойства.</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 41}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Арифме́тика </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 24}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Set the same font family in the paragraph
TEST_F(DocumentTest, fonts31)
{
    Start(612);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetItalic(true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>arithmós «число») — раздел математики, изучающий числа, </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>их отношения и свойства.</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 24}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>Арифме́тика </em></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">(др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 24}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Set the same font family in a row
TEST_F(DocumentTest, fonts32)
{
    Start(610);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики,"\
        " изучающий числа, их отношения и свойства.", true));
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>их отношения и свойства.</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetItalic(true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><em>их отношения и свойства.</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1})) << document.GetEditorState().ToString();
    ASSERT_FALSE(document.CanRedo());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 56}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1})) << document.GetEditorState().ToString();
}

//subscript
TEST_F(DocumentTest, fonts33)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.WaitTask(document.InsertString("sub", document.GetStringFormat("Arial", 24, false, false, false, false, true, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sub>sub</sub></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("String", document.GetStringFormat("Arial", 24, false, true, false, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sub>sub</sub></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><em>String</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sub>sub</sub></span>"\
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
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//superscript
TEST_F(DocumentTest, fonts34)
{
    Start(600);

    document.InsertString("Arial", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.WaitTask(document.InsertString("super", document.GetStringFormat("Arial", 24, false, false, false, false, false, true), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Arial</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sup>super</sup></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("String", document.GetStringFormat("Arial", 24, false, true, false, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Arial</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sup>super</sup></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><em>String</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Arial</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sup>super</sup></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Arial</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5})) << document.GetEditorState().ToString();
}

//check caret after changing font
TEST_F(DocumentTest, fonts35)
{
    Start(600);

    document.InsertString("This example", true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.SetBold(true));
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">This </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>example</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Set font after undo
TEST_F(DocumentTest, fonts36)
{
    Start(600);

    document.WaitTask(document.InsertString("Text", document.GetStringFormat("Arial", 14, false, false, false, false), true));
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetBold(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>Text</strong></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//subscript for selection
TEST_F(DocumentTest, fonts37)
{
    Start(600);

    document.InsertString("Substring", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetSubscript(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sub>Substring</sub></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Substring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//subscript for selection
TEST_F(DocumentTest, fonts38)
{
    Start(600);

    document.InsertString("Substring", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetSubscript(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Sub</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sub>str</sub></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">ing</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Substring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 3, 3})) << document.GetEditorState().ToString();
}

//superscript for selection
TEST_F(DocumentTest, fonts39)
{
    Start(600);

    document.InsertString("Superstring", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.SetSuperscript(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sup>Superstring</sup></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Superstring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//superscript for selection
TEST_F(DocumentTest, fonts40)
{
    Start(600);

    document.InsertString("Superstring", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.SetSuperscript(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Super</span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\"><sup>str</sup></span>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">ing</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Superstring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 5, 3})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete1)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretLeft(false);
    document.DeleteElements(true, true);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Tex</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>talic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ialic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 1)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>talic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete2)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Normal", document.GetStringFormat("Arial", 22, false, false, false, false), true);
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Itali</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ital</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ital</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>tal</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 3)) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>ta</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 2)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, delete3)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 24, false, false, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false, false, false), true);
    document.InsertString("Normal", document.GetStringFormat("Arial", 22, false, false, false, false, false, false), true);
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true);
    document.WaitMainLoop();
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 6)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Norma</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 5)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertString("orm", true));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:24px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();
}

//Delete of Utf-8 characters
TEST_F(DocumentTest, delete4)
{
    Start(600);

    document.WaitTask(document.InsertString("La versión 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.", true));
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 8; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"La versin 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 8)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"La versión 3 de la especificación MathML fue lanzada como Recomendación de W3C el 20 de octubre de 2010.") << 
        ToBasicString(document.ToText());
}

//Delete 3 rows
TEST_F(DocumentTest, delete5)
{
    Start(400);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Delete all
TEST_F(DocumentTest, delete6)
{
    Start(378);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Concatinate a row below, with result wider then the window
TEST_F(DocumentTest, delete7)
{
    Start(340);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">alittle mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 35})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">alittle mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 34})) << document.GetEditorState().ToString();

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
}

//Concatinate a row below with a row wider then the window
TEST_F(DocumentTest, delete8)
{
    Start(227);

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_is a little mysterious.", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Concatinate a row below with a row wider then the window
TEST_F(DocumentTest, delete9)
{
    Start(300);

    document.WaitTask(document.InsertString("The_source_of_the_text_itself_is a little mysterious.", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isalittle </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_isa </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The_source_of_the_text_itself_is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete at the end of a string
TEST_F(DocumentTest, delete10)
{
    Start(300);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">s a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 30})) << document.GetEditorState().ToString();
}

//Backspace at the beginning of a string
TEST_F(DocumentTest, delete11)
{
    Start(298);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">itselfis a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">is a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 30})) << document.GetEditorState().ToString();
}

//Backspace at the end of a string on the second row
TEST_F(DocumentTest, delete12)
{
    Start(400);

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 10})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11})) << document.GetEditorState().ToString();
}

//Delete a string before a code block
TEST_F(DocumentTest, delete13)
{
    Start(400);

    document.WaitTask(document.InsertString("Text: ", true));
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.InsertString("123", true));
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"> </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text: </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Delete strings in a code block divided with a space
TEST_F(DocumentTest, delete14)
{
    Start(400);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertParagraph(true);
    document.InsertString("Initial amount", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("234", true));
    document.WaitTask(document.MoveCaretHome(false));

    for (int i = 0; i < 13; ++i)
        document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mo>×</mo>"\
                            "<mi>234</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete strings in a code block
TEST_F(DocumentTest, delete15)
{
    Start(400);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertParagraph(true);
    document.InsertString("Initial", true);
    document.InsertMultiply(true);
    document.WaitTask(document.InsertString("234", true));
    document.WaitTask(document.MoveCaretHome(false));

    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mo>×</mo>"\
                            "<mi>234</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete an empty selection
TEST_F(DocumentTest, delete16)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Delete an empty selection at the end of a string
TEST_F(DocumentTest, delete17)
{
    Start(600);

    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace string
TEST_F(DocumentTest, replace1)
{
    Start(600);

    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.ReplaceString(U"5678", true));
    ASSERT_TRUE(document.ToText() == 
        U"5678"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"5678"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Replace string
TEST_F(DocumentTest, replace2)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.ReplaceString(U"5678", true));
    ASSERT_TRUE(document.ToText() == 
        U"5678"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"5678"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Replace string with a subscript
TEST_F(DocumentTest, replace3)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.InsertString("v", true));
    document.WaitTask(document.ReplaceString(U"var{2}", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<msub>"\
                            "<mrow>"\
                                "<mi>var</mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi>2</mi>"\
                            "</mrow>"\
                        "</msub>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Replace string inside quotes
TEST_F(DocumentTest, replace4)
{
    Start(600);

    document.WaitTask(document.InsertString("\"алю\"", true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.ReplaceString(U"алюминий", true));
    ASSERT_TRUE(document.ToText() == 
        U"\"алюминий\""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"\"алю\""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"\"алюминий\""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9})) << document.GetEditorState().ToString();
}

//Replace string staring with quote
TEST_F(DocumentTest, replace5)
{
    Start(600);

    document.WaitTask(document.InsertString("\"алю", true));
    document.WaitTask(document.ReplaceString(U"алюминий", true));
    ASSERT_TRUE(document.ToText() == 
        U"\"алюминий"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"\"алю"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"\"алюминий"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9})) << document.GetEditorState().ToString();
}

//Restrict Undo
TEST_F(DocumentTest, undo1)
{
    Start(600);

    Config config;
    document.GetConfig(config);
    config.undo_size = 4;
    document.SetConfig(config, true);

    document.InsertString("Th", true);
    document.InsertString("e ", true);
    document.InsertString("so", true);
    document.InsertString("ur", true);
    document.WaitTask(document.InsertString("ce", true));

    for (int i = 0; i < 4; ++i)
    {
        document.Undo();
        document.WaitUndo();
    }
    document.Undo();
    std::this_thread::sleep_for(200ms);
    document.Undo();
    std::this_thread::sleep_for(200ms);
    document.Undo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Th</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"The ") << ToBasicString(document.ToText());
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"The so") << ToBasicString(document.ToText());
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"The sour") << ToBasicString(document.ToText());
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"The source") << ToBasicString(document.ToText());
    ASSERT_TRUE(!document.CanRedo());
    ASSERT_TRUE(document.GetUndoSize() == 4);
}

//Check undo-redo on a deleting a readonly element
TEST_F(DocumentTest, undo2)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false, false, false, false), true));
    auto el = document.FindByString({0}, U"Bold");
    el->editable = false;

    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.InsertString("t", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_FALSE(document.CanRedo());
    ASSERT_TRUE(document.GetUndoSize() == 3);
    ASSERT_TRUE(document.ToText() == U"TextBoldItalic") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 3, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();
}

//Set Undo size
TEST_F(DocumentTest, undo3)
{
    Start(600);

    document.InsertString("1", true);
    document.InsertString("2", true);
    document.InsertString("3", true);
    document.InsertString("4", true);
    document.InsertString("5", true);
    document.InsertString("6", true);
    document.InsertString("7", true);
    document.InsertString("8", true);
    document.InsertString("9", true);
    document.WaitTask(document.InsertString("0", true));

    Config config;
    document.GetConfig(config);
    config.undo_size = 4;
    document.SetConfig(config, false);
    ASSERT_TRUE(!document.CanRedo());

    document.InsertString("1", true);
    document.WaitTask(document.InsertString("2", true));
    ASSERT_TRUE(!document.CanRedo());
    ASSERT_TRUE(document.ToText() == U"123456789012") << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"12345678") << ToBasicString(document.ToText());

    ASSERT_TRUE(!document.CanUndo());
    ASSERT_TRUE(document.IsChanged());
    document.Undo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"12345678") << ToBasicString(document.ToText());

    ASSERT_TRUE(document.CanRedo());
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"123456789012") << ToBasicString(document.ToText());

    ASSERT_TRUE(document.IsChanged());
    ASSERT_TRUE(!document.CanRedo());
    document.Redo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"123456789012") << ToBasicString(document.ToText());
}

//Undo by words
TEST_F(DocumentTest, undo4)
{
    Start(600);

    document.InsertString("1", true);
    document.InsertString("2", true);
    document.InsertString("3", true);
    document.InsertString(" ", true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertString("5", true));

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"123") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"123 45") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6})) << document.GetEditorState().ToString();
}

//Undo by words
TEST_F(DocumentTest, undo5)
{
    Start(600);

    document.InsertString("1", true);
    document.InsertString("2", true);
    document.InsertString("3", true);
    document.InsertString(" ", true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertString("5", true));

    document.InsertParagraph(true);
    document.InsertString("6", true);
    document.InsertString("7", true);
    document.InsertString("8", true);
    document.WaitTask(document.InsertString("9", true));

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.InsertString("8", true);
    document.InsertString("8", true);
    document.WaitTask(document.InsertString("8", true));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45\n"\
        U"6789"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45\n"\
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45\n"\
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45\n"\
        U"6789"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123 45888\n"\
        U"6789"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9})) << document.GetEditorState().ToString();
}

//Undo by words
TEST_F(DocumentTest, undo6)
{
    Start(600);

    document.InsertString("1", true);
    document.InsertString("2", true);
    document.InsertString("3", true);
    document.InsertString(" ", true);
    document.InsertString(" ", true);
    document.InsertString("4", true);
    document.WaitTask(document.InsertString("5", true));

    document.MoveCaretHome(false);
    document.InsertString("8", true);
    document.WaitTask(document.InsertString("8", true));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123  45"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123 "
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123 "
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"123  45"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 7})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"88123  45"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

}
