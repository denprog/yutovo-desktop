/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef SOLVER_H
#define SOLVER_H

#include "ast.h"
#include "script.h"
#include "utils.h"
#include "export.h"
#include "giac_utils.h"
#include "expression.h"
#include <chrono>
#include <functional>
#include <variant>
#include <set>
#include <vector>
#include <type_traits>

namespace yutovo_calculator
{

using namespace std::chrono;

typedef Integer (*IntegerUnaryFunc)(const Integer &num);
typedef Integer (*IntegerBinaryFunc)(const Integer& num1, const Integer& num2);
typedef Integer (*IntegerStringFunc)(const std::u32string& str);
typedef Integer (*IntegerVariable)();

typedef Real (*RealUnaryFunc)(const Real& num);
typedef Real (*RealBinaryFunc)(const Real& num1, const Real& num2);
typedef Real (*RealTrigonaryFunc)(const Real& num1, const Real& num2, const Real& num3);
typedef Real (*RealQuaternaryFunc)(const Real& num1, const Real& num2, const Real& num3, const Real& num4);
typedef Real (*RealTrigonometricFunc)(const Real& num);
typedef Real (*RealPrecisionVariable)(const int precision);

typedef Complex (*ComplexUnaryFunc)(const Complex& num, int& res_pos);
typedef Complex (*ComplexBinaryFunc)(const Complex& num1, const Complex& num2, int& res_pos);
typedef Complex (*ComplexTernaryFunc)(const Complex& num1, const Complex& num2, const Complex& num3, int& res_pos);
typedef Complex (*ComplexQuaternaryFunc)(const Complex& num1, const Complex& num2, const Complex& num3, const Complex& num4);
typedef Complex (*ComplexPrecisionVariable)(const int precision, AngleMeasure angle_measure);
typedef Complex (*ComplexFunc)(const Complex& num);

typedef Rational (*RationalUnaryFunc)(const Rational &num);
typedef Rational (*RationalBinaryFunc)(const Rational& num1, const Rational& num2);
typedef Rational (*RationalVariable)();

typedef Array<Real> (*ArrayRealUnaryFunc)(const Array<Real>& num);
typedef Array<Real> (*ArrayRealBinaryFunc)(const Array<Real>& num1, const Array<Real>& num2);
typedef Array<Real> (*ArrayRealTernaryFunc)(const Array<Real>& num1, const Array<Real>& num2, const Array<Real>& num3);
typedef Array<Real> (*ArrayRealQuaternaryFunc)(const Array<Real>& num1, const Array<Real>& num2, const Array<Real>& num3, const Array<Real>& num4);
typedef Array<Real> (*ArrayRealTrigonometricFunc)(const Array<Real>& num);
typedef Array<Real> (*ArrayRealPrecisionVariable)(const int precision);

typedef Symbolic<Real> (*SymbolicUnaryFunc)(const Symbolic<Real>& num);
typedef Symbolic<Real> (*SymbolicBinaryFunc)(const Symbolic<Real>& num1, const Symbolic<Real>& num2);
typedef Symbolic<Real> (*SymbolicTernaryFunc)(const Symbolic<Real>& num1, const Symbolic<Real>& num2, const Symbolic<Real>& num3);
typedef Symbolic<Real> (*SymbolicQuaternaryFunc)(const Symbolic<Real>& num1, const Symbolic<Real>& num2, const Symbolic<Real>& num3, const Symbolic<Real>& num4);

typedef Symbolic<Rational> (*SymbolicRationalUnaryFunc)(const Symbolic<Rational>& num);
typedef Symbolic<Rational> (*SymbolicRationalBinaryFunc)(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2);
typedef Symbolic<Rational> (*SymbolicRationalTernaryFunc)(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2, const Symbolic<Rational>& num3);
typedef Symbolic<Rational> (*SymbolicRationalQuaternaryFunc)(const Symbolic<Rational>& num1, const Symbolic<Rational>& num2, const Symbolic<Rational>& num3, const Symbolic<Rational>& num4);

typedef Symbolic<Complex> (*SymbolicComplexUnaryFunc)(const Symbolic<Complex>& num);
typedef Symbolic<Complex> (*SymbolicComplexBinaryFunc)(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2);
typedef Symbolic<Complex> (*SymbolicComplexTernaryFunc)(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2, const Symbolic<Complex>& num3);
typedef Symbolic<Complex> (*SymbolicComplexQuaternaryFunc)(const Symbolic<Complex>& num1, const Symbolic<Complex>& num2, const Symbolic<Complex>& num3, const Symbolic<Complex>& num4);

typedef std::vector<std::u32string> Dependencies;

template<typename Number>
struct SolverSymbols
{
    typedef std::pair<std::u32string, Number> TempVariable;

    //build-in functions' typedefs
    typedef Number (*UnaryFunction)(const Number& num);
    typedef Number (*BinaryFunction)(const Number& num1, const Number& num2);
    typedef Number (*TernaryFunction)(const Number& num1, const Number& num2, const Number& num3);
    typedef Number (*QuaternaryFunction)(const Number& num1, const Number& num2, const Number& num3, const Number& num4);
    typedef Number (*StringFunction)(const std::u32string& str);
    typedef Number (*TrigonometricFunction)(const Number& num);
    typedef Number (*ComplexUnaryFunction)(const Number& num, int& res_pos);
    typedef Number (*ComplexBinaryFunction)(const Number& num1, const Number& num2, int& res_pos);

    typedef std::variant<UnaryFunction, BinaryFunction, TernaryFunction, QuaternaryFunction, StringFunction, ComplexUnaryFunction, ComplexBinaryFunction> BuiltinFunction;
    typedef std::variant<TrigonometricFunction, ComplexUnaryFunction> BuiltinTrigonometricFunction;
    
    //build-in variables' typedefs
    typedef Number (*Variable)();
    typedef Number (*PrecisionVariable)(const int precision);
    typedef std::variant<Variable, PrecisionVariable, ComplexPrecisionVariable> BuiltinVariable;

    mutable std::deque<TempVariable> temp_variables;
    mutable std::deque<VariableNode<Number>> variables; //user variables
    mutable std::vector<FunctionNode<Number>> functions; //user functions
    mutable std::map<std::u32string, std::vector<std::u32string>> lists; //user lists
    mutable std::map<std::u32string, std::u32string> strings; //user strings

    std::map<std::u32string, BuiltinFunction> buildin_functions;
    std::map<std::u32string, BuiltinTrigonometricFunction> trigonometric_functions;
    std::map<std::u32string, BuiltinVariable> buildin_variables;
    std::vector<Unit> buildin_units;
    std::vector<CustomUnit<Number>> units;
    std::map<std::u32string, Number> builtin_identifiers;

    std::vector<std::u32string> builtin_operations;

    std::u32string last_unit_system;
    bool buildin_elements = false;
};

template<typename Number>
struct Solver : public boost::static_visitor<Number>
{
    typedef typename std::list<ExpressionNode<Number> >::const_iterator ExpressionNodesIter;		
    typedef typename std::list<IdentifierNode<Number> >::const_iterator IdentifierNodesIter;
    
    typedef typename SolverSymbols<Number>::TempVariable TempVariable;
    typedef typename SolverSymbols<Number>::UnaryFunction UnaryFunction;
    typedef typename SolverSymbols<Number>::BinaryFunction BinaryFunction;
    typedef typename SolverSymbols<Number>::TernaryFunction TernaryFunction;
    typedef typename SolverSymbols<Number>::QuaternaryFunction QuaternaryFunction;
    typedef typename SolverSymbols<Number>::StringFunction StringFunction;
    typedef typename SolverSymbols<Number>::TrigonometricFunction TrigonometricFunction;
    typedef typename SolverSymbols<Number>::BuiltinFunction BuiltinFunction;
    typedef typename SolverSymbols<Number>::BuiltinTrigonometricFunction BuiltinTrigonometricFunction;
    typedef typename SolverSymbols<Number>::ComplexUnaryFunction ComplexUnaryFunction;
    typedef typename SolverSymbols<Number>::ComplexBinaryFunction ComplexBinaryFunction;
    
    typedef typename SolverSymbols<Number>::PrecisionVariable PrecisionVariable;
    typedef typename SolverSymbols<Number>::BuiltinVariable BuiltinVariable;
    typedef typename SolverSymbols<Number>::Variable Variable;
    
    std::shared_ptr<SolverSymbols<Number>> symbols;
    
    Solver(int _precision, AngleMeasure _default_angle_measure, std::u32string _im = U"i", Number _left_value = Number(), 
        std::shared_ptr<SolverSymbols<Number>> _symbols = nullptr) :
        precision(_precision),
        default_angle_measure(_default_angle_measure),
        left_value(_left_value),
        symbols(_symbols),
        im(_im)
    {
        if (!symbols)
            symbols.reset(new SolverSymbols<Number>());
        FillBuiltinOperations();
    }

    Solver(int _precision, AngleMeasure _default_angle_measure, AngleMeasure _result_angle_measure, int _default_notation, const std::u32string& _im, 
        Number _left_value = Number(), std::shared_ptr<SolverSymbols<Number>> _symbols = nullptr) :
        precision(_precision),
        default_angle_measure(_default_angle_measure),
        result_angle_measure(_result_angle_measure),
        default_notation(_default_notation),
        left_value(_left_value),
        symbols(_symbols),
        im(_im)
    {
        if (!symbols)
            symbols.reset(new SolverSymbols<Number>());
        FillBuiltinOperations();
    }
    
    Number operator()(Number n) const
    {
        n.SetPrecision(precision);
        return n;
    }

    Number operator()(NumberNode<Number> const& op) const
    {
        if constexpr (std::is_same_v<Number, Integer>)
        {
            return Integer(op.number, default_notation);
        }
        else if constexpr (std::is_same_v<Number, Rational>)
        {
            NumberNode<Rational> _op = op;
            auto p = _op.number.find(U'.');
            if (p != std::string::npos)
            {
                std::u32string d = U"1";
                for (int i = 0; i < _op.number.length() - p - 1; ++i)
                    d += U"0";
                return Rational(_op.number.substr(0, p)) + Rational(_op.number.substr(p + 1, _op.number.length() - p - 1)) / Rational(d);
            }
            return Rational(_op.number);
        }
        else if constexpr (std::is_same_v<Number, Complex> || std::is_same_v<Number, Real> || std::is_same_v<Number, Array<Real>>)
        {
            NumberNode<Number> _op = op;
            return Number(op.number);
        }
        else if constexpr (std::is_same_v<Number, Symbolic<Rational>>)
        {
            NumberNode<Number> _op = op;
            if (_op.number.find(U'.') != std::u32string::npos)
            {
                Rational r;
                r = _op.number;
                return Number(precision, r.ToString());
            }
            return Number(precision, _op.number);
        }
        else
        {
            NumberNode<Number> _op = op;
            return Number(precision, _op.number);
        }
    }

