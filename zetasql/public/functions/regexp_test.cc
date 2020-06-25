//
// Copyright 2019 ZetaSQL Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "zetasql/public/functions/regexp.h"

#include <utility>

#include "zetasql/compliance/functions_testlib.h"
#include "zetasql/public/type.h"
#include "zetasql/public/type.pb.h"
#include "zetasql/public/value.h"
#include "zetasql/testing/test_function.h"
#include "gtest/gtest.h"
#include "zetasql/base/status.h"

namespace zetasql {
namespace functions {
namespace {

std::string StringValue(const Value& value) {
  if (value.type_kind() == TYPE_STRING) {
    return value.string_value();
  } else {
    return value.bytes_value();
  }
}

typedef testing::TestWithParam<FunctionTestCall> RegexpTemplateTest;
TEST_P(RegexpTemplateTest, TestLib) {
  const FunctionTestCall& param = GetParam();
  const std::string& function = param.function_name;
  const std::vector<Value>& args = param.params.params();
  bool expected_ok = param.params.status().ok();
  for (const Value& arg : args) {
    // Ignore tests with null arguments.
    if (arg.is_null()) return;
  }
  RegExp re;
  bool ok;
  absl::Status status;
  if (args[1].type_kind() == TYPE_STRING) {
    ok = re.InitializePatternUtf8(args[1].string_value(), &status);
  } else {
    ok = re.InitializePatternBytes(args[1].bytes_value(), &status);
  }
  if (!ok) {
    ASSERT_FALSE(expected_ok);
    ASSERT_EQ(param.params.status().code(), status.code());
    return;
  }
  if (function == "regexp_contains") {
    bool out;
    ok = re.Contains(StringValue(args[0]), &out, &status);
    ASSERT_EQ(expected_ok, ok);
    ASSERT_EQ(param.params.status().code(), status.code());
    if (ok) {
      EXPECT_EQ(param.params.result().bool_value(), out);
    }
  } else if (function == "regexp_match") {
    bool out;
    ok = re.Match(StringValue(args[0]), &out, &status);
    ASSERT_EQ(expected_ok, ok);
    ASSERT_EQ(param.params.status().code(), status.code());
    if (ok) {
      EXPECT_EQ(param.params.result().bool_value(), out);
    }
  } else if (function == "regexp_extract" || function == "regexp_substr") {
    absl::string_view out;
    bool is_null;
    std::string in = StringValue(args[0]);
    int64_t position = 1;
    int64_t occurrence = 1;
    if (args.size() >= 3) {
      position = args[2].int64_value();
      if (args.size() == 4) {
        occurrence = args[3].int64_value();
      }
    }
    ok = re.Extract(in, position, occurrence, &out, &is_null, &status);
    ASSERT_EQ(expected_ok, ok);
    ASSERT_EQ(param.params.status().code(), status.code());
    if (ok) {
      if (param.params.result().is_null()) {
        EXPECT_TRUE(is_null);
      } else {
        EXPECT_FALSE(is_null);
        EXPECT_NE(nullptr, out.data());
        EXPECT_EQ(StringValue(param.params.result()), out);
      }
    }
  } else if (function == "regexp_replace") {
    std::string out;
    ok = re.Replace(StringValue(args[0]), StringValue(args[2]), &out, &status);
    ASSERT_EQ(expected_ok, ok);
    ASSERT_EQ(param.params.status().code(), status.code());
    if (ok) {
      EXPECT_EQ(StringValue(param.params.result()), out);
    }
  } else if (function == "regexp_extract_all") {
    std::string in_str = StringValue(args[0]);
    absl::string_view in = in_str;
    re.ExtractAllReset(in);
    std::vector<Value> values;
    while (true) {
      absl::string_view out;
      if (!re.ExtractAllNext(&out, &status)) {
        ASSERT_EQ(expected_ok, status.ok());
        if (!expected_ok) {
          ASSERT_EQ(param.params.status().code(), status.code());
        } else {
          EXPECT_EQ(param.params.result(),
                    Value::Array(
                        types::ArrayTypeFromSimpleTypeKind(args[0].type_kind()),
                        values));
        }
        break;
      } else {
        if (args[0].type_kind() == TYPE_STRING) {
          values.push_back(Value::String(out));
        } else {
          values.push_back(Value::Bytes(out));
        }
      }
    }
  } else {
    FAIL() << "Unrecognized regexp function: " << function;
  }
}

INSTANTIATE_TEST_SUITE_P(
    Regexp2, RegexpTemplateTest,
    testing::ValuesIn(GetFunctionTestsRegexp2(/*include_feature_set=*/false)));

INSTANTIATE_TEST_SUITE_P(Regexp, RegexpTemplateTest,
                         testing::ValuesIn(GetFunctionTestsRegexp()));

TEST(RegexpExtract, NullStringView) {
  // Tests for b/25378427.
  absl::string_view null_string;
  absl::string_view empty_string("", 0);
  const std::vector<std::pair<absl::string_view, absl::string_view>>
      patterns_and_inputs = {{null_string, null_string},
                             {null_string, empty_string},
                             {empty_string, null_string},
                             {empty_string, empty_string}};

  for (const auto& pattern_and_input : patterns_and_inputs) {
    RegExp regexp;
    absl::Status status;
    ASSERT_TRUE(regexp.InitializePatternUtf8(pattern_and_input.first, &status))
        << status;
    absl::string_view out;
    bool is_null;
    ASSERT_TRUE(
        regexp.Extract(pattern_and_input.second, &out, &is_null, &status))
        << status;
    EXPECT_NE(nullptr, out.data());
    EXPECT_TRUE(out.empty());
    EXPECT_FALSE(is_null);
  }
}

TEST(RegexpReplace, MemLimit) {
  RegExp re;
  absl::Status error;
  ASSERT_TRUE(re.InitializePatternUtf8("A", &error));

  std::string in(64 * 1024, 'A');
  std::string out;
  EXPECT_TRUE(re.Replace(in, "BB", &out, &error));

  re.SetMaxOutSize(64 * 1024);
  EXPECT_TRUE(re.Replace(in, "B", &out, &error));
  EXPECT_FALSE(re.Replace(in, "BB", &out, &error));
}

}  // anonymous namespace
}  // namespace functions
}  // namespace zetasql
