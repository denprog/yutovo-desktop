/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include <QPainter>
#include "editor_utils.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

struct ParagraphTest : DocumentTest
{
};

TEST_F(ParagraphTest, resizing1)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    width = 420;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 15)) << document.GetEditorState().ToString();

    width = 450;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 7)) << document.GetEditorState().ToString();

    width = 340;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 23)) << document.GetEditorState().ToString();

    width = 260;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 7)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(false));
    width = 250;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 8)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    width = 230;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 4, 0, 1)) << document.GetEditorState().ToString();

    width = 250;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 9)) << document.GetEditorState().ToString();

    width = 345;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 17)) << document.GetEditorState().ToString();

    width = 340;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 17)) << document.GetEditorState().ToString();

    width = 210;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 4, 0, 1)) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, resizing2)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false, false), true));
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 21}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 21, 2})) << document.GetEditorState().ToString();

    width = 450;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 5, 2})) << document.GetEditorState().ToString();

    width = 540;
    document.WaitTask(document.Resize(width, 400));
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 8, 8, 7)) << document.GetEditorState().ToString();

    width = 450;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 8, 8, 7)) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    width = 400;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 3, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    width = 430;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 3, 5}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();
}

//Delete, resize and undo.
TEST_F(ParagraphTest, resizing3)
{
    Start(400);

    int width = 400;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.DeleteElements(true, true));
    width = 600;
    document.WaitTask(document.Resize(width, 400));

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 53})) << document.GetEditorState().ToString();
}

//Resize with selection
TEST_F(ParagraphTest, resizing4)
{
    Start(315);

    int width = 305;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 22}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 13})) << document.GetEditorState().ToString();

    width = 260;
    document.WaitTask(document.Resize(width, 400));

    width = 236;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 7}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 4})) << document.GetEditorState().ToString();
}

//Resize with selection
TEST_F(ParagraphTest, resizing5)
{
    Start(510);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.InsertDivision(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 37}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 41, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 37})) << document.GetEditorState().ToString();

    width = 400;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 15}, 
        ElementSelectionState{ElementId{0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 15})) << document.GetEditorState().ToString();
}

//Resize with selection
TEST_F(ParagraphTest, resizing6)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false, false), true));
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));

    width = 250;
    document.WaitTask(document.Resize(width, 400));

    width = 200;
    document.WaitTask(document.Resize(width, 400));

    width = 150;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 8}, 
        ElementSelectionState{ElementId{0, 0}, 4, 1})) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, paragraph1)
{
    Start(530);

    int width = 530;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false, false), true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 23)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Ita</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>lic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 3)) << document.GetEditorState().ToString();

    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(3, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(2, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(3, 0, 0, 0)) << document.GetEditorState().ToString();

    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(4, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, paragraph2)
{
    Start(510);

    int width = 510;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    ASSERT_TRUE(document.ToText() == U"The source of the text itself is a little mysterious.") << ToBasicString(document.ToText());

    document.WaitTask(document.SetCurrentParagraphFormat("Header 1"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>The source of the text </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>itself is a little </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>mysterious.</strong></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 11)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 53)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>The source of the text </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>itself is a little </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>mysterious.</strong></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 2, 0, 11)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 53)) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBold(true));
    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">The </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"><strong>source </strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">of the text itself is a little </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 7}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>source </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 7}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11, 4, 7)) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>source </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 7}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">The </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"><strong>source </strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">of the text itself is a little </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 7}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">The </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\"><strong>source </strong></span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">of the text itself is a little </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 7}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>source </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">of the text itself is a little mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 7}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Divide by rows with a code block
TEST_F(ParagraphTest, paragraph3)
{
    Start(100);

    int width = 100;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("45", document.GetStringFormat("Arial", 14, false, false, false, false), true);
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">45</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Insert a formula at the beginning of a row and insert a paragraph at the beginning
TEST_F(ParagraphTest, paragraph4)
{
    Start(380);

    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 380, 400};
        });
    
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//Insert paragraphs in a loaded file
TEST_F(ParagraphTest, paragraph5)
{
    Start(600);

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertParagraph(true));

    auto el = document.GetElement(ElementId{0, 0});
    ASSERT_TRUE(el->type == ElementType::PARAGRAPH && el->ToText() == U"Арифме́тика ") << ToBasicString(el->ToText());
    ASSERT_TRUE(el->elements->Count() == 1) << el->elements->Count();

    el = document.GetElement(ElementId{0, 1});
    ASSERT_TRUE(el->type == ElementType::PARAGRAPH && el->ToText().rfind(U"(др.-греч.", 0) == 0) << ToBasicString(el->ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();
    ASSERT_TRUE(el->elements->Count() == 15) << el->elements->Count();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    el = document.GetElement(ElementId{0, 0});
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(el->type == ElementType::PARAGRAPH && el->ToText().rfind(U"Арифме́тика (др.-греч.", 0) == 0) << ToBasicString(el->ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 12)) << document.GetEditorState().ToString();
}

