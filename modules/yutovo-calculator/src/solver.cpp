/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "stdafx.h"
#include "solver.h"

namespace yutovo_calculator
{

//Solver

template<>
Array<Real> Solver<Array<Real>>::operator()(LineGraphNode<Array<Real>> const& op) const
{
    Array<Real> res;
    Array<Real> x;
    if (!op.identifier.subscript.empty())
        throw SyntaxException(op.identifier.id, UnknownIdentifier, op.identifier.pos, op.identifier.name.length(), op.line);
    PushTempVariable(op.identifier.name, x);
    Array<Real> x_right, y_bottom, y_top;
    Array<Real> inc;
    Array<Real> start_pos, end_pos;
    Real nan;
    nan.SetNaN();

    try
    {
        x = (*this)(op.x_left);
        SetTempVariable(op.identifier.name, x);
        x_right = (*this)(op.x_right);
        y_bottom = (*this)(op.y_bottom);
        y_top = (*this)(op.y_top);
        inc = (x_right - x) / ((*this)(op.points_count));
    }
    catch (...)
    {
        PopTempVariables(1);
        throw;
    }

    Array<Real> max_float(64, std::numeric_limits<float>::max());
    if (abs(x) > max_float || abs(x_right) > max_float)
        throw MathException(Overflow);

    if (x == x_right || y_bottom == y_top)
        throw MathException(IncorrectOperation);
    
    //the first items are bounds of the graph
    res.Add(x);
    res.Add(x_right);
    res.Add(y_bottom);
    res.Add(y_top);

    auto get_next_point = 
        [&](Array<Real>& y)
        {
            try
            {
                y = (*this)(op.expression);
            }
            catch (TimeExceedException)
            {
                PopTempVariables(1);
                throw;
            }
            catch (BreakException)
            {
                PopTempVariables(1);
                throw;
            }
            catch (SyntaxException)
            {
                PopTempVariables(1);
                throw;
            }
            catch (...)
            {
                y.Clear();
                y.Add(nan);
            }
        };

    Array<Real> y_div = abs(y_bottom - y_top) / 20;
    Array<Real> y_eighth = abs(y_bottom - y_top) / 8;
    Array<Real> y_gap = abs(y_bottom - y_top) / 100;
    Array<Real> y, _y;
    Array<Real> y_prev = 0;

    auto is_increasing = 
        [&]()
        {
            int p = res.Size() - 1;
            while (p > 10)
            {
                const Array<Real>& r1 = res.Get(p);
                p -= 2;
                const Array<Real>& r2 = res.Get(p);
                if (r1.IsNaN() || r2.IsNaN() || r1 == r2)
                    continue;
                if (r1 > r2)
                    return true;
                return false;
            }
            return false;
        };

    std::function<void (const Array<Real>&, const Array<Real>&, const Array<Real>&, const Array<Real>&, int)> div_segment = 
        [&](const Array<Real>& x1, const Array<Real>& x2, const Array<Real>& y1, const Array<Real>& y2, int div_depth)
        {
            Array<Real> x_div = x1 + (x2 - x1) / 2;
            SetTempVariable(op.identifier.name, x_div);
            get_next_point(_y);

            if (div_depth > 0 && x_div != x1)
            {
                if (_y > y_bottom && abs(_y - y1) > y_div)
                    div_segment(x1, x_div, y1, _y, div_depth - 1);
                if (_y < y_top && abs(_y - y2) > y_div)
                    div_segment(x_div, x2, _y, y2, div_depth - 1);
            }
            
            bool incr = is_increasing();
            if (abs(_y - y_prev) > y_div && ((incr && _y < y_prev) || (!incr && _y > y_prev)))
            {
                res.Add(x_div);
                res.Add(nan);
            }

            res.Add(x_div);
            res.Add(_y);
            y_prev = _y;
        };

    //next items are points of the graph x, y
    while (x < x_right)
    {
        if (parser_context)
            parser_context->ReInit();
        
        SetTempVariable(op.identifier.name, x);
        get_next_point(y);

        if (res.Size() < 8)
        {
            res.Add(x);
            res.Add(y);
            y_prev = y;
            x += inc;
            continue;
        }

        if (!y.IsNaN() && !y_prev.IsNaN())
        {
            if (abs(y - y_prev) > y_eighth)
            {
                bool incr = is_increasing();
                if (incr && y < y_prev)
                {
                    res.Add(x);
                    res.Add(y_top * 10);
                    res.Add(x);
                    res.Add(nan);
                    x += inc;
                    res.Add(x);
                    res.Add(y_bottom * 10);
                    y_prev = y_bottom * 10;
                    x += inc;
                    continue;
                }
                if (!incr && y > y_prev)
                {
                    res.Add(x);
                    res.Add(y_bottom * 10);
                    res.Add(x);
                    res.Add(nan);
                    x += inc;
                    res.Add(x);
                    res.Add(y_top * 10);
                    y_prev = y_top * 10;
                    x += inc;
                    continue;
                }
            }
            if (abs(y - y_prev) > y_div)
            {
                div_segment(x - inc, x, y_prev, y, 20);

                bool incr = is_increasing();
                if ((incr && y < y_prev) || (!incr && y > y_prev))
                {
                    res.Add(x);
                    res.Add(nan);
                }
            }
        }

        res.Add(x);
        res.Add(y);

        if (!y.IsNaN() && (y < y_bottom || y > y_top))
        {
            res.Add(x);
            res.Add(nan);
        }

        y_prev = y;
        x += inc;
    }

    PopTempVariables(1);
    return res;
}

template<>
Real Solver<Real>::operator()(UnitNode<Real> const& op) const
{
    //store the unit
    AddUnit(op);
    return Real();
}

template<>
Rational Solver<Rational>::operator()(UnitNode<Rational> const& op) const
{
    AddUnit(op);
    return Rational();
}

template<>
Array<Real> Solver<Array<Real>>::operator()(UnitNode<Array<Real>> const& op) const
{
    //store the unit
    AddUnit(op);
    return Array<Real>();
}

template<>
Integer Solver<Integer>::operator()(UnitNode<Integer> const& op) const
{
    throw SyntaxException(op.id, SyntaxError, U"Identifier '" + op.name.name + U"' not found", op.pos, op.name.name.length(), op.line);
}

// template<>
// Array<Real> Solver<Array<Real>>::operator()(UnitNode<Array<Real>> const& op) const
// {
//     //store the unit
//     AddUnit(op);
//     return Array<Real>();
// }

template<>
Integer Solver<Integer>::operator()(UnaryOperationNode<Integer> const& op) const
{
    CheckBreak(parser_context);

    Integer right = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'+':
        return right;
    case U'-':
        return -right;
    case U'¬':
        return !right;
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, 1, op.line);
}

