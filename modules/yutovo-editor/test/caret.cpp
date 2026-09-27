/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"

namespace yutovo_test
{

using namespace yutovo;

using namespace std::chrono_literals;

TEST_F(DocumentTest, caret1)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("Text", true));
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 4, 1, 3)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0, 0, 3)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.InsertString(" Word2", true);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.InsertString(" Word3", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false, false), true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5)) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 1, 4)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 2, 6)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text Word2 Word3</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Courier';font-size:24px;\"><em>Italic</em></span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 5},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 5, 11},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 5, 0, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 16}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 16, 11, 5)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 5},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret2)
{
    Start(600);

    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 42)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 41)) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 1)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 10)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 1, 0, 11)) << document.GetEditorState().ToString();

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret3)
{
    Start(600);

    document.SetFontSize(22);
    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the text itself is a little </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 0)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 0)) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(1, 0, 0, 11)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 0, 42)) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret4)
{
    Start(600);

    document.InsertString("Text Word2 Word3 ", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.InsertString("Italic", document.GetStringFormat("Courier", 24, false, true, false, false), true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(true);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 6},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret5)
{
    Start(600);

    document.InsertString("Text", document.GetStringFormat("Arial", 22, false, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 18, false, true, false, false), true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.InsertString("String1 String2 String3", document.GetStringFormat("Arial", 20, false, false, false, false), true);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 String2 String3</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">Text</span>"\
                "<span style=\"font-family:'Times New Roman';font-size:18px;\"><em>Italic</em></span>"\
                "<span style=\"font-family:'Times New Roman';font-size:34px;\"><strong>Bold</strong></span>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String1 </span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:20px;\">String2 String3</span>"\
            "</p>"\
        "</body>") 
        << document.ToHtml();
    
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 7}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 7, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3, 6}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 6, 2}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(0, 0, 3, 6)) << document.GetEditorState().ToString();
    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 3}, 6, 2}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret6)
{
    Start(600);
    
    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertDivision(true));
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("String", true));
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretUp(false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 3})) << document.GetEditorState().ToString();
}

//PageUp/PageDown
TEST_F(DocumentTest, caret7)
{
    Start(680);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 9, 0, 12})) << document.GetEditorState().ToString();

    document.MoveCaretPageUp(false);
    document.WaitTask(document.MoveCaretPageUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 11, 0, 7})) << document.GetEditorState().ToString();

    for (int i = 0; i < 6; ++i)
        document.MoveCaretPageDown(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 5, 1, 0, 42})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretPageUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 10, 0, 38})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret8)
{
    Start(378);
    
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(200ms);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 11}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretEnd(false));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 4})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret9)
{
    Start(600);

    document.InsertString("1234", true);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret10)
{
    Start(600);

    document.InsertString("1234", true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret11)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.SelectAll());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret12)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 4})) << document.GetEditorState().ToString();

    for (int i = 0; i < 3; ++i)
        document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 1, 3})) << document.GetEditorState().ToString();
}

//Select all
TEST_F(DocumentTest, caret13)
{
    Start(378);
    
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Text.", true));
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 4, 7},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 38},
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 41},
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret14)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertString("Bold", document.GetStringFormat("Times New Roman", 34, true, false, false, false), true);
    document.InsertString("Italic", document.GetStringFormat("Times New Roman", 24, false, true, false, false), true);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 6}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2},
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 5})) << document.GetEditorState().ToString();

    for (int i = 0; i < 5; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 3})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 3})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret15)
{
    Start(600);

    document.InsertString("12345", true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret16)
{
    Start(600);

    document.InsertString("12345", true);
    document.InsertParagraph(true);
    document.InsertString("Text", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, caret17)
{
    Start(380);

    document.InsertString("The source of the text itself is a little mysterious.", true);
    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//PageUp with selection
TEST_F(DocumentTest, caret18)
{
    Start(678);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 10, 0, 31}, 
        ElementSelectionState{ElementId{0, 3, 10, 0}, 31, 7},
        ElementSelectionState{ElementId{0}, 4, 1},
        ElementSelectionState{ElementId{0, 5}, 0, 1},
        ElementSelectionState{ElementId{0, 5, 1, 0}, 0, 30})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretUp(false);
    document.MoveCaretWordLeft(false);
    document.InsertDivision(true);
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 9, 0, 42}, 
        ElementSelectionState{ElementId{0, 3, 9, 0}, 42, 9},
        ElementSelectionState{ElementId{0, 3}, 10, 1},
        ElementSelectionState{ElementId{0}, 4, 1},
        ElementSelectionState{ElementId{0, 5, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 3, 9, 0, 41}, 
        ElementSelectionState{ElementId{0, 3, 9, 0}, 41, 10},
        ElementSelectionState{ElementId{0, 3}, 10, 1},
        ElementSelectionState{ElementId{0}, 4, 1},
        ElementSelectionState{ElementId{0, 5, 0}, 0, 1})) << document.GetEditorState().ToString();
}

