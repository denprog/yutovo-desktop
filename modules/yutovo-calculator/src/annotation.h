/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef ANNOTATION_H
#define ANNOTATION_H

#include "ast.h"

namespace yutovo_calculator
{

//Annotation for nodes. Used in purposes of searching a node by a text position.
template<typename Number>
struct Annotation
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

	Annotation(std::u32string::iterator _first, std::u32string::iterator _last, LogicalId& _id, ParserContext** _parser_context) : 
		id(_id),
		first(_first),
		last(_last),
		parser_context(_parser_context)
	{
	}

	//Updates the position in the expression.
	void UpdatePosition(std::u32string::iterator p, ExpressionPosition const& position) const
	{
		UpdatePosition(p, (ExpressionPosition&)position);
	}
	
	//Updates the position in the expression.
	void UpdatePosition(std::u32string::iterator p, ExpressionPosition& position) const
	{
		CheckBreak(*parser_context);

		int i = 0, j = 0;
		std::u32string::iterator k = first;
		
		while (k != p)
		{
			char32_t ch = *k;
			if (ch == U'\r')
			{
				++k;
				++j;
			}
		
			++k;
			++i;
		}
		
		position.id = id;
		position.pos = i;
		position.line = j;
	}
	
	void operator()(Operand& op, std::u32string::iterator pos) const
	{
		boost::apply_visitor(OperandVisitor<Number>(this, pos), op);
	}

	void operator()(IdentifierNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(NumberNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ExpressionNode<Number>& expr, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, expr);
	}

	void operator()(UnaryOperationNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(PostfixOperationNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(OperationNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(MixedDivivsionNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ImplicitStringMulNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ImplicitDivMulNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ImplicitMulNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ImplicitFractionMulNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ImplicitFunctionMulNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ImplicitPostFunctionMulNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(FunctionCallNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
		auto it = pos;
		it += static_cast<std::ptrdiff_t>(op.name.name.length());
		while (it != last && *it == U' ')
			++it;
		if (it == last || *it != U'(')
		{
			op.size = static_cast<int>(op.name.name.length());
			return;
		}
		int depth = 1;
		++it;
		for (; it != last; ++it)
		{
			char32_t ch = *it;
			if (ch == U'(')
				++depth;
			else if (ch == U')')
				--depth;
			if (depth == 0)
				break;
		}
		if (depth != 0)
			op.size = static_cast<int>(op.name.name.length());
		else
			op.size = static_cast<int>(std::distance(pos, it)) + 1;
	}

	void operator()(FunctionCallStringNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
		auto it = pos;
		it += static_cast<std::ptrdiff_t>(op.name.name.length());
		while (it != last && *it == U' ')
			++it;
		if (it == last || *it != U'[')
		{
			op.size = static_cast<int>(op.name.name.length());
			return;
		}
		int depth = 1;
		++it;
		for (; it != last; ++it)
		{
			char32_t ch = *it;
			if (ch == U'[')
				++depth;
			else if (ch == U']')
				--depth;
			if (depth == 0)
				break;
		}
		if (depth != 0)
			op.size = static_cast<int>(op.name.name.length());
		else
			op.size = static_cast<int>(std::distance(pos, it)) + 1;
	}

	void operator()(DefiniteIntegralNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
		auto it = pos;
		while (it != last && *it != U'(')
			++it;
		if (it == last)
		{
			op.size = static_cast<int>(op.expression.length()) + static_cast<int>(op.variable.length()) + 3;
			return;
		}
		int depth = 1;
		++it;
		for (; it != last; ++it)
		{
			char32_t ch = *it;
			if (ch == U'(')
				++depth;
			else if (ch == U')')
				--depth;
			if (depth == 0)
				break;
		}
		if (depth != 0)
			op.size = static_cast<int>(std::distance(pos, it));
		else
			op.size = static_cast<int>(std::distance(pos, it)) + 1;
	}

	void operator()(DerivativeAtPointNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
		auto it = pos;
		while (it != last && *it != U'(')
			++it;
		if (it == last)
		{
			int fallback_size = 1;
			if (!op.variables.empty())
				fallback_size = static_cast<int>(op.variables.front().name.length()) + 1;
			op.size = fallback_size;
			return;
		}
		int depth = 1;
		++it;
		for (; it != last; ++it)
		{
			char32_t ch = *it;
			if (ch == U'(')
				++depth;
			else if (ch == U')')
				--depth;
			if (depth == 0)
				break;
		}
		if (depth != 0)
			op.size = static_cast<int>(std::distance(pos, it));
		else
			op.size = static_cast<int>(std::distance(pos, it)) + 1;
	}

	void operator()(NoFencesFunctionCallNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
		op.size = static_cast<int>(op.name.name.length());
	}

	void operator()(CompareNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(LoopNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(ArrayNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(VariableNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(DefinitionNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(BarGraphNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(LineGraphNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	void operator()(GraphNode<Number>& op, std::u32string::iterator pos) const
	{
		UpdatePosition(pos, op);
	}

	template<typename Num>
	struct OperandVisitor
	{
		OperandVisitor(Annotation<Num> const* _annotation, std::u32string::iterator _iter) : annotation(_annotation), iter(_iter)
		{
		}
		
		void operator()(OperationNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(NumberNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(UnaryOperationNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(PostfixOperationNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(MixedDivivsionNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ImplicitStringMulNode<Number>& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ImplicitDivMulNode<Number>& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ImplicitMulNode<Number>& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ImplicitFractionMulNode<Number>& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ImplicitFunctionMulNode<Number>& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ImplicitPostFunctionMulNode<Number>& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(IdentifierNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(FunctionCallNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(FunctionCallStringNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(DefiniteIntegralNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(DerivativeAtPointNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(NoFencesFunctionCallNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(CompareNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(LoopNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ArrayNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(ExpressionNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(DefinitionNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(VariableNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(LineGraphNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(BarGraphNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(GraphNode<Num> const& op) const
		{
			annotation->UpdatePosition(iter, op);
		}

		void operator()(Num const& op) const
		{
			//((Number)op).SetPrecision(10);
		}
		
		typedef void result_type;
		Annotation<Num> const* annotation;
		std::u32string::iterator iter;
	};

	template<typename, typename>
	struct result
	{
		typedef void type;
	};

	LogicalId id;
	std::u32string::iterator first;
	std::u32string::iterator last;
	ParserContext** parser_context;
};

}

#endif