//Insert paragraphs in a multiline text
TEST_F(ParagraphTest, paragraph6)
{
    Start(395);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Backspace at the beginning of a paragraph
TEST_F(ParagraphTest, paragraph7)
{
    Start(410);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, wh</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ether this object is a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, whether this object is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, wh</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ether this object is a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
}

//Insert a paragraph when caret is on a code block
TEST_F(ParagraphTest, paragraph8)
{
    Start(400);

    document.WaitTask(document.InsertString("Text Code", document.GetStringFormat("Arial", 14, false, false, false, false), true));
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text </span>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Code</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Code</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert paragraph when the caret is on a formula
TEST_F(ParagraphTest, paragraph9)
{
    Start(400);

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertPlus(true);
    document.InsertString("5", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>d</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mo>+</mo>"\
                            "<mi>5</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>d</mi>"\
                        "<mo>+</mo>"\
                        "<mi>5</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert paragraphs in a multiline text
TEST_F(ParagraphTest, paragraph10)
{
    Start(395);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 2, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">theory, a text is any object that can be </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">read, whether this object is a work of </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
}

//Insert a paragraph and a text after a code block
TEST_F(ParagraphTest, paragraph11)
{
    Start(400);

    document.InsertCode(false, true);
    document.WaitTask(document.MoveCaretRight(false));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 1}, format));
    ASSERT_TRUE(format.name == "Text body");
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 1, 0, 11})) << document.GetEditorState().ToString();
}

//Insert a paragraph before a code block
TEST_F(ParagraphTest, paragraph12)
{
    Start(400);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    for (int i = 0; i < 7; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Te</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">xt</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Copy 2 paragraphs and paste them in the beginning
TEST_F(ParagraphTest, paragraph13)
{
    Start(600);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertParagraph(true);
    document.InsertString("Арифметика является древнейшей и одной из основных математических наук; "
        "она тесно связана с алгеброй, геометрией и теорией чисел[1][2].", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и"
        " вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        U"Арифметика является древнейшей и одной из основных математических наук; она тесно связана с алгеброй, геометрией и теорией чисел[1][2].\n"\
        U"Причиной возникновения арифметики стала практическая потребность в счёте и вычислениях, связанных с задачами учёта при централизации сельского хозяйства."
        ) << ToBasicString(document.ToText());
    
    for (int i = 0; i < 5; ++i)
        document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretDown(true);

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифметика является древнейшей и одной из основных математических наук; она тесно связана с алгеброй, геометрией и теорией чисел[1][2].\n"\
        U"Причиной возникновения арифметики стала практическая потребность в счёте и вычислениях, связанных с задачами учёта при централизации сельского хозяйства."\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        U"Арифметика является древнейшей и одной из основных математических наук; она тесно связана с алгеброй, геометрией и теорией чисел[1][2].\n"\
        U"Причиной возникновения арифметики стала практическая потребность в счёте и вычислениях, связанных с задачами учёта при централизации сельского хозяйства."
        ) << ToBasicString(document.ToText());
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        U"Арифметика является древнейшей и одной из основных математических наук; она тесно связана с алгеброй, геометрией и теорией чисел[1][2].\n"\
        U"Причиной возникновения арифметики стала практическая потребность в счёте и вычислениях, связанных с задачами учёта при централизации сельского хозяйства."
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифметика является древнейшей и одной из основных математических наук; она тесно связана с алгеброй, геометрией и теорией чисел[1][2].\n"\
        U"Причиной возникновения арифметики стала практическая потребность в счёте и вычислениях, связанных с задачами учёта при централизации сельского хозяйства."\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        U"Арифметика является древнейшей и одной из основных математических наук; она тесно связана с алгеброй, геометрией и теорией чисел[1][2].\n"\
        U"Причиной возникновения арифметики стала практическая потребность в счёте и вычислениях, связанных с задачами учёта при централизации сельского хозяйства."
        ) << ToBasicString(document.ToText());
}

//Copy 2 paragraphs and paste them in the beginning
TEST_F(ParagraphTest, paragraph14)
{
    Start(600);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.SelectAll());

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"
        ) << ToBasicString(document.ToText());
    
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"\
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"In literary theory, a text is any object that can be read, whether this object is a work of literature\n"
        ) << ToBasicString(document.ToText());
}

//Insert a paragraph between two code blocks
TEST_F(ParagraphTest, paragraph15)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertCode(false, true);
    document.InsertString("56", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));

    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == 
        U"Text123\n"\
        U"56"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Text12356"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Selection an empty paragraph
TEST_F(ParagraphTest, paragraph16)
{
    Start(600);

    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.InsertString("Text", true));
    ASSERT_TRUE(document.ToText() == 
        U"Text"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == 
        U"Text"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Selection empty paragraphs
TEST_F(ParagraphTest, paragraph17)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection empty paragraphs
TEST_F(ParagraphTest, paragraph18)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 2, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of empty paragraphs
TEST_F(ParagraphTest, paragraph19)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());
    
    //select downward
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    //unselect upward
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 3}, 
        ElementSelectionState{ElementId{0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 3})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    // document.WaitTask(document.MoveCaretLeft(true));
    // ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    //select upward
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 3}, 
        ElementSelectionState{ElementId{0}, 2, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();
    
    //unselect downward
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.MoveCaretRight(true);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 3},
        ElementSelectionState{ElementId{0}, 2, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4},
        ElementSelectionState{ElementId{0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of empty paragraphs
TEST_F(ParagraphTest, paragraph20)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());

    //select upward
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 3},
        ElementSelectionState{ElementId{0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 3, 1})) << document.GetEditorState().ToString();

    //unselect downward
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4},
        ElementSelectionState{ElementId{0}, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of empty paragraphs
TEST_F(ParagraphTest, paragraph21)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());

    //select upward
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 0, 4})) << document.GetEditorState().ToString();

    //unselect downward
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 1, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4},
        ElementSelectionState{ElementId{0}, 2, 2})) << document.GetEditorState().ToString();

    // document.WaitTask(document.MoveCaretWordRight(true));
    // ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0},
    //     ElementSelectionState{ElementId{0}, 2, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of empty paragraphs