//PageDown with selection
TEST_F(DocumentTest, caret19)
{
    Start(680);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2s);

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 9, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 12, 58},
        ElementSelectionState{ElementId{0, 0}, 1, 8},
        ElementSelectionState{ElementId{0, 0, 9, 0}, 0, 12})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentEnd(false);
    for (int i = 0; i < 5; ++i)
        document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 5, 1, 0, 42}, 
        ElementSelectionState{ElementId{0, 4, 3, 0}, 43, 18},
        ElementSelectionState{ElementId{0, 4}, 4, 3},
        ElementSelectionState{ElementId{0}, 5, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.InsertDivision(true);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 7, 0, 14}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0}, 1, 6},
        ElementSelectionState{ElementId{0, 0, 7, 0}, 0, 14})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 8, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 10},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 0}, 1, 7},
        ElementSelectionState{ElementId{0, 0, 8, 0}, 0, 3})) << document.GetEditorState().ToString();
}

//Selection a string with mouse
TEST_F(DocumentTest, caret20)
{
    Start(600);

    document.WaitTask(document.InsertString("TextString", true));
    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left, rect.top + 5));
    document.WaitTask(document.Select(rect.left, rect.top + 5, rect.left + 15, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect.left, rect.top + 5, rect.left + 85, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 10}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect.left, rect.top + 5, rect.left + 185, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 10}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.WaitTask(document.Select(rect.left + 85, rect.top + 5, rect.left + 65, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 8}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 8, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect.left + 85, rect.top + 5, rect.left, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect.left + 85, rect.top + 5, rect.left - 10, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection a string with mouse
TEST_F(DocumentTest, caret21)
{
    Start(600);

    document.WaitTask(document.InsertString("TextString", true));
    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0, 0}, rect);

    document.WaitTask(document.Select(rect.GetRight() + 5, rect.top + 5, rect.GetRight() + 4, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 10})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect.GetRight() + 5, rect.top + 5, rect.left + 20, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 8})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaret(rect.left - 5, rect.top + 5));
    document.WaitTask(document.Select(rect.left - 5, rect.top + 5, rect.left - 4, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect.left - 5, rect.top + 5, rect.left + 20, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Selection a row with mouse
TEST_F(DocumentTest, caret22)
{
    Start(600);

    document.InsertString("Text", true);
    document.SetBold(true);
    document.WaitTask(document.InsertString("Bold", true));
    Rect rect1, rect2;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 0, 1}, rect2);
    document.WaitTask(document.MoveCaret(rect1.left + 15, rect1.top + 5));
    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect1.left + 20, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect2.left + 10, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection a row with a code block with mouse
TEST_F(DocumentTest, caret23)
{
    Start(600);

    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));
    Rect rect1, rect2;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 0, 1}, rect2);
    document.WaitTask(document.MoveCaret(rect1.left + 15, rect1.top + 5));
    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect1.left + 20, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect2.left + 5, rect2.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Selection a row with a code block with mouse
TEST_F(DocumentTest, caret24)
{
    Start(600);

    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.SetBold(true);
    document.WaitTask(document.InsertString("Bold", true));
    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 0, 2}, rect3);
    document.WaitTask(document.MoveCaret(rect1.left + 15, rect1.top + 5));
    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect1.left + 20, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect2.left + 5, rect2.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect1.left + 15, rect1.top + 5, rect3.left + 10, rect3.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection a row with a code block with mouse from right to left
TEST_F(DocumentTest, caret25)
{
    Start(600);

    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.SetBold(true);
    document.WaitTask(document.InsertString("Bold", true));
    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 0, 2}, rect3);
    document.WaitTask(document.MoveCaret(rect3.GetRight() - 5, rect3.top + 5));
    document.WaitTask(document.Select(rect3.GetRight(), rect3.top + 5, rect3.GetRight() - 10, rect3.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 3}, 
        ElementSelectionState{ElementId{0, 0, 0, 2}, 3, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect3.GetRight(), rect3.top + 5, rect2.GetRight() - 10, rect2.top + 15));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect3.GetRight(), rect3.top + 5, rect2.left + rect2.width / 2, rect2.top + 15));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect3.GetRight(), rect3.top + 5, rect2.left + 10, rect2.top + 15));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(rect3.GetRight(), rect3.top + 5, rect1.GetRight() - 15, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 2, 2},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Selection from a code block outside