    Number operator()(ExpressionNode<Number> const& expr) const
    {
        //calculate all the expression's nodes
        Number res = boost::apply_visitor(*this, expr.first);
        BOOST_FOREACH(typename OperationNode<Number>::Operand const& op, expr.rest)
        {
            CheckBreak(parser_context);
            
            Solver<Number> solver(precision, default_angle_measure, result_angle_measure, default_notation, im, res, symbols);
            solver.parser_context = parser_context;
            solver.id = id;
            solver.exported_id = exported_id;
            solver.cast_units = cast_units;
            solver.max_cast_unit_size = max_cast_unit_size;
            solver.cur_subscript = cur_subscript;
            solver.dependencies = dependencies;
            res = boost::apply_visitor(solver, op);
        }
        if (parser_context)
            parser_context->no_result = false;
        return res;
    }
    
    Number operator()(DefinitionNode<Number> const& op) const
    {
        //pass the definition to the special functor
        boost::apply_visitor(*this, op.definition);
        if (parser_context)
            parser_context->no_result = true;
        return Number();
    }

    Number operator()(GraphNode<Number> const& op) const
    {
        return boost::apply_visitor(*this, op.graph);
    }

    Number operator()(LineGraphNode<Number> const& op) const
    {
        throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
    }

    Number operator()(BarGraphNode<Number> const& op) const
    {
        return Number();
    }

    Number operator()(VariableNode<Number> const& op) const
    {
        //store the variable
        AddVariable(op);
        return Number();
    }

    Number operator()(UnitNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
    }

    Number operator()(ListNode<Number> const& op) const
    {
        AddList(op);
        return Number();
    }

    Number operator()(StringNode<Number> const& op) const
    {
        AddString(op);
        return Number();
    }

    Number operator()(FunctionNode<Number> const& op) const
    {
        //store the function
        AddFunction(op);
        return Number();
    }