TEST_F(ParagraphTest, paragraph22)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());

    //select downward
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 4})) << document.GetEditorState().ToString();
    
    //unselect upward
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    //select downward
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 4})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 4, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 4})) << document.GetEditorState().ToString();

    //unselect upward
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Selection of empty paragraphs
TEST_F(ParagraphTest, paragraph23)
{
    Start(600);

    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("that can be read", true));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"\n"\
        U"In literary theory, a text is any object\n"\
        U"\n"\
        U"\n"\
        U"that can be read"
        ) << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 5, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 5, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 4, 1}, 
        ElementSelectionState{ElementId{0, 5, 0, 0}, 0, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 40}, 
        ElementSelectionState{ElementId{0}, 3, 2}, 
        ElementSelectionState{ElementId{0, 5, 0, 0}, 0, 5})) << document.GetEditorState().ToString();

    for (int i = 0; i < 9; ++i)
        document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 2, 3}, 
        ElementSelectionState{ElementId{0, 5, 0, 0}, 0, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 4}, 
        ElementSelectionState{ElementId{0, 5, 0, 0}, 0, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 5}, 
        ElementSelectionState{ElementId{0, 5, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Selection paragraphs with code blocks
TEST_F(ParagraphTest, paragraph24)
{
    Start(1000);

    document.Load("../../test/tests/paragraph24.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 0, 2}, 
        ElementSelectionState{ElementId{0, 4, 0, 0}, 0, 23})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 1}, 
        ElementSelectionState{ElementId{0}, 3, 1},
        ElementSelectionState{ElementId{0, 2, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 4, 0, 0}, 0, 23})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 2, 2},
        ElementSelectionState{ElementId{0, 4, 0, 0}, 0, 23})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 19}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 19, 11},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 3},
        ElementSelectionState{ElementId{0, 4, 0, 0}, 0, 23})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 4},
        ElementSelectionState{ElementId{0, 4, 0, 0}, 0, 23})) << document.GetEditorState().ToString();
}

//Loading Brazilian Portuguese document with translated paragraph names
TEST_F(ParagraphTest, paragraph25)
{
    Start(1000);

    document.Load("../../test/tests/paragraph25.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Check format
TEST_F(ParagraphTest, format1)
{
    Start(600);

    document.InsertString("In literary theory", true);
    document.SetCurrentParagraphFormat("Header 1");
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("a text is any object", true));
    document.WaitTask(document.SetCurrentParagraphFormat("Monospace"));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 4}, format));
    ASSERT_TRUE(format.name == "Header 1");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 1, 0, 0, 4}, format));
    ASSERT_TRUE(format.name == "Monospace");
}

//Check format
TEST_F(ParagraphTest, format2)
{
    Start(600);

    document.InsertString("theory", true);
    document.SetCurrentParagraphFormat("Monospace");
    for (int i = 0; i < 5; ++i)
        document.DeleteElements(true, true);
    document.WaitTask(document.DeleteElements(true, true));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Monospace");
    StringFormat str_format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, str_format));
    ASSERT_TRUE(str_format.family == "Courier New");
}

//Change style of a paragraph with a code block
TEST_F(ParagraphTest, format3)
{
    Start(400);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.SetCurrentParagraphFormat("Header 1"));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:30px;\"><strong>Text</strong></span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Header 1");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 1, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Code") << format.name;

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Text body");
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 1, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Code") << format.name;
}

//Change style across of paragraphs with resize
TEST_F(ParagraphTest, format4)
{
    Start(645);

    int width = 645;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Арифметика является древнейшей и одной из основных математических наук; "
        "она тесно связана с алгеброй, геометрией и теорией чисел[1][2].", true);
    document.InsertParagraph(true);
    document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и"
        " вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetUnderline(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифметика является древнейшей и одной из основных </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">математических наук; она тесно связана с алгеброй, геометрией </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">и теорией чисел[1][2].</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">Причиной возникновения арифметики стала практическая </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">потребность в счёте и вычислениях, связанных с задачами учёта </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">при централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();

    width = 390;
    document.WaitTask(document.Resize(width, 400));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифметика является древнейшей </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">и одной из основных </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">математических наук; она тесно </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">связана с алгеброй, геометрией и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">теорией чисел[1][2].</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Причиной возникновения </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">арифметики стала практическая </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">потребность в счёте и вычислениях, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">связанных с задачами учёта при </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 30}, 
        ElementSelectionState{ElementId{0, 0, 3, 0}, 31, 2},
        ElementSelectionState{ElementId{0, 0}, 4, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 2})) << document.GetEditorState().ToString();
}