TEST_F(DocumentTest, caret26)
{
    Start(600);

    document.InsertDivision(true);
    document.InsertDivision(true);
    document.WaitTask(document.InsertString("123", true));
    auto el = document.FindByString({0}, U"123");
    Rect rect1, rect2;
    document.GetElementRect(el->id, rect1);
    document.WaitTask(document.MoveCaret(rect1.left + 1, rect1.top + 1));
    document.WaitTask(document.Select(rect1.left + 1, rect1.top + 1, rect1.left + 1, rect1.GetBottom() + 5));
    ElementId p_id = document.FindCurrentParentByType(ElementType::DIVISION);
    auto p = document.GetElement(p_id);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{p->id, 0, 1})) << document.GetEditorState().ToString();

    document.GetElementRect(p->id, rect2);
    document.WaitTask(document.Select(rect1.left + 1, rect1.top + 1, rect2.left + 1, rect2.GetBottom() + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    p_id = document.FindCurrentParentByType(ElementType::CODE_BLOCK);
    document.GetElementRect(p_id, rect2);
    document.WaitTask(document.Select(rect1.left + 1, rect1.top + 1, rect2.left, rect2.GetBottom() + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaret(rect1.left + 1, rect1.top + 1));
    p_id = document.FindCurrentParentByType(ElementType::DIVISION);
    auto s = document.FindByType(p_id, ElementType::SHAPE);
    document.GetElementRect(s->id, rect2);
    document.WaitTask(document.Select(rect1.left + 1, rect1.top + 1, rect2.left + 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    
    auto d = document.GetElement(p_id);
    d = document.GetElement(d->parent->parent->id);
    document.GetElementRect(d->id, rect2);
    document.WaitTask(document.Select(rect1.left + 10, rect1.top + 10, rect2.left - 1, rect2.top + rect2.height / 2));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection inside a paragraph
TEST_F(DocumentTest, caret27)
{
    Start(600);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);
    Rect rect1, rect2;
    document.GetElementRect(ElementId{0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 1}, rect2);
    document.WaitTask(document.MoveCaret(100, rect1.top + 1));

    document.WaitTask(document.Select(100, rect1.top + 1, 100, rect1.GetBottom() + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 10})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 100, rect2.GetBottom() - 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 57}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 9})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 100, rect2.GetBottom() + 10));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 57}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 9})) << document.GetEditorState().ToString();
}

//Selection inside a paragraph
TEST_F(DocumentTest, caret28)
{
    Start(370);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 2}, rect3);
    document.WaitTask(document.MoveCaret(100, rect1.top + 1));

    document.WaitTask(document.Select(100, rect1.top + 1, 90, rect1.GetBottom() + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 9, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 90, rect2.GetBottom() - 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 31},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 90, rect2.GetBottom() + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 10, 31},
        ElementSelectionState{ElementId{0, 0, 1, 0}, 0, 7})) << document.GetEditorState().ToString();
}

//Selection inside a paragraph upward
TEST_F(DocumentTest, caret29)
{
    Start(370);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 2}, rect3);
    document.WaitTask(document.MoveCaret(100, rect3.top + 1));

    document.WaitTask(document.Select(100, rect3.top + 1, 110, rect3.top - 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 10}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 9, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect3.top + 1, 110, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 10}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 10, 21},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 9})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect3.top + 1, 110, rect2.top - 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 10}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 10, 21},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 9})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect3.top + 1, 110, rect1.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 11, 30},
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 9})) << document.GetEditorState().ToString();
}

//Selection inside a paragraph upward
TEST_F(DocumentTest, caret30)
{
    Start(370);

    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 2}, rect3);
    document.WaitTask(document.MoveCaret(100, rect3.top + 1));

    document.WaitTask(document.Select(100, rect3.top + 1, 90, rect3.top - 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 8}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 8, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect3.top + 1, 90, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 24},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 9})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect3.top + 1, 90, rect2.top - 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 24},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 9})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect3.top + 1, 90, rect1.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 9, 32},
        ElementSelectionState{ElementId{0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0, 2, 0}, 0, 9})) << document.GetEditorState().ToString();
}

//Selection between paragraphs
TEST_F(DocumentTest, caret31)
{
    Start(565);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3, rect4, rect5;
    document.GetElementRect(ElementId{0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 2}, rect3);
    document.GetElementRect(ElementId{0, 1, 0}, rect4);
    document.GetElementRect(ElementId{0, 1, 1}, rect5);

    document.WaitTask(document.MoveCaret(100, rect3.top + 1));
    document.WaitTask(document.Select(100, rect3.top + 1, 110, rect4.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 35},
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 11})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaret(100, rect2.top + 1));
    document.WaitTask(document.Select(100, rect2.top + 1, 110, rect4.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 41},
        ElementSelectionState{ElementId{0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 11})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect2.top + 1, 110, rect5.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 41},
        ElementSelectionState{ElementId{0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 9})) << document.GetEditorState().ToString();
}

//Selection between paragraphs upward
TEST_F(DocumentTest, caret32)
{
    Start(565);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3, rect4, rect5;
    document.GetElementRect(ElementId{0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 0, 1}, rect2);
    document.GetElementRect(ElementId{0, 0, 2}, rect3);
    document.GetElementRect(ElementId{0, 1, 0}, rect4);
    document.GetElementRect(ElementId{0, 1, 1}, rect5);

    document.WaitTask(document.MoveCaret(100, rect5.top + 1));
    document.WaitTask(document.Select(100, rect5.top + 1, 110, rect4.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 11, 48},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect5.top + 1, 110, rect3.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 7}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 7, 34},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect5.top + 1, 110, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 9, 39},
        ElementSelectionState{ElementId{0, 0}, 2, 1},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 7})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect5.top + 1, 110, rect1.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 8}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 8, 44},
        ElementSelectionState{ElementId{0, 0}, 1, 2},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 7})) << document.GetEditorState().ToString();
}

