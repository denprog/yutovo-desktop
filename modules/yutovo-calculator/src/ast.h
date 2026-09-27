/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef AST_H
#define AST_H

#include "stdafx.h"
#include "integer.h"
#include "real.h"
#include "rational.h"
#include "complex.h"
#include "symbolic.h"
#include "array.h"
#include "utils.h"

#ifdef _WIN32
#undef access
#endif

namespace yutovo_calculator
{

namespace unicode = boost::spirit::unicode;

template<typename Number>
struct IdentifierNode;
template<typename Number>
struct NumberNode;
template<typename Number>
struct UnaryOperationNode;
template<typename Number>
struct OperationNode;
template<typename Number>
struct PostfixOperationNode;
template<typename Number>
struct MixedDivivsionNode;
template<typename Number>
struct ImplicitStringMulNode;
template<typename Number>
struct ImplicitDivMulNode;
template<typename Number>
struct ImplicitFractionMulNode;
template<typename Number>
struct ImplicitFunctionMulNode;
template<typename Number>
struct ImplicitPostFunctionMulNode;
template<typename Number>
struct ImplicitMulNode;
template<typename Number>
struct VariableNode;
template<typename Number>
struct UnitNode;
template<typename Number>
struct ListNode;
template<typename Number>
struct StringNode;
template<typename Number>
struct FunctionCallNode;
template<typename Number>
struct FunctionCallStringNode;
template<typename Number>
struct DefiniteIntegralNode;
template<typename Number>
struct DerivativeVariableNode;
template<typename Number>
struct DerivativeAtPointNode;
template<typename Number>
struct NoFencesFunctionCallNode;
template<typename Number>
struct CompareNode;
template<typename Number>
struct LoopNode;
template<typename Number>
struct ArrayNode;
template<typename Number>
struct ExpressionNode;
template<typename Number>
struct GraphNode;
template<typename Number>
struct LineGraphNode;
template<typename Number>
struct BarGraphNode;

//Expression position
struct ExpressionPosition
{
    ExpressionPosition()
    {
    }
    
    mutable LogicalId id;
    int pos = -1; //position in a line
    int line = -1; //line number
    int size = 0; //source text size of the node (0 means unset)
};

//Identifier node
template<typename Number>
struct IdentifierNode : public ExpressionPosition
{
    bool operator==(const IdentifierNode& other) const
    {
        return name == other.name && subscript == other.subscript;
    }

    std::u32string name;
    std::u32string subscript;
    std::u32string description;
};

template<typename Number>
struct NumberNode : public ExpressionPosition
{
    std::u32string number;
};

//Unary operation node.
template<typename Number>
struct UnaryOperationNode : public ExpressionPosition
{
    typedef boost::variant<
        boost::recursive_wrapper<NumberNode<Number>>, 
        boost::recursive_wrapper<IdentifierNode<Number>>, 
        boost::recursive_wrapper<UnaryOperationNode<Number>>, 
        boost::recursive_wrapper<OperationNode<Number>>, 
        boost::recursive_wrapper<PostfixOperationNode<Number>>, 
        boost::recursive_wrapper<MixedDivivsionNode<Number>>, 
        boost::recursive_wrapper<ImplicitStringMulNode<Number>>,
        boost::recursive_wrapper<ImplicitDivMulNode<Number>>,
        boost::recursive_wrapper<ImplicitFractionMulNode<Number>>, 
        boost::recursive_wrapper<ImplicitFunctionMulNode<Number>>, 
        boost::recursive_wrapper<ImplicitPostFunctionMulNode<Number>>, 
        boost::recursive_wrapper<ImplicitMulNode<Number>>,
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
    
    char32_t op;
    Operand operand;
};

//Operation node.
template<typename Number>
struct OperationNode : ExpressionPosition
{
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
    
    char32_t op;
    Operand operand;
};

//Postfix operation node.
template<typename Number>
struct PostfixOperationNode : ExpressionPosition
{
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
    
