/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include <gtest/gtest.h>
#include "mock.h"
#include "style.h"
#include "str.h"

namespace yutovo_test
{

using namespace yutovo;
using namespace std::chrono_literals;

//Set font
TEST_F(ArrayTest, fonts1)
{
    Start(600);

    EXPECT_CALL(window_mock, Translate).WillRepeatedly([&](ElementId id, const std::u32string& str)
        {
            return str;
        });

    document.InsertCode(false, true);
    document.InsertString("d", true);
    document.InsertAssignment(true);
    document.InsertOpenSquareBracket(true);
    document.InsertString("123", true);
    document.InsertCloseSquareBracket(true);
    document.InsertParagraph(true);

    document.InsertString("d", true);
    document.WaitTask(document.InsertEquation(ResultType::AUTO, true));
    document.WaitSolver();
    std::this_thread::sleep_for(3s);
    ASSERT_TRUE(document.ToText() == 
        U"d=[123]\n"\
        U"d=123."
        ) << ToBasicString(document.ToText());

    document.MoveCaretHome(false);
    document.WaitTask(document.MoveCaretEnd(true));
    document.WaitTask(document.ChangeStringFormat("Noto Mono", 12, false, false, false, false, false, false, Color::Black(), Color::White(), true));
    std::this_thread::sleep_for(1s);
    ElementPtr el = document.FindByString(ElementId{0}, U"123.");
    ASSERT_TRUE(((String*)el.get())->format->family == "Noto Mono");
}

}