//Selection between paragraphs
TEST_F(DocumentTest, caret33)
{
    Start(565);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("Tradicionalmente, el medio de un documento era el papel y la información", true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3, rect4;
    document.GetElementRect(ElementId{0, 0, 2}, rect1);
    document.GetElementRect(ElementId{0, 1, 0}, rect2);
    document.GetElementRect(ElementId{0, 1, 1}, rect3);
    document.GetElementRect(ElementId{0, 2, 0}, rect4);

    document.WaitTask(document.MoveCaret(100, rect1.top + 1));
    document.WaitTask(document.Select(100, rect1.top + 1, 110, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 35},
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 11})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 110, rect3.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 35},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 9})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 110, rect4.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 9}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 35},
        ElementSelectionState{ElementId{0}, 1, 1},
        ElementSelectionState{ElementId{0, 2, 0, 0}, 0, 9})) << document.GetEditorState().ToString();
}

//Selection a row with a code block upward
TEST_F(DocumentTest, caret34)
{
    Start(540);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true));
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    std::this_thread::sleep_for(100ms);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 2}, rect1);
    document.GetElementRect(ElementId{0, 1, 0}, rect2);
    document.GetElementRect(ElementId{0, 1, 1}, rect3);

    document.WaitTask(document.Select(390, rect3.top + 1, 380, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 46}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 46, 13},
        ElementSelectionState{ElementId{0, 1, 1}, 0, 2},
        ElementSelectionState{ElementId{0, 1, 1, 2}, 0, 8})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(390, rect3.top + 1, 380, rect1.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 36}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 36, 5},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1}, 0, 2},
        ElementSelectionState{ElementId{0, 1, 1, 2}, 0, 8})) << document.GetEditorState().ToString();
}

//Selection a row with a code block downward
TEST_F(DocumentTest, caret35)
{
    Start(540);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true));
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    std::this_thread::sleep_for(100ms);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.InsertDivision(true));
    std::this_thread::sleep_for(100ms);

    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 2}, rect1);
    document.GetElementRect(ElementId{0, 1, 0}, rect2);
    document.GetElementRect(ElementId{0, 1, 1}, rect3);

    document.WaitTask(document.Select(100, rect1.top + 1, 180, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 20}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 26},
        ElementSelectionState{ElementId{0, 0, 2}, 1, 2},
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 20})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, 180, rect3.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 1, 0, 18}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 26},
        ElementSelectionState{ElementId{0, 0, 2}, 1, 2},
        ElementSelectionState{ElementId{0, 1}, 0, 1},
        ElementSelectionState{ElementId{0, 1, 1, 0}, 0, 18})) << document.GetEditorState().ToString();
}

//Selection a row with a division
TEST_F(DocumentTest, caret36)
{
    Start(540);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true));
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.InsertDivision(true));

    Rect rect1, rect2, rect3;
    document.GetElementRect(ElementId{0, 0, 1}, rect1);
    document.GetElementRect(ElementId{0, 0, 2, 1, 0, 0, 0, 0, 0}, rect2);

    document.WaitTask(document.Select(100, rect1.top + 1, rect2.left + 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 41},
        ElementSelectionState{ElementId{0, 0, 2}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.Select(100, rect1.top + 1, rect2.GetRight() - 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 41},
        ElementSelectionState{ElementId{0, 0, 2}, 0, 2})) << document.GetEditorState().ToString();

    document.GetElementRect(ElementId{0, 0, 2, 1, 0, 0, 0, 0}, rect2);
    document.WaitTask(document.Select(100, rect1.top + 1, rect2.GetRight() - 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 41},
        ElementSelectionState{ElementId{0, 0, 2}, 0, 2})) << document.GetEditorState().ToString();

    document.GetElementRect(ElementId{0, 0, 2, 0}, rect3);
    document.GetElementRect(ElementId{0, 0, 2, 1}, rect2);
    document.WaitTask(document.Select(100, rect1.top + 1, rect2.left - 2, rect3.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 1}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 7, 41},
        ElementSelectionState{ElementId{0, 0, 2}, 0, 1})) << document.GetEditorState().ToString();
}

//Selection a row with a division
TEST_F(DocumentTest, caret37)
{
    Start(540);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true));
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.InsertDivision(true));

    Rect rect1, rect2;
    document.GetElementRect(ElementId{0, 0, 2, 2}, rect1);
    document.GetElementRect(ElementId{0, 0, 2, 1, 0, 0, 0, 1}, rect2);
    document.WaitTask(document.Select(100, rect1.top + 1, rect2.left + 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 26},
        ElementSelectionState{ElementId{0, 0, 2}, 1, 1})) << document.GetEditorState().ToString();

    document.GetElementRect(ElementId{0, 0, 2, 1, 0, 0, 0}, rect2);
    document.WaitTask(document.Select(100, rect1.top + 1, rect2.left + 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 6, 26},
        ElementSelectionState{ElementId{0, 0, 2}, 1, 1})) << document.GetEditorState().ToString();
}