    char32_t op;
    Operand operand;
};

template<typename Number>
struct MixedDivivsionNode : ExpressionPosition
{
    NumberNode<Number> left;
    NumberNode<Number> numerator;
    NumberNode<Number> denominator;
};

template<typename Number>
struct ImplicitStringMulNode : ExpressionPosition
{
    NumberNode<Number> left;
    IdentifierNode<Number> identifier;
};

template<typename Number>
struct ImplicitDivMulNode : ExpressionPosition
{
    ExpressionNode<Number> upper;
    ExpressionNode<Number> lower;
    IdentifierNode<Number> identifier;
};

template<typename Number>
struct ImplicitFractionMulNode : ExpressionPosition
{
    NumberNode<Number> numerator;
    NumberNode<Number> denominator;
    IdentifierNode<Number> identifier;
};

template<typename Number>
struct ImplicitFunctionMulNode : ExpressionPosition
{
    NumberNode<Number> left;
    FunctionCallNode<Number> function_call;
};

template<typename Number>
struct ImplicitPostFunctionMulNode : ExpressionPosition
{
    FunctionCallNode<Number> function_call;
    IdentifierNode<Number> right;
};

template<typename Number>
struct ImplicitMulNode : ExpressionPosition
{
    NumberNode<Number> before;
    ExpressionNode<Number> inside_braces;
};

template<typename Number>
struct VariableNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    ExpressionNode<Number> expression;
    bool exported = false;
};

template<typename Number>
struct FunctionNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    std::list<IdentifierNode<Number>> arguments;
    ExpressionNode<Number> return_expression;
    bool exported = false;
};

template<typename Number>
struct UnitNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    ExpressionNode<Number> expression;
};

template<typename Number>
struct ListNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    std::vector<std::u32string> list;
};

template<typename Number>
struct StringNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    std::u32string str;
};

template<typename Number>
struct DefinitionNode : ExpressionPosition
{
    typedef boost::variant<
        boost::recursive_wrapper<VariableNode<Number>>, 
        boost::recursive_wrapper<UnitNode<Number>>, 
        boost::recursive_wrapper<FunctionNode<Number>>,
        boost::recursive_wrapper<ListNode<Number>>,
        boost::recursive_wrapper<StringNode<Number>>> 
        Definition;
    
    Definition definition;
};

template<typename Number>
struct FunctionCallNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    std::list<ExpressionNode<Number>> arguments;
};

template<typename Number>
struct FunctionCallStringNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    std::u32string argument;
};

template<typename Number>
struct DefiniteIntegralNode : ExpressionPosition
{
    ExpressionNode<Number> lower;
    ExpressionNode<Number> upper;
    std::u32string expression;
    std::u32string variable;
};

template<typename Number>
struct DerivativeVariableNode : ExpressionPosition
{
    std::u32string name;
    ExpressionNode<Number> value;
};

template<typename Number>
struct DerivativeAtPointNode : ExpressionPosition
{
    using FunctionType = boost::variant<std::u32string, ExpressionNode<Number>>;
    FunctionType function;
    std::list<DerivativeVariableNode<Number>> variables;
};

template<typename Number>
struct FunctionParamNode : ExpressionPosition
{
    typedef Number value_type;
    typedef boost::variant<
        boost::recursive_wrapper<NumberNode<Number>>, 
        boost::recursive_wrapper<IdentifierNode<Number>>, 
        boost::recursive_wrapper<ExpressionNode<Number>>>
        Operand;
    
    Operand op;
};

template<typename Number>
struct NoFencesFunctionCallNode : ExpressionPosition
{
    IdentifierNode<Number> name;
    std::list<ExpressionNode<Number>> arguments;
    FunctionParamNode<Number> last_argument;
};

template<typename Number>
struct CompareNode : ExpressionPosition
{
    ExpressionNode<Number> left;
    std::u32string sign;
    ExpressionNode<Number> right;
};

template<typename Number>
struct LoopNode : ExpressionPosition
{
    VariableNode<Number> counter;
    CompareNode<Number> counter_max;
    VariableNode<Number> counter_increment;
    VariableNode<Number> loop_var;
    VariableNode<Number> loop_expression;
};

template<typename Number>
struct ArrayNode : ExpressionPosition
{
    std::list<ExpressionNode<Number>> array;
};

template<typename Number>
struct ExpressionNode : ExpressionPosition
{
    typedef Number value_type;
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
    
    ExpressionNode() = default;

    ExpressionNode(const DefiniteIntegralNode<Number>& node) : 
        first(node)
    {
    }
    
    ExpressionNode(const DerivativeAtPointNode<Number>& node) : 
        first(node)
    {
    }

    Operand first;
    std::list<OperationNode<Number>> rest;
};

//Line graph node
template<typename Number>
struct LineGraphNode : ExpressionPosition
{
    ExpressionNode<Number> expression;
    IdentifierNode<Number> identifier;
    ExpressionNode<Number> x_left;
    ExpressionNode<Number> x_right;
    ExpressionNode<Number> y_bottom;
    ExpressionNode<Number> y_top;
    NumberNode<Number> points_count;
};

//Bar graph node
template<typename Number>
struct BarGraphNode : ExpressionPosition
{
    ExpressionNode<Number> expression;
    NumberNode<Number> points_count;
};

