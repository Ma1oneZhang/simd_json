//
// Created by zzy on 12/17/23.
//

#ifndef JSON_H
#define JSON_H

#include "implement/x86_implement.h"
#include "internal.h"
#include "result.h"

namespace simdjson {

using JsonParser = JsonParserBase<x86_implement>;
using simdjson::Json;

}  // namespace simdjson
#endif  // JSON_H
