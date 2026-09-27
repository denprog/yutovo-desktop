/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

struct CodeTest : DocumentTest
{
    struct NumberGapData
    {
        std::vector<int> xs_no_gap;
        std::vector<int> xs_gap;
        int width_no_gap = 0;
        int width_gap = 0;
    };

    CodeTest::NumberGapData CollectNumberGapData(const std::string& digits, int gap)
    {
        NumberGapData data;
        int len = (int)digits.size();

        auto move_to_number = 
            [&](const ElementId& number_id) -> bool
            {
                document.WaitTask(document.MoveCaretToDocumentBegin(false));
                for (int i = 0; i < 10; ++i)
                {
                    if (document.GetEditorState().caret_state.id == number_id)
                        return true;
                    if (!document.WaitTask(document.MoveCaretRight(false), 1000))
                    {
                        ADD_FAILURE() << "MoveCaretRight timed out";
                        return false;
                    }
                }
                ADD_FAILURE() << "Could not move caret to number element";
                return false;
            };

        document.WaitTask(document.MoveCaretToDocumentBegin(false));

        ElementId number_id;
        for (int i = 0; i < 10; ++i)
        {
            auto el = document.GetElement(document.GetEditorState().caret_state.id);
            if (el && el->type == ElementType::CODE_STRING)
            {
                number_id = document.GetEditorState().caret_state.id;
                break;
            }
            if (!document.WaitTask(document.MoveCaretRight(false), 1000))
            {
                ADD_FAILURE() << "MoveCaretRight timed out while locating number";
                return data;
            }
        }
        if (number_id.empty())
        {
            ADD_FAILURE() << "Could not locate number CodeString";
            return data;
        }

        //without gaps
        {
            Config cfg = document.config;
            cfg.use_numbers_gaps = false;
            document.WaitTask(document.SetConfig(cfg, false));
        }

        if (!move_to_number(number_id))
            return data;

        Rect el;
        EXPECT_TRUE(document.GetElementRect(number_id, el)) << "element rect without gaps";
        data.width_no_gap = el.width;

        for (int i = 0; i <= len; ++i)
        {
            Rect r;
            EXPECT_TRUE(document.GetCaretRect(r)) << "caret rect without gaps at " << i;
            data.xs_no_gap.push_back(r.left);
            if (i < len)
            {
                if (!document.WaitTask(document.MoveCaretRight(false), 1000))
                {
                    ADD_FAILURE() << "MoveCaretRight timed out at " << i;
                    return data;
                }
            }
        }

        //with gaps
        {
            Config cfg = document.config;
            cfg.use_numbers_gaps = true;
            document.WaitTask(document.SetConfig(cfg, false), 5000);
        }

        if (!move_to_number(number_id))
            return data;

        EXPECT_TRUE(document.GetElementRect(number_id, el)) << "element rect with gaps";
        data.width_gap = el.width;

        for (int i = 0; i <= len; ++i)
        {
            Rect r;
            EXPECT_TRUE(document.GetCaretRect(r)) << "caret rect with gaps at " << i;
            data.xs_gap.push_back(r.left);
            if (i < len)
                document.WaitTask(document.MoveCaretRight(false), 1000);
        }

        return data;
    }

    void CheckNumberGaps(const NumberGapData& data, int len, int gap)
    {
        ASSERT_EQ(data.xs_no_gap.size(), len + 1);
        ASSERT_EQ(data.xs_gap.size(), len + 1);
        ASSERT_GT(data.width_gap, data.width_no_gap);

        int gap_width = (data.xs_gap[gap] - data.xs_no_gap[gap]) - (data.xs_gap[gap - 1] - data.xs_no_gap[gap - 1]);
        ASSERT_GT(gap_width, 0);

        int num_gaps = (len - 1) / gap;
        ASSERT_EQ(data.width_gap, data.width_no_gap + num_gaps * gap_width);

        for (int i = 0; i <= len; ++i)
        {
            int expected_extra = (std::min(i, len - 1) / gap) * gap_width;
            ASSERT_EQ(data.xs_gap[i], data.xs_no_gap[i] + expected_extra) << "caret position " << i;
        }
    }
};

