/*
 * Yutovo Editor
 * Copyright (C) 2022-2026 Yutovo developers. All rights reserved.
 * This file is a part of the Yutovo project
 * SPDX-License-Identifier: GPL-3.0-only
 */

#include "result_codes.h"

namespace yutovo
{

std::string Result::ToString()
{
    std::string r = "result_type:" + ResultTypeToString(type) + ",";
    size_t i = 0;
    r += "values:{";
    for (auto& v : values)
    {
        if (i++ > 0)
            r += ",";
        r += v.value["mantissa"] + "E" + v.value["exponent"] + ",";
        r += "angle_measure:" + AngleMeasureToString(v.angle_measure) + ",";
        r += "notation:" + NotationToString(v.notation) + ",";
        if (!v.unit.IsEmpty())
            r += "unit:" + ToBasicString(v.unit.ToString()) + ",";
    }
    r += "},";
    if (error.error_code != yutovo_solver::ErrorCode::OK)
        r += "error:" + ErrorCodeToString(error.error_code) + ",";
    if (error.parser_error_code != yutovo_calculator::ParserExceptionCode::None)
    {
        r += "parser_error:" + ErrorCodeToString(error.parser_error_code) + ",pos:" + std::to_string(error.pos) + ",size:" + std::to_string(error.size) + 
            ",line:" + std::to_string(error.line) + ",";
    }
    if (!error.description.empty())
        r += "description:" + ToBasicString(error.description);

    return r;
}

}