//Graph node
template<typename Number>
struct GraphNode : ExpressionPosition
{
    typedef boost::variant<
        boost::recursive_wrapper<LineGraphNode<Number>>, 
        boost::recursive_wrapper<BarGraphNode<Number>>> 
        Graph;
    
    Graph graph;
};

//Script node.
template<typename Number>
struct ScriptNode
{
    typedef Number value_type;
    typedef boost::variant<
        boost::recursive_wrapper<DefinitionNode<Number>>, 
        boost::recursive_wrapper<GraphNode<Number>>, 
        boost::recursive_wrapper<ExpressionNode<Number>>> 
        Operand;
    
    std::list<Operand> list;
};

};

using namespace yutovo_calculator;

//Integer adaptors

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Integer>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Integer>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Integer>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Integer>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Integer>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Integer>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Integer>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Integer>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Integer>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Integer>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Integer>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Integer>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Integer>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Integer>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Integer>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Integer>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Integer>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Integer>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Integer>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Integer>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Integer>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Integer>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Integer>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Integer>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Integer>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Integer>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Integer>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Integer>::Graph, graph))

//Real adaptors

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Real>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Real>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Real>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Real>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Real>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Real>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Real>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Real>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Real>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Real>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Real>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Real>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Real>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Real>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Real>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Real>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Real>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Real>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Real>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Real>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefiniteIntegralNode<yutovo_calculator::Real>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, lower)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, upper)
    (std::u32string, expression)
    (std::u32string, variable))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Real>, 
    (std::u32string, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, value))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Real>, 
    (yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Real>::FunctionType, function)
    (std::list<yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Real>>, variables))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Real>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Real>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Real>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Real>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Real>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Real>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Real>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Real>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Real>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Real>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Real>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Real>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Real>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Real>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Real>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Real>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Real>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Real>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Real>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Real>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Real>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Real>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Real>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Real>::Graph, graph))

//Rational adaptors

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Rational>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Rational>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Rational>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Rational>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Rational>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Rational>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Rational>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Rational>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Rational>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Rational>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Rational>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Rational>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefiniteIntegralNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, lower)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, upper)
    (std::u32string, expression)
    (std::u32string, variable))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Rational>, 
    (std::u32string, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, value))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Rational>::FunctionType, function)
    (std::list<yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Rational>>, variables))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Rational>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Rational>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Rational>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Rational>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Rational>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Rational>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Rational>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Rational>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Rational>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Rational>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Rational>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Rational>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Rational>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Rational>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Rational>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Rational>::Graph, graph))

//Complex adaptors

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Complex>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Complex>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Complex>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Complex>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Complex>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Complex>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Complex>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Complex>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Complex>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Complex>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Complex>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Complex>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefiniteIntegralNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, lower)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, upper)
    (std::u32string, expression)
    (std::u32string, variable))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Complex>, 
    (std::u32string, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, value))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Complex>::FunctionType, function)
    (std::list<yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Complex>>, variables))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Complex>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Complex>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Complex>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Complex>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Complex>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Complex>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Complex>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Complex>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Complex>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Complex>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Complex>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Complex>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Complex>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Complex>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Complex>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Complex>::Graph, graph))

//Symbolic adaptors

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Symbolic<Real>>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Symbolic<Real>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Real>>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Real>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Symbolic<Real>>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Symbolic<Real>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Symbolic<Real>>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Symbolic<Real>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Real>>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Real>>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Symbolic<Real>>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Symbolic<Real>>, 
    (std::u32string, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, value))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Symbolic<Real>>::FunctionType, function)
    (std::list<yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Symbolic<Real>>>, variables))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Real>>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Real>>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Real>>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Symbolic<Real>>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Symbolic<Real>>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Real>>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Symbolic<Real>>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Real>>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Real>>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Real>>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Symbolic<Real>>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Real>>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Real>>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Real>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Symbolic<Real>>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Symbolic<Real>>::Graph, graph))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Symbolic<Rational>>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Symbolic<Rational>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Rational>>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Rational>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Symbolic<Rational>>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Symbolic<Rational>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Symbolic<Rational>>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Symbolic<Rational>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Rational>>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Rational>>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Symbolic<Rational>>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Symbolic<Rational>>, 
    (std::u32string, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, value))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Symbolic<Rational>>::FunctionType, function)
    (std::list<yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Symbolic<Rational>>>, variables))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Rational>>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Rational>>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Rational>>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Symbolic<Rational>>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Symbolic<Rational>>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Rational>>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Symbolic<Rational>>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Rational>>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Rational>>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Rational>>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Symbolic<Rational>>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Rational>>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Rational>>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Rational>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Symbolic<Rational>>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Symbolic<Rational>>::Graph, graph))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Symbolic<Complex>>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Symbolic<Complex>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Complex>>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Complex>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Symbolic<Complex>>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Symbolic<Complex>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Symbolic<Complex>>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Symbolic<Complex>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Complex>>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Complex>>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Symbolic<Complex>>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Symbolic<Complex>>, 
    (std::u32string, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, value))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::DerivativeAtPointNode<yutovo_calculator::Symbolic<Complex>>::FunctionType, function)
    (std::list<yutovo_calculator::DerivativeVariableNode<yutovo_calculator::Symbolic<Complex>>>, variables))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Complex>>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Symbolic<Complex>>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Symbolic<Complex>>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Symbolic<Complex>>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Symbolic<Complex>>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Complex>>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Symbolic<Complex>>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Complex>>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Complex>>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Symbolic<Complex>>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Symbolic<Complex>>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Symbolic<Complex>>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Symbolic<Complex>>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Symbolic<Complex>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Symbolic<Complex>>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Symbolic<Complex>>::Graph, graph))

