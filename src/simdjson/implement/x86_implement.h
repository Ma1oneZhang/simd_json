//
// Created by zzy on 12/17/23.
//

#ifndef X86_IMPLEMENT_H
#define X86_IMPLEMENT_H
#include "../internal.h"
#include "../result.h"

constexpr bool enable_simd = false;

namespace simdjson {
class x86_implement final : public JsonParserBase<x86_implement> {
 public:
  Json parse_impl(const std::string& json) {
    if constexpr (!enable_simd) {
      return parse_normal_impl(json);
    }
    return parse_simd_impl(json);
  }
  Json parse_simd_impl(const std::string& json);
  Json parse_normal_impl(const std::string& json);
  virtual ~x86_implement() = default;

 private:
  using JsonToken = std::string_view;
  std::vector<JsonToken> _tokens;
};
}  // namespace simdjson

#endif  // X86_IMPLEMENT_H