TEST_F(CodeTest, code1)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
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

    document.WaitTask(document.InsertString("m", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    std::this_thread::sleep_for(100ms);
    document.WaitTask(document.InsertString("Text", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertString("Normal", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>m</mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Normal</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2, 6})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code2)
{
    Start(600);

    document.InsertString("Text", true);
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertParagraph(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code3)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
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

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code4)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
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

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
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

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code5)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code6)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertCode(false, true));
    document.MoveCaretLeft(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, ElementSelectionState{{0, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, ElementSelectionState{{0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code7)
{
    Start(327);

    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretRight(false));
    document.SetFontSize(22);
    document.WaitTask(document.InsertString("The source of the text itself is a little mysterious.", true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The source of the </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">text itself is a little </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.MoveCaretUp(false);
    document.WaitTask(document.MoveCaretUp(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(400ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sour</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">ce of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.WaitTask(document.MoveCaretToDocumentEnd(false));
    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sour</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">ce of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.WaitTask(document.MoveCaretRight(false));
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
                "<span style=\"font-family:'Arial';font-size:22px;\">The sour</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">ce of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little mysterious.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">The sour</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">ce of </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">the text itself is a </span>"\
                "<span style=\"font-family:'Arial';font-size:22px;\">little mysterious.</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 2, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code8)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    std::this_thread::sleep_for(100ms);
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

    document.WaitTask(document.MoveCaretRight(false));
    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.MoveCaretRight(false));
    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 4})) << document.GetEditorState().ToString();
}

//Multiline code
TEST_F(CodeTest, code9)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertParagraph(true));
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
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
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

    document.Redo();
    document.WaitRedo();
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
                            "<mi></mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Multiline code
TEST_F(CodeTest, code10)
{
    Start(600);

    document.InsertString("Text", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("1234", true));
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertParagraph(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>1234</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertDivision(true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mfrac>"\
                                "<mrow>"\
                                    "<mi></mi>"\
                                "</mrow>"\
                                "<mrow>"\
                                    "<mi>4</mi>"\
                                "</mrow>"\
                            "</mfrac>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Text</span>"\
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>4</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Deletion of code block
TEST_F(CodeTest, code11)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretLeft(false);
    document.MoveCaretRight(true);
    document.WaitTask(document.MoveCaretRight(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\"></span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Split a code string with a space
TEST_F(CodeTest, code12)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.WaitTask(document.InsertString(" ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi></mi>"\
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
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();
}

//Split a code string with a space
TEST_F(CodeTest, code13)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretHome(false);
    document.WaitTask(document.InsertString(" ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();

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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Split a code string with a space
TEST_F(CodeTest, code14)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.InsertString("   ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>12</mi>"\
                        "<mi>3</mi>"\
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
                        "<mi>123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Split a code string with a space
TEST_F(CodeTest, code15)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString(" ", true);
    document.InsertString("123", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.InsertString("  ", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.InsertString("  ", true);
    document.InsertString("45", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.InsertString(" ", true);
    document.MoveCaretRight(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.InsertString(" ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi></mi>"\
                        "<mi>45</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 4, 0})) << document.GetEditorState().ToString();

    for (int i = 0; i < 4; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi>45</mi>"\
                        "<mi></mi>"\
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
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi></mi>"\
                        "<mi>45</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi>45</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 1, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                        "<mi>123</mi>"\
                        "<mi></mi>"\
                        "<mi>45</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0, 0, 2, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, code16)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
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

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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

//Check string format in front of a code block and behind it
TEST_F(CodeTest, code17)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.WaitTask(document.MoveCaretLeft(false));
    StringFormat f;
    ASSERT_TRUE(document.GetStringFormat({0, 0, 0}, f));
    ASSERT_TRUE(f.size == 14);
    ASSERT_TRUE(document.GetStringFormat({0, 0, 0, 1}, f));
    ASSERT_TRUE(f.size == 14);
}

//Copy-paste
TEST_F(CodeTest, code18)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretRight(true));
    document.WaitTask(document.Copy(clipboard_json, clipboard_text));
    document.MoveCaretEnd(false);
    document.WaitTask(document.Paste(clipboard_json));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2})) << document.GetEditorState().ToString();
}

//Selection with Shift-End
TEST_F(CodeTest, code19)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("55", true);
    document.MoveCaretEnd(false);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("6", true);
    for (int i = 0; i < 6; ++i)
        document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 3}, 
        ElementSelectionState{ElementId{0, 0, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Selection with Shift-Home
TEST_F(CodeTest, code20)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentEnd(false);
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("55", true);
    document.MoveCaretEnd(false);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("6", true);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretHome(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Check caret position after delete
TEST_F(CodeTest, code21)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.InsertDivision(true);
    document.InsertString("3", true);
    document.MoveCaretRight(false);
    document.InsertMultiply(true);
    document.InsertString("5", true);
    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(true, true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Remove a row and undo
TEST_F(CodeTest, code22)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.InsertString("34", true);
    document.InsertParagraph(true);
    document.InsertString("567", true);
    document.MoveCaretUp(false);
    document.MoveCaretLeft(true);
    document.WaitTask(document.MoveCaretLeft(true));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"123\n" \
        U"567"
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToText() == 
        U"123\n" \
        U"34\n" \
        U"567"
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0, 0}, 
        ElementSelectionState{{0, 0, 0, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Insert a code block inside a string with selection
TEST_F(CodeTest, code23)
{
    Start(600);

    document.WaitTask(document.InsertString("Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, "\
        "изучающий числа, их отношения и свойства.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretWordRight(true));
    document.WaitTask(document.InsertCode(false, true));
    ASSERT_TRUE(document.ToText() == 
        U"А(др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("234", true));
    ASSERT_TRUE(document.ToText() == 
        U"А234(др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"А(др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."\
        ) << ToBasicString(document.ToText());

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToText() == 
        U"Арифме́тика (др.-греч. ἀριθμητική, arithmētikḗ — от ἀριθμός, arithmós «число») — раздел математики, изучающий числа, их отношения и свойства."\
        ) << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 12}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 11})) << document.GetEditorState().ToString();
}

//Paste a code block inside a code block with selection
TEST_F(CodeTest, code24)
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
    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("5555", true);
    document.InsertParagraph(true);
    document.InsertString("77", true);
    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.WaitTask(document.MoveCaretEnd(true));

    document.WaitTask(document.Copy(clipboard_json, clipboard_text));

    document.MoveCaretDown(false);
    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
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
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>77</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();

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
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>5555</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>77</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 0, 0, 0, 4}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 1})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
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
                "<span style=\"white-space:nowrap; display:inline-block;line-height:2;vertical-align:top;\">"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>123</mi>"\
                        "</mrow>"\
                    "</math>"\
                    "<br>"
                    "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                        "<mrow>"\
                            "<mi>77</mi>"\
                        "</mrow>"\
                    "</math>"\
                "</span>"
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 1, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Select outside a code block with an empty paragraph
TEST_F(CodeTest, code25)
{
    Start(600);

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123", true);
    document.WaitTask(document.InsertParagraph(true));
    document.MoveCaretEnd(false);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertString("String", true);
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretRight(false);
    document.MoveCaretDown(true);
    document.WaitTask(document.MoveCaretDown(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 2}, 
        ElementSelectionState{ElementId{0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretToDocumentBegin(false));
    document.MoveCaretRight(false);
    document.WaitTask(document.MoveCaretRight(false));
    document.WaitTask(document.MoveCaretDown(true));
    document.WaitTask(document.MoveCaretDown(true));
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 0, 2}, 
        ElementSelectionState{ElementId{0}, 0, 1}, 
        ElementSelectionState{ElementId{0, 1, 0, 0}, 0, 2})) << document.GetEditorState().ToString();
}

//Select outside a code block with an empty paragraph
TEST_F(CodeTest, code26)
{
    Start(600);

    document.InsertString("String", true);
    document.WaitTask(document.InsertParagraph(true));
    document.InsertCode(false, true);
    document.InsertParagraph(true);
    document.InsertString("123", true);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 5},
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.MoveCaretToDocumentEnd(false);
    document.MoveCaretLeft(false);
    document.MoveCaretLeft(false);
    document.MoveCaretUp(true);
    document.WaitTask(document.MoveCaretUp(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 5}, 
        ElementSelectionState{ElementId{0}, 1, 1})) << document.GetEditorState().ToString();
}

//Select outside a code block with an empty paragraph
TEST_F(CodeTest, code27)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("123", true);
    document.InsertString(" ", true);
    document.InsertString(" ", true);
    document.WaitTask(document.InsertString(" ", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();

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
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>123</mi>"\
                        "<mi></mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 1, 0})) << document.GetEditorState().ToString();
}

//Insert tab
TEST_F(CodeTest, code28)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("	", true);
    document.WaitTask(document.InsertString("123", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>	123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 4})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>	</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
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
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.InsertCode(false, true);
    document.InsertString("	", true);
    document.InsertString("	", true);
    document.WaitTask(document.InsertString("123", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>		123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 5})) << document.GetEditorState().ToString();

    document.WaitTask(document.InsertString("	", true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>		123	</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 6})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<math xmlns='http://www.w3.org/1998/Math/MathML'>"\
                    "<mrow>"\
                        "<mi>		123</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 0, 0, 0, 5})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(CodeTest, code29)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.SwitchInsertMode();
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.InsertString("S", true));
    ASSERT_TRUE(document.ToText() == U"S") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Replace mode
TEST_F(CodeTest, code30)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.SwitchInsertMode();
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretToDocumentEnd(true);
    document.WaitTask(document.InsertString("S", true));
    ASSERT_TRUE(document.ToText() == U"S") << ToBasicString(document.ToText());
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1})) << document.GetEditorState().ToString();

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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Create code blocks in replace mode
TEST_F(CodeTest, code31)
{
    Start(600);

    document.SwitchInsertMode();
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("123", true));
    document.MoveCaretToDocumentEnd(false);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("55", true));
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
                        "<mi>55</mi>"\
                    "</mrow>"\
                "</math>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1, 0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
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
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Selection a code block in a row
TEST_F(CodeTest, code32)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertString("str", true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("456456", true));
    document.MoveCaretHome(false);
    document.MoveCaretLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2}, 
        ElementSelectionState{ElementId{0, 1, 0}, 1, 1})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretWordLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2}, 
        ElementSelectionState{ElementId{0, 1, 0}, 1, 1})) << document.GetEditorState().ToString();
}

