//
// Created by zzy on 12/17/23.
//
#include <gtest/gtest.h>
#include <simdjson/json.h>
#include <filesystem>
#include <fstream>
#include <iostream>

TEST(simdjson, declare) {
  simdjson::JsonParser parser;
}

TEST(simdjson, normal_impl_number) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("123");
  EXPECT_EQ(json_obj.is_int64(), true);
  EXPECT_EQ(json_obj.get_value<int64_t>(), 123);

  // for float
  auto json_obj2 = parser.parse("123.456");
  EXPECT_EQ(json_obj2.is_double(), true);
  EXPECT_EQ(json_obj2.get_value<double>(), 123.456);

  // for negative
  auto json_obj3 = parser.parse("-123");
  EXPECT_EQ(json_obj3.is_int64(), true);
  EXPECT_EQ(json_obj3.get_value<int64_t>(), -123);

  // for negative float
  auto json_obj4 = parser.parse("-123.456");
  EXPECT_EQ(json_obj4.is_double(), true);
  EXPECT_EQ(json_obj4.get_value<double>(), -123.456);

  // for overflow int64
  auto json_obj5 = parser.parse("9223372036854775808");
  EXPECT_EQ(json_obj5.is_double(), true);
  EXPECT_EQ(json_obj5.get_value<double>(), 9223372036854775808.0);

  // for illegal number
  auto json_obj6 = parser.parse("123.456.789");
  // we just care first match, so it is not error
  EXPECT_EQ(json_obj6.is_error(), false);
  EXPECT_EQ(json_obj6.is_double(), true);
  EXPECT_EQ(json_obj6.get_value<double>(), 123.456);

  auto json_obj7 = parser.parse("123.4e6.789");
  EXPECT_EQ(json_obj7.is_error(), false);
  EXPECT_EQ(json_obj7.is_double(), true);
  EXPECT_EQ(json_obj7.get_value<double>(), 123.4e6);
}

TEST(simdjson, normal_impl_string) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("\"123\"");
  EXPECT_EQ(json_obj.is_int64(), false);
  EXPECT_EQ(json_obj.is_string(), true);
  EXPECT_EQ(json_obj.get_value<std::string>(), "123");
}

TEST(simdjson, normal_impl_bool) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("true");
  EXPECT_EQ(json_obj.is_bool(), true);
  EXPECT_EQ(json_obj.get_value<bool>(), true);

  auto json_obj2 = parser.parse("false");
  EXPECT_EQ(json_obj2.is_bool(), true);
  EXPECT_EQ(json_obj2.get_value<bool>(), false);
}

TEST(simdjson, normal_impl_null) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("null");
  EXPECT_EQ(json_obj.is_null(), true);
}

TEST(simdjson, normal_impl_object) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("{\"key\": \"value\"}");
  EXPECT_EQ(json_obj.is_object(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>().size(), 1);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"].is_string(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"]
                .get_value<std::string>(),
            "value");
}

TEST(simdjson, normal_impl_complex_object) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse(
      "{\"key\": \"value\", \"key2\": 123, \"key3\": true, \"key4\": null}");
  EXPECT_EQ(json_obj.is_object(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>().size(), 4);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"].is_string(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"]
                .get_value<std::string>(),
            "value");
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key2"].is_int64(),
            true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key2"].get_value<int64_t>(),
      123);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key3"].is_bool(), true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key3"].get_value<bool>(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key4"].is_null(), true);
}

TEST(simdjson, normal_impl_nested_object) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse(
      "{\"key\": \"value\", \"key2\": 123, \"key3\": true, \"key4\": null, "
      "\"key5\": {\"key6\": \"value6\"}}");
  EXPECT_EQ(json_obj.is_object(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>().size(), 5);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"].is_string(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"]
                .get_value<std::string>(),
            "value");
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key2"].is_int64(),
            true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key2"].get_value<int64_t>(),
      123);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key3"].is_bool(), true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key3"].get_value<bool>(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key4"].is_null(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"].is_object(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"]
                .get_value<simdjson::JsonObject>()
                .size(),
            1);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key5"]["key6"].is_string(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"]["key6"]
                .get_value<std::string>(),
            "value6");
}

