/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <boost/locale.hpp>
#include <vector>
#include <atomic>
#ifdef _WIN32
#include <windows.h>
#endif
#include "math_helper.h"

namespace yutovo_calculator
{

typedef unsigned int uint;

#ifdef _WIN32
extern HANDLE thread_handle;
#else
extern clockid_t thread_clock_id;
#endif

struct ElementId : std::vector<int>
{
    ElementId() = default;

    ElementId(std::initializer_list<int> c) : 
        std::vector<int>{c}
    {
    }

    ElementId(const ElementId& source, const int size) : 
        std::vector<int>(source.begin(), source.begin() + size)
    {
    }
};

struct LogicalId : std::vector<int> //logical Id does not include row id, so it does not depend on the formatting of rows
{
    LogicalId() = default;

    LogicalId(std::initializer_list<int> c) : 
        std::vector<int>{c}
    {
    }

    LogicalId(const LogicalId& source, const int size) : 
        std::vector<int>(source.begin(), source.begin() + size)
    {
    }
};

void InitThreadTime();
void GetThreadTime(uint64_t& time);

struct Export;

struct ParserContext
{
    ParserContext();

    void Init(uint64_t _max_time)
    {
        break_solving = false;
        max_time = _max_time;

        if (max_time == 0)
        {
            end_time = 0;
            return;
        }

        uint64_t now;
        GetThreadTime(now);
        end_time = now + max_time;
    }

    void ReInit()
    {
        uint64_t now;
        GetThreadTime(now);
        end_time = now + max_time;
    }

    std::atomic<bool> break_solving{false};
    uint64_t end_time = 0; //solve before this time or rise TimeExceedException
    uint64_t max_time = 0;
    bool no_result = false;
    bool include_document = false;
    std::shared_ptr<Export> exports;
};

std::u32string ToUtfString(const std::string& str);
std::u32string ToUtfString(const char* str);
std::string ToBasicString(const std::u32string& str);
std::wstring ToWString(const std::string& str);
std::string ToBasicString(const std::wstring& str);

int StringToInt(const std::u32string& str);

std::string ElementIdToString(const ElementId& id);
std::string LogicalIdToString(const LogicalId& id);

ElementId ElementIdFromString(const std::string& id);
LogicalId LogicalIdFromString(const std::string& id);

bool IsLess(const LogicalId& id1, const LogicalId& id2);

void CheckBreak(ParserContext* parser_context);

std::string LanguageToString(const Language language);

}

#endif