//Click below a row
TEST_F(DocumentTest, caret38)
{
    Start(540);

    document.InsertString("Text", true);
    document.WaitTask(document.InsertDivision(true));

    Rect rect1;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect1);
    document.WaitTask(document.MoveCaret(rect1.left + 15, rect1.GetBottom() + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Click on a formula element
TEST_F(DocumentTest, caret39)
{
    Start(540);

    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("345", true));

    Rect rect1;
    auto el = document.FindByType(ElementId{0, 0, 0}, ElementType::PLUS);
    document.GetElementRect(el->id, rect1);
    document.WaitTask(document.MoveCaret(rect1.left + 1, rect1.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(el->id)) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Click on a division
TEST_F(DocumentTest, caret40)
{
    Start(540);

    document.InsertDivision(true);
    document.InsertString("1", true);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.InsertString("345678", true));

    Rect rect1;
    auto el = document.FindByType(ElementId{0, 0, 0}, ElementType::DIVISION);
    document.GetElementRect(el->id, rect1);
    document.WaitTask(document.MoveCaret(rect1.left + 1, rect1.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(el->id)) << document.GetEditorState().ToString();
}

//Selection a row inside a code block
TEST_F(DocumentTest, caret41)
{
    Start(540);

    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("345", true));

    auto el = document.FindByString(ElementId{0, 0, 0}, U"12");
    Rect rect1, rect2;
    document.GetElementRect(el->id, rect1);
    el = document.FindByType(ElementId{0, 0, 0}, ElementType::PLUS);
    document.GetElementRect(el->id, rect2);
    document.WaitTask(document.MoveCaret(rect1.left + 1, rect1.top + 1));
    document.WaitTask(document.Select(rect1.left + 1, rect1.top + 1, rect2.left + 1, rect2.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Click on a row
TEST_F(DocumentTest, caret42)
{
    Start(540);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    std::this_thread::sleep_for(100ms);

    Rect rect;
    document.GetElementRect(ElementId{0, 2, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 100, rect.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 0, 14})) << document.GetEditorState().ToString();
}

//Click before an image, on an image and after it
TEST_F(DocumentTest, caret43)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertString("Text", true);
    document.WaitTask(document.InsertImage(data, true, true));

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 1, rect.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.GetElementRect(ElementId{0, 0, 0, 1}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 1, rect.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaret(rect.GetRight() + 1, rect.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("String", true));

    document.GetElementRect(ElementId{0, 0, 0, 2}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 1, rect.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0})) << document.GetEditorState().ToString();
}

//Click after a code block
TEST_F(DocumentTest, caret44)
{
    Start(600);

    document.WaitTask(document.InsertDivision(true));

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.GetRight() + 5, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Select a string and a code block on another string
TEST_F(DocumentTest, caret45)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertDivision(true));

    Rect rect1, rect2;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect1);
    document.GetElementRect(ElementId{0, 1, 0, 0}, rect2);
    document.WaitTask(document.Select(rect1.left + 1, rect1.top + 1, rect2.GetRight() + 10, rect2.top + 10));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Move caret inside a column
TEST_F(DocumentTest, caret46)
{
    Start(600);
    
    document.InsertCode(false, true);
    document.InsertSquareRoot(true);
    document.InsertString("i", true);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertEquation(ResultType::COMPLEX, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    ASSERT_TRUE(document.ToText() == 
        U"sqrt(i)=0.707+0.707i,-0.707-0.707i"
        ) << ToBasicString(document.ToText());
    
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretHome(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    
    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(false);
    document.MoveCaretDown(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretHome(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    for (int i = 0; i < 7; ++i)
        document.MoveCaretRight(false);
    document.MoveCaretEnd(false);
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Select a word
TEST_F(DocumentTest, caret47)
{
    Start(600);
    
    document.InsertString("Text", true);
    document.WaitTask(document.SelectOut());
    ASSERT_TRUE(document.ToText() == U"Text") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.InsertString(" String", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.SelectOut());
    ASSERT_TRUE(document.ToText() == U"Text String") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 11}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 5, 6})) << document.GetEditorState().ToString();
}

//Select a word
TEST_F(DocumentTest, caret48)
{
    Start(610);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true));
    
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretWordRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.SelectOut());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 26}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 20, 6})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.SelectOut());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 54}, 
        ElementSelectionState{ElementId{0, 0, 1, 0}, 49, 5})) << document.GetEditorState().ToString();

    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.SelectOut());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 66}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 59, 7})) << document.GetEditorState().ToString();
}