//Change style across of paragraphs with resize
TEST_F(ParagraphTest, format5)
{
    Start(645);

    int width = 635;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Арифметика является древнейшей и одной из основных математических наук; "
        "она тесно связана с алгеброй, геометрией и теорией чисел[1][2].", true);
    document.InsertParagraph(true);
    document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и"
        " вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetUnderline(true));

    width = 275;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 4, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 5}, 1, 1},
        ElementSelectionState{ElementId{0, 0}, 6, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 4})) << document.GetEditorState().ToString();
    width = 400;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифметика является древнейшей и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">одной из основных математических </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">наук; она тесно связана с алгеброй, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">геометрией и теорией чисел[1][2].</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">Причиной возникновения </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">арифметики стала практическая </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">потребность в счёте и вычислениях, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">связанных с задачами учёта при </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 2, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 3, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 2})) << document.GetEditorState().ToString();
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифметика является древнейшей и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">одной из основных математических </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">наук; она тесно связана с алгеброй, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">геометрией и теорией чисел[1][2].</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Причиной возникновения </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">арифметики стала практическая </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">потребность в счёте и вычислениях, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">связанных с задачами учёта при </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 30}, 
        ElementSelectionState{ElementId{0, 0}, 3, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 2})) << document.GetEditorState().ToString();
}

//Change style across of paragraphs with resize
TEST_F(ParagraphTest, format6)
{
    Start(705);

    int width = 680;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Арифметика является древнейшей и одной из основных математических наук; "
        "она тесно связана с алгеброй, геометрией и теорией чисел[1][2].", true);
    document.InsertParagraph(true);
    document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и"
        " вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.SetUnderline(true));
    document.WaitTask(document.SetBold(true));

    width = 480;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифметика является древнейшей и одной из </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">основных </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong>математических наук; она тесно </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong>связана с алгеброй, геометрией и теорией </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong>чисел[1][2].</strong></span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong>Причиной возникновения арифметики </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong>стала практическая потребность в счёте и </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\"><strong>вычислениях, связанных с задачами учёта </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">при централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1},
        ElementSelectionState{ElementId{0, 0}, 2, 2},
        ElementSelectionState{ElementId{0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифметика является древнейшей и одной из </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">основных </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">математических наук; она тесно </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">связана с алгеброй, геометрией и теорией </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">чисел[1][2].</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">Причиной возникновения арифметики стала </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">практическая потребность в счёте и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;text-decoration: underline;\">вычислениях, связанных с задачами учёта </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">при централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 3, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1},
        ElementSelectionState{ElementId{0, 0}, 2, 2},
        ElementSelectionState{ElementId{0, 1}, 0, 3})) << document.GetEditorState().ToString();
}

//Change paragraph format
TEST_F(ParagraphTest, format7)
{
    Start(495);

    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.ChangeParagraphFormat("Monospace", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Tradicionalmente, el medio de un documento </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 18}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 18, 11})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 14}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 14, 11})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Tradicionalmente, el medio de un documento </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 18}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 18, 11})) << document.GetEditorState().ToString();
}

//Change paragraph format, resize, undo and redo
TEST_F(ParagraphTest, format8)
{
    Start(500);

    int width = 500;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.ChangeParagraphFormat("Monospace", true));

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 61}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 61, 11})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Tradicionalmente, el medio de un documento era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 61}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 61, 11})) << document.GetEditorState().ToString();
}

//Change paragraph alignment
TEST_F(ParagraphTest, format9)
{
    Start(510);

    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 22})) << document.GetEditorState().ToString();

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 1, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 10, rect.top + 10));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 22})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 22})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">papel y la información</span>"\
            "</p>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Change paragraph alignment and resize
TEST_F(ParagraphTest, format10)
{
    Start(495);

    int width = 495;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 25})) << document.GetEditorState().ToString();

    width = 800;
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"right\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 72})) << document.GetEditorState().ToString();
}

//Edit the second line of paragraph with justify alignment
TEST_F(ParagraphTest, format11)
{
    Start(620);

    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Justify, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"justify\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString(" ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"justify\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la in </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">formación</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Change paragraph format for selected paragraphs
TEST_F(ParagraphTest, format12)
{
    Start(495);

    document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true);
    document.InsertParagraph(true);
    document.InsertString("Причиной возникновения арифметики стала практическая потребность в счёте и "
        "вычислениях, связанных с задачами учёта при централизации сельского хозяйства.", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.ChangeParagraphFormat("Monospace", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Tradicionalmente, el medio de un documento </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">era el papel y la información</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Причиной возникновения арифметики стала </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">практическая потребность в счёте и </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">вычислениях, связанных с задачами учёта при </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">el papel y la información</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Причиной возникновения арифметики стала </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">практическая потребность в счёте и </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">вычислениях, связанных с задачами учёта при </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 40}, 
        ElementSelectionState{ElementId{0}, 0, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Tradicionalmente, el medio de un documento </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">era el papel y la información</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">Причиной возникновения арифметики стала </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">практическая потребность в счёте и </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">вычислениях, связанных с задачами учёта при </span>"\
                "<span style=\"font-family:'Courier New';font-size:12px;\">централизации сельского хозяйства.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 40}, 
        ElementSelectionState{ElementId{0}, 0, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 1})) << document.GetEditorState().ToString();
}

//Resize centered paragraph
TEST_F(ParagraphTest, format13)
{
    Start(495);

    int width = 495;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"center\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 25})) << document.GetEditorState().ToString();

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 1, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 10, rect.top + 10));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));

    width = 800;
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p align=\"center\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Tradicionalmente, el medio de un documento era el papel y la información</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 72})) << document.GetEditorState().ToString();
}

