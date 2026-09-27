/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef PARSEREXCEPTION_H
#define PARSEREXCEPTION_H

#include <string>
#include <vector>
#include "utils.h"

namespace yutovo_calculator
{

typedef unsigned int uint;

enum ParserExceptionCode
{
	None = -1, 
	SyntaxError = 1, 
	WrongArgumentsCount = 2, 
	UnknownIdentifier = 3, 
	ExpressionExpected = 4,
	IncorrectIdentifier = 5, 
	DivisionByZero = 100, 
	Overflow = 101, 
	ArgumentIsOver = 102, 
	ConversionDoesNotFit = 103,
	IncorrectComparison = 104,
	IncorrectOperation = 105,
	UnitsAreIncompatible = 201,
	CannotCastToUnit = 202,
	UnknownLanguage = 301,
	NotImplemented = 302,
	SerializationError = 303,
	TimeExceed = 401,
	Break = 501
};

struct ParserException
{
	ParserException() = default;
	
	ParserException(LogicalId _id, ParserExceptionCode _ex_id, int _pos, int _line) :
		id(_id),
		ex_id(_ex_id),
		pos(_pos),
		line(_line)
	{
	}

	ParserException(LogicalId _id, ParserExceptionCode _ex_id, int _pos, int _size, int _line) :
		id(_id),
		ex_id(_ex_id),
		pos(_pos),
		size(_size),
		line(_line)
	{
	}

	ParserException(LogicalId _id, ParserExceptionCode _ex_id) :
		id(_id),
		ex_id(_ex_id)
	{
	}

	ParserException(LogicalId _id, ParserExceptionCode _ex_id, std::u32string _description, int _pos, int _line) :
		id(_id),
		ex_id(_ex_id),
		description(_description),
		pos(_pos),
		line(_line)
	{
	}

	ParserException(LogicalId _id, ParserExceptionCode _ex_id, std::u32string _description, int _pos, int _size, int _line) :
		id(_id),
		ex_id(_ex_id),
		description(_description),
		pos(_pos),
		size(_size),
		line(_line)
	{
	}

	friend bool operator==(const ParserException& e1, const ParserException& e2)
	{
		return e1.id == e2.id && e1.ex_id == e2.ex_id && e1.pos == e2.pos && e1.line == e2.line;
	}
	
	LogicalId id;
	ParserExceptionCode ex_id = None;
	int pos = -1;
	int size = 0; //0 means whole substring
	int line = -1;
	std::u32string description;
};

struct SyntaxException : public ParserException
{
	SyntaxException(ParserExceptionCode _id) : ParserException(LogicalId{}, _id, -1, -1)
	{
	}

	SyntaxException(LogicalId _id, ParserExceptionCode _ex_id, int _pos, int _line) : 
		ParserException(_id, _ex_id, _pos, _line)
	{
	}

	SyntaxException(LogicalId _id, ParserExceptionCode _ex_id, int _pos, int _size, int _line) : 
		ParserException(_id, _ex_id, _pos, _size, _line)
	{
	}

	SyntaxException(LogicalId _id, ParserExceptionCode _ex_id, const std::u32string& _description, int _pos, int _line) : 
		ParserException(_id, _ex_id, _description, _pos, _line)
	{
	}

	SyntaxException(LogicalId _id, ParserExceptionCode _ex_id, const std::u32string& _description, int _pos, int _size, int _line) : 
		ParserException(_id, _ex_id, _description, _pos, _size, _line)
	{
	}
};

struct MathException : public ParserException
{
	MathException(ParserExceptionCode _id) : ParserException(LogicalId{}, _id, -1, -1)
	{
	}

	MathException(LogicalId _id, ParserExceptionCode _ex_id) : ParserException(_id, _ex_id)
	{
	}

	MathException(LogicalId _id, ParserExceptionCode _ex_id, int _pos, int _line) : ParserException(_id, _ex_id, _pos, _line)
	{
	}

	MathException(LogicalId _id, ParserExceptionCode _ex_id, int _pos, int _size, int _line) : ParserException(_id, _ex_id, _pos, _size, _line)
	{
	}
};

struct TimeExceedException : public ParserException
{
	TimeExceedException() : ParserException(LogicalId{}, ParserExceptionCode::TimeExceed)
	{
	}
};

struct BreakException : public ParserException
{
	BreakException() : ParserException(LogicalId{}, ParserExceptionCode::Break)
	{
	}
};

}

#endif
