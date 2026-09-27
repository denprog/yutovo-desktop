/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __RESULT_CODES_H__
#define __RESULT_CODES_H__

#include <yutovo-solver/types.h>
#include <yutovo-calculator/parser_exception.h>
#include <yutovo-calculator/unit.h>
#include <yutovo-calculator/math_helper.h>
#include <map>
#include <vector>
#include "caret_state.h"

namespace yutovo
{

enum class IOResult
{
    None = -1,
    Success = 0,
    OtherException,    //any exception not listed below
    UnsupportedVersion, //version is unsupported
    InputStreamError, //error on input stream
    OutputStreamError, //error on output stream
    ConnectionError, //error of connecting
    PermissionDenied //read-only or other permission error
};

enum class CopyResult
{
    Success = 0,
    EmptySelection,
    CopyError
};

enum class PasteResult
{
    Success = 0,
    EmptyBuffer,
    PasteError
};

enum class PdfResult
{
    None = -1,
    Success = 0,
    Error = 1
};

struct Error
{
    yutovo_solver::ErrorCode error_code = yutovo_solver::ErrorCode::OK;
    yutovo_calculator::ParserExceptionCode parser_error_code = yutovo_calculator::ParserExceptionCode::None;
    int solver_error_code = -1;
    LogicalId id;
    int pos = -1;
    int size = 0;
    int line = -1;
    std::u32string description;
};

enum class WarningCode
{
    NONE = 0
};

struct Warning
{
    WarningCode code = WarningCode::NONE;
    int pos = -1;
};

typedef std::vector<std::string> Dependencies;

struct Value
{
    std::map<std::string, std::string> value;
    yutovo_calculator::Unit unit;
    yutovo_calculator::AngleMeasure angle_measure = yutovo_calculator::AngleMeasure::None;
    yutovo_calculator::Notation notation = yutovo_calculator::Notation::None;
    std::vector<yutovo_calculator::Unit> cast_units;
};

struct Result
{
    std::string ToString();

    std::string guid;
    yutovo_solver::ResultType type = yutovo_solver::ResultType::NONE;
    std::vector<Value> values;
    Dependencies dependencies;
    Error error;
    std::vector<Warning> warnings;
};

}

#endif