//Copy-paste a paragraph with different style
TEST_F(ParagraphTest, format14)
{
    Start(600);

    document.InsertString("Sample", true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.SetCurrentParagraphFormat("Example"));
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretHome(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Paste(clipboard_json));
    ParagraphFormat f;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 2, 0, 0, 0}, f));
    ASSERT_TRUE(f.name == "Example");
}

//Change document's indents
TEST_F(ParagraphTest, format15)
{
    Start(400);

    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, 400, 400};
        });

    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    TextFormat format;
    ASSERT_TRUE(document.GetTextFormat(format));
    format.left_indent = 40;
    format.right_indent = 40;
    document.WaitTask(document.SetTextFormat(format, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Delete a paragraph
TEST_F(ParagraphTest, delete1)
{
    Start(600);

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">The </span>"\
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
                "<span style=\"font-family:'Arial';font-size:14px;\">The source of the text itself is a little mysterious.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 49},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

//Delete a paragraph
TEST_F(ParagraphTest, delete2)
{
    Start(600);

    document.InsertString("Text.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String.", true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretHome(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 7}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

//Delete a selection between paragraphs
TEST_F(ParagraphTest, delete3)
{
    Start(600);

    document.InsertString("ParagraphText.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String.", true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Parag.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ParagraphText.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 10}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

TEST_F(ParagraphTest, delete4)
{
    Start(600);

    document.InsertString("Paragraph1.", true);
    document.InsertParagraph(true);
    document.InsertString("Paragraph2", true);
    document.InsertParagraph(true);
    document.InsertString("ParagraphText.", true);
    document.InsertParagraph(true);
    document.InsertString("String.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Paragraph5.", true));
    document.MoveCaretUp(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Parag.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph5.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph1.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph2</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ParagraphText.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph5.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 2, 0, 0}, 4, 10}, 
        ElementSelectionState{ElementId{0, 3, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete5)
{
    Start(395);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete6)
{
    Start(400);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretHome(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.SetBold(true);
    document.MoveCaretLeft(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>that </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>that </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, whether this object is </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete7)
{
    Start(400);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.SetBold(true);
    document.MoveCaretHome(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete8)
{
    Start(395);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.SetBold(true);
    document.MoveCaretHome(false);
    document.MoveCaretWordRight(false);
    document.InsertCode(true, true);
    document.MoveCaretHome(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, whether this </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">can be read, whether this </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><strong>is </strong></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 20}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Delete rows between paragraphs
TEST_F(ParagraphTest, delete9)
{
    Start(390);

    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Ttring</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a selection between paragraphs
TEST_F(ParagraphTest, delete10)
{
    Start(600);

    document.InsertString("ParagraphText.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String.", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Paragraph3.", true));
    document.MoveCaretUp(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Parag.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ParagraphText.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">String.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Paragraph3.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 10}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 5})) << document.GetEditorState().ToString();
}

//Delete rows with a formula
TEST_F(ParagraphTest, delete11)
{
    Start(385);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">object is a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretHome(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether re</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 26})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">that can be read, whether this </span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mfrac>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                            "<mrow>"\
                                "<mi></mi>"\
                            "</mrow>"\
                        "</mfrac>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">object is a work of literature</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 28}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 26, 5}, 
        ElementSelectionState{ElementId{0, 0, 1}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 28})) << document.GetEditorState().ToString();
}

//Delete rows with a formula
TEST_F(ParagraphTest, delete12)
{
    Start(310);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 8})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this ()/()object is a work of literature") << 
        ToBasicString(document.ToText());;
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 8}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 8, 5}, 
        ElementSelectionState{ElementId{0, 0, 2}, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 3, 0}, 0, 8})) << document.GetEditorState().ToString();
}

//Delete rows in two paragraphs
TEST_F(ParagraphTest, delete13)
{
    Start(610);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 16}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 12, 49}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 16})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Арифме́тика  work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.\n"\
        "In literary theory, a text is any object that can be read, whether this object is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 16}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 12, 49}, 
        ElementSelectionState{ElementId{0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 1}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 16})) << document.GetEditorState().ToString();
}

//Delete rows with a code block
TEST_F(ParagraphTest, delete14)
{
    Start(490);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    document.SetBold(true);
    document.MoveCaretUp(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.InsertDivision(true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 26}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 30, 4}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 26})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is ct is a work of literature") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 30})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any ()/()object that can be read, whether this object is a work of literature") << 
        ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 26}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 30, 4}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 26})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete15)
{
    Start(490);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики"\
        ", изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 13, 26}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1}, 
        ElementSelectionState{ElementId{0, 0, 3, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, aедметом арифметики является понятие числа "\
        "(натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики"\
        ", изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 3, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 13, 26}, 
        ElementSelectionState{ElementId{0, 0}, 2, 1}, 
        ElementSelectionState{ElementId{0, 0, 3, 0}, 0, 12})) << document.GetEditorState().ToString();
}

//Delete rows
TEST_F(ParagraphTest, delete16)
{
    Start(495);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики"\
        ", изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(true);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 4, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 13, 26}, 
        ElementSelectionState{ElementId{0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 4, 0}, 0, 12})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, aа "\
        "(натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 13})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики"\
        ", изучающий числа, их отношения и свойства. Предметом арифметики является понятие числа (натуральные, целые, рациональные, "\
        "вещественные, комплексные числа) и его свойства.") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 4, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 13, 26}, 
        ElementSelectionState{ElementId{0, 0}, 2, 2}, 
        ElementSelectionState{ElementId{0, 0, 4, 0}, 0, 12})) << document.GetEditorState().ToString();
}