template<>
Real Solver<Real>::operator()(UnaryOperationNode<Real> const& op) const
{
    CheckBreak(parser_context);

    Real right = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'+':
        return right;
    case U'-':
        return -right;
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, 1, op.line);
}

template<>
Rational Solver<Rational>::operator()(UnaryOperationNode<Rational> const& op) const
{
    CheckBreak(parser_context);

    Rational right = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'+':
        return right;
    case U'-':
        return -right;
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, 1, op.line);
}

template<>
Complex Solver<Complex>::operator()(UnaryOperationNode<Complex> const& op) const
{
    CheckBreak(parser_context);

    Complex right = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'+':
        return right;
    case U'-':
        return -right;
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, 1, op.line);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(UnaryOperationNode<Array<Real>> const& op) const
{
    CheckBreak(parser_context);

    Array<Real> right = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'+':
        return right;
    case U'-':
        return -right;
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, 1, op.line);
}

template<>
Integer Solver<Integer>::operator()(OperationNode<Integer> const& op) const
{
    CheckBreak(parser_context);

    Integer right = boost::apply_visitor(*this, op.operand);
    
    try
    {
        //calculate the operation
        switch (op.op)
        {
        case U'+':
            return left_value + right;
        case U'-':
            return left_value - right;
        case U'*':
            return left_value * right;
        case U'/':
            return left_value / right;
        case U'∧':
            return left_value & right;
        case U'∨':
            return left_value | right;
        case U'⊕':
            return left_value ^ right;
        }
    }
    catch (MathException e)
    {
        throw MathException(e.id, e.ex_id, op.pos, op.line);
    }
    
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Real Solver<Real>::operator()(OperationNode<Real> const& op) const
{
    CheckBreak(parser_context);

    Real right = boost::apply_visitor(*this, op.operand);
    
    try
    {
        //calculate the operation
        switch (op.op)
        {
        case U'+':
            return left_value + right;
        case U'-':
            return left_value - right;
        case U'*':
            return left_value * right;
        case U'/':
            return left_value / right;
        case U'%':
            return left_value * (float)0.01 * right;
        }
    }
    catch (MathException e)
    {
        throw MathException(e.id, e.ex_id, op.pos, op.line);
    }

    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Rational Solver<Rational>::operator()(OperationNode<Rational> const& op) const
{
    CheckBreak(parser_context);

    Rational right = boost::apply_visitor(*this, op.operand);
    
    try
    {
        //calculate the operation
        switch (op.op)
        {
        case U'+':
            return left_value + right;
        case U'-':
            return left_value - right;
        case U'*':
            return left_value * right;
        case U'/':
            return left_value / right;
        case U'%':
            return left_value / 100 * right;
        }
    }
    catch (MathException e)
    {
        throw MathException(e.id, e.ex_id, op.pos, op.line);
    }

    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Complex Solver<Complex>::operator()(OperationNode<Complex> const& op) const
{
    CheckBreak(parser_context);

    Complex right = boost::apply_visitor(*this, op.operand);
    
    try
    {
        //calculate the operation
        switch (op.op)
        {
        case U'+':
            return left_value + right;
        case U'-':
            return left_value - right;
        case U'*':
            return left_value * right;
        case U'/':
            return left_value / right;
        case U'%':
            return left_value * (float)0.01 * right;
        }
    }
    catch (MathException e)
    {
        throw MathException(e.id, e.ex_id, op.pos, op.line);
    }

    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(OperationNode<Array<Real>> const& op) const
{
    Array<Real> right = boost::apply_visitor(*this, op.operand);
    
    try
    {
        //calculate the operation
        switch (op.op)
        {
        case U'+':
            return left_value + right;
        case U'-':
            return left_value - right;
        case U'*':
            return left_value * right;
        case U'/':
            return left_value / right;
        case U'%':
            return left_value * (float)0.01 * right;
        }
    }
    catch (MathException e)
    {
        throw MathException(e.id, e.ex_id, op.pos, op.line);
    }
    
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Integer Solver<Integer>::operator()(PostfixOperationNode<Integer> const& op) const
{
    Integer left = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'!':
        return fact(left, parser_context);
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Real Solver<Real>::operator()(PostfixOperationNode<Real> const& op) const
{
    Real left = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'!':
        return fact(left, parser_context);
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Rational Solver<Rational>::operator()(PostfixOperationNode<Rational> const& op) const
{
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Complex Solver<Complex>::operator()(PostfixOperationNode<Complex> const& op) const
{
    Complex left = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'!':
        return fact(left, parser_context);
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Symbolic<Real> Solver<Symbolic<Real>>::operator()(PostfixOperationNode<Symbolic<Real>> const& op) const
{
    Symbolic<Real> left = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'!':
        return fact(left, parser_context);
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Symbolic<Rational> Solver<Symbolic<Rational>>::operator()(PostfixOperationNode<Symbolic<Rational>> const& op) const
{
    Symbolic<Rational> left = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'!':
        return fact(left, parser_context);
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Symbolic<Complex> Solver<Symbolic<Complex>>::operator()(PostfixOperationNode<Symbolic<Complex>> const& op) const
{
    Symbolic<Complex> left = boost::apply_visitor(*this, op.operand);
    switch (op.op)
    {
    case U'!':
        return fact(left, parser_context);
    }
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Integer Solver<Integer>::operator()(FunctionCallNode<Integer> const& op) const
{
    AddDependency(op.name);

    Integer res;
    
    try
    {
        //find in the user defined functions
        FunctionNode<Integer>* user_func = FindFunction(op);
        if (user_func)
        {
            IdentifierNodesIter funcIter = user_func->arguments.begin();
            for (ExpressionNodesIter iter = op.arguments.begin(); iter != op.arguments.end(); ++iter, ++funcIter)
            {
                CheckBreak(parser_context);

                Integer arg = (*this)(*iter);
                PushTempVariable(funcIter->name, arg);
            }

            bool _exported_id = exported_id;
            exported_id = user_func->exported;
            res = (*this)(user_func->return_expression);
            exported_id = _exported_id;
            PopTempVariables(op.arguments.size());
            return res;
        }

        //find in the build-in functions		
        BuiltinFunction* func = FindBuiltinFunction(op.name.name);
        if (func)
        {
            try
            {
                UnaryFunction u = std::get<UnaryFunction>(*func);
                if (op.arguments.size() != 1)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Integer arg = (*this)(*iter);
                return (*u)(arg);
            }
            catch (const std::bad_variant_access&)
            {
            }
            
            try
            {
                BinaryFunction b = std::get<BinaryFunction>(*func);
                if (op.arguments.size() != 2)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Integer arg1 = (*this)(*iter++);
                Integer arg2 = (*this)(*iter);
                return (*b)(arg1, arg2);
            }
            catch (const std::bad_variant_access&)
            {
            }
        }
    }
    catch (const MathException& e)
    {
        throw MathException(op.id, e.ex_id, op.pos, op.line);
    }

    //there is no such a function		
    throw SyntaxException(op.id, UnknownIdentifier, op.pos, op.name.name.length(), op.line);
}

template<>
Real Solver<Real>::operator()(FunctionCallNode<Real> const& op) const
{
    return FunctionCall(op);
}

template<>
Rational Solver<Rational>::operator()(FunctionCallNode<Rational> const& op) const
{
    AddDependency(op.name);

    Rational res;
    
    try
    {
        //find in the user defined functions
        FunctionNode<Rational>* user_func = FindFunction(op);
        if (user_func)
        {
            IdentifierNodesIter funcIter = user_func->arguments.begin();
            for (ExpressionNodesIter iter = op.arguments.begin(); iter != op.arguments.end(); ++iter, ++funcIter)
            {
                CheckBreak(parser_context);

                Rational arg = (*this)(*iter);
                PushTempVariable(funcIter->name, arg);
            }

            bool _exported_id = exported_id;
            exported_id = user_func->exported;
            res = (*this)(user_func->return_expression);
            exported_id = _exported_id;
            PopTempVariables(op.arguments.size());
            return res;
        }

        //find in the build-in functions		
        BuiltinFunction* func = FindBuiltinFunction(op.name.name);
        if (func)
        {
            try
            {
                UnaryFunction u = std::get<UnaryFunction>(*func);
                if (op.arguments.size() != 1)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Rational arg = (*this)(*iter);
                return (*u)(arg);
            }
            catch (const std::bad_variant_access&)
            {
            }
            
            try
            {
                BinaryFunction b = std::get<BinaryFunction>(*func);
                if (op.arguments.size() != 2)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Rational arg1 = (*this)(*iter++);
                Rational arg2 = (*this)(*iter);
                return (*b)(arg1, arg2);
            }
            catch (const std::bad_variant_access&)
            {
            }

            try
            {
                TernaryFunction t = std::get<TernaryFunction>(*func);
                if (op.arguments.size() != 3)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Rational arg1 = (*this)(*iter++);
                Rational arg2 = (*this)(*iter++);
                Rational arg3 = (*this)(*iter);
                return (*t)(arg1, arg2, arg3);
            }
            catch (const std::bad_variant_access&)
            {
            }

            try
            {
                QuaternaryFunction q = std::get<QuaternaryFunction>(*func);
                if (op.arguments.size() != 4)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Rational arg1 = (*this)(*iter++);
                Rational arg2 = (*this)(*iter++);
                Rational arg3 = (*this)(*iter++);
                Rational arg4 = (*this)(*iter);
                return (*q)(arg1, arg2, arg3, arg4);
            }
            catch (const std::bad_variant_access&)
            {
            }
        }
    }
    catch (const MathException& e)
    {
        throw MathException(op.id, e.ex_id, op.pos, op.line);
    }

    //there is no such a function		
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name.name + U"' not found", op.pos, op.name.name.length(), op.line);
}

template<>
Complex Solver<Complex>::operator()(FunctionCallNode<Complex> const& op) const
{
    return FunctionCall(op);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(FunctionCallNode<Array<Real>> const& op) const
{
    return FunctionCall(op);
}

template<>
Integer Solver<Integer>::operator()(FunctionCallStringNode<Integer> const& op) const
{
    AddDependency(op.name);

    Integer res;

    BuiltinFunction* func = FindBuiltinFunction(op.name.name);
    if (func)
    {
        try
        {
            StringFunction u = std::get<StringFunction>(*func);
            return (*u)(op.argument);
        }
        catch (const MathException& ex)
        {
            throw MathException(op.id, ex.ex_id, op.pos, op.size, op.line);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    //there is no such a function		
    throw SyntaxException(op.id, UnknownIdentifier, op.pos, op.name.name.length(), op.line);
}

template<>
Real Solver<Real>::operator()(FunctionCallStringNode<Real> const& op) const
{
    //there is no such a function		
    throw SyntaxException(op.id, UnknownIdentifier, op.pos, op.name.name.length(), op.line);
}

template<>
Rational Solver<Rational>::operator()(FunctionCallStringNode<Rational> const& op) const
{
    //there is no such a function		
    throw SyntaxException(op.id, UnknownIdentifier, op.pos, op.name.name.length(), op.line);
}

template<>
Complex Solver<Complex>::operator()(FunctionCallStringNode<Complex> const& op) const
{
    //there is no such a function		
    throw SyntaxException(op.id, UnknownIdentifier, op.pos, op.name.name.length(), op.line);
}

template<>
VariableNode<Array<Real>>* Solver<Array<Real>>::FindVariable(const std::u32string& name, const std::u32string& subscript) const
{
    for (int i = (int)symbols->variables.size() - 1; i >= 0; --i)
    {
        auto& var = symbols->variables[i];
        if ((exported_id || IsLess(var.id, id)) && var.name.name == name)
        {
            if (var.name.subscript == subscript)
                return &var;
            if (!var.name.subscript.empty())
                continue;
            return &var;
        }
    }

    if (FindTempVariable(subscript))
        return FindExportVariable(name, U"");
    return FindExportVariable(name, subscript);
}

template<>
Integer Solver<Integer>::operator()(NoFencesFunctionCallNode<Integer> const& op) const
{
    AddDependency(op.name);

    Integer res;
    return res;
}

template<>
Real Solver<Real>::operator()(NoFencesFunctionCallNode<Real> const& op) const
{
    return NoFencesFunctionCall(op);
}

template<>
Rational Solver<Rational>::operator()(NoFencesFunctionCallNode<Rational> const& op) const
{
    AddDependency(op.name);

    Rational res;
    return res;
}

template<>
Complex Solver<Complex>::operator()(NoFencesFunctionCallNode<Complex> const& op) const
{
    return NoFencesFunctionCall(op);
}

template<>
Integer Solver<Integer>::operator()(IdentifierNode<Integer> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op);

    TempVariable* t = FindTempVariable(op.name);
    if (t)
        return t->second;
    
    //find in user defined variables
    VariableNode<Integer>* v = FindVariable(op.name, op.subscript);
    if (v)
    {
        LogicalId _id = id;
        bool _exported_id = exported_id;
        id = v->id;
        exported_id = v->exported;
        Integer res = (*this)(v->expression);
        id = _id;
        exported_id = _exported_id;
        return res;
    }
    
    BuiltinVariable* var = FindBuiltinVariable(op.name);
    if (var)
    {
        try
        {
            IntegerVariable v = std::get<Variable>(*var);
            return (*v)();
        }
        catch (const std::bad_variant_access&)
        {
        }
    }
    
    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name + U"' not found", op.pos, op.name.length(), op.line);
}

template<>
Real Solver<Real>::operator()(IdentifierNode<Real> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op);

    TempVariable* t = FindTempVariable(op.name);
    if (t)
        return t->second;
    
    //find in the user defined variables
    VariableNode<Real>* v = FindVariable(op.name, op.subscript);
    if (v)
    {
        LogicalId _id = id;
        bool _exported_id = exported_id;
        id = v->id;
        exported_id = v->exported;
        Real res = (*this)(v->expression);
        id = _id;
        exported_id = _exported_id;
        return res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.name);
    if (var)
    {
        try
        {
            RealPrecisionVariable v = std::get<PrecisionVariable>(*var);
            return (*v)(precision);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.subscript.empty() || op.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            return Real(precision, *unit);
        }
    }

    auto* custom_unit = FindUnit(op.name, op.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        return custom_unit->value;
    }

    if (!cur_subscript.empty())
    {
        auto it = symbols->lists.find(cur_subscript);
        if (it != symbols->lists.end())
        {
            if (std::find(it->second.begin(), it->second.end(), op.name) != it->second.end())
                return Real();
        }
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name + U"' not found", op.pos, op.name.length(), op.line);
}

template<>
Rational Solver<Rational>::operator()(IdentifierNode<Rational> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op);

    TempVariable* t = FindTempVariable(op.name);
    if (t)
        return t->second;
    
    //find in user defined variables
    VariableNode<Rational>* v = FindVariable(op.name, op.subscript);
    if (v)
    {
        LogicalId _id = id;
        bool _exported_id = exported_id;
        id = v->id;
        exported_id = v->exported;
        Rational res = (*this)(v->expression);
        id = _id;
        exported_id = _exported_id;
        return res;
    }
    
    BuiltinVariable* var = FindBuiltinVariable(op.name);
    if (var)
    {
        try
        {
            RationalVariable v = std::get<Variable>(*var);
            return (*v)();
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.subscript.empty() || op.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            return Rational(*unit);
        }
    }

    auto* custom_unit = FindUnit(op.name, op.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        return custom_unit->value;
    }

    if (!cur_subscript.empty())
    {
        auto it = symbols->lists.find(cur_subscript);
        if (it != symbols->lists.end())
        {
            if (std::find(it->second.begin(), it->second.end(), op.name) != it->second.end())
                return Rational();
        }
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name + U"' not found", op.pos, op.name.length(), op.line);
}

template<>
Complex Solver<Complex>::operator()(IdentifierNode<Complex> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op);

    TempVariable* t = FindTempVariable(op.name);
    if (t)
        return t->second;
    
    //find in the user defined variables
    VariableNode<Complex>* v = FindVariable(op.name, op.subscript);
    if (v)
    {
        LogicalId _id = id;
        bool _exported_id = exported_id;
        id = v->id;
        exported_id = v->exported;
        Complex res = (*this)(v->expression);
        id = _id;
        exported_id = _exported_id;
        return res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.name);
    if (var)
    {
        try
        {
            ComplexPrecisionVariable v = std::get<ComplexPrecisionVariable>(*var);
            return (*v)(precision, default_angle_measure);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.name == im)
        return Complex(MathHelper::ToBitPrecision(precision), 0, 1);

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name + U"' not found", op.pos, op.name.length(), op.line);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(IdentifierNode<Array<Real>> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op);

    TempVariable* t = FindTempVariable(op.name);
    if (t)
        return t->second;
    
    //find in the user defined variables
    VariableNode<Array<Real>>* v = FindVariable(op.name, op.subscript);
    if (v)
    {
        LogicalId _id = id;
        bool _exported_id = exported_id;
        id = v->id;
        exported_id = v->exported;
        Array<Real> res = (*this)(v->expression);
        if (!op.subscript.empty() && v->name.subscript != op.subscript)
        {
            VariableNode<Array<Real>>* s = FindVariable(op.subscript, U"");
            if (s)
            {
                Array<Real> s_res = (*this)(s->expression);
                if (s_res.Size() == 1)
                {
                    int index = (int)(s_res.Get(0));
                    if (res.Size() > index && index >= 0)
                    {
                        Real r = res.Get(index);
                        res.Clear();
                        res.Add(r);
                    }
                    else
                        throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.name + U"' is over", op.pos, op.name.length(), op.line);
                }
                else
                    throw SyntaxException(op.id, IncorrectOperation, U"Index of '" + op.name + U"' is over", op.pos, op.name.length(), op.line);
            }
            else
            {
                TempVariable* t = FindTempVariable(op.subscript);
                if (t)
                {
                    Array<Real> s_res = t->second;
                    if (s_res.Size() == 1)
                    {
                        int index = (int)(s_res.Get(0));
                        if (res.Size() > index && index >= 0)
                        {
                            Real r = res.Get(index);
                            res.Clear();
                            res.Add(r);
                        }
                        else
                            throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.name + U"' is over", op.pos, op.name.length(), op.line);
                    }
                    else
                        throw SyntaxException(op.id, IncorrectOperation, U"Index of '" + op.name + U"' is over", op.pos, op.name.length(), op.line);
                }
                else
                {
                    try
                    {
                        int index = StringToInt(op.subscript);
                        if (res.Size() > index && index >= 0)
                        {
                            Real r = res.Get(index);
                            res.Clear();
                            res.Add(r);
                        }
                        else
                            throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.name + U"' is over", op.pos, op.name.length(), op.line);
                    }
                    catch (const std::exception& e)
                    {
                        throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.name + U"' is over", op.pos, op.name.length(), op.line);
                    }
                }
            }
        }
        id = _id;
        exported_id = _exported_id;
        return res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.name);
    if (var)
    {
        try
        {
            ArrayRealPrecisionVariable v = std::get<PrecisionVariable>(*var);
            return (*v)(precision);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.subscript.empty() || op.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            return Array<Real>(precision, *unit);
        }
    }

    auto* custom_unit = FindUnit(op.name, op.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        return custom_unit->value;
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name + U"' not found", op.pos, op.name.length(), op.line);
}

template<>
Integer Solver<Integer>::operator()(ImplicitStringMulNode<Integer> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
        return (*this)(op.left) * t->second;
    
    //find in user defined variables
    VariableNode<Integer>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Integer res = (*this)(v->expression);
        id = _id;
        return (*this)(op.left) * res;
    }
    
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            IntegerVariable v = std::get<Variable>(*var);
            return (*this)(op.left) * (*v)();
        }
        catch (const std::bad_variant_access&)
        {
        }
    }
    
    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Real Solver<Real>::operator()(ImplicitStringMulNode<Real> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
        return (*this)(op.left) * t->second;
    
    //find in the user defined variables
    VariableNode<Real>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Real res = (*this)(v->expression);
        id = _id;
        return (*this)(op.left) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            RealPrecisionVariable v = std::get<PrecisionVariable>(*var);
            return (*this)(op.left) * (*v)(precision);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.identifier.subscript.empty() || op.identifier.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.identifier.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            return (*this)(op.left) * Real(precision, *unit);
        }
    }

    auto* custom_unit = FindUnit(op.identifier.name, op.identifier.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        return (*this)(op.left) * custom_unit->value;
    }

    Real val;
    if (FindBuiltinIdentifier(op.identifier.name, val))
        return (*this)(op.left) * val;

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Rational Solver<Rational>::operator()(ImplicitStringMulNode<Rational> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
        return (*this)(op.left) * t->second;
    
    //find in user defined variables
    VariableNode<Rational>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Rational res = (*this)(v->expression);
        id = _id;
        return (*this)(op.left) * res;
    }
    
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            RationalVariable v = std::get<Variable>(*var);
            return (*this)(op.left) * (*v)();
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.identifier.subscript.empty() || op.identifier.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.identifier.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            return (*this)(op.left) * Rational(*unit);
        }
    }

    auto* custom_unit = FindUnit(op.identifier.name, op.identifier.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        return (*this)(op.left) * custom_unit->value;
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Complex Solver<Complex>::operator()(ImplicitStringMulNode<Complex> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
        return (*this)(op.left) * t->second;
    
    //find in the user defined variables
    VariableNode<Complex>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Complex res = (*this)(v->expression);
        id = _id;
        return (*this)(op.left) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            ComplexPrecisionVariable v = std::get<ComplexPrecisionVariable>(*var);
            return (*this)(op.left) * (*v)(precision, default_angle_measure);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.identifier.name == im)
        return (*this)(op.left) * Complex(MathHelper::ToBitPrecision(precision), 0, 1);

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(ImplicitStringMulNode<Array<Real>> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
        return (*this)(op.left) * t->second;
    
    //find in the user defined variables
    VariableNode<Array<Real>>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Array<Real> res = (*this)(v->expression);
        if (!op.identifier.subscript.empty() && v->name.subscript != op.identifier.subscript)
        {
            try
            {
                int index = StringToInt(op.identifier.subscript);
                if (res.Size() > index && index >= 0)
                {
                    Real r = res.Get(index);
                    res.Clear();
                    res.Add(r);
                }
                else
                    throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.identifier.name + U"' is over", op.pos, op.identifier.name.length(), op.line);
            }
            catch (const std::exception& e)
            {
                throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.identifier.name + U"' is over", op.pos, op.identifier.name.length(), op.line);
            }
        }
        id = _id;
        return (*this)(op.left) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            ArrayRealPrecisionVariable v = std::get<PrecisionVariable>(*var);
            return (*this)(op.left) * (*v)(precision);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.identifier.subscript.empty() || op.identifier.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.identifier.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            return (*this)(op.left) * Array<Real>(precision, *unit);
        }
    }

    auto* custom_unit = FindUnit(op.identifier.name, op.identifier.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        return (*this)(op.left) * custom_unit->value;
    }

    Array<Real> val;
    if (FindBuiltinIdentifier(op.identifier.name, val))
        return (*this)(op.left) * val;

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Integer Solver<Integer>::operator()(ImplicitDivMulNode<Integer> const& op) const
{
    throw SyntaxException(op.id, SyntaxError, op.pos, op.line);
}

