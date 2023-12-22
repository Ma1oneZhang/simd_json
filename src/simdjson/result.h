//
// Created by ziyang on 12/17/23.
//
#ifndef RESULT_H
#define RESULT_H

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace simdjson {

class Json;
class NULL_T {
  bool operator==(const NULL_T&) const { return true; }
  bool operator<(const NULL_T&) const { return false; }
};

using JsonObject = std::unordered_map<std::string, Json>;
using JsonArray = std::vector<Json>;
using JsonValue = std::variant<std::string, int64_t, double, bool, JsonObject,
                               JsonArray, NULL_T>;
using JsonParseError = std::string;

class Json {
 public:
  // from other result
  Json() : _result(std::make_unique<JsonValue>()) {}
  Json(const Json& other)
      : _result(std::make_unique<JsonValue>(*other._result)) {}
  Json(Json&& other) noexcept { _result = std::move(other._result); }
  Json& operator=(const Json& other) {
    _result = std::make_unique<JsonValue>(*other._result);
    return *this;
  }
  Json& operator=(Json&& other) noexcept {
    _result = std::move(other._result);
    return *this;
  }

  // from value and error
  explicit Json(const JsonValue& value)
      : _result(std::make_unique<JsonValue>(value)) {}
  explicit Json(JsonValue&& value)
      : _result(std::make_unique<JsonValue>(std::move(value))) {}
  explicit Json(const JsonParseError& error)
      : _result(std::make_unique<JsonValue>(error)) {}
  explicit Json(JsonParseError&& error)
      : _result(std::make_unique<JsonValue>(std::move(error))) {}
  ~Json() = default;

  // check if error
  bool is_error() const { return !_error.empty(); }
  JsonParseError get_error() const { return _error; }

  // access values
  template <typename T>
  T get_value() const {
    assert(is_type<T>(*_result));
    return std::get<T>(*_result);
  }

  Json& operator[](const std::string& key) {
    assert(is_object());
    return std::get<JsonObject>(*_result).count(key)
               ? std::get<JsonObject>(*_result).at(key)
               : (std::get<JsonObject>(*_result)[key] =
                      Json(JsonValue(NULL_T{})));
  }
  Json& operator[](size_t index) {
    assert(is_array());
    return std::get<JsonArray>(*_result).at(index);
  }

  // check type
  template <typename T, typename... Args>
  bool is_type(const std::variant<Args...>& v) const {
    return std::holds_alternative<T>(v);
  }
  bool is_string() const { return is_type<std::string>(*_result); }
  bool is_int64() const { return is_type<int64_t>(*_result); }
  bool is_double() const { return is_type<double>(*_result); }
  bool is_bool() const { return is_type<bool>(*_result); }
  bool is_object() const { return is_type<JsonObject>(*_result); }
  bool is_array() const { return is_type<JsonArray>(*_result); }
  bool is_null() const { return is_type<NULL_T>(*_result); }

  // dump the json value as a string
  std::string dump() { return {}; }

  // append and remove values
  void append_value(const JsonValue& value) {
    assert(is_array());
    std::get<JsonArray>(*_result).emplace_back(value);
  }
  void append_value(JsonValue&& value) {
    assert(is_array());
    std::get<JsonArray>(*_result).emplace_back(std::move(value));
  }
  // for array, index is the position to remove, return false if index out of
  bool remove_value(size_t index) {
    assert(is_array());
    auto& array = std::get<JsonArray>(*_result);
    if (index >= array.size()) {
      return false;
    }
    array.erase(array.begin() + index);
    return true;
  }
  // for object, key is the key to remove, return false if key not found
  bool remove_value(const std::string& key) {
    assert(is_object());
    auto& object = std::get<JsonObject>(*_result);
    if (object.find(key) == object.end()) {
      return false;
    }
    object.erase(key);
    return true;
  }

 protected:
  // for parser to pass the errors
  void set_error(std::string_view error) { _error = error; }

 private:
  std::unique_ptr<JsonValue> _result;
  std::string _error;
};

};      // namespace simdjson
#endif  // RESULT_H