//Selection code blocks in a row
TEST_F(CodeTest, code33)
{
    Start(600);

    document.InsertCode(false, true);
    document.InsertString("234", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertString("str", true);
    document.InsertParagraph(true);
    document.InsertString("String", true);
    document.InsertCode(false, true);
    document.InsertString("456456", true);
    document.MoveCaretToDocumentEnd(false);
    document.InsertCode(false, true);
    document.InsertString("555", true);
    document.MoveCaretHome(false);
    document.MoveCaretWordLeft(false);
    document.MoveCaretWordLeft(false);
    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 3}, 
        ElementSelectionState{ElementId{0, 1, 0}, 1, 2})) << document.GetEditorState().ToString();
    
    document.WaitTask(document.MoveCaretLeft(false));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1})) << document.GetEditorState().ToString();
    document.WaitTask(document.MoveCaretEnd(true));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 3}, 
        ElementSelectionState{ElementId{0, 1, 0}, 1, 2})) << document.GetEditorState().ToString();
}

//Select a multi-paragraph code block to the left
TEST_F(CodeTest, code34)
{
    Start(600);

    document.InsertCode(false, true);
    document.WaitTask(document.InsertString("aaa", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("bbb", true));
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString("ccc", true));
    
    for (int i = 0; i < 12; ++i)
        document.WaitTask(document.MoveCaretLeft(true));
    
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();
}

