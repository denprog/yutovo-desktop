/*
 * Yutovo Calculator
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "ast.h"
#include "expression.h"

namespace yutovo_calculator
{

template<typename Number>
struct Graph : qi::grammar<std::u32string::iterator, GraphNode<Number>(), unicode::space_type>
{
    Graph(LogicalId id, std::u32string& expr, Solver<Number>* solver) :
        Graph::base_type(graph),
        expression(id, expr, solver)
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
        qi::_1_type _1;
        qi::_3_type _3;

        graph = line_graph | bar_graph;

        line_graph = "graph_line(" > expression > ',' > identifier > ',' > expression > ',' > expression > ',' > 
            expression > ',' > expression > ',' > number > ')';

        bar_graph = "graph_bar(" > expression > ',' > number > ')';
        
        identifier = name >> -('{' > (+char_("0-9") | name) > '}') >> -('`' > description > '`');
        
        name = (raw[lexeme[(alpha | char_(U'_')) >> *(alnum | char_(U'_'))]]);

        description = raw[lexeme[+(alnum | '_' | ' ')]];

        number = exp_number | digits_number;

        digits_number = +char_("0-9.");

        exp_number = +char_("0-9.") >> raw[lexeme[(no_case[char_("E")] > (char_('+') | char_('-')))]] > +(char_("0-9"));

        on_success(line_graph, 
            boost::phoenix::function<Annotation<Number>>(Annotation<Number>(expr.begin(), expr.end(), id, 
            &solver->parser_context))(qi::_val, _1));

        on_success(identifier, 
            boost::phoenix::function<Annotation<Number>>(Annotation<Number>(expr.begin(), expr.end(), id, 
            &solver->parser_context))(qi::_val, _1));

        on_success(number, 
            boost::phoenix::function<Annotation<Number>>(Annotation<Number>(expr.begin(), expr.end(), id, 
            &solver->parser_context))(qi::_val, _1));

        on_error<fail>(graph, 
            boost::phoenix::function<ErrorHandler<SyntaxException>>(ErrorHandler<SyntaxException>(id, expr.begin(), expr.end(), SyntaxError))(_3));

        // BOOST_SPIRIT_DEBUG_NODE(graph);
        // BOOST_SPIRIT_DEBUG_NODE(line_graph);
        // BOOST_SPIRIT_DEBUG_NODE(bar_graph);
        // BOOST_SPIRIT_DEBUG_NODE(identifier);
        // BOOST_SPIRIT_DEBUG_NODE(number);
    }

    qi::rule<std::u32string::iterator, GraphNode<Number>(), unicode::space_type> graph;
    qi::rule<std::u32string::iterator, LineGraphNode<Number>(), unicode::space_type> line_graph;
    qi::rule<std::u32string::iterator, BarGraphNode<Number>(), unicode::space_type> bar_graph;
    qi::rule<std::u32string::iterator, std::u32string(), unicode::space_type> name, description;
    qi::rule<std::u32string::iterator, IdentifierNode<Number>(), unicode::space_type> identifier;
    qi::rule<std::u32string::iterator, std::u32string(), unicode::space_type> digits_number, exp_number;
    qi::rule<std::u32string::iterator, NumberNode<Number>(), unicode::space_type> number;

    Expression<Number> expression;
};

}

#endif
