/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "stdafx.h"
#include "ast.h"
#include "error_handler.h"
#include "annotation.h"

namespace yutovo_calculator
{

template<typename Number> struct Solver;

//Spirit grammar for expression.
template<typename Number>
struct Expression : qi::grammar<std::u32string::iterator, ExpressionNode<Number>(), unicode::space_type>
{
	Expression(LogicalId id, std::u32string& expr, Solver<Number>* _solver);

	typedef boost::variant<
		boost::recursive_wrapper<NumberNode<Number>>, 
		boost::recursive_wrapper<IdentifierNode<Number>>, 
		boost::recursive_wrapper<UnaryOperationNode<Number>>, 
		boost::recursive_wrapper<OperationNode<Number>>, 
		boost::recursive_wrapper<PostfixOperationNode<Number>>, 
		boost::recursive_wrapper<MixedDivivsionNode<Number>>, 
		boost::recursive_wrapper<ImplicitStringMulNode<Number>>, 
		boost::recursive_wrapper<ImplicitDivMulNode<Number>>, 
		boost::recursive_wrapper<ImplicitMulNode<Number>>, 
		boost::recursive_wrapper<ImplicitFractionMulNode<Number>>, 
		boost::recursive_wrapper<ImplicitFunctionMulNode<Number>>, 
		boost::recursive_wrapper<ImplicitPostFunctionMulNode<Number>>, 
		boost::recursive_wrapper<FunctionCallNode<Number>>, 
		boost::recursive_wrapper<FunctionCallStringNode<Number>>, 
		boost::recursive_wrapper<DefiniteIntegralNode<Number>>, 
		boost::recursive_wrapper<DerivativeAtPointNode<Number>>, 
		boost::recursive_wrapper<NoFencesFunctionCallNode<Number>>, 
		boost::recursive_wrapper<CompareNode<Number>>, 
		boost::recursive_wrapper<LoopNode<Number>>, 
		boost::recursive_wrapper<ArrayNode<Number>>, 
		boost::recursive_wrapper<ExpressionNode<Number>>>
		Operand;
	
	qi::rule<std::u32string::iterator, ExpressionNode<Number>(), unicode::space_type> expression, term, addition, multiplication;
	qi::rule<std::u32string::iterator, Operand(), unicode::space_type> unary;
	qi::rule<std::u32string::iterator, OperationNode<Number>(), unicode::space_type> multiply;
	qi::rule<std::u32string::iterator, UnaryOperationNode<Number>(), unicode::space_type> unary_operation;
	qi::rule<std::u32string::iterator, PostfixOperationNode<Number>(), unicode::space_type> postfix_operation;
	qi::rule<std::u32string::iterator, MixedDivivsionNode<Number>(), unicode::space_type> mixed_division;
	qi::rule<std::u32string::iterator, ImplicitStringMulNode<Number>(), unicode::space_type> implicit_string_mul;
	qi::rule<std::u32string::iterator, ImplicitDivMulNode<Number>(), unicode::space_type> implicit_div_mul;
	qi::rule<std::u32string::iterator, ImplicitMulNode<Number>(), unicode::space_type> implicit_mul;
	qi::rule<std::u32string::iterator, ImplicitFractionMulNode<Number>(), unicode::space_type> implicit_fraction_mul;
	qi::rule<std::u32string::iterator, ImplicitFunctionMulNode<Number>(), unicode::space_type> implicit_function_mul;
	qi::rule<std::u32string::iterator, ImplicitPostFunctionMulNode<Number>(), unicode::space_type> implicit_post_function_mul;
	qi::rule<std::u32string::iterator, IdentifierNode<Number>(), unicode::space_type> identifier;
	qi::rule<std::u32string::iterator, FunctionCallNode<Number>(), unicode::space_type> function_call;
	qi::rule<std::u32string::iterator, FunctionCallStringNode<Number>(), unicode::space_type> function_call_string;
	qi::rule<std::u32string::iterator, DefiniteIntegralNode<Number>(), unicode::space_type> definite_integral;
	qi::rule<std::u32string::iterator, DerivativeVariableNode<Number>(), unicode::space_type> derivative_variable;
	qi::rule<std::u32string::iterator, DerivativeAtPointNode<Number>(), unicode::space_type> derivative_at_point;
	qi::rule<std::u32string::iterator, NoFencesFunctionCallNode<Number>(), unicode::space_type> no_fences_function_call;
	qi::rule<std::u32string::iterator, FunctionParamNode<Number>(), unicode::space_type> function_param;
	qi::rule<std::u32string::iterator, CompareNode<Number>(), unicode::space_type> compare;
	qi::rule<std::u32string::iterator, LoopNode<Number>(), unicode::space_type> loop;
	qi::rule<std::u32string::iterator, ArrayNode<Number>(), unicode::space_type> array;
	qi::rule<std::u32string::iterator, std::u32string(), unicode::space_type> dec_number, hex_number, digits_number, exp_number, integer_number_str;
	qi::rule<std::u32string::iterator, std::u32string(), unicode::space_type> name;
	qi::rule<std::u32string::iterator, std::u32string(), boost::spirit::unused_type> symbolic_arg;
	qi::rule<std::u32string::iterator, std::u32string(), boost::spirit::unused_type> symbolic_body;

	qi::rule<std::u32string::iterator, NumberNode<Number>(), unicode::space_type> number, real_number, integer_number;
	qi::rule<std::u32string::iterator, VariableNode<Number>(), unicode::space_type> variable;

	Solver<Number>* solver;
};

};

#endif