//Select upward and downward in a code block
TEST_F(CodeTest, code35)
{
    Start(800, 550);

    document.Load("../../test/tests/code35.yut");
    document.WaitLoad();
    document.WaitSolver();
    std::this_thread::sleep_for(2s);

    for (int i = 0; i < 8; ++i)
    {
        document.WaitTask(document.MoveCaretUp(true));
        std::this_thread::sleep_for(100ms);
    }

    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 1, 0, 0}, 
        ElementSelectionState{ElementId{0, 0, 0, 0}, 1, 9})) << document.GetEditorState().ToString();

    for (int i = 0; i < 9; ++i)
    {
        document.WaitTask(document.MoveCaretDown(true));
        std::this_thread::sleep_for(100ms);
    }

    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 1})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretUp(true));
    std::this_thread::sleep_for(100ms);
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0})) << document.GetEditorState().ToString();
}

TEST_F(CodeTest, number_gaps_binary)
{
    Start(600);
    document.config.binary_gap = 4;
    document.config.use_numbers_gaps = false;

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("10101010", true);
    document.InsertSubscript(true);
    document.WaitTask(document.InsertString("bin", true));
    std::this_thread::sleep_for(100ms);

    auto data = CollectNumberGapData("10101010", 4);
    CheckNumberGaps(data, 8, 4);
}

TEST_F(CodeTest, number_gaps_octal)
{
    Start(600);
    document.config.octal_gap = 3;
    document.config.use_numbers_gaps = false;

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123456701", true);
    document.InsertSubscript(true);
    document.WaitTask(document.InsertString("oct", true));
    std::this_thread::sleep_for(100ms);

    auto data = CollectNumberGapData("123456701", 3);
    CheckNumberGaps(data, 9, 3);
}

TEST_F(CodeTest, number_gaps_decimal)
{
    Start(600);
    document.config.decimal_gap = 3;
    document.config.use_numbers_gaps = false;

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("123456789", true);
    document.InsertSubscript(true);
    document.WaitTask(document.InsertString("dec", true));
    std::this_thread::sleep_for(100ms);

    auto data = CollectNumberGapData("123456789", 3);
    CheckNumberGaps(data, 9, 3);
}

TEST_F(CodeTest, number_gaps_hexadecimal)
{
    Start(600);
    document.config.hexadecimal_gap = 4;
    document.config.use_numbers_gaps = false;

    document.WaitTask(document.InsertCode(false, true));
    document.InsertString("1a2b3c4d", true);
    document.InsertSubscript(true);
    document.WaitTask(document.InsertString("hex", true));
    std::this_thread::sleep_for(100ms);

    auto data = CollectNumberGapData("1a2b3c4d", 4);
    CheckNumberGaps(data, 8, 4);
}

}
