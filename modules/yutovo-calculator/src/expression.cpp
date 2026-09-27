/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "stdafx.h"
#include "expression.h"
#include "solver.h"

namespace yutovo_calculator
{

//Expression

template<>
Expression<Integer>::Expression(LogicalId id, std::u32string& expr, Solver<Integer>* _solver) : 
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::on_success;
    using boost::spirit::qi::fail;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();
    
    addition = multiplication >> *((char_('+') > multiplication) | (char_('-') > multiplication));
    
    multiplication = unary >> *(multiply);

    multiply = char_(U'*') > unary | char_(U'/') > unary | char_(U'∧') > unary | char_(U'∨') > unary | char_(U'⊕') > unary;

    if (solver->default_notation == 16)
    {
        unary = loop | array | compare | postfix_operation | implicit_mul | function_call | function_call_string | 
            no_fences_function_call | number | identifier | implicit_string_mul | unary_operation | '(' > expression > ')';
    }
    else
    {
        unary = loop | array | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | implicit_string_mul | implicit_mul | 
            number | function_call | function_call_string | identifier | no_fences_function_call | unary_operation | '(' > expression > ')';
    }
    
    if (solver->default_notation == 16)	
        number = hex_number;
    else
        number = dec_number;
    
    hex_number = +char_("0-9a-fA-F");

    dec_number = +char_("0-9");

    identifier = name >> -('{' > (dec_number | name) > '}');
    
    name = raw[lexeme[(alpha | char_(U'∞') | '_') >> *(alnum | '_')]];

    unary_operation = (char_(U'+') > unary) | (char_(U'-') > unary) | (char_(U'¬') > unary);

    postfix_operation = (identifier >> char_(U'!')) | ((number | '(' > expression > ')') >> char_(U'!'));

    implicit_string_mul = (number >> identifier);
    
    implicit_mul = number >> '(' >> expression > ')';

    implicit_function_mul = dec_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    function_call = identifier >> '(' >> -(expression % ',') > ')';