//Select a code block
TEST_F(DocumentTest, caret49)
{
    Start(600);

    document.InsertDivision(true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.SelectOut());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Click after image, which is a single element on the row
TEST_F(DocumentTest, caret50)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, true, true));

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.GetRight() + 1, rect.top + 1));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaret(rect.GetRight() + 10, rect.top + 10));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Move to document begin/end with selection
TEST_F(DocumentTest, caret51)
{
    Start(680);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 5, 1, 0, 42}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 13, 57},
        ElementSelectionState{ElementId{0, 0}, 1, 13},
        ElementSelectionState{ElementId{0}, 1, 5})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretToDocumentBegin(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 5},
        ElementSelectionState{ElementId{0, 5}, 0, 1},
        ElementSelectionState{ElementId{0, 5, 1, 0}, 0, 30})) << document.GetEditorState().ToString();
}

//Move to document begin/end with selection
TEST_F(DocumentTest, caret52)
{
    Start(680);

    EXPECT_CALL(window_mock, OnLoadResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.Load("../../test/tests/file1.txt");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    document.InsertCode(false, true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertCode(false, true);

    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 5, 1, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1},
        ElementSelectionState{ElementId{0, 0}, 1, 13},
        ElementSelectionState{ElementId{0}, 1, 5})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretToDocumentBegin(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 5},
        ElementSelectionState{ElementId{0, 5}, 0, 1},
        ElementSelectionState{ElementId{0, 5, 1, 0}, 0, 30})) << document.GetEditorState().ToString();
}

//Move caret to word left and to word right
TEST_F(DocumentTest, caret53)
{
    Start(600);

    document.WaitTask(document.InsertString("Text right", true));
    document.MoveCaretToDocumentBegin(false);
    document.InsertCode(false, true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.InsertCode(false, true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Move caret to word left and to word right with selection
TEST_F(DocumentTest, caret54)
{
    Start(600);

    document.WaitTask(document.InsertString("Text right", true));
    document.MoveCaretToDocumentBegin(false);
    document.InsertCode(false, true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 5}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 5})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.InsertCode(false, true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 5}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 5, 5},
        ElementSelectionState{ElementId{0, 0, 0}, 2, 1})) << document.GetEditorState().ToString();
}

//Move caret to word left and to word right with code blocks
TEST_F(DocumentTest, caret55)
{
    Start(600);

    document.InsertCode(false, true);
    document.MoveCaretRight(false);
    document.InsertCode(false, true);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0},
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1},
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2},
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Move caret to word left and to word right beetween rows
TEST_F(DocumentTest, caret56)
{
    Start(390);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretEnd(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 41})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 34})) << document.GetEditorState().ToString();
}

//Move caret to word left and to word right beetween rows
TEST_F(DocumentTest, caret57)
{
    Start(310);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    for (int i = 0; i < 8; ++i)
        document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretWordRight(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 29})) << document.GetEditorState().ToString();
}