template<>
Real Solver<Real>::operator()(ImplicitDivMulNode<Real> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
    {
        Real arg1 = (*this)(op.upper);
        Real arg2 = (*this)(op.lower);
        return (arg1 / arg2) * t->second;
    }
    
    //find in the user defined variables
    VariableNode<Real>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Real res = (*this)(v->expression);
        id = _id;
        Real arg1 = (*this)(op.upper);
        Real arg2 = (*this)(op.lower);
        return (arg1 / arg2) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            RealPrecisionVariable v = std::get<PrecisionVariable>(*var);
            Real arg1 = (*this)(op.upper);
            Real arg2 = (*this)(op.lower);
            return (arg1 / arg2) * (*v)(precision);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    auto* custom_unit = FindUnit(op.identifier.name, op.identifier.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        Real arg1 = (*this)(op.upper);
        Real arg2 = (*this)(op.lower);
        return (arg1 / arg2) * custom_unit->value;
    }

    if (op.identifier.subscript.empty() || op.identifier.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.identifier.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            Real arg1 = (*this)(op.upper);
            Real arg2 = (*this)(op.lower);
            return (arg1 / arg2) * Real(precision, *unit);
        }
    }

    Real val;
    if (FindBuiltinIdentifier(op.identifier.name, val))
    {
        Real arg1 = (*this)(op.upper);
        Real arg2 = (*this)(op.lower);
        return (arg1 / arg2) * val;
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Rational Solver<Rational>::operator()(ImplicitDivMulNode<Rational> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
    {
        Rational arg1 = (*this)(op.upper);
        Rational arg2 = (*this)(op.lower);
        return (arg1 / arg2) * t->second;
    }
    
    //find in the user defined variables
    VariableNode<Rational>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Rational res = (*this)(v->expression);
        id = _id;
        Rational arg1 = (*this)(op.upper);
        Rational arg2 = (*this)(op.lower);
        return (arg1 / arg2) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            RationalVariable v = std::get<Variable>(*var);
            Rational arg1 = (*this)(op.upper);
            Rational arg2 = (*this)(op.lower);
            return (arg1 / arg2) * (*v)();
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    auto* custom_unit = FindUnit(op.identifier.name, op.identifier.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        Rational arg1 = (*this)(op.upper);
        Rational arg2 = (*this)(op.lower);
        return (arg1 / arg2) * custom_unit->value;
    }

    if (op.identifier.subscript.empty() || op.identifier.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.identifier.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            Rational arg1 = (*this)(op.upper);
            Rational arg2 = (*this)(op.lower);
            return (arg1 / arg2) * Rational(*unit);
        }
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Complex Solver<Complex>::operator()(ImplicitDivMulNode<Complex> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
    {
        Complex arg1 = (*this)(op.upper);
        Complex arg2 = (*this)(op.lower);
        return (arg1 / arg2) * t->second;
    }
    
    //find in the user defined variables
    VariableNode<Complex>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        id = v->id;
        Complex res = (*this)(v->expression);
        id = _id;
        Complex arg1 = (*this)(op.upper);
        Complex arg2 = (*this)(op.lower);
        return (arg1 / arg2) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            ComplexPrecisionVariable v = std::get<ComplexPrecisionVariable>(*var);
            Complex arg1 = (*this)(op.upper);
            Complex arg2 = (*this)(op.lower);
            return (arg1 / arg2) * (*v)(precision, default_angle_measure);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    if (op.identifier.name == im)
        return Complex(MathHelper::ToBitPrecision(precision), 0, 1);

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(ImplicitDivMulNode<Array<Real>> const& op) const
{
    CheckBreak(parser_context);
    AddDependency(op.identifier);

    TempVariable* t = FindTempVariable(op.identifier.name);
    if (t)
    {
        Array<Real> arg1 = (*this)(op.upper);
        Array<Real> arg2 = (*this)(op.lower);
        return (arg1 / arg2) * t->second;
    }
    
    //find in the user defined variables
    VariableNode<Array<Real>>* v = FindVariable(op.identifier.name, op.identifier.subscript);
    if (v)
    {
        LogicalId _id = id;
        bool _exported_id = exported_id;
        id = v->id;
        exported_id = v->exported;
        Array<Real> res = (*this)(v->expression);
        if (!op.identifier.subscript.empty() && v->name.subscript != op.identifier.subscript)
        {
            try
            {
                int index = StringToInt(op.identifier.subscript);
                if (res.Size() > index && index >= 0)
                {
                    Real r = res.Get(index);
                    res.Clear();
                    res.Add(r);
                }
                else
                    throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.identifier.name + U"' is over", op.pos, op.identifier.name.length(), op.line);
            }
            catch (const std::exception& e)
            {
                throw SyntaxException(op.id, ArgumentIsOver, U"Index of '" + op.identifier.name + U"' is over", op.pos, op.identifier.name.length(), op.line);
            }
        }
        id = _id;
        exported_id = _exported_id;
        Array<Real> arg1 = (*this)(op.upper);
        Array<Real> arg2 = (*this)(op.lower);
        return (arg1 / arg2) * res;
    }
    
    //find in the build-in variables
    BuiltinVariable* var = FindBuiltinVariable(op.identifier.name);
    if (var)
    {
        try
        {
            ArrayRealPrecisionVariable v = std::get<PrecisionVariable>(*var);
            Array<Real> arg1 = (*this)(op.upper);
            Array<Real> arg2 = (*this)(op.lower);
            return (arg1 / arg2) * (*v)(precision);
        }
        catch (const std::bad_variant_access&)
        {
        }
    }

    auto* custom_unit = FindUnit(op.identifier.name, op.identifier.subscript);
    if (custom_unit)
    {
        symbols->last_unit_system = custom_unit->system;
        Array<Real> arg1 = (*this)(op.upper);
        Array<Real> arg2 = (*this)(op.lower);
        return (arg1 / arg2) * custom_unit->value;
    }

    if (op.identifier.subscript.empty() || op.identifier.subscript == U"SI")
    {
        Unit* unit = FindBuiltinUnit(op.identifier.name);
        if (unit)
        {
            symbols->last_unit_system = U"SI";
            Array<Real> arg1 = (*this)(op.upper);
            Array<Real> arg2 = (*this)(op.lower);
            return (arg1 / arg2) * Array<Real>(precision, *unit);
        }
    }

    Array<Real> val;
    if (FindBuiltinIdentifier(op.identifier.name, val))
    {
        Array<Real> arg1 = (*this)(op.upper);
        Array<Real> arg2 = (*this)(op.lower);
        return (arg1 / arg2) * val;
    }

    //there is no such an identifier
    throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
}

template<>
Array<Real> Solver<Array<Real>>::operator()(ArrayNode<Array<Real>> const& op) const
{
    CheckBreak(parser_context);
    Array<Real> res;
    for (const auto& v : op.array)
    {
        CheckBreak(parser_context);
        res.Add((*this)(v));
    }
    return res;
}

template<>
Integer Solver<Integer>::operator()(ScriptNode<Integer> const& script, LogicalId _id, AngleMeasure _default_angle_measure,
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const
{
    if (script.list.empty())
        throw SyntaxException(_id, ParserExceptionCode::ExpressionExpected, -1, 0, -1);
    
    id = _id;
    dependencies = _dependencies;
    
    Integer res;

    //calculate all the script nodes
    BOOST_FOREACH(typename ScriptNode<Integer>::Operand const& op, script.list)
    {
        CheckBreak(parser_context);
        res = boost::apply_visitor(*this, op);
    }

    return res;
}

template<>
Rational Solver<Rational>::operator()(ScriptNode<Rational> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const
{
    if (script.list.empty())
        throw SyntaxException(_id, ParserExceptionCode::ExpressionExpected, -1, 0, -1);
    
    id = _id;
    dependencies = _dependencies;
   
    Rational res;

    //calculate all the script nodes
    BOOST_FOREACH(typename ScriptNode<Rational>::Operand const& op, script.list)
    {
        CheckBreak(parser_context);
        res = boost::apply_visitor(*this, op);
    }

    return res;
}

template<>
Complex Solver<Complex>::operator()(ScriptNode<Complex> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const
{
    if (script.list.empty())
        throw SyntaxException(_id, ParserExceptionCode::ExpressionExpected, -1, 0, -1);
    
    id = _id;
    dependencies = _dependencies;
    default_angle_measure = _default_angle_measure;
    result_angle_measure = _result_angle_measure;
    if (_precision != -1)
        precision = _precision;

    Complex res;
    //calculate all the script nodes
    BOOST_FOREACH(typename ScriptNode<Complex>::Operand const& op, script.list)
    {
        CheckBreak(parser_context);
        res = boost::apply_visitor(*this, op);
    }
    
    if (res.GetAngleMeasure() != AngleMeasure::None)
    {
        switch (result_angle_measure)
        {
        case AngleMeasure::Radian:
            return res.ToRadian();
        case AngleMeasure::Degree:
            return res.ToDegree();
        case AngleMeasure::Grad:
            return res.ToGrad();
        case AngleMeasure::None:
            break;
        }
    }
    return res;
}

template<>
Array<Real> Solver<Array<Real>>::operator()(ScriptNode<Array<Real>> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const
{
    if (script.list.empty())
        throw SyntaxException(_id, ParserExceptionCode::ExpressionExpected, -1, 0, -1);
    
    id = _id;
    dependencies = _dependencies;
    default_angle_measure = _default_angle_measure;
    result_angle_measure = _result_angle_measure;
    if (_precision != -1)
        precision = _precision;

    Array<Real> res;
    //calculate all the script nodes
    BOOST_FOREACH(typename ScriptNode<Array<Real>>::Operand const& op, script.list)
    {
        CheckBreak(parser_context);
        res = boost::apply_visitor(*this, op);
    }
    
    if (res.GetAngleMeasure() != AngleMeasure::None)
    {
        switch (result_angle_measure)
        {
        case AngleMeasure::Radian:
            return res.ToRadian();
        case AngleMeasure::Degree:
            return res.ToDegree();
        case AngleMeasure::Grad:
            return res.ToGrad();
        case AngleMeasure::None:
            break;
        }
    }
    return res;
}

template<>
Real Solver<Real>::GetSuitableUnit(const LogicalId _id, const Real& val, const std::u32string& system, const bool buildin) const
{
    return GetSuitableUnitImpl(_id, val, system, buildin);
}

template<>
Rational Solver<Rational>::GetSuitableUnit(const LogicalId _id, const Rational& val, const std::u32string& system, const bool buildin) const
{
    return GetSuitableUnitImpl(_id, val, system, buildin);
}

template<>
Integer Solver<Integer>::GetSuitableUnit(const LogicalId _id, const Integer& val, const std::u32string& system, const bool buildin) const
{
    return val;
}

template<>
Complex Solver<Complex>::GetSuitableUnit(const LogicalId _id, const Complex& val, const std::u32string& system, const bool buildin) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
Symbolic<Real> Solver<Symbolic<Real>>::GetSuitableUnit(const LogicalId _id, const Symbolic<Real>& val, const std::u32string& system, const bool buildin) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
Real Solver<Real>::CastToUnit(const LogicalId id, const Real& val, const Unit& unit) const
{
    return CastToUnitImpl(id, val, unit);
}

template<>
Rational Solver<Rational>::CastToUnit(const LogicalId id, const Rational& val, const Unit& unit) const
{
    return CastToUnitImpl(id, val, unit);
}

template<>
Integer Solver<Integer>::CastToUnit(const LogicalId id, const Integer& val, const Unit& unit) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
Complex Solver<Complex>::CastToUnit(const LogicalId id, const Complex& val, const Unit& unit) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
Symbolic<Real> Solver<Symbolic<Real>>::CastToUnit(const LogicalId id, const Symbolic<Real>& val, const Unit& unit) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
void Solver<Symbolic<Rational>>::FillBuiltinOperations()
{
    if (!symbols->builtin_operations.empty())
        return;
    symbols->builtin_operations.push_back(U"plus");
    symbols->builtin_operations.push_back(U"minus");
    symbols->builtin_operations.push_back(U"mul");
    symbols->builtin_operations.push_back(U"div");
    symbols->builtin_operations.push_back(U"power");
    symbols->builtin_operations.push_back(U"sqrt");
    symbols->builtin_operations.push_back(U"root");
    symbols->builtin_operations.push_back(U"sub");
    symbols->builtin_operations.push_back(U"sum");
    symbols->builtin_operations.push_back(U"prod");
}

template<>
Symbolic<Rational> Solver<Symbolic<Rational>>::GetSuitableUnit(const LogicalId _id, const Symbolic<Rational>& val, const std::u32string& system, const bool buildin) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
Symbolic<Rational> Solver<Symbolic<Rational>>::CastToUnit(const LogicalId id, const Symbolic<Rational>& val, const Unit& unit) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
void Solver<Symbolic<Complex>>::FillBuiltinOperations()
{
    if (!symbols->builtin_operations.empty())
        return;
    symbols->builtin_operations.push_back(U"plus");
    symbols->builtin_operations.push_back(U"minus");
    symbols->builtin_operations.push_back(U"mul");
    symbols->builtin_operations.push_back(U"div");
    symbols->builtin_operations.push_back(U"power");
    symbols->builtin_operations.push_back(U"sqrt");
    symbols->builtin_operations.push_back(U"root");
    symbols->builtin_operations.push_back(U"sub");
    symbols->builtin_operations.push_back(U"sum");
    symbols->builtin_operations.push_back(U"prod");
}

template<>
Symbolic<Complex> Solver<Symbolic<Complex>>::GetSuitableUnit(const LogicalId _id, const Symbolic<Complex>& val, const std::u32string& system, const bool buildin) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

template<>
Symbolic<Complex> Solver<Symbolic<Complex>>::CastToUnit(const LogicalId id, const Symbolic<Complex>& val, const Unit& unit) const
{
    throw MathException(id, ParserExceptionCode::CannotCastToUnit);
}

#ifdef _MSC_VER
//force the symbols into yutovo-calculator.lib in Windows Release build
template Real Solver<Real>::operator()(UnitNode<Real> const&) const;
template Real Solver<Real>::operator()(UnaryOperationNode<Real> const&) const;
template Real Solver<Real>::operator()(OperationNode<Real> const&) const;
template Real Solver<Real>::operator()(PostfixOperationNode<Real> const&) const;
template Real Solver<Real>::operator()(FunctionCallNode<Real> const&) const;
template Real Solver<Real>::operator()(FunctionCallStringNode<Real> const&) const;
template Real Solver<Real>::operator()(NoFencesFunctionCallNode<Real> const&) const;
template Real Solver<Real>::operator()(IdentifierNode<Real> const&) const;
template Real Solver<Real>::operator()(ImplicitStringMulNode<Real> const&) const;
template Real Solver<Real>::operator()(ImplicitDivMulNode<Real> const&) const;
template Real Solver<Real>::operator()(ScriptNode<Real> const&, LogicalId, AngleMeasure, AngleMeasure, int, Dependencies*) const;
template Array<Real> Solver<Array<Real>>::operator()(ScriptNode<Array<Real>> const&, LogicalId, AngleMeasure, AngleMeasure, int, Dependencies*) const;
template Rational Solver<Rational>::operator()(ScriptNode<Rational> const&, LogicalId, AngleMeasure, AngleMeasure, int, Dependencies*) const;
template Integer Solver<Integer>::operator()(ScriptNode<Integer> const&, LogicalId, AngleMeasure, AngleMeasure, int, Dependencies*) const;
template Complex Solver<Complex>::operator()(ScriptNode<Complex> const&, LogicalId, AngleMeasure, AngleMeasure, int, Dependencies*) const;
template void Solver<Symbolic<Rational>>::FillBuiltinOperations();
template void Solver<Symbolic<Complex>>::FillBuiltinOperations();
#endif

}