    function_call_string = identifier >> '[' >> +(alnum | '_') > ']';

    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);
    
    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] | 
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with an expression's position
    on_success(unary, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(addition, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(postfix_operation, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call_string, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(compare, 
        boost::phoenix::function<Annotation<yutovo_calculator::Integer>>(Annotation<yutovo_calculator::Integer>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    
    //work out the exceptions
    on_error<fail>(expression, 
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));
    
    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(unary);
    // BOOST_SPIRIT_DEBUG_NODE(unary_operation);
    // BOOST_SPIRIT_DEBUG_NODE(postfix_operation);
}

template<>
Expression<Real>::Expression(LogicalId id, std::u32string& expr, Solver<Real>* _solver) : 
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();
    
    addition = multiplication >> *((char_(U'+') > multiplication) | (char_(U'-') > multiplication));
    
    multiplication = unary >> *(multiply);

    multiply = char_(U'*') > unary | char_(U'/') > unary | char_(U'%') >> unary;

    unary = hold[derivative_at_point] | definite_integral | loop | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | 
        implicit_div_mul | implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | 
        no_fences_function_call | identifier | unary_operation | '(' > expression > ')';
    
    number = exp_number | digits_number;
    
    digits_number = (+char_(U'0', U'9') >> -(char_(U'.') >> *char_(U'0', U'9'))) | (char_(U'.') >> +char_(U'0', U'9'));

    integer_number_str = +char_(U'0', U'9');

    integer_number = integer_number_str;

    mixed_division = integer_number >> '(' >> integer_number >> '/' >> integer_number > ')';

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    real_number = digits_number;

    exp_number = digits_number >> raw[lexeme[no_case[char_(U'E')] >> (char_(U'+') | char_(U'-'))]] > +(char_(U'0', U'9'));

    identifier = name >> -('{' > (integer_number_str | name) > '}');

    implicit_div_mul = '(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = real_number >> '(' >> expression > ')';

    //0x00B0 = U'°', 39 = U'\'', 0x20BD = U'₽', 0x00A2 = U'¢', 0x0024 = U'$', 0x20AC = U'€', 0x00A5 = U'¥', 0x20B9 = U'₹'
    name = (raw[lexeme[(alpha | char_(U'∞') | char_(0x00B0) | char_(39) | char_(U'_') | char_(0x20BD) | char_(0x0024) | char_(0x00A2) | 
        char_(0x20AC) | char_(0x00A5) | char_(0x20B9)) >> *(alnum | char_(39) | char_(U'_') | char_(0x0024))]]);

    unary_operation = (char_(U'+') > unary) | (char_(U'-') > unary);

    postfix_operation = (identifier >> char_('!')) | ((number | '(' > expression > ')') >> char_('!'));

    symbolic_body = +((char_(U'(') >> *symbolic_body >> char_(U')')) | (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')));

    symbolic_arg = raw[+( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')))];

    definite_integral = boost::spirit::qi::lit("definite_integral") >>
        '(' > expression > ',' > expression > ',' > symbolic_arg > ',' > name > ')';

    derivative_variable = name >> '=' > expression;
    
    derivative_at_point = lit("derivative") >> '(' >> symbolic_arg >> ',' >> '[' >> derivative_variable % ',' >> ']' >> ')';

    function_call = identifier >> '(' >> -(expression % ',') > ')';
    
    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);
    
    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] | 
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    variable = identifier >> ('=' > expression);

    //annotate the items with the expression's position
    on_success(unary, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(addition, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(definite_integral, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(derivative_at_point, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(compare, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(loop, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(variable, 
        boost::phoenix::function<Annotation<yutovo_calculator::Real>>(Annotation<yutovo_calculator::Real>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    
    on_error<fail>(expression, 
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));
    
    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(unary);
    // BOOST_SPIRIT_DEBUG_NODE(derivative_at_point);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(multiply);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(unary_operation);
    // BOOST_SPIRIT_DEBUG_NODE(postfix_operation);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_string_mul);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(function_param);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_div_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_function_mul);
    // BOOST_SPIRIT_DEBUG_NODE(compare);
    // BOOST_SPIRIT_DEBUG_NODE(loop);
    // BOOST_SPIRIT_DEBUG_NODE(variable);
    // BOOST_SPIRIT_DEBUG_NODE(name);
}

template<>
Expression<yutovo_calculator::Rational>::Expression(LogicalId id, std::u32string& expr, Solver<Rational>* _solver) : 
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();
    
    addition = multiplication >> *((char_('+') > multiplication) | (char_('-') > multiplication));
    
    multiplication = (unary >> *(multiply));

    multiply = (char_('*') > unary) | (char_('/') > unary) | (char_('%') > unary);

    symbolic_body = +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')) );

    symbolic_arg = raw[ +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')) ) ];

    definite_integral = boost::spirit::qi::lit("definite_integral") >>
        '(' > expression > ',' > expression > ',' > symbolic_arg > ',' > name > ')';

    derivative_variable = name >> '=' > expression;
    
    derivative_at_point = lit("derivative") >> '(' >> symbolic_arg >> ',' >> '[' >> derivative_variable % ',' >> ']' >> ')';
    
    unary = hold[derivative_at_point] | definite_integral | loop | array | compare | implicit_function_mul | implicit_post_function_mul | implicit_div_mul | 
        implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | identifier | 
        no_fences_function_call | unary_operation | '(' > expression > ')';
    
    number = digits_number;

    mixed_division = (number >> '(' >> number >> '/' >> number >> ')');

    implicit_div_mul = ('(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier);

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = number >> '(' >> expression > ')';

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;
    
    digits_number = (+char_("0-9") >> -(char_('.') >> *char_("0-9"))) | (char_('.') >> +char_("0-9"));

    number = digits_number;

    identifier = name >> -('{' > (digits_number | name) > '}');
    
    //0x00B0 = U'°', 39 = U'\'', 0x20BD = U'₽', 0x00A2 = U'¢', 0x0024 = U'$', 0x00A2 = U'€', 0x00A5 = U'¥', 0x20B9 = U'₹'
    name = (raw[lexeme[(alpha | char_(U'∞') | char_(0x00B0) | char_(39) | char_(U'_') | char_(0x20BD) | char_(0x0024) | char_(0x00A2) | 
        char_(0x20AC) | char_(0x00A5) | char_(0x20B9)) >> *(alnum | char_(39) | char_(U'_') | char_(0x0024))]]);

    unary_operation = (char_('+') > unary) | (char_('-') > unary);
    
    function_call = (identifier >> '(' >> -(expression % ',') > ')');

    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);
    
    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] | 
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with an expression's position
    on_success(unary, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(addition, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(multiplication, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(multiply, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(identifier, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul, boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), 
        expr.end(), id, &solver->parser_context))(qi::_val, _1));
    on_success(compare, 
        boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(definite_integral,
        boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(derivative_at_point,
        boost::phoenix::function<Annotation<yutovo_calculator::Rational>>(Annotation<yutovo_calculator::Rational>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));

    //work out the exceptions
    on_error<fail>(expression, 
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));
    
    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
}

template<>
Expression<Complex>::Expression(LogicalId id, std::u32string& expr, Solver<Complex>* _solver) : 
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();
    
    addition = multiplication >> *((char_('+') > multiplication) | (char_('-') > multiplication));
    
    multiplication = unary >> *(multiply);

    multiply = char_('*') > unary | char_('/') > unary | char_('%') > unary;

    unary = hold[derivative_at_point] | definite_integral | loop | array | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | 
        implicit_div_mul | implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | 
        no_fences_function_call | identifier | unary_operation | '(' > expression > ')';
    
    number = exp_number | digits_number;
    
    digits_number = (+char_("0-9") >> -(char_('.') >> *char_("0-9"))) | (char_('.') >> +char_("0-9"));

    integer_number_str = +char_("0-9");

    integer_number = integer_number_str;

    mixed_division = integer_number >> '(' >> integer_number >> '/' >> integer_number > ')';

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    real_number = digits_number;

    exp_number = digits_number >> raw[lexeme[(no_case[char_("E")] > (char_('+') | char_('-')))]] > +(char_("0-9"));

    identifier = name >> -('{' > (integer_number_str | name) > '}');

    implicit_div_mul = '(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = real_number >> '(' >> expression > ')';

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    name = raw[lexeme[(alpha | '_') >> *(alnum | '_')]];

    unary_operation = (char_('+') > unary) | (char_('-') > unary);

    postfix_operation = ((number | '(' > expression > ')') >> char_('!'));
    
    symbolic_body = +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')) );

    symbolic_arg = raw[ +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')) ) ];

    definite_integral = lit("definite_integral") >> '(' > expression > ',' > expression > ',' > symbolic_arg > ',' > name > ')';

    derivative_variable = name >> '=' > expression;

    derivative_at_point = lit("derivative") >> '(' >> symbolic_arg >> ',' >> '[' >> derivative_variable % ',' >> ']' >> ')';

    function_call = identifier >> '(' >> -(expression % ',') > ')';
    
    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);
    
    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] | 
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with the expression's position
    on_success(unary, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(addition, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(definite_integral, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(derivative_at_point, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(compare, 
        boost::phoenix::function<Annotation<yutovo_calculator::Complex>>(Annotation<yutovo_calculator::Complex>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    
    on_error<fail>(expression, 
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));
    
    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_div_mul);
}

template<>
Expression<Array<Real>>::Expression(LogicalId id, std::u32string& expr, Solver<Array<Real>>* _solver) : 
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();
    
    addition = multiplication >> *((char_('+') > multiplication) | (char_('-') > multiplication));
    
    multiplication = unary >> *(multiply);

    multiply = char_('*') > unary | char_('/') > unary | char_('%') >> unary;
    
    unary = definite_integral | loop | array | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | 
        implicit_div_mul | implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | 
        no_fences_function_call | identifier | unary_operation | '(' > expression > ')';
    
    number = exp_number | digits_number;
    
    number = exp_number | digits_number;
    
    digits_number = (+char_("0-9") >> -(char_('.') >> *char_("0-9"))) | (char_('.') >> +char_("0-9"));

    integer_number_str = +char_("0-9");

    integer_number = integer_number_str;

    mixed_division = integer_number >> '(' >> integer_number >> '/' >> integer_number > ')';

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    real_number = digits_number;

    exp_number = digits_number >> raw[lexeme[(no_case[char_("E")] > (char_('+') | char_('-')))]] > +(char_("0-9"));

    identifier = name >> -('{' > (integer_number_str | name) > '}');

    implicit_div_mul = '(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = real_number >> '(' >> expression > ')';

    //0x00B0 = U'°', 39 = U'\'', 0x20BD = U'₽', 0x00A2 = U'¢', 0x0024 = U'$', 0x00A2 = U'€', 0x00A5 = U'¥', 0x20B9 = U'₹'
    name = (raw[lexeme[(alpha | char_(0x00B0) | char_(U'\'') | char_(U'_') | char_(0x20BD) | char_(U'$') | char_(0x00A2) | char_(0x20AC) | char_(0x00A5) |
        char_(0x20B9)) >> *(alnum | char_(U'\'') | char_(U'_') | char_(U'$'))]]);

    unary_operation = (char_('+') > unary) | (char_('-') > unary);

    postfix_operation = (identifier >> char_('!')) | ((number | '(' > expression > ')') >> char_('!'));
    
    symbolic_body = +((char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')));

    symbolic_arg = raw[+( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')))];

    definite_integral = lit("definite_integral") >> '(' > expression > ',' > expression > ',' > symbolic_arg > ',' > name > ')';

    function_call = identifier >> '(' >> -(expression % ',') > ')';
    
    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);
    
    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] | 
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with the expression's position
    on_success(unary, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(addition, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(definite_integral, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(compare, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(loop, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    on_success(variable, 
        boost::phoenix::function<Annotation<yutovo_calculator::Array<Real>>>(Annotation<yutovo_calculator::Array<Real>>(expr.begin(), expr.end(), id, 
        &solver->parser_context))(qi::_val, _1));
    
    on_error<fail>(expression, 
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));
    
    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(unary_operation);
    // BOOST_SPIRIT_DEBUG_NODE(postfix_operation);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(function_param);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_div_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_function_mul);
    // BOOST_SPIRIT_DEBUG_NODE(compare);
    // BOOST_SPIRIT_DEBUG_NODE(loop);
    // BOOST_SPIRIT_DEBUG_NODE(variable);
}

template<>
Expression<Symbolic<Real>>::Expression(LogicalId id, std::u32string& expr, Solver<Symbolic<Real>>* _solver) :
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();

    addition = multiplication >> *((char_(U'+') > multiplication) | (char_(U'-') > multiplication));

    multiplication = unary >> *(multiply);

    multiply = char_('*') > unary | char_('/') > unary | char_('%') >> unary;
    
    unary = hold[derivative_at_point] | loop | array | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | implicit_div_mul |
        implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | no_fences_function_call | identifier |
        unary_operation | '(' > expression > ')';

    number = exp_number | digits_number;

    digits_number = (+char_(U'0', U'9') >> -(char_(U'.') >> *char_(U'0', U'9'))) | (char_(U'.') >> +char_(U'0', U'9'));

    integer_number_str = +char_(U'0', U'9');

    integer_number = integer_number_str;

    mixed_division = integer_number >> '(' >> integer_number >> '/' >> integer_number > ')';

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    real_number = digits_number;

    exp_number = digits_number >> raw[lexeme[no_case[char_(U'E')] >> (char_(U'+') | char_(U'-'))]] > +(char_(U'0', U'9'));

    identifier = (name | raw[lexeme[char_(U'∞')]]) >> -('{' > (integer_number_str | name) > '}');

    implicit_div_mul = '(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = real_number >> '(' >> expression > ')';

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    name = raw[lexeme[(alpha | char_(U'_')) >> *(alnum | char_(U'_'))]];

    unary_operation = (char_(U'+') > unary) | (char_(U'-') > unary);

    postfix_operation = (identifier >> char_(U'!')) | ((number | '(' > expression > ')') >> char_(U'!'));

    function_call = identifier >> '(' >> -(expression % ',') > ')';

    symbolic_body = +((char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')));

    symbolic_arg = raw[+( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')) )];

    derivative_variable = name >> '=' > expression;

    derivative_at_point = lit("derivative") >> '(' >> symbolic_arg >> ',' >> '[' >> derivative_variable % ',' >> ']' >> ')';

    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);

    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] |
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with the expression's position
    on_success(unary,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(addition,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(derivative_at_point,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(compare,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(loop,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(variable,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(addition,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(compare,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(loop,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(variable,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Real>>>(Annotation<yutovo_calculator::Symbolic<Real>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));

    on_error<fail>(expression,
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));

    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_div_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_function_mul);
    // BOOST_SPIRIT_DEBUG_NODE(compare);
    // BOOST_SPIRIT_DEBUG_NODE(loop);
    // BOOST_SPIRIT_DEBUG_NODE(variable);
}

template<>
Expression<Symbolic<Rational>>::Expression(LogicalId id, std::u32string& expr, Solver<Symbolic<Rational>>* _solver) :
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();

    addition = multiplication >> *((char_(U'+') > multiplication) | (char_(U'-') > multiplication));

    multiplication = unary >> *(multiply);

    multiply = char_('*') > unary | char_('/') > unary | char_('%') >> unary;

    unary = hold[derivative_at_point] | loop | array | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | implicit_div_mul |
        implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | no_fences_function_call | identifier |
        unary_operation | '(' > expression > ')';

    number = exp_number | digits_number;

    digits_number = (+char_(U'0', U'9') >> -(char_(U'.') >> *char_(U'0', U'9'))) | (char_(U'.') >> +char_(U'0', U'9'));

    integer_number_str = +char_(U'0', U'9');

    integer_number = integer_number_str;

    mixed_division = integer_number >> '(' >> integer_number >> '/' >> integer_number > ')';

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    real_number = digits_number;

    exp_number = digits_number >> raw[lexeme[no_case[char_(U'E')] >> (char_(U'+') | char_(U'-'))]] > +(char_(U'0', U'9'));

    identifier = (name | raw[lexeme[char_(U'∞')]]) >> -('{' > (integer_number_str | name) > '}');

    implicit_div_mul = '(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = real_number >> '(' >> expression > ')';

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    name = raw[lexeme[(alpha | char_(U'_')) >> *(alnum | char_(U'_'))]];

    unary_operation = (char_(U'+') > unary) | (char_(U'-') > unary);

    postfix_operation = (identifier >> char_(U'!')) | ((number | '(' > expression > ')') >> char_(U'!'));

    function_call = identifier >> '(' >> -(expression % ',') > ')';

    symbolic_body = +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')) );

    symbolic_arg = raw[ +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')) ) ];

    derivative_variable = name >> '=' > expression;

    derivative_at_point = lit("derivative") >> '(' >> symbolic_arg >> ',' >> '[' >> derivative_variable % ',' >> ']' >> ')';

    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);

    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] |
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with the expression's position
    on_success(unary,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(addition,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(derivative_at_point,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(compare,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(loop,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(variable,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Rational>>>(Annotation<yutovo_calculator::Symbolic<Rational>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));

    on_error<fail>(expression,
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));

    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_div_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_function_mul);
    // BOOST_SPIRIT_DEBUG_NODE(compare);
    // BOOST_SPIRIT_DEBUG_NODE(loop);
    // BOOST_SPIRIT_DEBUG_NODE(variable);
}

template<>
Expression<Symbolic<Complex>>::Expression(LogicalId id, std::u32string& expr, Solver<Symbolic<Complex>>* _solver) :
    Expression::base_type(expression),
    solver(_solver)
{
    using unicode::char_;
    using boost::spirit::qi::raw;
    using boost::spirit::qi::lexeme;
    using unicode::alnum;
    using unicode::alpha;
    using boost::spirit::qi::omit;
    using boost::spirit::qi::no_case;
    using boost::spirit::qi::lit;
    using boost::spirit::qi::hold;
    using boost::spirit::qi::on_error;
    using boost::spirit::qi::fail;
    using boost::phoenix::function;
    using namespace boost::phoenix::arg_names;
    qi::_1_type _1;
    qi::_3_type _3;

    expression = addition.alias();

    addition = multiplication >> *((char_(U'+') > multiplication) | (char_(U'-') > multiplication));

    multiplication = unary >> *(multiply);

    multiply = char_('*') > unary | char_('/') > unary | char_('%') >> unary;

    unary = hold[derivative_at_point] | loop | array | compare | implicit_function_mul | implicit_post_function_mul | postfix_operation | implicit_div_mul |
        implicit_string_mul | implicit_fraction_mul | mixed_division | implicit_mul | number | function_call | no_fences_function_call | identifier |
        unary_operation | '(' > expression > ')';

    number = exp_number | digits_number;

    digits_number = (+char_(U'0', U'9') >> -(char_(U'.') >> *char_(U'0', U'9'))) | (char_(U'.') >> +char_(U'0', U'9'));

    integer_number_str = +char_(U'0', U'9');

    integer_number = integer_number_str;

    mixed_division = integer_number >> '(' >> integer_number >> '/' >> integer_number > ')';

    implicit_fraction_mul = '(' >> number >> '/' >> number >> ')' >> identifier;

    real_number = digits_number;

    exp_number = digits_number >> raw[lexeme[no_case[char_(U'E')] >> (char_(U'+') | char_(U'-'))]] > +(char_(U'0', U'9'));

    identifier = (name | raw[lexeme[char_(U'∞')]]) >> -('{' > (integer_number_str | name) > '}');

    implicit_div_mul = '(' >> expression >> ')' >> '/' >> '(' >> expression >> ')' >> identifier;

    implicit_string_mul = (number >> identifier);

    implicit_mul = real_number >> '(' >> expression > ')';

    implicit_function_mul = digits_number >> function_call;

    implicit_post_function_mul = function_call >> identifier;

    name = raw[lexeme[(alpha | char_(U'_')) >> *(alnum | char_(U'_'))]];

    unary_operation = (char_(U'+') > unary) | (char_(U'-') > unary);

    postfix_operation = (identifier >> char_(U'!')) | ((number | '(' > expression > ')') >> char_(U'!'));

    function_call = identifier >> '(' >> -(expression % ',') > ')';

    symbolic_body = +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U'(') - unicode::char_(U')')) );

    symbolic_arg = raw[ +( (char_(U'(') >> *symbolic_body >> char_(U')')) |
        (unicode::char_ - unicode::char_(U',') - unicode::char_(U'(') - unicode::char_(U')')) ) ];

    derivative_variable = name >> '=' > expression;
    
    derivative_at_point = lit("derivative") >> '(' >> symbolic_arg >> ',' >> '[' >> derivative_variable % ',' >> ']' >> ')';

    no_fences_function_call = (identifier >> ':' >> *(expression >> omit[',']) >> function_param);

    function_param = number | identifier | '(' > expression > ')';

    compare = '(' >> expression >> (raw[lexeme["<>"]] | raw[lexeme["=="]] | raw[lexeme["<="]] | raw[lexeme[">="]] |
        raw[lexeme["<"]] | raw[lexeme[">"]]) >> expression >> ')';

    loop = "loop(" > variable > ',' > compare > ',' > variable > ',' > variable > ',' > variable > ')';

    array = '[' >> -(expression % ',') > ']';

    variable = identifier >> ('=' > expression);

    //annotate the items with the expression's position
    on_success(unary,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(addition,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiplication,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(multiply,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(derivative_at_point,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(no_fences_function_call,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(identifier,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(mixed_division,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_string_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_div_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(implicit_fraction_mul,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(compare,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(loop,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));
    on_success(variable,
        boost::phoenix::function<Annotation<yutovo_calculator::Symbolic<Complex>>>(Annotation<yutovo_calculator::Symbolic<Complex>>(expr.begin(), expr.end(), id,
        &solver->parser_context))(qi::_val, _1));

    on_error<fail>(expression,
        boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));

    // BOOST_SPIRIT_DEBUG_NODE(expression);
    // BOOST_SPIRIT_DEBUG_NODE(addition);
    // BOOST_SPIRIT_DEBUG_NODE(multiplication);
    // BOOST_SPIRIT_DEBUG_NODE(mixed_division);
    // BOOST_SPIRIT_DEBUG_NODE(number);
    // BOOST_SPIRIT_DEBUG_NODE(function_call);
    // BOOST_SPIRIT_DEBUG_NODE(identifier);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_div_mul);
    // BOOST_SPIRIT_DEBUG_NODE(implicit_function_mul);
    // BOOST_SPIRIT_DEBUG_NODE(compare);
    // BOOST_SPIRIT_DEBUG_NODE(loop);
    // BOOST_SPIRIT_DEBUG_NODE(variable);
}


};
