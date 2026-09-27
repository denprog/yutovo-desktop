/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "utils.h"
#include "parser_exception.h"
#include <chrono>
#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif
#ifdef _WIN32
#include <processthreadsapi.h>
#include <timezoneapi.h>
#endif
#include "real.h"
#include "export.h"

namespace yutovo_calculator
{

#ifdef _WIN32
HANDLE thread_handle = 0;
#else
clockid_t thread_clock_id = 0;
#endif

std::u32string ToUtfString(const std::string& str)
{
    return boost::locale::conv::utf_to_utf<char32_t>(str);
}

std::u32string ToUtfString(const char* str)
{
    return boost::locale::conv::utf_to_utf<char32_t>(str);
}

std::string ToBasicString(const std::u32string& str)
{
    return boost::locale::conv::utf_to_utf<char>(str);
}

std::wstring ToWString(const std::string& str)
{
	return boost::locale::conv::utf_to_utf<wchar_t>(str);
}

std::string ToBasicString(const std::wstring& str)
{
	return boost::locale::conv::utf_to_utf<char>(str);
}

int StringToInt(const std::u32string& str)
{
    if (str.empty())
        throw std::out_of_range("Empty string");

    int sign = 1;
    size_t i = 0;

    if (str[0] == U'-')
    {
        sign = -1;
        i = 1;
    }
    else if (str[0] == U'+')
        i = 1;

    int value = 0;
    for (; i < str.size(); ++i)
    {
        char32_t ch = str[i];
        if (ch < U'0' || ch > U'9')
            throw std::out_of_range("Invalid digit");
        int digit = static_cast<int>(ch - U'0');
        if (value > (std::numeric_limits<int>::max() - digit) / 10)
            throw std::overflow_error("Integer overflow");
        value = value * 10 + digit;
    }

    return value * sign;        
}

std::string ElementIdToString(const ElementId& id)
{
	std::string res;
	for (size_t i = 0; i < id.size(); ++i)
	{
		res += std::to_string(id[i]);
		if (i < id.size() - 1)
			res += ",";
	}
	return res;
}

std::string LogicalIdToString(const LogicalId& id)
{
	std::string res;
	for (size_t i = 0; i < id.size(); ++i)
	{
		res += std::to_string(id[i]);
		if (i < id.size() - 1)
			res += ",";
	}
	return res;
}

ElementId ElementIdFromString(const std::string& id)
{
    ElementId res;
    std::stringstream s(id);
    for (int i; s >> i;)
    {
        res.push_back(i);
        if (s.peek() == ',')
            s.ignore();
    }
    if (!s.eof())
        throw ParserException(LogicalId{}, IncorrectIdentifier, 0, 0);
    return res;
}

LogicalId LogicalIdFromString(const std::string& id)
{
    LogicalId res;
    std::stringstream s(id);
    for (int i; s >> i;)
    {
        res.push_back(i);
        if (s.peek() == ',')
            s.ignore();
    }
    if (!s.eof())
        throw ParserException(LogicalId{}, IncorrectIdentifier, 0, 0);
    return res;
}

bool IsLess(const LogicalId& id1, const LogicalId& id2)
{
	for (size_t i = 0, j = 0; i < id1.size() && j < id2.size(); ++i, ++j)
	{
		if (id1[i] > id2[j])
			return false;
		if (id1[i] < id2[j])
			return true;
	}
	return id1.size() < id2.size();
}

void CheckBreak(ParserContext* parser_context)
{
	if (!parser_context || parser_context->end_time == 0)
		return;
    if (parser_context->break_solving)
        throw BreakException();
    
    uint64_t now;
    GetThreadTime(now);
	if (now > parser_context->end_time)
		throw TimeExceedException();
}

std::string LanguageToString(const Language language)
{
    switch (language)
    {
    case Language::English:
        return "English";
    case Language::Russian:
        return "Russian";
    case Language::Spanish:
        return "Spanish";
    case Language::BrazilianPortuguese:
        return "Portuguese (Brazil)";
    case Language::None:
        return "";
    }
    return "";
}

void InitThreadTime()
{
#ifdef _WIN32
    thread_handle = GetCurrentThread();
#else
    pthread_getcpuclockid(pthread_self(), &thread_clock_id);
#endif
}

void GetThreadTime(uint64_t& time)
{
#ifdef EMSCRIPTEN
    time = (uint64_t)emscripten_get_now();
#elif _WIN32
    FILETIME creation_time, exit_time, kernel_time, user_time;
    if (GetThreadTimes(thread_handle, &creation_time, &exit_time, &kernel_time, &user_time))
    {
        ULARGE_INTEGER kernel, user;
        kernel.LowPart = kernel_time.dwLowDateTime;
        kernel.HighPart = kernel_time.dwHighDateTime;
        user.LowPart = user_time.dwLowDateTime;
        user.HighPart = user_time.dwHighDateTime;
        time = (kernel.QuadPart + user.QuadPart) / 10000;
    }
#else
    timespec s;
    clock_gettime(thread_clock_id, &s);
    time = s.tv_sec * 1000 + s.tv_nsec / 1000000;
#endif
}

//ParserContext

ParserContext::ParserContext() :
    exports(new Export())
{
}

}