//Move caret out of a code block
TEST_F(DocumentTest, caret58)
{
    Start(300);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.InsertDivision(true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Move caret to word left and to word right beetween paragraphs
TEST_F(DocumentTest, caret59)
{
    Start(410);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertParagraph(true);
    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.MoveCaretUp(false);
    document.MoveCaretUp(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 18})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Edit a code block at the right corner
TEST_F(DocumentTest, caret60)
{
    Start(472);

    document.InsertString("In literary theory, a text is any object that can be read, whether this object is a work of literature", true);
    document.InsertCode(false, true);
    document.InsertString("3", true);
    document.WaitTask(document.InsertMultiply(true));
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Move a code block on the next row
TEST_F(DocumentTest, caret61)
{
    Start(480);

    int width = 470;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertString("In literary theory, a text is any object that can be read, which is a work of literature", true);
    document.InsertCode(false, true);
    document.InsertString("7", true);
    document.InsertDivision(true);
    document.InsertString("6", true);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);
    document.WaitTask(document.MoveCaretLeft(false));

    width = 410;
    document.WaitTask(document.Resize(width, 400));

    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Move to a row element in the middle of a code block
TEST_F(DocumentTest, caret62)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertParagraph(true);
    document.InsertParagraph(true);
    document.WaitTask(document.MoveCaretUp(false));
    document.InsertString("123", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    Rect rect;
    document.GetElementRect(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + 1, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Set caret into a solving result when it is about to be recalculated
TEST_F(DocumentTest, caret63)
{
    Start(600);

    document.WaitTask(document.SetLocale(yutovo_calculator::Language::Russian, true));
    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertString("2", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString(" см", true));
    document.InsertParagraph(true);
    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"d=2см\n"
        U"d=2.см"
        ) << ToBasicString(document.ToText());

    document.config.solve_delay = 500;
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("1", true));
    document.WaitTask(document.MoveCaretDown(false));
    for (int i = 0; i < 8; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.ToText() == 
        U"d=12см\n"
        U"d=2.см"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"d=12см\n"
        U"d=1.2дм"
        ) << ToBasicString(document.ToText());
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Move a code block on the previous row
TEST_F(DocumentTest, caret64)
{
    Start(600);

    int width = 600;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    document.InsertCode(false, true);
    document.InsertString("234234345435345345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertCode(false, true);
    document.InsertString("234234345435345345", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(600ms);

    width = 1040;
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.Resize(width, 400));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete a code block and check caret
TEST_F(DocumentTest, caret65)
{
    Start(700);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234567891", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234567892", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234567893", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    ASSERT_TRUE(document.ToText() == 
        U"1234567891=1234567891."\
        U"1234567892=1234567892."\
        U"1234567893=1234567893."
        ) << ToBasicString(document.ToText());

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"1234567891=1234567891."\
        U"1234567893=1234567893."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Select a code block with word right, word left
TEST_F(DocumentTest, caret66)
{
    Start(600);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretWordLeft(true);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select a code block with word right, word left
TEST_F(DocumentTest, caret67)
{
    Start(600);

    document.InsertString("Арифме́тика ", true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretRight(false);
    document.MoveCaretWordLeft(true);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a code block with word right, word left
TEST_F(DocumentTest, caret68)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("23", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, изучающий числа, их отношения и свойства.", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordRight(false);
    document.MoveCaretWordLeft(true);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 12})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();
    
    document.MoveCaretWordRight(true);
    document.WaitTask(document.MoveCaretWordRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 12}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 1}, 0, 12})) << document.GetEditorState().ToString();
}

//Check non-editable result
TEST_F(DocumentTest, caret69)
{
    Start(700);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();

    document.MoveCaretRight(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.ToText() == 
        U"123=123."
        ) << ToBasicString(document.ToText());

    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"123=123."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select equation with Home
TEST_F(DocumentTest, caret70)
{
    Start(700);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    document.MoveCaretEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == 
        U"123=123."
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretHome(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select a single element inside a code block
TEST_F(DocumentTest, caret71)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.InsertPower(true);
    document.InsertString("45", true);
    document.MoveCaretHome(true);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.ToText() == 
        U"pow(234,45)"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretEnd(true);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Word right, word left through out rows with code blocks
TEST_F(DocumentTest, caret72)
{
    Start(635);

    for (int i = 0; i < 7; ++i)
    {
        document.InsertCode(false, true);
        document.InsertString("1234567", true);
        document.MoveCaretToDocumentEnd(false);
    }
    document.MoveCaretToDocumentBegin(false);
    for (int i = 0; i < 4; ++i)
        document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 5})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 5})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 4})) << document.GetEditorState().ToString();
}

//Word right, word left through out paragraphs with code blocks
TEST_F(DocumentTest, caret73)
{
    Start(600);

    for (int i = 0; i < 4; ++i)
    {
        document.InsertCode(false, true);
        document.InsertString("1234567", true);
        document.MoveCaretToDocumentEnd(false);
    }
    document.InsertParagraph(false);
    for (int i = 0; i < 2; ++i)
    {
        document.InsertCode(false, true);
        document.InsertString("1234567", true);
        document.MoveCaretToDocumentEnd(false);
    }
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 4})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 4})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2})) << document.GetEditorState().ToString();
}

//Click above an image
TEST_F(DocumentTest, caret74)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertString("Text", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertImage(data, true, true));

    Rect rect;
    document.GetElementRect(ElementId{0, 1, 0, 0}, rect);
    document.WaitTask(document.MoveCaret(rect.left + rect.width / 2, rect.top - 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0})) << document.GetEditorState().ToString();
}

//PgUp after selection of a code block
TEST_F(DocumentTest, caret75)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
    {
        return Rect{0, 0, 630, 255};
    });

    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.MoveCaretPageUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//PgDown after selection of a code block
TEST_F(DocumentTest, caret76)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(false);
    document.InsertString("String", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 6})) << document.GetEditorState().ToString();
}

//Move word right in a string with commas, quotes, braces, dotas
TEST_F(DocumentTest, caret77)
{
    Start(585);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, 123, изучающий числа, их отношения и свойства. String.", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 13})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 15})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 17})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 21})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 23})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 33})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 35})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 47})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 49})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 52})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 9})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 18})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 19})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 24})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 25})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 27})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 29})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 36})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 46})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 48})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 51})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 53})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 10})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 15})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 17})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 30})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 32})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 40})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 42})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 48})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 49})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 49})) << document.GetEditorState().ToString();
}

//Move word left in a string with commas, quotes, braces, dotas
TEST_F(DocumentTest, caret78)
{
    Start(585);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, 123, изучающий числа, их отношения и свойства. String.", true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 48})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 42})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 40})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 32})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 30})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 20})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 17})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 15})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 10})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 53})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 51})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 48})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 46})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 36})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 29})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 27})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 26})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 25})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 24})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 19})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 18})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 9})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 1, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 52})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 49})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 47})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 35})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 33})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 23})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 21})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 17})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 15})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 13})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Select word left in a string with commas, quotes, braces, dotas
TEST_F(DocumentTest, caret79)
{
    Start(585);

    document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — "\
        "раздел математики, 123, изучающий числа, их отношения и свойства. String.", true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 48}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 48, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 42}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 42, 7})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 40}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 40, 9})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 32}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 32, 17})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 0, 30}, 
        ElementSelectionState{ElementId{0, 0, 2, 0}, 30, 19})) << document.GetEditorState().ToString();
}

