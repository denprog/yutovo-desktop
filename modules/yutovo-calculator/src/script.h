/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include "ast.h"
#include "expression.h"
#include "definition.h"
#include "graph.h"
#include "error_handler.h"

namespace yutovo_calculator
{

//Spirit grammar for main script
template<typename Number>
struct Script : qi::grammar<std::u32string::iterator, ScriptNode<Number>(), unicode::space_type>
{
	Script(LogicalId id, std::u32string& expr, Solver<Number>* solver) : 
		Script::base_type(script), 
		definition(id, expr, solver), 
		graph(id, expr, solver), 
		expression(id, expr, solver)
	{
		using boost::spirit::qi::on_error;
		using boost::spirit::qi::fail;
		qi::_3_type _3;

		script = 
			*(
				(graph > ";") | 
				(definition > ";") | 
				(expression > ";")
			);

		on_error<fail>(script, 
			boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));
	}

	Definition<Number> definition;
	Graph<Number> graph;
	Expression<Number> expression;
	qi::rule<std::u32string::iterator, ScriptNode<Number>(), unicode::space_type> script;
};

};

#endif