//Delete a paragraph
TEST_F(ParagraphTest, delete17)
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

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"In literary theory, a text is any object that can be read, whether this "\
        "object is a work of literature\nText") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a paragraph after a result
TEST_F(ParagraphTest, delete18)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertEquation(ResultType::AUTO, true);
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == 
        U"234=234.\n"
        U""
        ) << ToBasicString(document.ToText());

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"234=234."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"234=234.\n"
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a paragraph after a code block
TEST_F(ParagraphTest, delete19)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertString("Next1", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Next2", true));
    ASSERT_TRUE(document.ToText() == 
        U"Text234\n"
        U"Next1\n"
        U"Next2"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"Text234Next1\n"
        U"Next2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Text234\n"
        U"Next1\n"
        U"Next2"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Delete words at the beginning of a row
TEST_F(ParagraphTest, delete20)
{
    Start(1250);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.SetBold(true));
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их а."
        ) << ToBasicString(document.ToText());
}

//Delete parts of rows
TEST_F(ParagraphTest, delete21)
{
    Start(745);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, "\
        "комплексные числа) и его свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика аздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
    
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика аздел математики, изучающий числа, их отношения и свойства.\n"\
        U"Предметом арифметики является понятие числа (натуральные, целые, рациональные, вещественные, комплексные числа) и его свойства."
        ) << ToBasicString(document.ToText());
}

//Merge a paragraph with a code block with an above one
TEST_F(ParagraphTest, delete22)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("55", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Text12355"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Text123\n"
        U"55"
        ) << ToBasicString(document.ToText());

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"Text12355"
        ) << ToBasicString(document.ToText());
}

//Delete empty paragraphs
TEST_F(ParagraphTest, delete23)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertString("Text", true));
    document.WaitTask(document.InsertParagraph(false));
    document.WaitTask(document.InsertParagraph(false));
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretUp(true);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U""
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"\n"\
        U"Text"\
        U"\n"\
        U"\n"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 3})) << document.GetEditorState().ToString();
}

//Backspace at the beginning of the line
TEST_F(ParagraphTest, delete24)
{
    Start(686);

    document.WaitTask(document.InsertString("Instantaneous values are determined for a certain time t, based on the U", true));
    document.WaitTask(document.SetSubscript(true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString("m", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.SetSubscript(false));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString(", phase shift angles", true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Instantaneous values are determined for a certain time t, based on the U</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><sub>m</sub></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">, phase shift angles</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    document.WaitTask(document.MoveCaretHome(false));
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Instantaneous values are determined for a certain time t, based on the U, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">phase shift angles</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 72})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Instantaneous values are determined for a certain time t, based on the U</span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"><sub>m</sub></span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">, phase shift angles</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Instantaneous values are determined for a certain time t, based on the U, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">phase shift angles</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 72})) << document.GetEditorState().ToString();
}

//Delete a number of paragraphs
TEST_F(ParagraphTest, delete25)
{
    Start(1300);

    document.Load("../../test/tests/delete_paragraphs1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(4s);

    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Yutovo es una calculadora poderosa con la visualización y "
                    "edición gráfica habitual de operaciones matemáticas dentro de un editor de texto. Con </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">él, puede hacer varios cálculos combinando cálculos y "
                    "texto en un solo documento, a saber:</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Calcular expresiones de cualquier tamaño con cualquier precisión.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 65})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetElement({0})->elements->Count() == 31);
}

//Delete a number of paragraphs
TEST_F(ParagraphTest, delete26)
{
    Start(1300, 740);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 1260, 700};
        });

    document.Load("../../test/tests/delete_paragraphs1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(4s);

    for (int i = 0; i < 9; ++i)
        document.WaitTask(document.MoveCaretPageDown(true));
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Yutovo es una calculadora poderosa con la visualización y "
                    "edición gráfica habitual de operaciones matemáticas dentro de un editor de texto. Con </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">él, puede hacer varios cálculos combinando cálculos y "
                    "texto en un solo documento, a saber:</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Calcular expresiones de cualquier tamaño con cualquier precisión.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Cálculos con números enteros. Se admiten operaciones básicas "
                    "en los sistemas binario, octal, decimal y hexadecimal. Para convertir entre ellos, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">seleccione el sistema numérico en el menú contextual de resultados.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 65})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 29, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 65, 76}, 
        ElementSelectionState{ElementId{0, 1}, 1, 1}, 
        ElementSelectionState{ElementId{0}, 2, 28})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.GetElement({0})->elements->Count() == 31);
}

