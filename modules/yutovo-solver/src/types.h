/*
 * Yutovo Solver
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __TYPES_H__
#define __TYPES_H__

#include <vector>

typedef unsigned int uint;

//The calculator parsers run in a separate worker process on desktop platforms
#if !defined(__EMSCRIPTEN__)
#define YUTOVO_SOLVER_WORKER
#endif

namespace yutovo_solver
{

enum class SolverType
{
    NONE = 0,
    CALCULATOR = 1,
    PYTHON = 2
};

enum class ExpressionType
{
    NONE = 0,
	SOLVE = 1, //expression for solving
	USER_SYMBOL //symbol of user variable or function
};

//Returned result type
enum class ResultType
{
	NONE = 0,
	REAL,
	INTEGER,
	RATIONAL,
	COMPLEX,
	AUTO,
    ARRAY_REAL,
    SYMBOLIC_REAL,
    SYMBOLIC_RATIONAL,
    SYMBOLIC_COMPLEX
};

enum class ErrorCode
{
    OK = 0,
    UNKNOWN_COMMAND,
    JSON_ERROR, //json was not parsed
    NO_FIELD_ERROR, //there is no field requeried
    FIELD_ERROR, //a field is wrong
    SOLVER_ERROR, //solver was not created or executed
    PARSER_ERROR, //expression was not parsed
    OPERATION_ERROR, //there was error
    SOLVER_RESTARTED_ERROR, //all connected code blocks need to be reevaluated
    TIMEOUT_ERROR, //there was timeout of the service response
    NO_RESULT, //the operation did not produce a result
    NOT_IMPLEMENTED //the operation is not implemented
};

struct ServiceException
{
    ErrorCode error_code;
};

}

#endif
