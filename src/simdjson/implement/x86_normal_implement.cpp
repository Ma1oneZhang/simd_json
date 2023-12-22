//
// Created by ziyang on 12/17/23.
//
#include <stdexcept>
#include <string_view>
#include "x86_implement.h"

namespace simdjson {
static inline Json parse_object(std::string_view& json, JsonParseError& error);
static inline Json parse_array(std::string_view& json, JsonParseError& error);
static inline Json parse_number(std::string_view&, JsonParseError& error);
static inline Json parse_string(std::string_view&, JsonParseError& error);
static inline Json parse_bool(std::string_view&, JsonParseError& error);
static inline Json parse_null(std::string_view&, JsonParseError& error);
static inline Json parse_normal_impl(std::string_view&, JsonParseError&);
static inline std::string_view skip_whitespace(std::string_view& json);

static inline Json parse_normal_impl(std::string_view& json,
                                     JsonParseError& error) {
  json = skip_whitespace(json);
  if (json.empty()) {
    error = "Empty json";
    return Json(error);
  }
  switch (json[0]) {
    case '{': {
      json.remove_prefix(1);
      return parse_object(json, error);
    }
    case '[': {
      json.remove_prefix(1);
      return parse_array(json, error);
    }
    case '\"': {
      json.remove_prefix(1);
      return parse_string(json, error);
    }
    case 't':
    case 'f':
      return parse_bool(json, error);
    case 'n':
      return parse_null(json, error);
    default:
      return parse_number(json, error);
  }
}

static inline std::string_view skip_whitespace(std::string_view& json) {
  while (!json.empty() && (json[0] == ' ' || json[0] == '\n' ||
                           json[0] == '\r' || json[0] == '\t')) {
    json.remove_prefix(1);
  }
  return json;
}

static inline Json parse_object(std::string_view& json, JsonParseError& error) {
  JsonObject obj;
  std::string_view key{};
  Json value{};
  json = skip_whitespace(json);
  while (json.size() > 0) {
    if (json[0] == '}') {
      json.remove_prefix(1);
      return Json(obj);
    }
    if (json[0] != '\"') {
      error = "Expected '\"' in object";
      return Json(error);
    }
    json.remove_prefix(1);
    key = json.substr(0, json.find_first_of('\"'));
    json.remove_prefix(key.size() + 1);
    json = skip_whitespace(json);
    if (json[0] != ':') {
      error = "Expected ':' in object";
      return Json(error);
    }
    json.remove_prefix(1);
    json = skip_whitespace(json);
    value = parse_normal_impl(json, error);
    if (value.is_error()) {
      return Json(error);
    }
    obj[std::string(key)] = value;
    json = skip_whitespace(json);
    if (json[0] == ',') {
      json.remove_prefix(1);
      json = skip_whitespace(json);
    } else if (json[0] == '}') {
      json.remove_prefix(1);
      return Json(obj);
    } else {
      error = "Unexpected charater squence in object";
      return Json(error);
    }
  }
  // never use
  return {};
}

static inline Json parse_array(std::string_view& json, JsonParseError& error) {
  JsonArray arr;
  Json value{};
  json = skip_whitespace(json);
  while (!json.empty()) {
    if (json[0] == ']') {
      json.remove_prefix(1);
      return Json(arr);
    }
    value = parse_normal_impl(json, error);
    if (value.is_error()) {
      return Json(error);
    }
    arr.push_back(value);
    json = skip_whitespace(json);
    if (json[0] == ',') {
      json.remove_prefix(1);
      json = skip_whitespace(json);
    }
  }
  return Json(JsonParseError("Expected ']' in array"));
}

static inline Json parse_null(std::string_view& json, JsonParseError& error) {
  if (json.size() < 4) {
    error = "Expected 'null'";
    return Json(error);
  }
  if (json[0] == 'n' && json[1] == 'u' && json[2] == 'l' && json[3] == 'l') {
    json.remove_prefix(sizeof("null") - 1);
    return Json(NULL_T{});
  }
  error = "Expected 'null'";
  return Json(error);
}

static inline Json parse_bool(std::string_view& json, JsonParseError& error) {
  if (json.size() < 4) {
    error = "Expected \'true\' or \'false\'";
    return Json(error);
  }
  if (json[0] == 't' && json[1] == 'r' && json[2] == 'u' && json[3] == 'e') {
    json.remove_prefix(sizeof("true") - 1);
    return Json(JsonValue(true));
  }
  if (json.size() >= 5 && json[0] == 'f' && json[1] == 'a' && json[2] == 'l' &&
      json[3] == 's' && json[4] == 'e') {
    json.remove_prefix(sizeof("false") - 1);
    return Json(JsonValue(false));
  }
  error = "Expected 'true' or 'false'";
  return Json(error);
}

static inline Json parse_string(std::string_view& json, JsonParseError& error) {
  if (json.empty()) {
    error = "Expected '\"' in string";
    return Json(error);
  }
  const auto end = json.find_first_of('\"');
  if (end == std::string_view::npos) {
    error = "Expected '\"' in string";
    return Json(error);
  }
  const auto str = json.substr(0, end);
  json.remove_prefix(end + 1);
  return Json(JsonValue(std::string(str)));
}

static inline Json parse_number(std::string_view& json, JsonParseError& error) {
  if (json.empty()) {
    error = "Expected number";
    return Json(error);
  }
  std::string_view str;
  const auto end = json.find_first_not_of(
      "0123456789+-.eE");  // TODO: check if this is the correct way to parse
  if (end == std::string_view::npos) {
    str = json;
    json.remove_prefix(json.size());
  } else {
    str = json.substr(0, end);
    json.remove_prefix(end);
  }
  if (str.find_first_not_of("0123456789+-.eE") != std::string_view::npos) {
    error = "Expected number";
    return Json(error);
  }
  const auto current_ending = str.find_first_of(".eE");
  if (current_ending != std::string_view::npos) {
    try {
      return Json(std::stod(std::string(str)));
    } catch ([[maybe_unused]] std::exception& e) {
      error = "Expected number";
      return Json(error);
    }
  }
  try {
    return Json(std::stoll(std::string(str)));
  } catch ([[maybe_unused]] std::out_of_range& e) {
    return Json(std::stod(std::string(str)));
  } catch ([[maybe_unused]] std::exception& e) {
    error = "Expected number";
    return Json(error);
    // _mm_set1_pi16(short w)
  }
}

Json x86_implement::parse_normal_impl(const std::string& json) {
  JsonParseError error;
  std::string_view json_view(json);
  return ::simdjson::parse_normal_impl(json_view, error);
}
}  // namespace simdjson