//Array<Real> adaptors

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (char32_t, op)(yutovo_calculator::UnaryOperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (std::u32string, number))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::OperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (char32_t, op)(yutovo_calculator::OperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Operand, operand))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::PostfixOperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::PostfixOperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Operand, operand)(char32_t, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::MixedDivivsionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, left)
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, denominator))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitStringMulNode<yutovo_calculator::Array<yutovo_calculator::Real>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, left)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitDivMulNode<yutovo_calculator::Array<yutovo_calculator::Real>>,
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, upper)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, lower)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitMulNode<yutovo_calculator::Array<yutovo_calculator::Real>>,
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, before)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, inside_braces))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFractionMulNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, numerator)
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, denominator)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, identifier))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitFunctionMulNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, left)
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Array<yutovo_calculator::Real>>, function_call))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ImplicitPostFunctionMulNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::FunctionCallNode<yutovo_calculator::Array<yutovo_calculator::Real>>, function_call)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::VariableNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::UnitNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ListNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (std::vector<std::u32string>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::StringNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (std::u32string, str))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (std::list<yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>>, arguments)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, return_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefinitionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::DefinitionNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Definition, definition))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (std::u32string, name)
    (std::u32string, subscript)
    (std::u32string, description))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>>, arguments))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionCallStringNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (std::u32string, argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::DefiniteIntegralNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, lower)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, upper)
    (std::u32string, expression)
    (std::u32string, variable))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::NoFencesFunctionCallNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, name)
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>>, arguments)
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Array<yutovo_calculator::Real>>, last_argument))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::FunctionParamNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::FunctionParamNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Operand, op))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Operand, first)
    (std::list<yutovo_calculator::OperationNode<yutovo_calculator::Array<yutovo_calculator::Real>>>, rest))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ScriptNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (std::list<yutovo_calculator::ScriptNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Operand>, list))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::CompareNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, left)
    (std::u32string, sign)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, right))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LoopNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::VariableNode<yutovo_calculator::Array<yutovo_calculator::Real>>, counter)
    (yutovo_calculator::CompareNode<yutovo_calculator::Array<yutovo_calculator::Real>>, counter_max)
    (yutovo_calculator::VariableNode<yutovo_calculator::Array<yutovo_calculator::Real>>, counter_increment)
    (yutovo_calculator::VariableNode<yutovo_calculator::Array<yutovo_calculator::Real>>, loop_var)
    (yutovo_calculator::VariableNode<yutovo_calculator::Array<yutovo_calculator::Real>>, loop_expression))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::ArrayNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (std::list<yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>>, array))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::LineGraphNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, expression)
    (yutovo_calculator::IdentifierNode<yutovo_calculator::Array<yutovo_calculator::Real>>, identifier)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, x_left)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, x_right)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, y_bottom)
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, y_top)
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::BarGraphNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::ExpressionNode<yutovo_calculator::Array<yutovo_calculator::Real>>, expression)
    (yutovo_calculator::NumberNode<yutovo_calculator::Array<yutovo_calculator::Real>>, points_count))

BOOST_FUSION_ADAPT_STRUCT(yutovo_calculator::GraphNode<yutovo_calculator::Array<yutovo_calculator::Real>>, 
    (yutovo_calculator::GraphNode<yutovo_calculator::Array<yutovo_calculator::Real>>::Graph, graph))

#endif