//Delete a paragraph after a code block
TEST_F(ParagraphTest, delete27)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("5555555555555555", true);
    document.MoveCaretToDocumentEnd(false);

    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("5555555555555555", true);
    document.MoveCaretToDocumentEnd(false);

    document.InsertCode(false, true);
    document.InsertString("5555555555555555", true);
    document.MoveCaretToDocumentEnd(false);

    document.InsertCode(false, true);
    document.InsertString("5555555555555555", true);
    document.MoveCaretToDocumentEnd(false);

    document.InsertCode(false, true);
    document.InsertString("5555555555555555", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(false));

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>5555555555555555</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Insert a char after selection
TEST_F(ParagraphTest, delete28)
{
    Start(1000);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.Load("../../test/tests/paragraph24.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    document.WaitTask(document.InsertString("?", true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Ускорение свободного падения: g_a=9.807(м)/(pow(с,2))\n"\
        U"Высота и масса тела: m=1кг\n"\
        U"h=1м\n"\
        U"Потенциальная энергия: \n"\
        U"?потенциальной энергии с противоположным знаком: A=-(E2-E1)\n"\
        U"A=Unknown identifier"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"Ускорение свободного падения: g_a=9.807(м)/(pow(с,2))\n"\
        U"Высота и масса тела: m=1кг\n"\
        U"h=1м\n"\
        U"Потенциальная энергия: E=m*g_a*h\n"\
        U"E=9.807Дж\n"\
        U"Пусть: E1=2Дж\n"\
        U"E2=1Дж\n"\
        U"Работа - это изменение потенциальной энергии с противоположным знаком: A=-(E2-E1)\n"\
        U"A=1.Дж"
        ) << ToBasicString(document.ToText());
}

//Rescale
TEST_F(ParagraphTest, scale1)
{
    Start(600);

    document.Load("../../test/tests/арифметика_раздел_математики.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be read, whether </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">this object is a work of literature</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<msub>"\
                                "<mrow>"\
                                    "<mi>5654775</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>dec</mi>"\
                                "</mrow>"\
                            "</msub>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5648f7</mi>"\
                                "<mi>(hex)</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"
            "</p>"\
        "</body>") << 
        document.ToHtml();

    Config config;
    document.GetConfig(config);
    config.scale *= 1.1;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">ἀριθμός, arithmós «число») — раздел математики, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">изучающий числа, их отношения и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be read, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">whether this object is a work of literature</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<msub>"\
                                "<mrow>"\
                                    "<mi>5654775</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>dec</mi>"\
                                "</mrow>"\
                            "</msub>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5648f7</mi>"\
                                "<mi>(hex)</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"
            "</p>"\
        "</body>") << 
        document.ToHtml();

    document.GetConfig(config);
    config.scale *= 0.9;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(1s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">arithmós «число») — раздел математики, изучающий числа, </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">их отношения и свойства.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">In literary theory, a text is any object that can be read, whether </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">this object is a work of literature</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<msub>"\
                                "<mrow>"\
                                    "<mi>5654775</mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>dec</mi>"\
                                "</mrow>"\
                            "</msub>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>5648f7</mi>"\
                                "<mi>(hex)</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Rescale with an include document
TEST_F(ParagraphTest, scale2)
{
    Start(600);

    EXPECT_CALL(window_mock, OnLoadInclude).WillRepeatedly([&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    document.Load("../../test/tests/scale2_2.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0, 0, 0, 0, 0, 7})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.GetConfig(config);
    config.scale *= 0.9;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mrow>"\
                            "<mi>проводник</mi>"\
                        "</mrow>"\
                        "<mo>=</mo>"\
                        "<mrow>"\
                            "<mrow>"\
                                "<mi>12.</mi>"\
                                "<mi>Ом</mi>"\
                            "</mrow>"\
                        "</mrow>"\
                    "</mrow>"\
                "</math>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 1})) << document.GetEditorState().ToString();
}

//Scale change with center-aligned text
TEST_F(ParagraphTest, paragraph26)
{
    Start(600);

    document.InsertString("Centered text", true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true));
    Config config;
    document.GetConfig(config);
    config.scale *= 1.2;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(200ms);
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Center);
}

//Scale change with right-aligned text
TEST_F(ParagraphTest, paragraph27)
{
    Start(600);

    document.InsertString("Right text", true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true));
    Config config;
    document.GetConfig(config);
    config.scale *= 0.9;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(200ms);
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Right);
}

//Undo after scale change
TEST_F(ParagraphTest, paragraph28)
{
    Start(600);

    document.InsertString("Text", true);
    Config config;
    document.GetConfig(config);
    config.scale *= 1.2;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.InsertString(" after scale", true));
    ASSERT_TRUE(document.ToText() == U"Text after scale");
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"Text");
}

//Format empty paragraph with center alignment, then type
TEST_F(ParagraphTest, paragraph29)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true));
    document.WaitTask(document.InsertString("Centered", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p>"
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"
            "</p>"
            "<p align=\"center\">"
                "<span style=\"font-family:'Arial';font-size:14px;\">Centered</span>"
            "</p>"
        "</body>") << document.ToHtml();
}

//New paragraph inherits center alignment from previous
TEST_F(ParagraphTest, paragraph30)
{
    Start(600);

    document.InsertString("Center", true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("Also centered", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"
            "<p align=\"center\">"
                "<span style=\"font-family:'Arial';font-size:14px;\">Center</span>"
            "</p>"
            "<p align=\"center\">"
                "<span style=\"font-family:'Arial';font-size:14px;\">Also centered</span>"
            "</p>"
        "</body>") << document.ToHtml();
}

//Resize with center-aligned wrapping text
TEST_F(ParagraphTest, paragraph31)
{
    Start(600);

    int width = 600;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });
    document.InsertString("Long centered text that will wrap when resized", true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Center, true));
    width = 300;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(100ms);
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Center);
}

//Insert paragraph inside code block splits text
TEST_F(ParagraphTest, paragraph32)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == U"1\n23");
}