    Number operator()(UnaryOperationNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            Number arg = boost::apply_visitor(*this, op.operand);
            switch (op.op)
            {
            case U'+':
                return +arg;
            case U'-':
                return -arg;
            default:
                throw SyntaxException(op.id, IncorrectOperation, op.pos, 1, op.line);
            }
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(OperationNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            Number right = boost::apply_visitor(*this, op.operand);
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
            case U'&':
            case U'|':
                throw SyntaxException(op.id, IncorrectOperation, op.pos, 1, op.line);
            default:
                throw SyntaxException(op.id, IncorrectOperation, op.pos, 1, op.line);
            }
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(PostfixOperationNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            throw SyntaxException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(MixedDivivsionNode<Number> const& op) const
    {
        return (*this)(op.left) + (*this)(op.numerator) / (*this)(op.denominator);
    }

    Number operator()(FunctionCallNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            AddDependency(op.name);
            Number res;
            try
            {
                FunctionNode<Number>* user_func = FindFunction(op);
                if (user_func)
                {
                    IdentifierNodesIter funcIter = user_func->arguments.begin();
                    for (ExpressionNodesIter iter = op.arguments.begin(); iter != op.arguments.end(); ++iter, ++funcIter)
                    {
                        CheckBreak(parser_context);
                        Number arg = (*this)(*iter);
                        PushTempVariable(funcIter->name, arg);
                    }
                    bool _exported_id = exported_id;
                    exported_id = user_func->exported;
                    try
                    {
                        res = (*this)(user_func->return_expression);
                    }
                    catch (...)
                    {
                        exported_id = _exported_id;
                        PopTempVariables(op.arguments.size());
                        throw;
                    }
                    exported_id = _exported_id;
                    PopTempVariables(op.arguments.size());
                    return res;
                }
                BuiltinFunction* func = FindBuiltinFunction(op.name.name);
                if (func)
                {
                    try
                    {
                        auto u = std::get<typename SolverSymbols<Number>::UnaryFunction>(*func);
                        if (op.arguments.size() != 1)
                            throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                        ExpressionNodesIter iter = op.arguments.begin();
                        Number arg = (*this)(*iter);
                        return (*u)(arg);
                    }
                    catch (const std::bad_variant_access&)
                    {
                    }
                    try
                    {
                        auto b = std::get<typename SolverSymbols<Number>::BinaryFunction>(*func);
                        if (op.arguments.size() != 2)
                            throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                        ExpressionNodesIter iter = op.arguments.begin();
                        Number arg1 = (*this)(*iter++);
                        Number arg2 = (*this)(*iter);
                        return (*b)(arg1, arg2);
                    }
                    catch (const std::bad_variant_access&)
                    {
                    }
                    try
                    {
                        auto t = std::get<typename SolverSymbols<Number>::TernaryFunction>(*func);
                        if (op.arguments.size() != 3)
                            throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos,
                                CallSize(op), op.line);
                        ExpressionNodesIter iter = op.arguments.begin();
                        Number arg1 = (*this)(*iter++);
                        Number arg2 = (*this)(*iter++);
                        Number arg3 = (*this)(*iter);
                        return (*t)(arg1, arg2, arg3);
                    }
                    catch (const std::bad_variant_access&)
                    {
                    }
                    try
                    {
                        auto q = std::get<typename SolverSymbols<Number>::QuaternaryFunction>(*func);
                        if (op.arguments.size() != 4)
                            throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos,
                                CallSize(op), op.line);
                        ExpressionNodesIter iter = op.arguments.begin();
                        Number arg1 = (*this)(*iter++);
                        Number arg2 = (*this)(*iter++);
                        Number arg3 = (*this)(*iter++);
                        Number arg4 = (*this)(*iter);
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
            catch (const ParserException&)
            {
                throw;
            }
            catch (...)
            {
                if constexpr (is_symbolic_v<Number> && (std::is_same_v<Number, Symbolic<Real>> || std::is_same_v<Number, Symbolic<Complex>>))
                    return Number(precision, std::u32string(U"nan"));
                throw MathException(op.id, IncorrectOperation, op.pos, op.line);
            }
            throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name.name + U"' not found", op.pos, op.name.name.length(), op.line);
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(FunctionCallStringNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            AddDependency(op.name);
            if (op.name.name == U"subs")
            {
                BuiltinFunction* func = FindBuiltinFunction(op.name.name);
                if (func)
                {
                    Number var(precision, op.argument);
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Incorrect subs arguments", op.pos, CallSize(op), op.line);
                }
            }
            throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name.name + U"' not found", op.pos, op.name.name.length(), op.line);
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(DefiniteIntegralNode<Number> const& op) const
    {
        if constexpr (std::is_same_v<Number, Real> || std::is_same_v<Number, Complex> ||
            std::is_same_v<Number, Rational> || std::is_same_v<Number, Array<Real>>)
        {
            std::lock_guard<std::mutex> lock(giac_evaluation_mutex);
            try
            {
                Number lower = (*this)(op.lower);
                Number upper = (*this)(op.upper);

                if constexpr (std::is_same_v<Number, Array<Real>>)
                {
                    if (lower.Size() != 1 || upper.Size() != 1)
                        throw MathException(op.id, IncorrectOperation, op.pos, op.line);
                }

                GiacMpfrStateGuard mpfr_guard;
                GiacOutputGuard output_guard;

                giac::context ctx;
                if constexpr (!std::is_same_v<Number, Rational>)
                    giac::decimal_digits(std::max(1, precision + 1), &ctx);

                auto limit_string = [](const Number& value) -> std::u32string {
                    if constexpr (std::is_same_v<Number, Array<Real>>)
                    {
                        if (value[0].IsInfinity())
                            return value[0].GetSign() ? U"-inf" : U"+inf";
                        return value[0].ToString();
                    }
                    else if constexpr (std::is_same_v<Number, Real>)
                    {
                        if (value.IsInfinity())
                            return value.GetSign() ? U"-inf" : U"+inf";
                        return value.ToString();
                    }
                    else if constexpr (std::is_same_v<Number, Complex>)
                    {
                        if (value.IsInfinity())
                            return value.GetRe().GetSign() ? U"-inf" : U"+inf";
                        return value.ToString();
                    }
                    else
                    {
                        return value.ToString();
                    }
                };

                std::u32string lower_str = limit_string(lower);
                std::u32string upper_str = limit_string(upper);

                giac::gen lower_gen = ParseGen(ToBasicString(lower_str), &ctx);
                giac::gen upper_gen = ParseGen(ToBasicString(upper_str), &ctx);
                giac::gen expr_gen = ParseGen(ToBasicString(op.expression), &ctx);
                giac::gen var_gen = ParseGen(ToBasicString(op.variable), &ctx);

                if (var_gen.type != giac::_IDNT)
                    throw MathException(IncorrectOperation);

                giac::vecteur args;
                args.push_back(expr_gen);
                args.push_back(var_gen);
                args.push_back(lower_gen);
                args.push_back(upper_gen);

                if constexpr (std::is_same_v<Number, Real> || std::is_same_v<Number, Complex>)
                {
                    if (HasUserFunctionCall(op.expression))
                        return NumericalDefiniteIntegral(op.expression, op.variable, lower, upper);
                }

                giac::gen res;
                try
                {
                    res = giac::_integrate(args, &ctx);
                    if (res.type == giac::_SYMB || res == giac::undef)
                        res = giac::_romberg(args, &ctx);
                    if constexpr (std::is_same_v<Number, Rational>)
                        res = giac::normal(res, &ctx);
                    else
                        res = giac::evalf(res, 1, &ctx);
                }
                catch (...)
                {
                    if constexpr (std::is_same_v<Number, Real> || std::is_same_v<Number, Complex>)
                        return NumericalDefiniteIntegral(op.expression, op.variable, lower, upper);
                    throw MathException(IncorrectOperation);
                }

                return FromGiac<Number>(res, precision);
            }
            catch (const MathException& e)
            {
                throw MathException(op.id, e.ex_id, op.pos, op.line);
            }
            catch (...)
            {
                throw;
            }
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(DerivativeAtPointNode<Number> const& op) const
    {
        if constexpr (is_symbolic_v<Number>)
        {
            try
            {
                if (op.variables.empty())
                    throw ParserException({}, ParserExceptionCode::IncorrectOperation);

                const std::u32string& expression_str = boost::get<std::u32string>(op.function);
                std::u32string expr = expression_str;
                Expression<Number> expression(op.id, expr, const_cast<Solver<Number>*>(this));
                ExpressionNode<Number> function_expr;
                std::u32string::iterator iter = expr.begin();
                std::u32string::iterator end = expr.end();
                unicode::space_type space;
                if (!phrase_parse(iter, end, expression, space, function_expr))
                    throw MathException(IncorrectOperation);
                Number function = (*this)(function_expr);

                auto var_iter = op.variables.begin();
                Number var(precision, var_iter->name);
                Number result = derivative(function, var);
                for (const auto& variable : op.variables)
                {
                    Number value = (*this)(variable.value);
                    Number variable_symbol(precision, variable.name);
                    result = subs(result, variable_symbol, value);
                }
                return result;
            }
            catch (const MathException& e)
            {
                throw MathException(op.id, e.ex_id, op.pos, op.line);
            }
            catch (const ParserException&)
            {
                throw MathException(op.id, ParserExceptionCode::IncorrectOperation, op.pos, op.line);
            }
            catch (const std::bad_variant_access&)
            {
                throw MathException(op.id, ParserExceptionCode::IncorrectOperation, op.pos, op.line);
            }
            catch (...)
            {
                if constexpr (std::is_same_v<Number, Symbolic<Real>> || std::is_same_v<Number, Symbolic<Complex>>)
                    return Number(precision, std::u32string(U"nan"));
                throw MathException(op.id, ParserExceptionCode::IncorrectOperation, op.pos, op.line);
            }
        }
        else if constexpr (std::is_same_v<Number, Real> || std::is_same_v<Number, Complex> || std::is_same_v<Number, Rational>)
        {
            try
            {
                if (op.variables.empty())
                    throw ParserException({}, ParserExceptionCode::IncorrectOperation);
                const std::u32string& expression_str = boost::get<std::u32string>(op.function);
                try
                {
                    return EvaluateDerivativeAtPoint(expression_str, op.variables);
                }
                catch (...)
                {
                    return NumericalDerivativeAtPoint(expression_str, op.variables);
                }
            }
            catch (const MathException& e)
            {
                throw MathException(op.id, e.ex_id, op.pos, op.line);
            }
            catch (const ParserException&)
            {
                throw;
            }
            catch (const std::bad_variant_access&)
            {
                throw MathException(op.id, ParserExceptionCode::IncorrectOperation, op.pos, op.line);
            }
            catch (...)
            {
                throw MathException(op.id, ParserExceptionCode::IncorrectOperation, op.pos, op.line);
            }
        }
        else
        {
            throw MathException(op.id, ParserExceptionCode::IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number StepForNumericalDerivative(const Number& value) const
    {
        if constexpr (std::is_same_v<Number, Real>)
            return Number(precision, "1e-7");
        else if constexpr (std::is_same_v<Number, Complex>)
            return Number(Real(precision, "1e-7"), Real(precision, "0"));
        else
            return Number(U"1/1000000");
    }

    Number EvaluateWithTempVariable(const std::u32string& expression_str, const std::u32string& variable, const Number& value) const
    {
        Solver<Number> sub_solver(precision, default_angle_measure, result_angle_measure, default_notation, im, Number(), symbols);
        sub_solver.parser_context = parser_context;
        sub_solver.id = id;
        sub_solver.exported_id = exported_id;
        sub_solver.res_pos = res_pos;
        sub_solver.cast_units = cast_units;
        sub_solver.max_cast_unit_size = max_cast_unit_size;
        sub_solver.cur_subscript = cur_subscript;
        sub_solver.dependencies = dependencies;
        Number value_copy = value;
        sub_solver.PushTempVariable(variable, value_copy);
        Number res;
        try
        {
            std::u32string expr = expression_str;
            Expression<Number> expression(id, expr, &sub_solver);
            ExpressionNode<Number> node;
            std::u32string::iterator iter = expr.begin();
            std::u32string::iterator end = expr.end();
            unicode::space_type space;
            if (!phrase_parse(iter, end, expression, space, node))
                throw MathException(IncorrectOperation);
            res = sub_solver(node);
        }
        catch (...)
        {
            sub_solver.PopTempVariables(1);
            throw;
        }
        sub_solver.PopTempVariables(1);
        return res;
    }

    Number NumericalDerivativeAtPoint(const std::u32string& expression_str, const std::u32string& variable, const Number& value) const
    {
        Number h = StepForNumericalDerivative(value);
        Number plus = value + h;
        Number minus = value - h;
        Number f_plus = EvaluateWithTempVariable(expression_str, variable, plus);
        Number f_minus = EvaluateWithTempVariable(expression_str, variable, minus);
        return (f_plus - f_minus) / (h + h);
    }

    Number EvaluateWithTempVariables(const std::u32string& expression_str, const std::list<std::pair<std::u32string, Number>>& variable_values) const
    {
        Solver<Number> sub_solver(precision, default_angle_measure, result_angle_measure, default_notation, im, Number(), symbols);
        sub_solver.parser_context = parser_context;
        sub_solver.id = id;
        sub_solver.exported_id = exported_id;
        sub_solver.res_pos = res_pos;
        sub_solver.cast_units = cast_units;
        sub_solver.max_cast_unit_size = max_cast_unit_size;
        sub_solver.cur_subscript = cur_subscript;
        sub_solver.dependencies = dependencies;

        for (const auto& variable_value : variable_values)
        {
            Number value_copy = variable_value.second;
            sub_solver.PushTempVariable(variable_value.first, value_copy);
        }

        Number res;
        try
        {
            std::u32string expr = expression_str;
            Expression<Number> expression(id, expr, &sub_solver);
            ExpressionNode<Number> node;
            std::u32string::iterator iter = expr.begin();
            std::u32string::iterator end = expr.end();
            unicode::space_type space;
            if (!phrase_parse(iter, end, expression, space, node))
                throw MathException(IncorrectOperation);
            res = sub_solver(node);
        }
        catch (...)
        {
            sub_solver.PopTempVariables(variable_values.size());
            throw;
        }
        sub_solver.PopTempVariables(variable_values.size());
        return res;
    }

    Number NumericalDerivativeAtPoint(const std::u32string& expression_str, const std::list<DerivativeVariableNode<Number>>& variables) const
    {
        auto var_iter = variables.begin();
        std::u32string first_variable = var_iter->name;
        Number first_value = (*this)(var_iter->value);

        std::list<std::pair<std::u32string, Number>> other_variables;
        ++var_iter;
        for (; var_iter != variables.end(); ++var_iter)
        {
            Number value = (*this)(var_iter->value);
            other_variables.emplace_back(var_iter->name, value);
        }

        auto evaluate_at =
            [&](const Number& value) -> Number
            {
                std::list<std::pair<std::u32string, Number>> variable_values = other_variables;
                variable_values.emplace_front(first_variable, value);
                return EvaluateWithTempVariables(expression_str, variable_values);
            };

        Number h = StepForNumericalDerivative(first_value);
        Number plus = first_value + h;
        Number minus = first_value - h;
        Number f_plus = evaluate_at(plus);
        Number f_minus = evaluate_at(minus);
        return (f_plus - f_minus) / (h + h);
    }

    bool HasUserFunctionCall(const std::u32string& expression_str) const
    {
        auto is_identifier_char =
            [](char32_t c) -> bool
            {
                return (c >= U'a' && c <= U'z') || (c >= U'A' && c <= U'Z') || (c >= U'0' && c <= U'9') || c == U'_';
            };

        for (const auto& func : symbols->functions)
        {
            const std::u32string& name = func.name.name;
            size_t pos = 0;
            while ((pos = expression_str.find(name, pos)) != std::u32string::npos)
            {
                size_t name_end = pos + name.size();
                bool prev_ok = (pos == 0) || !is_identifier_char(expression_str[pos - 1]);
                size_t after = name_end;
                while (after < expression_str.size() && expression_str[after] == U' ')
                    ++after;
                bool is_call = (after < expression_str.size()) && (expression_str[after] == U'(');
                if (prev_ok && is_call)
                    return true;
                ++pos;
            }
        }
        return false;
    }

    Number NumericalDefiniteIntegral(const std::u32string& expression_str, const std::u32string& variable,
        const Number& lower, const Number& upper) const
    {
        if constexpr (!std::is_same_v<Number, Real> && !std::is_same_v<Number, Complex>)
            throw MathException(IncorrectOperation);

        if (lower.IsInfinity() || upper.IsInfinity())
            throw MathException(IncorrectOperation);

        Solver<Number> sub_solver(precision, default_angle_measure, result_angle_measure, default_notation, im, Number(), symbols);
        sub_solver.parser_context = parser_context;
        sub_solver.id = id;
        sub_solver.exported_id = exported_id;
        sub_solver.res_pos = res_pos;
        sub_solver.cast_units = cast_units;
        sub_solver.max_cast_unit_size = max_cast_unit_size;
        sub_solver.cur_subscript = cur_subscript;
        sub_solver.dependencies = dependencies;

        std::u32string expr = expression_str;
        Expression<Number> expression(sub_solver.id, expr, &sub_solver);
        ExpressionNode<Number> node;
        std::u32string::iterator iter = expr.begin();
        std::u32string::iterator end = expr.end();
        unicode::space_type space;
        if (!phrase_parse(iter, end, expression, space, node))
            throw MathException(IncorrectOperation);

        auto evaluate_at =
            [&](const Number& value) -> Number
            {
                Number value_copy = value;
                sub_solver.PushTempVariable(variable, value_copy);
                Number res;
                try
                {
                    res = sub_solver(node);
                }
                catch (...)
                {
                    sub_solver.PopTempVariables(1);
                    throw;
                }
                sub_solver.PopTempVariables(1);
                return res;
            };

        auto make_number =
            [&](const Real& re) -> Number
            {
                return Number(re);
            };

        auto magnitude =
            [&](const Number& value) -> Real
            {
                if constexpr (std::is_same_v<Number, Complex>)
                    return abs(value).GetRe();
                else
                    return abs(value);
            };

        Number a = lower;
        Number b = upper;
        Number two = make_number(Real(precision, 2));
        Number six = make_number(Real(precision, 6));
        Number fifteen = make_number(Real(precision, 15));

        Number c = (a + b) / two;
        Number fa = evaluate_at(a);
        Number fb = evaluate_at(b);
        Number fc = evaluate_at(c);

        auto simpson =
            [&](const Number& left, const Number& right, const Number& f_left, const Number& f_mid, const Number& f_right) -> Number
            {
                return (right - left) / six * (f_left + f_right + f_mid * two + f_mid * two);
            };

        Number S = simpson(a, b, fa, fc, fb);
        std::string tol_str = std::string("1e-") + std::to_string(precision + 2);
        Number eps = make_number(Real(precision, tol_str.c_str()));

        std::function<Number(const Number&, const Number&, const Number&, const Number&,
            const Number&, const Number&, const Number&, int)> adaptive =
            [&](const Number& left, const Number& right, const Number& tol, const Number& estimate, const Number& f_left,
                const Number& f_right, const Number& f_mid, int depth) -> Number
            {
                Number mid = (left + right) / two;
                Number left_mid = (left + mid) / two;
                Number right_mid = (mid + right) / two;
                Number f_left_mid = evaluate_at(left_mid);
                Number f_right_mid = evaluate_at(right_mid);
                Number left_estimate = simpson(left, mid, f_left, f_left_mid, f_mid);
                Number right_estimate = simpson(mid, right, f_mid, f_right_mid, f_right);
                Number total_estimate = left_estimate + right_estimate;
                if (depth <= 0 || magnitude(total_estimate - estimate) <= magnitude(tol * fifteen))
                    return total_estimate + (total_estimate - estimate) / fifteen;
                Number half_tol = tol / two;
                return adaptive(left, mid, half_tol, left_estimate, f_left, f_mid, f_left_mid, depth - 1) + 
                    adaptive(mid, right, half_tol, right_estimate, f_mid, f_right, f_right_mid, depth - 1);
            };

        return adaptive(a, b, eps, S, fa, fb, fc, 20);
    }

    Number EvaluateDerivativeAtPoint(const std::u32string& expression_str, const std::u32string& variable, const Number& value) const
    {
        std::lock_guard<std::mutex> lock(giac_evaluation_mutex);
        GiacMpfrStateGuard mpfr_guard;
        GiacOutputGuard output_guard;
        giac::context ctx;
        if constexpr (!std::is_same_v<Number, Rational>)
            giac::decimal_digits(std::max(1, precision + 1), &ctx);
        giac::gen expr_gen = ParseGen(ToBasicString(expression_str), &ctx);
        giac::identificateur var_id(ToBasicString(variable));
        giac::gen var_gen(var_id);
        giac::gen value_gen = NumberToGiac(value);
        giac::gen derivative = giac::derive(expr_gen, var_gen, &ctx);
        giac::gen res = giac::subst(derivative, var_gen, value_gen, false, &ctx);
        if constexpr (std::is_same_v<Number, Rational>)
            res = giac::normal(res, &ctx);
        else
            res = giac::evalf(res, 1, &ctx);
        return FromGiac<Number>(res, precision);
    }

    Number EvaluateDerivativeAtPoint(const std::u32string& expression_str, const std::list<DerivativeVariableNode<Number>>& variables) const
    {
        std::lock_guard<std::mutex> lock(giac_evaluation_mutex);
        GiacMpfrStateGuard mpfr_guard;
        GiacOutputGuard output_guard;
        giac::context ctx;
        if constexpr (!std::is_same_v<Number, Rational>)
            giac::decimal_digits(std::max(1, precision + 1), &ctx);
        giac::gen expr_gen = ParseGen(ToBasicString(expression_str), &ctx);

        auto var_iter = variables.begin();
        giac::identificateur first_var_id(ToBasicString(var_iter->name));
        giac::gen first_var_gen(first_var_id);
        Number first_value = (*this)(var_iter->value);
        giac::gen first_value_gen = NumberToGiac(first_value);

        giac::gen res = giac::derive(expr_gen, first_var_gen, &ctx);
        res = giac::subst(res, first_var_gen, first_value_gen, false, &ctx);

        ++var_iter;
        for (; var_iter != variables.end(); ++var_iter)
        {
            giac::identificateur var_id(ToBasicString(var_iter->name));
            giac::gen var_gen(var_id);
            Number value = (*this)(var_iter->value);
            giac::gen value_gen = NumberToGiac(value);
            res = giac::subst(res, var_gen, value_gen, false, &ctx);
        }

        if constexpr (std::is_same_v<Number, Rational>)
            res = giac::normal(res, &ctx);
        else
            res = giac::evalf(res, 1, &ctx);
        return FromGiac<Number>(res, precision);
    }

    Number operator()(FunctionParamNode<Number> const& expr) const
    {
        return boost::apply_visitor(*this, expr.op);
    }

    Number operator()(NoFencesFunctionCallNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
    }

    Number operator()(IdentifierNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            if (!op.subscript.empty())
                throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name + U"' not found", op.pos, op.name.length(), op.line);
            AddDependency(op);
            auto* t = FindTempVariable(op.name);
            if (t)
                return t->second;
            auto* v = FindVariable(op.name, op.subscript);
            if (v)
            {
                LogicalId _id = id;
                bool _exported_id = exported_id;
                id = v->id;
                exported_id = v->exported;
                try
                {
                    Number res = (*this)(v->expression);
                    id = _id;
                    exported_id = _exported_id;
                    return res;
                }
                catch (...)
                {
                    id = _id;
                    exported_id = _exported_id;
                    throw;
                }
            }
            auto* var = FindBuiltinVariable(op.name);
            if (var)
            {
                try
                {
                    auto pv = std::get<typename SolverSymbols<Number>::PrecisionVariable>(*var);
                    return (*pv)(precision);
                }
                catch (const std::bad_variant_access&)
                {
                }
            }
            Number builtin_id_val;
            if (FindBuiltinIdentifier(op.name, builtin_id_val))
                return builtin_id_val;
            if constexpr (std::is_same_v<Number, Symbolic<Complex>>)
            {
                if (op.name == U"I" ||
                    (op.name == U"i" && im == U"i") ||
                    (op.name == U"j" && im == U"j"))
                    return Number(precision, std::string("I"));
            }
            return Number(precision, op.name);
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(ImplicitStringMulNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            if (op.identifier.name == std::u32string(1, 0x221E))
                throw SyntaxException(op.id, SyntaxError, op.pos, static_cast<int>(op.identifier.name.length()), op.line);
            if (!op.identifier.subscript.empty())
                throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.identifier.name + U"' not found", op.pos, op.identifier.name.length(), op.line);
            AddDependency(op.identifier);
            auto* t = FindTempVariable(op.identifier.name);
            if (t)
                return (*this)(op.left) * t->second;
            auto* v = FindVariable(op.identifier.name, op.identifier.subscript);
            if (v)
            {
                LogicalId _id = id;
                id = v->id;
                Number res = (*this)(v->expression);
                id = _id;
                return (*this)(op.left) * res;
            }
            auto* var = FindBuiltinVariable(op.identifier.name);
            if (var)
            {
                try
                {
                    auto pv = std::get<typename SolverSymbols<Number>::PrecisionVariable>(*var);
                    return (*this)(op.left) * (*pv)(precision);
                }
                catch (const std::bad_variant_access&)
                {
                }
            }
            Number builtin_id_val;
            if (FindBuiltinIdentifier(op.identifier.name, builtin_id_val))
                return (*this)(op.left) * builtin_id_val;
            if constexpr (std::is_same_v<Number, Symbolic<Complex>>)
            {
                if (op.identifier.name == U"I" ||
                    (op.identifier.name == U"i" && im == U"i") ||
                    (op.identifier.name == U"j" && im == U"j"))
                    return (*this)(op.left) * Number(precision, std::string("I"));
            }
            return (*this)(op.left) * Number(precision, op.identifier.name);
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(ImplicitDivMulNode<Number> const& op) const
    {
        CheckBreak(parser_context);
        if constexpr (is_symbolic_v<Number>)
        {
            AddDependency(op.identifier);
            auto* t = FindTempVariable(op.identifier.name);
            if (t)
            {
                return (*this)(op.upper) / (*this)(op.lower) * t->second;
            }
            auto* v = FindVariable(op.identifier.name, op.identifier.subscript);
            if (v)
            {
                LogicalId _id = id;
                id = v->id;
                Number res = (*this)(v->expression);
                id = _id;
                return (*this)(op.upper) / (*this)(op.lower) * res;
            }
            auto* var = FindBuiltinVariable(op.identifier.name);
            if (var)
            {
                try
                {
                    auto pv = std::get<typename SolverSymbols<Number>::PrecisionVariable>(*var);
                    return (*this)(op.upper) / (*this)(op.lower) * (*pv)(precision);
                }
                catch (const std::bad_variant_access&)
                {
                }
            }
            Number val;
            if (FindBuiltinIdentifier(op.identifier.name, val))
            {
                return (*this)(op.upper) / (*this)(op.lower) * val;
            }
            if constexpr (std::is_same_v<Number, Symbolic<Complex>>)
            {
                if (op.identifier.name == U"I" ||
                    (op.identifier.name == U"i" && im == U"i") ||
                    (op.identifier.name == U"j" && im == U"j"))
                    return (*this)(op.upper) / (*this)(op.lower) * Number(precision, std::string("I"));
            }
            return (*this)(op.upper) / (*this)(op.lower) * Number(precision, op.identifier.name);
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(ImplicitMulNode<Number> const& op) const
    {
        Number arg1 = (*this)(op.before);
        Number arg2 = (*this)(op.inside_braces);
        return arg1 * arg2;
    }

    Number operator()(ImplicitFractionMulNode<Number> const& op) const
    {
        Number arg1 = (*this)(op.numerator);
        Number arg2 = (*this)(op.denominator);
        return arg1 / arg2 * (*this)(op.identifier);
    }

    Number operator()(ImplicitFunctionMulNode<Number> const& op) const
    {
        return (*this)(op.left) * (*this)(op.function_call);
    }

    Number operator()(ImplicitPostFunctionMulNode<Number> const& op) const
    {
        return (*this)(op.function_call) * (*this)(op.right);
    }

    Number operator()(CompareNode<Number> const& op) const
    {
        Number left = (*this)(op.left);
        Number right = (*this)(op.right);
        if (op.sign == U"<")
            return left < right ? Number(precision, 1) : Number(precision, 0);
        else if (op.sign == U">")
            return left > right ? Number(precision, 1) : Number(precision, 0);
        else if (op.sign == U"<=")
            return left <= right ? Number(precision, 1) : Number(precision, 0);
        else if (op.sign == U">=")
            return left >= right ? Number(precision, 1) : Number(precision, 0);
        else if (op.sign == U"==")
            return left == right ? Number(precision, 1) : Number(precision, 0);
        else if (op.sign == U"<>")
            return left != right ? Number(precision, 1) : Number(precision, 0);
        throw SyntaxException(op.id, ArgumentIsOver, op.pos, op.line);
    }

    Number operator()(LoopNode<Number> const& op) const
    {
        if constexpr (is_symbolic_v<Number>)
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
        else
        {
            Number counter = (*this)(op.counter.expression);
            PushTempVariable(op.counter.name.name, counter);
            Number counter_max;
            Number res;
            try
            {
                counter_max = (*this)(op.counter_max);
                res = (*this)(op.loop_var.expression);
            }
            catch (...)
            {
                PopTempVariables(1);
                throw;
            }
            PushTempVariable(op.loop_var.name.name, res);
            while (counter_max != 0)
            {
                CheckBreak(parser_context);

                try
                {
                    res = (*this)(op.loop_expression.expression);
                    SetTempVariable(op.loop_expression.name.name, res);
                    counter = (*this)(op.counter_increment.expression);
                    SetTempVariable(op.counter.name.name, counter);
                    counter_max = (*this)(op.counter_max);
                }
                catch (...)
                {
                    PopTempVariables(2);
                    throw;
                }
            }
            PopTempVariables(2);
            return res;
        }
    }

    Number operator()(ArrayNode<Number> const& op) const
    {
        if constexpr (std::is_same_v<Number, Array<Real>>)
        {
            CheckBreak(parser_context);
            Number res;
            for (auto& operand : op.array)
            {
                CheckBreak(parser_context);
                res.Add((*this)(operand));
            }
            return res;
        }
        else
        {
            throw MathException(op.id, IncorrectOperation, op.pos, 1, op.line);
        }
    }

    Number operator()(ScriptNode<Number> const& script, LogicalId _id, AngleMeasure _default_angle_measure,
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

        Number res;
        if constexpr (is_symbolic_v<Number>)
        {
            try
            {
                BOOST_FOREACH(typename ScriptNode<Number>::Operand const& op, script.list)
                {
                    CheckBreak(parser_context);
                    res = boost::apply_visitor(*this, op);
                }
            }
            catch (const MathException& e)
            {
                throw MathException(id, e.ex_id, -1, -1);
            }
            catch (const ParserException&)
            {
                throw;
            }
            catch (...)
            {
                if constexpr (std::is_same_v<Number, Symbolic<Real>> || std::is_same_v<Number, Symbolic<Complex>>)
                    return Number(precision, std::u32string(U"nan"));
                throw;
            }
        }
        else
        {
            BOOST_FOREACH(typename ScriptNode<Number>::Operand const& op, script.list)
            {
                CheckBreak(parser_context);
                res = boost::apply_visitor(*this, op);
            }
        }

        if constexpr (std::is_same_v<Number, Real>)
        {
            if (res.angle_measure != AngleMeasure::None)
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
        }
        return res;
    }

    void PushTempVariable(const std::u32string& name, Number& value) const
    {
        symbols->temp_variables.push_back(TempVariable(name, value));
    }

    void SetTempVariable(const std::u32string& name, Number& value) const
    {
        auto it = std::find_if(symbols->temp_variables.begin(), symbols->temp_variables.end(), 
            [name](TempVariable& var)
            {
                return var.first == name;
            });
        if (it != symbols->temp_variables.end())
            it->second = value;
    }

    void PopTempVariables(size_t count) const
    {
        for (int i = 0; i < count && !symbols->temp_variables.empty(); ++i)
            symbols->temp_variables.pop_back();
    }

    void ClearTempVariables()
    {
        symbols->temp_variables.clear();
    }

    TempVariable* FindTempVariable(const std::u32string& name) const
    {
        for (int i = (int)symbols->temp_variables.size() - 1; i >= 0; --i)
        {
            if (symbols->temp_variables[i].first == name)
                return &symbols->temp_variables[i];
        }
        
        return nullptr;
    }

    void AddVariable(VariableNode<Number> const& var) const
    {
        int j = -1;
        LogicalId min_same_name_id;
        for (size_t i = 0; i < symbols->variables.size(); ++i)
        {
            auto& v = symbols->variables[i];
            if (v.id == id)
            {
                v = var;
                v.id = id;
                (*this)(var.expression); //for adding dependencies and throwing exceptions
                if (parser_context && parser_context->exports && parser_context->include_document)
                    parser_context->exports->AddVariable<Number>(var);
                return;
            }
            if (v.name.name == var.name.name)
            {
                if (IsLess(v.id, id))
                    j = i + 1;
                else
                {
                    if (!min_same_name_id.empty())
                    {
                        if (IsLess(v.id, min_same_name_id))
                        {
                            min_same_name_id = v.id;
                            j = i;
                        }
                    }
                    else
                    {
                        min_same_name_id = v.id;
                        j = i;
                    }
                }
            }
        }
        var.id = id;
        if (!parser_context || (parser_context && parser_context->exports && !parser_context->include_document))
        {
            if (j == -1)
                symbols->variables.push_back(var);
            else
                symbols->variables.insert(symbols->variables.begin() + j, var);
        }

        cur_subscript = var.name.subscript;
        (*this)(var.expression); //for adding dependencies and throwing exceptions
        if (parser_context && parser_context->exports && parser_context->include_document)
            parser_context->exports->AddVariable<Number>(var);
    }

    void AddUnit(UnitNode<Number> const& unit) const
    {
        auto it = std::find_if(symbols->units.begin(), symbols->units.end(), 
            [id = id](const CustomUnit<Number>& u)
            {
                return u.id == id;
            });
        if (it != symbols->units.end())
            symbols->units.erase(it);

        AddDependency(unit.name);
        Number res = (*this)(unit.expression);
        auto c = CustomUnit<Number>(id, unit.name.name, unit.name.subscript, res, symbols->buildin_elements);
        c.description = unit.name.description;

        if (parser_context && parser_context->exports && parser_context->include_document)
            parser_context->exports->AddUnit<Number>(c);
        else
            symbols->units.emplace_back(c);
    }

    void AddList(ListNode<Number> const& list) const
    {
        if (list.name.name.empty())
            return;
        symbols->lists[list.name.name] = list.list;
        if (parser_context && parser_context->exports && parser_context->include_document)
            parser_context->exports->AddList<Number>(list);
    }

    void AddString(StringNode<Number> const& str) const
    {
        if (str.name.name.empty())
            return;
        symbols->strings[str.name.name] = str.str;
        if (parser_context && parser_context->exports && parser_context->include_document)
            parser_context->exports->AddString<Number>(str);
    }

    VariableNode<Number>* FindVariable(const std::u32string& name, const std::u32string& subscript) const
    {
        std::u32string s = subscript;
        if (!subscript.empty())
        {
            auto it = symbols->strings.find(subscript);
            if (it != symbols->strings.end())
                s = it->second;
        }
        
        for (int i = (int)symbols->variables.size() - 1; i >= 0; --i)
        {
            auto& var = symbols->variables[i];
            if (var.name.name == name && var.name.subscript == s)
            {
                if (exported_id || IsLess(var.id, id))
                    return &var;
            }
        }

        return FindExportVariable(name, s);
    }

    VariableNode<Number>* FindExportVariable(const std::u32string& name, const std::u32string& subscript) const
    {
        if (!parser_context)
            return nullptr;
        return parser_context->exports->FindVariable<Number>(name, subscript);
    }

    FunctionNode<Number>* FindFunction(FunctionCallNode<Number> const& op) const
    {
        for (int i = (int)symbols->functions.size() - 1; i >= 0; --i)
        {
            auto& func = symbols->functions[i];
            if (func.name.name == op.name.name)
            {
                if (exported_id || IsLess(func.id, id))
                {
                    if (func.arguments.size() != op.arguments.size())
                        throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                    return &func;
                }
            }
        }
        return FindExportFunction(op);
    }

    FunctionNode<Number>* FindExportFunction(FunctionCallNode<Number> const& op) const
    {
        if (!parser_context)
            return nullptr;
        return parser_context->exports->FindFunction<Number>(op.name.name);
    }

    void AddFunction(FunctionNode<Number> const& func) const
    {
        int j = -1;
        LogicalId min_same_name_id;
        for (size_t i = 0; i < symbols->functions.size(); ++i)
        {
            auto& f = symbols->functions[i];
            if (f.id == id)
            {
                f = func;
                f.id = id;
                return;
            }
            if (f.name.name == func.name.name)
            {
                if (IsLess(f.id, id))
                    j = i + 1;
                else
                {
                    if (!min_same_name_id.empty())
                    {
                        if (IsLess(f.id, min_same_name_id))
                        {
                            min_same_name_id = f.id;
                            j = i;
                        }
                    }
                    else
                    {
                        min_same_name_id = f.id;
                        j = i;
                    }
                }
            }
        }

        func.id = id;

        if (parser_context && parser_context->exports && parser_context->include_document)
            parser_context->exports->AddFunction<Number>(func);
        else
        {
            if (j == -1)
                symbols->functions.push_back(func);
            else
                symbols->functions.insert(symbols->functions.begin() + j, func);
        }

        //parse for adding dependencies
        Number arg;
        arg = 1;
        for (IdentifierNodesIter iter = func.arguments.begin(); iter != func.arguments.end(); ++iter)
            PushTempVariable(iter->name, arg);
        (*this)(func.return_expression);
        PopTempVariables(func.arguments.size());
    }

    void AddBuiltinFunction(const char32_t* name, UnaryFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void AddBuiltinFunction(const char32_t* name, BinaryFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void AddBuiltinFunction(const char32_t* name, TernaryFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void AddBuiltinFunction(const char32_t* name, QuaternaryFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void AddBuiltinFunction(const char32_t* name, StringFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void AddTrigonometricFunction(const char32_t* name, TrigonometricFunction& func)
    {
        symbols->trigonometric_functions[name] = func;
    }

    void AddBuiltinFunction(const char32_t* name, ComplexUnaryFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void AddBuiltinFunction(const char32_t* name, ComplexBinaryFunction& func)
    {
        symbols->buildin_functions[name] = func;
    }

    void FillBuiltinOperations()
    {
        if (!symbols->builtin_operations.empty())
            return;
        symbols->builtin_operations.push_back(U"plus");
        symbols->builtin_operations.push_back(U"minus");
        symbols->builtin_operations.push_back(U"mul");
        symbols->builtin_operations.push_back(U"div");
        symbols->builtin_operations.push_back(U"power");
        symbols->builtin_operations.push_back(U"sub");
        symbols->builtin_operations.push_back(U"sum");
        symbols->builtin_operations.push_back(U"prod");
    }

    BuiltinFunction* FindBuiltinFunction(const std::u32string& name) const
    {
        typename std::map<std::u32string, BuiltinFunction>::const_iterator iter = symbols->buildin_functions.find(name);
        if (iter == symbols->buildin_functions.end())
            return nullptr;
        return (BuiltinFunction*)&(*iter).second;
    }

    BuiltinTrigonometricFunction* FindTrigonometricFunction(const std::u32string& name) const
    {
        typename std::map<std::u32string, BuiltinTrigonometricFunction>::const_iterator iter = symbols->trigonometric_functions.find(name);
        if (iter == symbols->trigonometric_functions.end())
            return nullptr;
        return (BuiltinTrigonometricFunction*)&(*iter).second;
    }
    
    void AddBuiltinVariable(const char32_t* name, PrecisionVariable& var)
    {
        symbols->buildin_variables[name] = var;
    }

    void AddBuiltinVariable(const char32_t* name, ComplexPrecisionVariable& var)
    {
        symbols->buildin_variables[name] = var;
    }

    BuiltinVariable* FindBuiltinVariable(const std::u32string& name) const
    {
        typename std::map<std::u32string, BuiltinVariable>::const_iterator iter = symbols->buildin_variables.find(name);
        if (iter == symbols->buildin_variables.end())
            return nullptr;
        return (BuiltinVariable*)&(*iter).second;
    }

    bool RemoveIdentifier(LogicalId id, const std::u32string& name)
    {
        auto var_it = symbols->variables.erase(std::remove_if(symbols->variables.begin(), symbols->variables.end(), 
            [id, name](auto& var)
            {
                if (var.id != id)
                    return false;
                if (var.name.subscript.empty())
                    return var.name.name == name;
                return var.name.name + U"{" + var.name.subscript + U"}" == name;
            }), 
            symbols->variables.end());
        if (var_it != symbols->variables.end())
            return true;

        auto func_it = symbols->functions.erase(std::remove_if(symbols->functions.begin(), symbols->functions.end(), 
            [id, name](auto& func)
            {
                if (func.id != id)
                    return false;
                if (func.name.subscript.empty())
                    return func.name.name == name && func.id == id;;
                return func.name.name + U"{" + func.name.subscript + U"}" == name;
            }), 
            symbols->functions.end());
        if (func_it != symbols->functions.end())
            return true;

        auto unit_it = symbols->units.erase(std::remove_if(symbols->units.begin(), symbols->units.end(), 
            [id, name](auto& unit)
            {
                return unit.name == name && unit.id == id;
            }), 
            symbols->units.end());
        return unit_it != symbols->units.end();
    }

    bool RemoveIdentifier(LogicalId id)
    {
        auto var_it = symbols->variables.erase(std::remove_if(symbols->variables.begin(), symbols->variables.end(), 
            [id](auto& var)
            {
                return var.id == id;
            }), 
            symbols->variables.end());
        if (var_it != symbols->variables.end())
            return true;

        auto func_it = symbols->functions.erase(std::remove_if(symbols->functions.begin(), symbols->functions.end(), 
            [id](auto& func)
            {
                return func.id == id;;
            }), 
            symbols->functions.end());
        if (func_it != symbols->functions.end())
            return true;

        auto unit_it = symbols->units.erase(std::remove_if(symbols->units.begin(), symbols->units.end(), 
            [id](auto& unit)
            {
                return unit.id == id;;
            }), 
            symbols->units.end());
        return unit_it != symbols->units.end();
    }

    void AddBuiltinUnit(const Unit& unit)
    {
        symbols->buildin_units.push_back(unit);
    }

    void ResetBuildinUnits()
    {
        symbols->buildin_units.clear();
    }

    void AddBuiltinIdentifier(const std::u32string& name, const Number& value)
    {
        symbols->builtin_identifiers[name] = value;
    }

    void ResetBuiltinIdentifiers()
    {
        symbols->builtin_identifiers.clear();
    }

    bool FindBuiltinIdentifier(const std::u32string& name, Number& value) const
    {
        auto iter = symbols->builtin_identifiers.find(name);
        if (iter == symbols->builtin_identifiers.end())
            return false;
        value = iter->second;
        return true;
    }

    void RemoveUserIdentifiers()
    {
        //user identifiers have ids' positions >= 0
        symbols->units.erase(std::remove_if(symbols->units.begin(), symbols->units.end(), 
            [](auto& unit)
            {
                for (int p : unit.id)
                {
                    if (p < 0)
                        return false;
                }
                return true;
            }), 
            symbols->units.end());

        symbols->variables.erase(std::remove_if(symbols->variables.begin(), symbols->variables.end(), 
            [](auto& var)
            {
                for (int p : var.id)
                {
                    if (p < 0)
                        return false;
                }
                return true;
            }), 
            symbols->variables.end());

        symbols->functions.erase(std::remove_if(symbols->functions.begin(), symbols->functions.end(), 
            [](auto& func)
            {
                for (int p : func.id)
                {
                    if (p < 0)
                        return false;
                }
                return true;
            }), 
            symbols->functions.end());
    }

    Unit* FindBuiltinUnit(const std::u32string& name) const
    {
        auto iter = std::find_if(symbols->buildin_units.begin(), symbols->buildin_units.end(), 
            [name](Unit& unit)
            {
                return unit == name;
            });
        if (iter == symbols->buildin_units.end())
            return nullptr;
        return &(*iter);
    }

    CustomUnit<Number>* FindUnit(const std::u32string& name, const std::u32string& system) const
    {
        CustomUnit<Number>* res = nullptr;
        LogicalId unit_id;
        auto _system = system;
        if (_system.empty())
            _system = U"SI";
        for (int i = symbols->units.size() - 1; i >= 0; --i)
        {
            CustomUnit<Number>& unit = symbols->units[i];
            auto s = unit.system;
            if (s.empty())
                s = U"SI";
            if (_system != s)
                continue;
            if (unit.buildin && symbols->buildin_elements && !IsLess(unit.id, id))
                continue;
            if (!unit.buildin && !symbols->buildin_elements && !IsLess(unit.id, id))
                continue;
            if (!unit.buildin && symbols->buildin_elements)
                continue;
            if (unit.name == name)
            {
                if (unit.buildin || IsLess(unit.id, id))
                {
                    if (!unit_id.empty())
                    {
                        if (unit.buildin || IsLess(unit.id, unit_id))
                            continue;
                    }
                    unit_id = unit.id;
                    res = &unit;
                }
            }
        }
        if (!res)
            return FindExportUnit(name, system);
        return res;
    }

    CustomUnit<Number>* FindExportUnit(const std::u32string& name, const std::u32string& system) const
    {
        if (!parser_context)
            return nullptr;
        return parser_context->exports->FindUnit<Number>(name, system);
    }

    void GetCastUnits(const LogicalId _id, const Number& val, std::vector<Unit>& _cast_units)
    {
        if (val.unit.IsEmpty())
            return;

        GetCastUnits(_id, val, U"SI", _cast_units);
        GetCastUnits(_id, val, U"rus", _cast_units);
        GetCastUnits(_id, val, U"us", _cast_units);
        std::sort(_cast_units.begin(), _cast_units.end());
    }

    void GetCastUnits(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Unit>& _cast_units)
    {
        auto it = cast_units.find(system);
        if (it != cast_units.end())
        {
            auto& v = it->second;
            for (auto& n : v)
                _cast_units.push_back(n.unit);
            return;
        }

        GetCastUnitsImpl(_id, val, system, _cast_units);
    }

    void GetCastUnits(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Number>& _cast_units) const
    {
        auto it = cast_units.find(system);
        if (it != cast_units.end())
        {
            _cast_units = it->second;
            return;
        }

        GetCastUnitsImpl(_id, val, system, _cast_units);

        cast_units[system] = _cast_units;
    }

    void GetCastUnitsImpl(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Unit>& _cast_units)
    {
        std::set<LogicalId> visited;
        GetCastUnitsImpl(_id, val, system, _cast_units, visited);
    }

    void GetCastUnitsImpl(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Unit>& _cast_units, std::set<LogicalId>& visited)
    {
        if (val.unit.IsEmpty())
            return;

        CheckBreak(parser_context);

        if (val.unit.system == system)
        {
            //add the cast unit if all of its parts have the same system
            size_t i = 0;
            for (; i < val.unit.unit.size(); ++i)
            {
                auto& u = val.unit.unit[i];
                if (system == U"SI" && FindBuiltinUnit(u.first))
                    continue;
                if (FindUnit(u.first, system) == nullptr)
                    break;
            }
            if (i == val.unit.unit.size())
            {
                Unit _unit = val.unit;
                _unit.Sort();
                _cast_units.push_back(_unit);
            }
        }

        for (size_t i = 0; i < symbols->units.size(); ++i)
        {
            CustomUnit<Number>& custom_unit = symbols->units[i];
            if ((!custom_unit.buildin && !IsLess(custom_unit.id, _id)) || custom_unit.value.unit.IsEmpty())
                continue;
            Number t = val;
            if (custom_unit.system == system)
            {
                if (custom_unit.Cast(t))
                {
                    if (system != U"SI")
                    {
                        //check all the unit parts are present in this system
                        size_t j = 0;
                        for (; j < t.unit.unit.size(); ++j)
                        {
                            std::pair<std::u32string, int>& p = t.unit.unit[j];
                            if (std::find_if(symbols->units.begin(), symbols->units.end(), 
                                [system, p](const CustomUnit<Number>& c)
                                {
                                    return c.system == system && c.name == p.first;
                                }
                                ) == symbols->units.end())
                            {
                                break;
                            }
                        }
                        if (j != t.unit.unit.size())
                            continue;
                    }

                    if (visited.count(custom_unit.id))
                        continue;
                    visited.insert(custom_unit.id);
                    if (t.unit.unit.size() <= max_cast_unit_size)
                        GetCastUnitsImpl(_id, t, system, _cast_units, visited);
                    else
                    {
                        Unit _unit = t.unit;
                        _unit.Sort();
                        _cast_units.push_back(_unit);
                    }
                    visited.erase(custom_unit.id);
                }
            }
        }
    }

    void GetCastUnitsImpl(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Number>& _cast_units) const
    {
        std::set<LogicalId> visited;
        GetCastUnitsImpl(_id, val, system, _cast_units, visited);
    }

    void GetCastUnitsImpl(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Number>& _cast_units, std::set<LogicalId>& visited) const
    {
        if (val.unit.IsEmpty())
            return;

        CheckBreak(parser_context);

        if (val.unit.system == system)
            _cast_units.push_back(val);
        for (size_t i = 0; i < symbols->units.size(); ++i)
        {
            CustomUnit<Number>& custom_unit = symbols->units[i];
            if ((!custom_unit.buildin && !IsLess(custom_unit.id, _id)) || custom_unit.value.unit.IsEmpty())
                continue;
            if (custom_unit.system == system)
            {
                Number t = val;
                if (custom_unit.Cast(t))
                {
                    if (visited.count(custom_unit.id))
                        continue;
                    visited.insert(custom_unit.id);
                    if (t.unit.unit.size() <= max_cast_unit_size)
                        GetCastUnitsImpl(_id, t, system, _cast_units, visited);
                    else
                        _cast_units.push_back(t);
                    visited.erase(custom_unit.id);
                }
            }
        }

        GetCastExportUnitsImpl(_id, val, system, _cast_units, visited);
    }

    void GetCastExportUnitsImpl(const LogicalId _id, const Number& val, const std::u32string& system, std::vector<Number>& _cast_units, std::set<LogicalId>& visited) const
    {
        if (!parser_context)
            return;
        std::vector<CustomUnit<Number>> units;
        parser_context->exports->GetUnits(system, units);
        for (auto& unit : units)
        {
            Number t = val;
            if (unit.Cast(t))
            {
                if (visited.count(unit.id))
                    continue;
                visited.insert(unit.id);
                if (t.unit.unit.size() <= max_cast_unit_size)
                    GetCastUnitsImpl(_id, t, system, _cast_units, visited);
                else
                    _cast_units.push_back(t);
                visited.erase(unit.id);
            }
        }
    }

    Number GetSuitableUnit(const LogicalId _id, const Number& val, const std::u32string& system, const bool buildin) const;

    Number CastToUnit(const LogicalId id, const Number& val, const Unit& unit) const;

    void ClearCastUnits()
    {
        cast_units.clear();
    }

    void ListBuiltinVariables(std::vector<std::u32string>& variables)
    {
        for (auto& v : symbols->buildin_variables)
            variables.push_back(v.first);
    }

    void ListBuiltinFunctions(std::vector<std::u32string>& functions)
    {
        for (auto& p : symbols->buildin_functions)
            functions.push_back(p.first);
        for (auto& p : symbols->trigonometric_functions)
            functions.push_back(p.first);
    }

    void ListUserVariables(std::vector<std::pair<std::u32string, std::u32string>>& variables)
    {
        for (auto& var : symbols->variables)
        {
            if (var.name.subscript.empty())
                variables.push_back(std::make_pair(var.name.name, var.name.description));
            else
                variables.push_back(std::make_pair(var.name.name + U"{" + var.name.subscript + U"}", var.name.description));
        }
    }

    void ListUserFunctions(std::vector<std::u32string>& functions)
    {
        for (auto& func : symbols->functions)
        {
            if (func.name.subscript.empty())
                functions.push_back(func.name.name);
            else
                functions.push_back(func.name.name + U"{" + func.name.subscript + U"}");
        }
    }

    void ListBuiltinUnits(std::vector<std::pair<std::u32string, std::u32string>>& units)
    {
        for (auto& u : symbols->buildin_units)
            units.push_back(std::make_pair(u.ToString(false), u.description));
    }

    void ListUserUnits(std::vector<CustomUnit<Number>>& units)
    {
        for (auto& u : symbols->buildin_units)
        {
            CustomUnit<Number> c(LogicalId{0, 0, 1}, u.unit[0].first, u.system, Number(precision, u), true);
            c.description = u.description;
            units.push_back(c);
        }
        units.insert(units.end(), symbols->units.begin(), symbols->units.end());
    }

    void ListBuiltinOperations(std::vector<std::u32string>& operations)
    {
        operations = symbols->builtin_operations;
    }

    void ListUserStrings(std::vector<std::u32string>& strings)
    {
        for (auto& str : symbols->strings)
        {
            strings.push_back(str.first);
            strings.push_back(str.second);
        }
    }

    void SetDefaultNotation(Notation notation)
    {
        switch (notation)
        {
        case Notation::Binary:
            default_notation = 2;
            break;
        case Notation::Octal:
            default_notation = 8;
            break;
        case Notation::Decimal:
            default_notation = 10;
            break;
        case Notation::Hexadecimal:
            default_notation = 16;
            break;
        default:
            throw MathException(ParserExceptionCode::ArgumentIsOver);
        }
    }

    void SetMaxCastUnitSize(int _max_cast_unit_size)
    {
        max_cast_unit_size = _max_cast_unit_size;
    }

    void SetParserContext(ParserContext* _context)
    {
        parser_context = _context;
    }

public:
    ParserContext* parser_context = nullptr;

    mutable std::u32string im;
    mutable int res_pos = 0;

private:
    Number GetSuitableUnitImpl(const LogicalId _id, const Number& val, const std::u32string& system, const bool buildin) const
    {
        if (val.unit.IsEmpty())
            return val;
        
        //check all the custom units to be suitable for the current one
        Number res = val;
        int m = val.ToString(10, 10, false).length();
        int s = val.unit.unit.size();
        int p1 = val.unit.GetPower();

        std::vector<Number> _cast_units;
        auto it = cast_units.find(system);
        if (it != cast_units.end())
            _cast_units = it->second;
        else
            GetCastUnits(_id, val, system, _cast_units);
        Number eps;
        eps = 0.999;

        for (Number& c : _cast_units)
        {
            CheckBreak(parser_context);

            if (c.unit.unit.size() <= s) //a unit should have minimal size
            {
                int exp = c.GetExp();
                if (exp > m + 2)
                    continue;
                size_t m2 = c.ToString(::abs(exp) + 1, ::abs(exp) + 1, false).length();
                if (c.unit.unit.size() == 1 && c.unit.unit[0].second == 1) //choose the simplest one
                {
                    if (!(res.unit.unit.size() == 1 && res.unit.unit[0].first.size() < c.unit.unit[0].first.size()))
                    {
                        if (s > 1 || m2 < m || res.unit.unit[0].second < 1 || (m2 == m && (c < res || res < 1) && c >= eps))
                        {
                            res = c;
                            m = m2;
                            s = c.unit.unit.size();
                        }
                        continue;
                    }
                }

                //result string should have minimal length and should have the simpliest unit
                if (m2 > m)
                    continue;
                if (m2 == m)
                {
                    if (p1 < 0 && c.unit.GetPower() < 0)
                        continue;
                    if (c.unit.unit.size() > s)
                        continue;
                    int base_units1 = 0, base_units2 = 0;
                    for (auto& u : res.unit.unit)
                    {
                        if (FindBuiltinUnit(u.first))
                            ++base_units1;
                    }
                    for (auto& u : c.unit.unit)
                    {
                        if (FindBuiltinUnit(u.first))
                            ++base_units2;
                    }
                    if (res.unit.unit.size() == c.unit.unit.size())
                    {
                        if (base_units1 > base_units2)
                        {
                            if (c.unit.unit.size() == 1 && res.unit.unit[0].second < 0)
                            {
                                res = c;
                                m = m2;
                                s = c.unit.unit.size();
                            }
                            continue;
                        }
                        if ((res.IsZero() || c.IsZero()) || ((res < 1 || c < 1) && (1 / res < 1 / c)))
                            continue;
                        if (res.unit.unit.size() == 1 && res.unit.unit[0].second == 1 && c.unit.unit[0].second == -1)
                            continue;
                    }
                }
                else
                {
                    if (res.unit.unit.size() == c.unit.unit.size())
                    {
                        if (res.unit.GetPower() < c.unit.GetPower())
                            continue;
                    }
                }

                if (res.unit.unit.size() == c.unit.unit.size())
                {
                    auto res_str = res.ToString(10, 10, false);
                    auto c_str = c.ToString(10, 10, false);
                    if (res_str == c_str)
                    {
                        //choose more convinient unit, which has to be before the current one
                        size_t i;
                        for (i = 0; i < c.unit.unit.size(); ++i)
                        {
                            auto& u1 = c.unit.unit[i];
                            auto& u2 = res.unit.unit[i];
                            auto* unit1 = FindUnit(u1.first, c.unit.system);
                            auto* unit2 = FindUnit(u2.first, res.unit.system);
                            if (unit1 && unit2 && IsLess(unit2->id, unit1->id))
                                break;
                        }
                        if (i < c.unit.unit.size())
                            continue;
                    }
                    else if (res_str.length() < c_str.length())
                        continue;
                }

                res = c;
                m = m2;
                s = c.unit.unit.size();
            }
        }

        cast_units[system] = _cast_units;

        return res;
    }

    Number CastToUnitImpl(const LogicalId id, const Number& val, const Unit& unit) const
    {
        if (val.unit == unit)
            return val;

        //cast to base units
        Number u(precision, 1);
        for (auto& p : unit.unit)
        {
            CheckBreak(parser_context);

            Number unit_val;
            if (unit.system == U"SI")
            {
                Unit* base_unit = FindBuiltinUnit(p.first);
                if (base_unit)
                    unit_val = Number(precision, *base_unit);
            }
            if (unit_val.unit.IsEmpty())
            {
                CustomUnit<Number>* custom_unit = FindUnit(p.first, unit.system);
                if (!custom_unit)
                    throw MathException(id, ParserExceptionCode::UnknownIdentifier);
                unit_val = custom_unit->value;
            }

            if (p.second == 1)
                u *= unit_val;
            else if (p.second == -1)
                u /= unit_val;
            else if (p.second > 1)
                u *= pow(unit_val, p.second);
            else
                u /= pow(unit_val, -p.second);
        }

        if (u.unit != val.unit)
            throw MathException(id, ParserExceptionCode::CannotCastToUnit);

        Number res = val / u;
        res.unit = unit;
        return res;
    }

    void AddDependency(const IdentifierNode<Number>& identifier) const
    {
        if (identifier.subscript.empty())
        {
            if (std::find(dependencies->begin(), dependencies->end(), identifier.name) == dependencies->end())
                dependencies->push_back(identifier.name);
        }
        else
        {
            auto name = identifier.name + U"{" + identifier.subscript + U"}";
            if (std::find(dependencies->begin(), dependencies->end(), name) == dependencies->end())
                dependencies->push_back(name);

            auto it = symbols->strings.find(identifier.subscript);
            if (it != symbols->strings.end())
            {
                if (std::find(dependencies->begin(), dependencies->end(), identifier.subscript) == dependencies->end())
                    dependencies->push_back(identifier.subscript);
            }
        }
    }

    bool FindAngleMeasure(const std::u32string& name, Number& res) const
    {
        return false;
    }

    Number FunctionCall(FunctionCallNode<Number> const& op) const
    {
        CheckBreak(parser_context);

        AddDependency(op.name);

        Number res;
        
        try
        {
            //find in the user defined functions
            FunctionNode<Number>* user_func = FindFunction(op);
            if (user_func)
            {
                IdentifierNodesIter func_iter = user_func->arguments.begin();
                for (ExpressionNodesIter iter = op.arguments.begin(); iter != op.arguments.end(); ++iter, ++func_iter)
                {
                    Number arg = (*this)(*iter);
                    PushTempVariable(func_iter->name, arg);
                }

                bool _exported_id = exported_id;
                exported_id = user_func->exported;
                try
                {
                    res = (*this)(user_func->return_expression);
                }
                catch (...)
                {
                    exported_id = _exported_id;
                    PopTempVariables(op.arguments.size());
                    throw;
                }
                exported_id = _exported_id;
                PopTempVariables(op.arguments.size());
                return res;
            }

            BuiltinTrigonometricFunction* t_func = FindTrigonometricFunction(op.name.name);
            if (t_func)
            {
                try
                {
                    TrigonometricFunction u = std::get<TrigonometricFunction>(*t_func);
                    if (op.arguments.size() != 1)
                        throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                    
                    ExpressionNodesIter iter = op.arguments.begin();
                    Number arg = (*this)(*iter);
                    if (arg.GetAngleMeasure() == AngleMeasure::None)
                        arg.SetAngleMeasure(default_angle_measure);
                    return (*u)(arg);
                }
                catch (const std::bad_variant_access&)
                {
                }
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
                    Number arg = (*this)(*iter);
                    return (*u)(arg);
                }
                catch (const std::bad_variant_access&)
                {
                }

                try
                {
                    ComplexUnaryFunction u = std::get<ComplexUnaryFunction>(*func);
                    if (op.arguments.size() != 1)
                        throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                    
                    ExpressionNodesIter iter = op.arguments.begin();
                    Number arg = (*this)(*iter);
                    if (arg.GetAngleMeasure() == AngleMeasure::None)
                        arg.SetAngleMeasure(default_angle_measure);
                    return (*u)(arg, res_pos);
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
                    Number arg1 = (*this)(*iter++);
                    Number arg2 = (*this)(*iter);
                    return (*b)(arg1, arg2);
                }
                catch (const std::bad_variant_access&)
                {
                }

                try
                {
                    ComplexBinaryFunction b = std::get<ComplexBinaryFunction>(*func);
                    if (op.arguments.size() != 2)
                        throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                    
                    ExpressionNodesIter iter = op.arguments.begin();
                    Number arg1 = (*this)(*iter++);
                    Number arg2 = (*this)(*iter);
                    return (*b)(arg1, arg2, res_pos);
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
                    Number arg1 = (*this)(*iter++);
                    Number arg2 = (*this)(*iter++);
                    Number arg3 = (*this)(*iter);
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
                    Number arg1 = (*this)(*iter++);
                    Number arg2 = (*this)(*iter++);
                    Number arg3 = (*this)(*iter++);
                    Number arg4 = (*this)(*iter);
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
        throw SyntaxException(op.id, UnknownIdentifier, op.pos, op.line);
    }

    Number NoFencesFunctionCall(NoFencesFunctionCallNode<Number> const& op) const
    {
        AddDependency(op.name);

        Number res;
        
        //find in the build-in functions		
        BuiltinFunction* func = FindBuiltinFunction(op.name.name);
        if (func)
        {
            try
            {
                BinaryFunction b = std::get<BinaryFunction>(*func);
                if (op.arguments.size() != 1)
                    throw SyntaxException(op.id, WrongArgumentsCount, U"Wrong arguments count in '" + op.name.name + U"'", op.pos, CallSize(op), op.line);
                
                ExpressionNodesIter iter = op.arguments.begin();
                Number arg1 = (*this)(*iter++);
                Number arg2 = (*this)(op.last_argument);
                return (*b)(arg1, arg2);
            }
            catch (std::bad_variant_access)
            {
            }
        }

        //there is no such a function
        throw SyntaxException(op.id, UnknownIdentifier, U"Identifier '" + op.name.name + U"' not found", op.pos, op.line);
    }

    template<typename Node>
    int CallSize(const Node& op) const
    {
        if (op.size > 0)
            return op.size;
        return static_cast<int>(op.name.name.length());
    }

    friend struct Expression<Number>;

    mutable LogicalId id;
    mutable int precision;
    mutable AngleMeasure default_angle_measure;
    mutable AngleMeasure result_angle_measure;
    mutable int default_notation = 10;
    Number left_value; //left solved value
    mutable Dependencies* dependencies = nullptr;
    mutable std::map<std::u32string, std::vector<Number>> cast_units;
    mutable bool exported_id = false;
    mutable std::u32string cur_subscript;

    int max_cast_unit_size = 2; //max size of each unit in the cast vector
};

//explicit specializations declared here so dependent TUs link to the definitions in solver.cpp instead of inlining the primary template
template<> Array<Real> Solver<Array<Real>>::operator()(LineGraphNode<Array<Real>> const& op) const;
template<> Real Solver<Real>::operator()(UnitNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(UnitNode<Rational> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(UnitNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(UnitNode<Integer> const& op) const;
template<> Integer Solver<Integer>::operator()(UnaryOperationNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(UnaryOperationNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(UnaryOperationNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(UnaryOperationNode<Complex> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(UnaryOperationNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(OperationNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(OperationNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(OperationNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(OperationNode<Complex> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(OperationNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(PostfixOperationNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(PostfixOperationNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(PostfixOperationNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(PostfixOperationNode<Complex> const& op) const;
template<> Symbolic<Real> Solver<Symbolic<Real>>::operator()(PostfixOperationNode<Symbolic<Real>> const& op) const;
template<> Symbolic<Rational> Solver<Symbolic<Rational>>::operator()(PostfixOperationNode<Symbolic<Rational>> const& op) const;
template<> Symbolic<Complex> Solver<Symbolic<Complex>>::operator()(PostfixOperationNode<Symbolic<Complex>> const& op) const;
template<> Integer Solver<Integer>::operator()(FunctionCallNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(FunctionCallNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(FunctionCallNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(FunctionCallNode<Complex> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(FunctionCallNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(FunctionCallStringNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(FunctionCallStringNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(FunctionCallStringNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(FunctionCallStringNode<Complex> const& op) const;
template<> VariableNode<Array<Real>>* Solver<Array<Real>>::FindVariable(const std::u32string& name, const std::u32string& subscript) const;
template<> Integer Solver<Integer>::operator()(NoFencesFunctionCallNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(NoFencesFunctionCallNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(NoFencesFunctionCallNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(NoFencesFunctionCallNode<Complex> const& op) const;
template<> Integer Solver<Integer>::operator()(IdentifierNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(IdentifierNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(IdentifierNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(IdentifierNode<Complex> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(IdentifierNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(ImplicitStringMulNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(ImplicitStringMulNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(ImplicitStringMulNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(ImplicitStringMulNode<Complex> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(ImplicitStringMulNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(ImplicitDivMulNode<Integer> const& op) const;
template<> Real Solver<Real>::operator()(ImplicitDivMulNode<Real> const& op) const;
template<> Rational Solver<Rational>::operator()(ImplicitDivMulNode<Rational> const& op) const;
template<> Complex Solver<Complex>::operator()(ImplicitDivMulNode<Complex> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(ImplicitDivMulNode<Array<Real>> const& op) const;
template<> Array<Real> Solver<Array<Real>>::operator()(ArrayNode<Array<Real>> const& op) const;
template<> Integer Solver<Integer>::operator()(ScriptNode<Integer> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const;
template<> Rational Solver<Rational>::operator()(ScriptNode<Rational> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const;
template<> Complex Solver<Complex>::operator()(ScriptNode<Complex> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const;
template<> Array<Real> Solver<Array<Real>>::operator()(ScriptNode<Array<Real>> const& script, LogicalId _id, AngleMeasure _default_angle_measure, 
    AngleMeasure _result_angle_measure, int _precision, Dependencies* _dependencies) const;
template<> Real Solver<Real>::GetSuitableUnit(const LogicalId _id, const Real& val, const std::u32string& system, const bool buildin) const;
template<> Rational Solver<Rational>::GetSuitableUnit(const LogicalId _id, const Rational& val, const std::u32string& system, const bool buildin) const;
template<> Integer Solver<Integer>::GetSuitableUnit(const LogicalId _id, const Integer& val, const std::u32string& system, const bool buildin) const;
template<> Complex Solver<Complex>::GetSuitableUnit(const LogicalId _id, const Complex& val, const std::u32string& system, const bool buildin) const;
template<> Symbolic<Real> Solver<Symbolic<Real>>::GetSuitableUnit(const LogicalId _id, const Symbolic<Real>& val, const std::u32string& system, 
    const bool buildin) const;
template<> Real Solver<Real>::CastToUnit(const LogicalId id, const Real& val, const Unit& unit) const;
template<> Rational Solver<Rational>::CastToUnit(const LogicalId id, const Rational& val, const Unit& unit) const;
template<> Integer Solver<Integer>::CastToUnit(const LogicalId id, const Integer& val, const Unit& unit) const;
template<> Complex Solver<Complex>::CastToUnit(const LogicalId id, const Complex& val, const Unit& unit) const;
template<> Symbolic<Real> Solver<Symbolic<Real>>::CastToUnit(const LogicalId id, const Symbolic<Real>& val, const Unit& unit) const;
template<> void Solver<Symbolic<Rational>>::FillBuiltinOperations();
template<> Symbolic<Rational> Solver<Symbolic<Rational>>::GetSuitableUnit(const LogicalId _id, const Symbolic<Rational>& val, const std::u32string& system, 
    const bool buildin) const;
template<> Symbolic<Rational> Solver<Symbolic<Rational>>::CastToUnit(const LogicalId id, const Symbolic<Rational>& val, const Unit& unit) const;
template<> void Solver<Symbolic<Complex>>::FillBuiltinOperations();
template<> Symbolic<Complex> Solver<Symbolic<Complex>>::GetSuitableUnit(const LogicalId _id, const Symbolic<Complex>& val, const std::u32string& system, 
    const bool buildin) const;
template<> Symbolic<Complex> Solver<Symbolic<Complex>>::CastToUnit(const LogicalId id, const Symbolic<Complex>& val, const Unit& unit) const;

};

#endif
