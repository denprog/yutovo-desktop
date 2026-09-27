/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef STDAFX_H
#define STDAFX_H

#ifndef NDEBUG
#define BOOST_SPIRIT_DEBUG
#endif
#define BOOST_SPIRIT_UNICODE

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_LIST_SIZE 30
#define BOOST_MPL_LIMIT_VECTOR_SIZE 30

#if defined(_MSC_VER)
#pragma warning(disable: 4345)
#endif

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#undef min
#undef max
#endif

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/foreach.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <stack>
#include <deque>
#include <list>
#include <map>

#include "mpfr.h"

#ifdef _WIN32
#undef access
#endif

namespace spirit = boost::spirit;
namespace qi = spirit::qi;

#endif