//Insert paragraph between code blocks after deleting middle paragraph
TEST_F(ParagraphTest, paragraph33)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("1+1", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertString("2+2", true);
    document.MoveCaretHome(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(true, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == U"1+12+2");
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == U"1+1\n2+2");
}

//Change alignment of multiple selected paragraphs
TEST_F(ParagraphTest, paragraph34)
{
    Start(600);

    document.InsertString("Paragraph 1", true);
    document.InsertParagraph(true);
    document.InsertString("Paragraph 2", true);
    document.InsertParagraph(true);
    document.InsertString("Paragraph 3", true);
    document.WaitTask(document.SelectAll());
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Right);
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 1, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Right);
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 2, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Right);
}

//Backspace merges paragraphs with different alignments
TEST_F(ParagraphTest, paragraph35)
{
    Start(600);

    document.InsertString("Left", true);
    document.InsertParagraph(true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Right, true));
    document.InsertString("Right", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"LeftRight");
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Right);
}

//Empty paragraph with Header 1 format
TEST_F(ParagraphTest, paragraph36)
{
    Start(600);

    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.SetCurrentParagraphFormat("Header 1"));
    document.WaitTask(document.InsertString("Header", true));
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 1, 0, 0, 0}, format));
    ASSERT_TRUE(format.name == "Header 1");
    StringFormat str_format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 1, 0, 0, 0}, str_format));
    ASSERT_TRUE(str_format.family == "Arial");
    ASSERT_TRUE(str_format.bold == true);
}

//Delete selection spanning across subscript and base text
TEST_F(ParagraphTest, paragraph37)
{
    Start(600);

    document.InsertString("base", true);
    document.WaitTask(document.InsertSubscript(true));
    document.InsertString("sub", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToText() == U"");
}

//Resize with justify-aligned wrapping text
TEST_F(ParagraphTest, paragraph38)
{
    Start(600);

    int width = 600;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });
    document.InsertString("Long text that should wrap when resized to smaller width", true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Justify, true));
    width = 300;
    document.WaitTask(document.Resize(width, 400));
    std::this_thread::sleep_for(100ms);
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Justify);
}

//Forward delete at end of paragraph merges with next
TEST_F(ParagraphTest, paragraph39)
{
    Start(600);

    document.InsertString("First", true);
    document.InsertParagraph(true);
    document.InsertString("Second", true);
    document.MoveCaretUp(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == U"FirstSecond");
}

//Undo/redo after inserting paragraph inside code block
TEST_F(ParagraphTest, paragraph40)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("abc", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == U"ab\nc");
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"abc");
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"ab\nc");
}

//Scale change with justify alignment
TEST_F(ParagraphTest, paragraph41)
{
    Start(600);

    document.InsertString("Justified text", true);
    document.WaitTask(document.ChangeParagraphFormat(ParagraphFormat::Alignment::Justify, true));
    Config config;
    document.GetConfig(config);
    config.scale *= 1.1;
    document.SetConfig(config, true);
    std::this_thread::sleep_for(200ms);
    ParagraphFormat format;
    ASSERT_TRUE(document.GetParagraphFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.alignment == ParagraphFormat::Alignment::Justify);
}

//Insert text after exiting code block
TEST_F(ParagraphTest, paragraph42)
{
    Start(600);

    document.InsertCode(false, true);
    document.MoveCaretRight(false);
    document.InsertString("1+1", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertString(" text", true));
    ASSERT_TRUE(document.ToText() == U"1+1 text") << ToBasicString(document.ToText());
}

//Change font size of selected text
TEST_F(ParagraphTest, paragraph43)
{
    Start(600);

    document.InsertString("Big text", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.ChangeStringFormat("Arial", 22, false, false, false, false, false, false, Color::Black(), Color::White(), true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.size == 22);
}

//Delete empty paragraph between two text paragraphs
TEST_F(ParagraphTest, paragraph44)
{
    Start(600);

    document.InsertString("First", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.InsertString("Second", true);
    document.MoveCaretUp(false);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToText() == U"First\nSecond");
}

//Insert paragraph at document beginning before text
TEST_F(ParagraphTest, paragraph45)
{
    Start(600);

    document.InsertString("Text", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToText() == U"\nText");
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Undo/redo single paragraph insertion
TEST_F(ParagraphTest, paragraph46)
{
    Start(600);

    document.InsertString("A", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("B", true));
    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == U"A\n") << ToBasicString(document.ToText());
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToText() == U"A\nB") << ToBasicString(document.ToText());
}

//Move caret word-left within paragraph
TEST_F(ParagraphTest, paragraph47)
{
    Start(600);

    document.InsertString("First word", true);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 6})) << document.GetEditorState().ToString();
}

//Insert paragraph after power formula
TEST_F(ParagraphTest, paragraph48)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("x", true);
    document.InsertPower(true);
    document.InsertString("2", true);
    document.MoveCaretEnd(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.InsertString("y", true));
    ASSERT_TRUE(document.ToText() == U"pow(x,2)\ny") << ToBasicString(document.ToText());
}

//Bold formatting of selected text
TEST_F(ParagraphTest, paragraph49)
{
    Start(600);

    document.InsertString("Bold text", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.SetBold(true));
    StringFormat format;
    ASSERT_TRUE(document.GetStringFormat(ElementId{0, 0, 0, 0, 0}, format));
    ASSERT_TRUE(format.bold == true);
}

}
