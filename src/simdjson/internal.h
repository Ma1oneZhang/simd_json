//
// Created by zzy on 12/17/23.
//

#ifndef INTERNAL_H
#define INTERNAL_H

#include "result.h"
#if defined(_M_X64) || defined(__x86_64__) || defined(__amd64__)
#define IS_X86_ARCH 1
#else
#define IS_X86_ARCH 0
#error "Not support other archtechures but x64/86, pull request welcome"
#endif

#include <expected>
#include <stdexcept>
#include <string>

namespace simdjson {

template <typename T>
class JsonParserBase {
 public:
  Json parse(const std::string& json) {
    return static_cast<T*>(this)->parse_impl(json);
  }

  virtual ~JsonParserBase() = default;

 private:
  void parse_impl(const std::string& json) {
    throw std::logic_error("unimplement");
  }
};
}  // namespace simdjson

#endif  // INTERNAL_H