TEST(simdjson, normal_impl_nested_complex_object) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse(
      "{\"key\": \"value\", \"key2\": 123, \"key3\": true, \"key4\": null, "
      "\"key5\": {\"key6\": \"value6\", \"key7\": 789, \"key8\": false, "
      "\"key9\": null}}");
  EXPECT_EQ(json_obj.is_object(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>().size(), 5);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"].is_string(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key"]
                .get_value<std::string>(),
            "value");
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key2"].is_int64(),
            true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key2"].get_value<int64_t>(),
      123);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key3"].is_bool(), true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key3"].get_value<bool>(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key4"].is_null(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"].is_object(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"]
                .get_value<simdjson::JsonObject>()
                .size(),
            4);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key5"]["key6"].is_string(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"]["key6"]
                .get_value<std::string>(),
            "value6");
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key5"]["key7"].is_int64(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"]["key7"]
                .get_value<int64_t>(),
            789);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key5"]["key8"].is_bool(),
      true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonObject>()["key5"]["key8"]
                .get_value<bool>(),
            false);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonObject>()["key5"]["key9"].is_null(),
      true);
}

TEST(simdjson, normal_impl_array) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("[\"value\"]");
  EXPECT_EQ(json_obj.is_array(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>().size(), 1);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0].is_string(), true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonArray>()[0].get_value<std::string>(),
      "value");
}

TEST(simdjson, normal_impl_complex_array) {
  simdjson::JsonParser parser;
  auto json_obj = parser.parse("[\"value\", 123, true, null]");
  EXPECT_EQ(json_obj.is_array(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>().size(), 4);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0].is_string(), true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonArray>()[0].get_value<std::string>(),
      "value");
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[1].is_int64(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[1].get_value<int64_t>(),
            123);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[2].is_bool(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[2].get_value<bool>(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[3].is_null(), true);
}

TEST(simdjson, normal_impl_complex_array_nested_object) {
  simdjson::JsonParser parser;
  auto json_obj =
      parser.parse("[\"value\", 123, true, null, {\"key\": \"value\"}]");
  EXPECT_EQ(json_obj.is_array(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>().size(), 5);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0].is_string(), true);
  EXPECT_EQ(
      json_obj.get_value<simdjson::JsonArray>()[0].get_value<std::string>(),
      "value");
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[1].is_int64(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[1].get_value<int64_t>(),
            123);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[2].is_bool(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[2].get_value<bool>(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[3].is_null(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[4].is_object(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[4]
                .get_value<simdjson::JsonObject>()
                .size(),
            1);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[4]
                .get_value<simdjson::JsonObject>()["key"]
                .is_string(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[4]
                .get_value<simdjson::JsonObject>()["key"]
                .get_value<std::string>(),
            "value");
}

#define __FILE_PATH__ \
  std::string(__FILE__, sizeof(__FILE__) - (1 + sizeof(__FILE_NAME__)))

TEST(simdjson, regression_test_1) {
  std::ifstream ifs(std::string(__FILE_PATH__) +
                    "/local_large_json/simple_array.json");
  ASSERT_TRUE(ifs.is_open());
  ifs.seekg(0, std::ios::end);
  size_t size = ifs.tellg();
  ifs.seekg(0, std::ios::beg);
  auto buf = new char[size];
  ifs.read(buf, size);
  std::string content(buf, size);
  simdjson::JsonParser parser;
  auto json_obj = parser.parse(content);
  EXPECT_EQ(json_obj.is_array(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>().size(), 10);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0].is_object(), true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()
                .size(),
            3);

  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()["id"]
                .is_int64(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()["id"]
                .get_value<int64_t>(),
            1);

  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()["city"]
                .is_string(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()["city"]
                .get_value<std::string>(),
            "beijing");

  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()["code"]
                .is_int64(),
            true);
  EXPECT_EQ(json_obj.get_value<simdjson::JsonArray>()[0]
                .get_value<simdjson::JsonObject>()["code"]
                .is_int64(),
            true);
  delete[] buf;
}

TEST(simdjson, regression_test_2) {
  std::ifstream ifs(std::string(__FILE_PATH__) +
                    "/local_large_json/amazon_cellphones.ndjson");
  ASSERT_TRUE(ifs.is_open());
  while (ifs.eof()) {
    simdjson::JsonParser parser;
    std::string content;
    ifs >> content;
    auto json_obj = parser.parse(content);
    ASSERT_FALSE(json_obj.is_error());
    ASSERT_TRUE(json_obj.is_array());
  }
}