//Move word right, word left in a string with spaces
TEST_F(DocumentTest, caret80)
{
    Start(590);

    document.InsertString("Арифме́тика  (др.-греч. ἀριθμητική,    arithmētikḗ — от ἀριθμός, arithmós «число»)  — "\
        "раздел математики, 123, изучающий числа, их отношения и свойства. String.", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 13})) << document.GetEditorState().ToString();
    for (int i = 0; i < 7; ++i)
        document.WaitTask(document.MoveCaretWordRight(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 39})) << document.GetEditorState().ToString();

    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 39})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 34})) << document.GetEditorState().ToString();
    for (int i = 0; i < 6; ++i)
        document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 13})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Select with PgDown a code block
TEST_F(DocumentTest, caret81)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretPageDown(true);
    document.MoveCaretPageDown(true);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select with PgUp a code block
TEST_F(DocumentTest, caret82)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretPageUp(true);
    document.MoveCaretPageUp(true);
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select with PgDown code blocks
TEST_F(DocumentTest, caret83)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertCode(false, true);
    document.InsertString("456", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select with PgUp code blocks
TEST_F(DocumentTest, caret84)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertCode(false, true);
    document.InsertString("456", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select to the document beginning code blocks
TEST_F(DocumentTest, caret85)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertCode(false, true);
    document.InsertString("456", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretToDocumentBegin(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretToDocumentBegin(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretToDocumentBegin(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select to the document end code blocks
TEST_F(DocumentTest, caret86)
{
    Start(600);

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
        });

    document.InsertCode(false, true);
    document.InsertString("456", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("12", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Click inside a code block
TEST_F(DocumentTest, caret87)
{
    Start(600);

    document.Load("../../test/tests/caret_move_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(2000ms);

    Rect rect;
    document.GetElementRect(ElementId{0, 1, 0, 0, 1, 0, 0, 2, 1}, rect);
    document.WaitTask(document.MoveCaret(rect.left + rect.width / 2, rect.top + 5));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1, 0, 0, 2, 1, 0})) << document.GetEditorState().ToString();
}

//Selection outside of a code block
TEST_F(DocumentTest, caret88)
{
    Start(600);

    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3", true));

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 5}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 5, 1},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Selection outside of a code block
TEST_F(DocumentTest, caret89)
{
    Start(600);

    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("2", true);
    document.InsertPlus(true);
    document.WaitTask(document.InsertString("3", true));
    document.WaitTask(document.MoveCaretLeft(false));

    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0, 2}, 
        ElementSelectionState{ElementId{0, 1, 0, 0, 0, 0, 0}, 2, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0, 1, 0}, 0, 2})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 4, 2}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Check caret moving with a document with an include file
TEST_F(DocumentTest, caret90)
{
    Start(600);

    EXPECT_CALL(window_mock, OnSaveResult).WillOnce([&](const uint task_id, IOResult result, const int document_id)
        {
            ASSERT_TRUE(result == IOResult::Success);
        });

    EXPECT_CALL(window_mock, OnLoadInclude).WillOnce([&](const std::string& file_name, const int document_id)
        {
            document.LoadInclude(file_name);
            std::this_thread::sleep_for(400ms);
        });

    EXPECT_CALL(window_mock, GetViewPort).WillRepeatedly([&](const int)
        {
            return Rect{0, 0, 630, 255};
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
    document.InsertString("String", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("var", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);
    ASSERT_TRUE(document.ToText() == 
        U"String\n"\
        U"var=5."\
        ) << ToBasicString(document.ToText());
    
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 1}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretToDocumentBegin(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretPageUp(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretPageDown(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretPageUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();

    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretPageDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 2, 0, 1}, 
        ElementSelectionState{ElementId{0}, 1, 2})) << document.GetEditorState().ToString();
}

//Check of selection in a number of paragraphs
TEST_F(DocumentTest, caret91)
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
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 30, 1, 0, 67}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 65, 76}, 
        ElementSelectionState{ElementId{0, 1}, 1, 1}, 
        ElementSelectionState{ElementId{0}, 2, 29})) << document.GetEditorState().ToString();
}

//Move caret on the "~" symbol while solving
TEST_F(DocumentTest, caret92)
{
    Start(600);

    document.Load("../../test/tests/caret92.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretHome(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.WaitTask(document.MoveCaretEnd(false));
    document.WaitTask(document.InsertParagraph(true));
    document.WaitTask(document.Paste(clipboard_json));

    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    document.WaitTask(document.MoveCaretHome(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 2, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"f(yy)=pow(yy,2)\n"\
        U"definite_integral(0,4,f(y),y)=21.333\n"
        ) << ToBasicString(document.ToText());
}

}
