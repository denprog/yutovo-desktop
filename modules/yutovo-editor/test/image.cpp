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

TEST_F(DocumentTest, images1)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

TEST_F(DocumentTest, images2)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();

    document.MoveCaretLeft(false);
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();
}

//Save/Load
TEST_F(DocumentTest, images3)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, true, false));
    document.Save("images3_1.yut");

    document.WaitTask(document.New());
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == "<body><p><span style=\"font-family:'Arial';font-size:14px;\"></span></p></body>") << document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 0, 0})) << document.GetEditorState().ToString();

    document.Load("images3_1.yut");
    document.WaitLoad();
    std::this_thread::sleep_for(200ms);
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Insert a large image
TEST_F(DocumentTest, images4)
{
    Start(200);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, true, false);
    document.InsertString("1", true);
    document.WaitTask(document.InsertString("2", true));

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

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 1, 0, 2})) << document.GetEditorState().ToString();
}

//Insert a large image and resize
TEST_F(DocumentTest, images5)
{
    Start(200);

    int width = 200;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, true, false);
    document.InsertString("1", true);
    document.WaitTask(document.InsertString("2", true));

    document.Undo();
    document.WaitUndo();

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">12</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 2})) << document.GetEditorState().ToString();
}

//Insert a large image and resize
TEST_F(DocumentTest, images6)
{
    Start(200);

    int width = 200;
    EXPECT_CALL(window_mock, GetRect).WillRepeatedly([&]()
        {
            return Rect{0, 0, width, 400};
        });

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_large.bmp");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertImage(data, true, false);
    document.InsertString("1", true);
    document.InsertString("2", true);
    document.WaitTask(document.DeleteElements(true, true));

    document.Undo();
    document.WaitUndo();

    width = 800;
    document.WaitTask(document.Resize(width, 400));

    document.Redo();
    document.WaitRedo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">1</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1, 1})) << document.GetEditorState().ToString();
}

//Move caret on the image on click on it
TEST_F(DocumentTest, images7)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertString("123", true);
    document.WaitTask(document.InsertImage(data, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 2})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaret(80, 40));
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState({0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Paste an image before an image
TEST_F(DocumentTest, images8)
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
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    
    ASSERT_TRUE(document.IsEditable(ElementId{0, 0, 0}));

    document.WaitTask(document.InsertImage(data, true, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
}

//Select text and image and change string format
TEST_F(DocumentTest, images9)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.InsertString("123", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertImage(data, true, false));

    document.WaitTask(document.SelectAll());
    document.WaitTask(document.ChangeStringFormat("Times New Roman", 22, false, false, false, false, false, false, Color::Black(), Color::White(), true));

    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Times New Roman';font-size:22px;\">123</span>"\
            "</p>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">123</span>"\
            "</p>"\
            "<p>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 1}, 
        ElementSelectionState{ElementId{0}, 0, 2})) << document.GetEditorState().ToString();
}

//Delete an image and Undo
TEST_F(DocumentTest, images10)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    document.WaitTask(document.InsertString(U"To plot a linear graph of a function, click the Graphs toolbar button, "\
        "then enter the axis endpoints, the function expression for the y-axis, and the x-axis parameter.", true));
    document.MoveCaretToDocumentBegin(false);
    document.WaitTask(document.MoveCaretWordRight(false));

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">plot a linear graph of a function, click the Graphs </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">toolbar button, then enter the axis endpoints, the function </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">expression for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 2, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To plot a linear graph of a function, click the Graphs toolbar </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">button, then enter the axis endpoints, the function expression </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 0, 3})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">plot a linear graph of a function, click the Graphs </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">toolbar button, then enter the axis endpoints, the function </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">expression for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 0, 0, 1})) << document.GetEditorState().ToString();
}

//Delete an image and Undo
TEST_F(DocumentTest, images11)
{
    Start(600);

    EXPECT_CALL(window_mock, GetImageSize).WillRepeatedly([&](const std::vector<unsigned char>& image)
        {
            return GetImageSizeMock(image);
        });

    document.InsertString(U"To plot a linear graph of a function, click the Graphs toolbar button, "\
        "then enter the axis endpoints, the function expression for the y-axis, and the x-axis parameter.", true);
    document.InsertParagraph(true);
    document.WaitTask(document.InsertString(U"To make a linear graph of a function, click the Graphs toolbar button, "\
        "then enter the axis endpoints, the function expression for the y-axis, and the x-axis parameter.", true));
    document.MoveCaretToDocumentBegin(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.MoveCaretDown(false);
    document.WaitTask(document.MoveCaretWordRight(false));

    QImage test_image("../../test/tests/Qt_small.png");
    std::vector<unsigned char> data;
    GetImageData(test_image, data);

    document.WaitTask(document.InsertImage(data, true, false));

    document.MoveCaretWordRight(false);
    document.WaitTask(document.MoveCaretWordRight(false));

    document.WaitTask(document.InsertImage(data, true, false));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To plot a linear graph of a function, click the Graphs toolbar </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">button, then enter the axis endpoints, the function expression </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">make a </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">linear graph of a function, click the </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Graphs toolbar button, then enter the axis endpoints, the </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">function expression for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 4, 0})) << document.GetEditorState().ToString();

    document.WaitTask(document.MoveCaretLeft(false));
    document.WaitTask(document.DeleteElements(false, true));
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To plot a linear graph of a function, click the Graphs toolbar </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">button, then enter the axis endpoints, the function expression </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">make a linear graph of a function, click the Graphs </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">toolbar button, then enter the axis endpoints, the function </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">expression for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 2, 7})) << document.GetEditorState().ToString();

    document.Undo();
    document.WaitUndo();
    ASSERT_TRUE(document.ToHtml() == 
        "<body>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To plot a linear graph of a function, click the Graphs toolbar </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">button, then enter the axis endpoints, the function expression </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
            "<p>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">To </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">make a </span>"\
                "<img src=\"data:image/png;base64," + Base64Encode(data) + "\">"\
                "<span style=\"font-family:'Arial';font-size:14px;\">linear graph of a function, click the </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">Graphs toolbar button, then enter the axis endpoints, the </span>"\
                "<span style=\"font-family:'Arial';font-size:14px;\">function expression for the y-axis, and the x-axis parameter.</span>"\
            "</p>"\
        "</body>") << 
        document.ToHtml();
    ASSERT_TRUE(document.GetEditorState() == MakeEditorState(ElementId{0, 1, 0, 3})) << document.GetEditorState().ToString();
}

}
