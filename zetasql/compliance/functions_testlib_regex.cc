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

#include "zetasql/compliance/functions_testlib.h"
#include "zetasql/compliance/functions_testlib_common.h"
#include "zetasql/testing/test_function.h"
#include "zetasql/testing/using_test_value.cc"
#include "absl/status/status.h"

namespace zetasql {
namespace {
constexpr absl::StatusCode OUT_OF_RANGE = absl::StatusCode::kOutOfRange;
}  // namespace

std::vector<FunctionTestCall> GetFunctionTestsRegexp() {
  Value null_string_array = Null(types::StringArrayType());
  Value null_bytes_array = Null(types::BytesArrayType());
  Value empty_string_array = EmptyArray(types::StringArrayType());
  Value empty_bytes_array = EmptyArray(types::BytesArrayType());

  return {
    // regexp_contains(string, string) -> bool
    {"regexp_contains", {"", NullString()}, NullBool()},
    {"regexp_contains", {NullString(), ""}, NullBool()},
    {"regexp_contains", {"", ""}, true},
    {"regexp_contains", {"", "abc"}, false},
    {"regexp_contains", {"abc", ""}, true},
    {"regexp_contains", {"abc", "abc"}, true},
    {"regexp_contains", {"abcdef", "a.c.*f"}, true},
    {"regexp_contains", {"abcdef", "ac.*e."}, false},
    {"regexp_contains", {"abcdef", "bcde"}, true},
    {"regexp_contains", {"abcdef", "b.*d"}, true},
    {"regexp_contains", {"щцфшцф", "щцф.{3}"}, true},
    {"regexp_contains", {"жэчыщчыщя", "чыщ.{2}"}, true},
    {"regexp_contains", {"жэчыщчыщя", "чы.{2}"}, true},
    {"regexp_contains", {"жэчыщчыщя", "чы{2}"}, false},
    {"regexp_contains", {"щцфшцфыщ", "(..){2}"}, true},

    {"regexp_contains", {"", "("}, NullBool(), OUT_OF_RANGE},

    // regexp_contains(bytes, bytes) -> bool
    {"regexp_contains", {Bytes(""), NullBytes()}, NullBool()},
    {"regexp_contains", {NullBytes(), Bytes("")}, NullBool()},
    {"regexp_contains", {Bytes(""), Bytes("")}, true},
    {"regexp_contains", {Bytes(""), Bytes("abc")}, false},
    {"regexp_contains", {Bytes("abc"), Bytes("")}, true},
    {"regexp_contains", {Bytes("abc"), Bytes("abc")}, true},
    {"regexp_contains", {Bytes("abcdef"), Bytes("a.c.*f")}, true},
    {"regexp_contains", {Bytes("abcdef"), Bytes("ac.*e.")}, false},
    {"regexp_contains", {Bytes("abcdef"), Bytes("bcde")}, true},
    {"regexp_contains", {Bytes("abcdef"), Bytes("b.*d")}, true},
    {"regexp_contains", {Bytes("щцфшцф"), Bytes("щцф.{3}")}, true},
    {"regexp_contains", {Bytes("жэчыщчыщя"), Bytes("чыщ.{2}")}, true},
    {"regexp_contains", {Bytes("жэчыщчыщя"), Bytes("чы.{2}")}, true},
    {"regexp_contains", {Bytes("жэчыщчыщя"), Bytes("чы{2}")}, false},
    {"regexp_contains", {Bytes("щцфшцфыщ"), Bytes("(..){2}")}, true},

    {"regexp_contains", {Bytes(""), Bytes("(")}, NullBool(), OUT_OF_RANGE},

    // regexp_match(string, string) -> bool
    {"regexp_match", {"", NullString()}, NullBool()},
    {"regexp_match", {NullString(), ""}, NullBool()},
    {"regexp_match", {"", ""}, true},
    {"regexp_match", {"", "abc"}, false},
    {"regexp_match", {"abc", ""}, false},
    {"regexp_match", {"abc", "abc"}, true},
    {"regexp_match", {"abcdef", "a.c.*f"}, true},
    {"regexp_match", {"abcdef", "ac.*e."}, false},
    {"regexp_match", {"abcdef", "bcde"}, false},
    {"regexp_match", {"щцфшцф", "щцф.{3}"}, true},
    {"regexp_match", {"щцфшцф", ".{12}"}, false},
    {"regexp_match", {"щцфшцф", "(..){3}"}, true},

    {"regexp_match", {"", "("}, NullBool(), OUT_OF_RANGE},

    // regexp_match(bytes, bytes) -> bool
    {"regexp_match", {Bytes(""), NullBytes()}, NullBool()},
    {"regexp_match", {NullBytes(), Bytes("")}, NullBool()},
    {"regexp_match", {Bytes(""), Bytes("")}, true},
    {"regexp_match", {Bytes(""), Bytes("abc")}, false},
    {"regexp_match", {Bytes("abc"), Bytes("")}, false},
    {"regexp_match", {Bytes("abc"), Bytes("abc")}, true},
    {"regexp_match", {Bytes("abcdef"), Bytes("a.c.*f")}, true},
    {"regexp_match", {Bytes("abcdef"), Bytes("ac.*e.")}, false},
    {"regexp_match", {Bytes("abcdef"), Bytes("bcde")}, false},
    {"regexp_match", {Bytes("щцфшцф"), Bytes(".{6}")}, false},
    {"regexp_match", {Bytes("щцфшцф"), Bytes(".{12}")}, true},
    {"regexp_match", {Bytes("щцфшцф"), Bytes("(..){3}")}, false},

    {"regexp_match", {Bytes(""), Bytes("(")}, NullBool(), OUT_OF_RANGE},

    // regexp_extract(string, string) -> string
    {"regexp_extract", {"", NullString()}, NullString()},
    {"regexp_extract", {NullString(), ""}, NullString()},
    {"regexp_extract", {"", ""}, ""},
    {"regexp_extract", {"", "abc"}, NullString()},
    {"regexp_extract", {"abc", "abc"}, "abc"},
    {"regexp_extract", {"abc", "^a"}, "a"},
    {"regexp_extract", {"abc", "^b"}, NullString()},
    {"regexp_extract", {"abcdef", "a.c.*f"}, "abcdef"},
    {"regexp_extract", {"abcdef", "ac.*e."}, NullString()},
    {"regexp_extract", {"abcdef", "bcde"}, "bcde"},
    {"regexp_extract", {"щцф", ".{3}"}, "щцф"},
    {"regexp_extract", {"щцф", ".{6}"}, NullString()},
    {"regexp_extract", {"", "()"}, ""},
    {"regexp_extract", {"", "(abc)"}, NullString()},
    {"regexp_extract", {"", "(abc)?"}, NullString()},
    {"regexp_extract", {"abc", "a(b)c"}, "b"},
    {"regexp_extract", {"abcdef", "a(.c.*)f"}, "bcde"},
    {"regexp_extract", {"abcdef", "(bcde)"}, "bcde"},
    {"regexp_extract", {"щцф", "щ(.)."}, "ц"},
    {"regexp_extract", {"щцф", "(?:щ|ц)(ц|ф)(?:щ|ф)"}, "ц"},
    {"regexp_extract", {"щцф", "(.{6})"}, NullString()},
    {"regexp_extract", {"abc", "((a))"}, NullString(), OUT_OF_RANGE},
    {"regexp_extract", {"abc", "(a)(b)"}, NullString(), OUT_OF_RANGE},

    {"regexp_extract", {"", "("}, NullString(), OUT_OF_RANGE},

    // regexp_extract(bytes, bytes) -> bytes
    {"regexp_extract", {Bytes(""), NullBytes()}, NullBytes()},
    {"regexp_extract", {NullBytes(), Bytes("")}, NullBytes()},
    {"regexp_extract", {Bytes(""), Bytes("")}, Bytes("")},
    {"regexp_extract", {Bytes(""), Bytes("abc")}, NullBytes()},
    {"regexp_extract", {Bytes("abc"), Bytes("abc")}, Bytes("abc")},
    {"regexp_extract", {Bytes("abcdef"), Bytes("a.c.*f")}, Bytes("abcdef")},
    {"regexp_extract", {Bytes("abcdef"), Bytes("ac.*e.")}, NullBytes()},
    {"regexp_extract", {Bytes("abcdef"), Bytes("bcde")}, Bytes("bcde")},
    {"regexp_extract", {Bytes("щцф"), Bytes(".{2}")}, Bytes("щ")},
    {"regexp_extract", {Bytes("щцф"), Bytes(".{6}")}, Bytes("щцф")},
    {"regexp_extract", {Bytes(""), Bytes("()")}, Bytes("")},
    {"regexp_extract", {Bytes(""), Bytes("(abc)")}, NullBytes()},
    {"regexp_extract", {Bytes(""), Bytes("(abc)?")}, NullBytes()},
    {"regexp_extract", {Bytes("abc"), Bytes("a(b)c")}, Bytes("b")},
    {"regexp_extract", {Bytes("abcdef"), Bytes("a(.c.*)f")}, Bytes("bcde")},
    {"regexp_extract", {Bytes("abcdef"), Bytes("(bcde)")}, Bytes("bcde")},
    {"regexp_extract", {Bytes("щцф"), Bytes("щ(..)..")}, Bytes("ц")},
    {"regexp_extract", {Bytes("щцф"), Bytes("(.{6})")}, Bytes("щцф")},
    {"regexp_extract", {Bytes("abc"), Bytes("((a))")},
        NullBytes(), OUT_OF_RANGE},
    {"regexp_extract", {Bytes("abc"), Bytes("(a)(b)")},
        NullBytes(), OUT_OF_RANGE},

    {"regexp_extract", {Bytes(""), Bytes("(")}, NullBytes(), OUT_OF_RANGE},

    // regexp_replace(string, string, string) -> string
    {"regexp_replace", {NullString(), "", ""}, NullString()},
    {"regexp_replace", {"", NullString(), ""}, NullString()},
    {"regexp_replace", {"", "", NullString()}, NullString()},
    {"regexp_replace", {"", "", ""}, ""},
    {"regexp_replace", {"abc", "", "x"}, "xaxbxcx"},
    {"regexp_replace", {"abc", "x?", "x"}, "xaxbxcx"},
    {"regexp_replace", {"abc", "b", "xyz"}, "axyzc"},
    {"regexp_replace", {"abcabc", "bc", "xyz"}, "axyzaxyz"},
    {"regexp_replace", {"abc", "abc", "xyz"}, "xyz"},
    {"regexp_replace", {"banana", "ana", "xyz"}, "bxyzna"},
    {"regexp_replace", {"banana", "ana", ""}, "bna"},
    {"regexp_replace", {"banana", "a", "z"}, "bznznz"},
    {"regexp_replace", {"banana", "(.)a(.)", "\\1\\2"}, "bnana"},
    {"regexp_replace", {"banana", ".", "x"}, "xxxxxx"},
    {"regexp_replace", {"щцфшцф", ".", "ы"}, "ыыыыыы"},
    {"regexp_replace", {"T€T", "", "K"}, "KTK€KTK"},
    {"regexp_replace", {"abc", "b*", "x"}, "xaxcx"},
    {"regexp_replace", {"ab", "b*", "x"}, "xax"},
    {"regexp_replace", {"bc", "b*", "x"}, "xcx"},
    {"regexp_replace", {"b", "b*", "x"}, "x"},
    {"regexp_replace", {"bb", "^b", "x"}, "xb"},
    {"regexp_replace", {"one", "\\b", "-"}, "-one-"},
    {"regexp_replace", {"one two  many", "\\b", "-"}, "-one- -two-  -many-"},
    // non-capturing group
    {"regexp_replace",
     {"http://www.google.com", "(?:http|ftp)://(.*?)\\.(.*?)\\.(com|org|net)",
      "\\3.\\2.\\1"},
     "com.google.www"},
    // nested non-capturing group
    {"regexp_replace",
     {"www3.archive.org", "(?:(?:http|ftp)://)?(.*?)\\.(.*?)\\.(com|org|net)",
      "\\3.\\2.\\1"},
     "org.archive.www3"},

    {"regexp_replace", {"", "(", ""}, NullString(), OUT_OF_RANGE},
    {"regexp_replace", {"", "", "\\x"}, NullString(), OUT_OF_RANGE},
    {"regexp_replace", {"", "", "\\"}, NullString(), OUT_OF_RANGE},
    {"regexp_replace", {"", "", "\\1"}, NullString(), OUT_OF_RANGE},
    {"regexp_replace", {"abc", "abc.*", "\\2@\\1"}, NullString(), OUT_OF_RANGE},
    {"regexp_replace", {"abc", "(.*)b([^d]*)", "\\4@\\3"}, NullString(),
          OUT_OF_RANGE},

    // The regex is invalid.
    {"regexp_replace", {"a", "AI!\x1b\r)V\r\x1c\x1eP3]|\x17\n9", "def"},
          NullString(), OUT_OF_RANGE},
    // This is the test case from b/21080653 and b/21079403, where the
    // regex is invalid but the original pattern string is NULL.
    {"regexp_replace", {"CAST(NULL as STRING)",
                        "AI!\x1b\r)V\r\x1c\x1eP3]|\x17\n9", "b"},
          NullString(), OUT_OF_RANGE},
    {"regexp_replace", {"NULL",
                        "AI!\x1b\r)V\r\x1c\x1eP3]|\x17\n9", "b"},
          NullString(), OUT_OF_RANGE},

    // regexp_replace(bytes, bytes, bytes) -> bytes
    {"regexp_replace", {NullBytes(), Bytes(""), Bytes("")}, NullBytes()},
    {"regexp_replace", {Bytes(""), NullBytes(), Bytes("")}, NullBytes()},
    {"regexp_replace", {Bytes(""), Bytes(""), NullBytes()}, NullBytes()},
    {"regexp_replace", {Bytes(""), Bytes(""), Bytes("")}, Bytes("")},
    {"regexp_replace", {Bytes("abc"), Bytes(""), Bytes("x")}, Bytes("xaxbxcx")},
    {"regexp_replace", {Bytes("abc"), Bytes("x?"), Bytes("x")},
        Bytes("xaxbxcx")},
    {"regexp_replace", {Bytes("abc"), Bytes("b"), Bytes("xyz")},
        Bytes("axyzc")},
    {"regexp_replace", {Bytes("abcabc"), Bytes("bc"), Bytes("xyz")},
        Bytes("axyzaxyz")},
    {"regexp_replace", {Bytes("abc"), Bytes("abc"), Bytes("xyz")},
        Bytes("xyz")},
    {"regexp_replace", {Bytes("banana"), Bytes("ana"), Bytes("xyz")},
        Bytes("bxyzna")},
    {"regexp_replace", {Bytes("banana"), Bytes("ana"), Bytes("")},
        Bytes("bna")},
    {"regexp_replace", {Bytes("banana"), Bytes("a"), Bytes("z")},
        Bytes("bznznz")},
    {"regexp_replace", {Bytes("banana"), Bytes("(.)a(.)"), Bytes("\\1\\2")},
        Bytes("bnana")},
    {"regexp_replace", {Bytes("banana"), Bytes("."), Bytes("x")},
        Bytes("xxxxxx")},

    {"regexp_replace", {Bytes(""), Bytes("("), Bytes("")},
        NullBytes(), OUT_OF_RANGE},
    {"regexp_replace", {Bytes(""), Bytes(""), Bytes("\\x")},
        NullBytes(), OUT_OF_RANGE},
    {"regexp_replace", {Bytes(""), Bytes(""), Bytes("\\")},
        NullBytes(), OUT_OF_RANGE},
    {"regexp_replace", {Bytes(""), Bytes(""), Bytes("\\1")},
        NullBytes(), OUT_OF_RANGE},

    // regexp_extract_all(string, string) -> Array of string
    {"regexp_extract_all", {"", NullString()}, null_string_array},
    {"regexp_extract_all", {NullString(), ""}, null_string_array},
    {"regexp_extract_all", {"", ""}, StringArray({""})},
    {"regexp_extract_all", {"", "abc"}, empty_string_array},
    {"regexp_extract_all", {"abc", "."}, StringArray({"a", "b", "c"})},
    {"regexp_extract_all", {"aaa", "^a"}, StringArray({"a"})},
    {"regexp_extract_all", {"abc", "abc"}, StringArray({"abc"})},
    {"regexp_extract_all", {"abcdef", "a.c.*f"}, StringArray({"abcdef"})},
    {"regexp_extract_all", {"abcdef", "ac.*e."}, empty_string_array},
    {"regexp_extract_all", {"abcdef", "bcde"}, StringArray({"bcde"})},
    {"regexp_extract_all", {"aabca", "a*"}, StringArray({"aa", "", "", "a"})},
    {"regexp_extract_all", {"aaba", "a?"}, StringArray({"a", "a", "", "a"})},
    {"regexp_extract_all", {"baac", "a*"}, StringArray({"", "aa", ""})},
    {"regexp_extract_all", {"abcd", "a(bc)*"}, StringArray({"bc"})},
    {"regexp_extract_all", {"щцф", "."}, StringArray({"щ", "ц", "ф"})},
    {"regexp_extract_all", {"щцф", ".{3}"}, StringArray({"щцф"})},
    {"regexp_extract_all", {"щцф", ".{6}"}, empty_string_array},
    {"regexp_extract_all", {"щццф", "ц*"}, StringArray({"", "цц", ""})},
    {"regexp_extract_all", {"", "()"}, StringArray({""})},
    {"regexp_extract_all", {"", "(abc)"}, empty_string_array},
    {"regexp_extract_all", {"", "(abc)?"}, StringArray({""})},
    {"regexp_extract_all", {"abc", "a(b)c"}, StringArray({"b"})},
    {"regexp_extract_all", {"abbb", "^a*(b)"}, StringArray({"b"})},
    {"regexp_extract_all", {"XbASDZb", "(.)b"}, StringArray({"X", "Z"})},
    {"regexp_extract_all", {"abcdef", "a(.c.*)f"}, StringArray({"bcde"})},
    {"regexp_extract_all", {"abcdef", "(bcde)"}, StringArray({"bcde"})},
    {"regexp_extract_all", {"this_is__a___Test", "(.*?)(?:_|$)"},
        StringArray({"this", "is", "", "a", "", "", "Test"})},
    {"regexp_extract_all", {"щцф", "щ(.)."}, StringArray({"ц"})},
    {"regexp_extract_all", {"щцф", "(.{6})"}, empty_string_array},
    {"regexp_extract_all", {"abc", "((a))"},
        null_string_array, OUT_OF_RANGE},
    {"regexp_extract_all", {"abc", "(a)(b)"},
        null_string_array, OUT_OF_RANGE},

    {"regexp_extract_all", {"", "("}, null_string_array, OUT_OF_RANGE},

    // regexp_extract_all(bytes, bytes) -> array of bytes
    {"regexp_extract_all", {Bytes(""), NullBytes()}, null_bytes_array},
    {"regexp_extract_all", {NullBytes(), Bytes("")}, null_bytes_array},
    {"regexp_extract_all", {Bytes(""), Bytes("")}, BytesArray({""})},
    {"regexp_extract_all", {Bytes(""), Bytes("abc")}, empty_bytes_array},
    {"regexp_extract_all", {Bytes("abc"), Bytes(".")},
        BytesArray({"a", "b", "c"})},
    {"regexp_extract_all", {Bytes("aaa"), Bytes("^a")}, BytesArray({"a"})},
    {"regexp_extract_all", {Bytes("abc"), Bytes("abc")}, BytesArray({"abc"})},
    {"regexp_extract_all", {Bytes("abcdef"), Bytes("a.c.*f")},
        BytesArray({"abcdef"})},
    {"regexp_extract_all", {Bytes("abcdef"), Bytes("ac.*e.")},
        empty_bytes_array},
    {"regexp_extract_all", {Bytes("abcdef"), Bytes("bcde")},
        BytesArray({"bcde"})},
    {"regexp_extract_all", {Bytes("aabca"), Bytes("a*")},
        BytesArray({"aa", "", "", "a"})},
    {"regexp_extract_all", {Bytes("aaba"), Bytes("a?")},
        BytesArray({"a", "a", "", "a"})},
    {"regexp_extract_all", {Bytes("baac"), Bytes("a*")},
        BytesArray({"", "aa", ""})},
    {"regexp_extract_all", {Bytes("abcd"), Bytes("a(bc)*")},
        BytesArray({"bc"})},
    {"regexp_extract_all", {Bytes("щцф"), Bytes(".{2}")},
        BytesArray({"щ", "ц", "ф"})},
    {"regexp_extract_all", {Bytes("щцф"), Bytes(".{6}")}, BytesArray({"щцф"})},
    {"regexp_extract_all", {Bytes("\001\002\003"), Bytes("\002?")},
        BytesArray({"", "\002", ""})},
    {"regexp_extract_all", {Bytes(""), Bytes("()")}, BytesArray({""})},
    {"regexp_extract_all", {Bytes(""), Bytes("(abc)")}, empty_bytes_array},
    {"regexp_extract_all", {Bytes(""), Bytes("(abc)?")}, BytesArray({""})},
    {"regexp_extract_all", {Bytes("abc"), Bytes("a(b)c")}, BytesArray({"b"})},
    {"regexp_extract_all", {Bytes("abbb"), Bytes("^a*(b)")}, BytesArray({"b"})},
    {"regexp_extract_all", {Bytes("XbASDZb"), Bytes("(.)b")},
        BytesArray({"X", "Z"})},
    {"regexp_extract_all", {Bytes("abcdef"), Bytes("a(.c.*)f")},
        BytesArray({"bcde"})},
    {"regexp_extract_all", {Bytes("abcdef"), Bytes("(bcde)")},
        BytesArray({"bcde"})},
    {"regexp_extract_all", {Bytes("this__test_case"), Bytes("(.*?)(?:_|$)")},
        BytesArray({"this", "", "test", "case"})},
    {"regexp_extract_all", {Bytes("щцф"), Bytes("щ(..)..")}, BytesArray({"ц"})},
    {"regexp_extract_all", {Bytes("щцф"), Bytes("(.{6})")},
        BytesArray({"щцф"})},
    {"regexp_extract_all", {Bytes("abc"), Bytes("((a))")},
        null_bytes_array, OUT_OF_RANGE},
    {"regexp_extract_all", {Bytes("abc"), Bytes("(a)(b)")},
        null_bytes_array, OUT_OF_RANGE},

    {"regexp_extract_all", {Bytes(""), Bytes("(")},
        null_bytes_array, OUT_OF_RANGE},
  };
}

std::vector<FunctionTestCall> GetFunctionTestsRegexp2(
    bool include_feature_set) {
  // regexp_substr is an alias of regexp_extract so the two are equivalent.
  std::vector<FunctionTestCall> test_calls = {
    // regexp_extract(string, string, int) -> string
    {"regexp_substr", {"", "", 2ll}, NullString()},
    {"regexp_extract", {"", "", 2ll}, NullString()},
    {"regexp_substr", {"aaa", "aa", 2ll}, "aa"},
    {"regexp_extract", {"aaa", "a", 4ll}, NullString()},
    {"regexp_substr", {"-2020-jack-class1", "-[^.-]*", 2ll},
       "-jack"},
    {"regexp_extract", {"щцф", ".{2}", 2ll}, "цф"},
    {"regexp_substr", {"aa", "a", int64max}, NullString()},
    {"regexp_extract", {"aa", "a", -1ll}, NullString(), OUT_OF_RANGE},
    {"regexp_substr", {"aa", "a", NullInt64()}, NullString()},
    // Invalid utf-8 second byte.
    {"regexp_extract", {"\x61\xa7\x65\x71", "a", 1ll}, "a"},
    {"regexp_substr", {"\x61\xa7\x65\x71", "a", 2ll}, NullString()},
    {"regexp_extract", {"\x61\xa7\x65\x71", "a", 2ll}, NullString()},
    {"regexp_substr", {"\x61\xa7\x65\x71", "a", 3ll}, NullString()},
    // Invalid regex.
    {"regexp_extract", {"\x61\xa7\x65\x71", "\xa7", 1ll}, NullString(),
       OUT_OF_RANGE},

    // regexp_extract(string, string, int, int) -> string
    {"regexp_extract", {"", NullString(), 2ll, 2ll}, NullString()},
    {"regexp_substr", {NullString(), "", 2ll, 2ll}, NullString()},
    {"regexp_extract", {NullString(), NullString(), 2ll, 2ll}, NullString()},
    {"regexp_substr", {"", "", 1ll, 2ll}, NullString()},
    {"regexp_extract", {"aaa", "a*", 2ll, 2ll}, NullString()},
    {"regexp_substr", {"aaa", "aa", 2ll, 1ll}, "aa"},
    {"regexp_extract", {"aaa", "aa", 1ll, 2ll}, NullString()},
    {"regexp_substr", {"", "abc", 1ll, 2ll}, NullString()},
    {"regexp_extract", {"abc", "abc", 2ll, 1ll}, NullString()},
    {"regexp_substr", {"abc", "c", 3ll, 1ll}, "c"},
    {"regexp_extract", {"abc", "c", 4ll, 1ll}, NullString()},
    {"regexp_substr", {"abcabc", "abc", 4ll, 1ll}, "abc"},
    {"regexp_extract", {"abcabcc", "abc", 1ll, 2ll}, "abc"},
    {"regexp_substr", {"abcdefaazcddefg", "a.c.*f", 5ll, 1ll}, "azcddef"},
    {"regexp_extract", {"abcabc", "a(b)c", 1ll, 2ll}, "b"},
    {"regexp_substr", {"-2020-jack-class1", "-[^.-]*", 1ll, 1ll},
       "-2020"},
    {"regexp_extract", {"-2020-jack-class1", "-[^.-]*", 2ll, 1ll},
       "-jack"},
    {"regexp_substr", {"-2020-jack-class1", "-[^.-]*", 7ll, 3ll},
       NullString()},
    {"regexp_extract", {"щцф", ".{2}", 2ll, 1ll}, "цф"},
    {"regexp_substr", {"щцф", ".{2}", 3ll, 1ll}, NullString()},
    {"regexp_extract", {"щцфщфф", "щ(.).", 1ll, 2ll}, "ф"},
    {"regexp_substr", {"", "()", 1ll, 2ll}, NullString()},
    {"regexp_extract", {"aa", "a", 1ll, int64max}, NullString()},
    {"regexp_substr", {"aa", "a", int64max, 1ll}, NullString()},
    {"regexp_extract", {"abc", "abc", 1ll, 0ll},
       NullString(), OUT_OF_RANGE},
    {"regexp_substr", {"abc", "abc", 0ll, 1ll},
       NullString(), OUT_OF_RANGE},
    {"regexp_extract", {"abc", "((a))", 1ll, 1ll},
       NullString(), OUT_OF_RANGE},
    {"regexp_substr", {"abc", "(a)(b)", 2ll, 1ll},
       NullString(), OUT_OF_RANGE},
    {"regexp_extract", {"aa", "a", 1ll, -1ll},
       NullString(), OUT_OF_RANGE},
    {"regexp_substr", {"aa", "a", -1ll, -1ll},
       NullString(), OUT_OF_RANGE},
    {"regexp_extract", {"aa", "a", 1ll, NullInt64()}, NullString()},
    {"regexp_substr", {"aa", "a", NullInt64(), NullInt64()}, NullString()},
    // Invalid utf-8 second byte.
    {"regexp_extract", {"\x61\xa7\x65\x71", "a", 1ll, 2ll}, NullString()},
    {"regexp_extract", {"\x61\xa7\x65\x71", "a", 2ll, 2ll}, NullString()},

    // regexp_extract(bytes, bytes, int) -> bytes
    {"regexp_extract", {Bytes(""), Bytes(""), 2ll}, NullBytes()},
    {"regexp_substr", {Bytes("aaa"), Bytes("aa"), 2ll}, Bytes("aa")},
    {"regexp_extract", {Bytes("aaa"), Bytes("a"), 4ll}, NullBytes()},
    {"regexp_substr", {Bytes("-2020-jack-class1"), Bytes("-[^.-]*"),
       2ll}, Bytes("-jack")},
    {"regexp_extract", {Bytes("щцф"), Bytes(".{2}"), 2ll}, Bytes("ц")},
    {"regexp_substr", {Bytes("щцщф"), Bytes("щ(..)"), 2ll}, Bytes("ф")},
    {"regexp_extract", {Bytes("aa"), Bytes("a"), int64max}, NullBytes()},
    {"regexp_substr", {Bytes("aa"), Bytes("a"), -1ll},
       NullBytes(), OUT_OF_RANGE},
    // Invalid utf-8 second byte.
    {"regexp_extract", {Bytes("\x61\xa7\x65\x71"), Bytes("a"), 1ll},
       Bytes("a")},
    {"regexp_substr", {Bytes("\x61\xa7\x65\x71"), Bytes("a"), 2ll},
       NullBytes()},
    {"regexp_extract", {Bytes("\x61\xa7\x65\x71"), Bytes("a"), 2ll},
       NullBytes()},
    {"regexp_substr", {Bytes("\x61\xa7\x65\x71"), Bytes("a"), 3ll},
       NullBytes()},

    // regexp_extract(bytes, bytes, int, int) -> bytes
    {"regexp_extract", {Bytes(""), NullBytes(), 2ll, 2ll}, NullBytes()},
    {"regexp_substr", {NullBytes(), Bytes(""), 2ll, 2ll}, NullBytes()},
    {"regexp_extract", {NullBytes(), NullBytes(), 2ll, 2ll}, NullBytes()},
    {"regexp_substr", {Bytes(""), Bytes(""), 1ll, 2ll}, NullBytes()},
    {"regexp_extract", {Bytes(""), Bytes(""), 2ll, 1ll}, NullBytes()},
    {"regexp_substr", {Bytes("aaa"), Bytes("a*"), 2ll, 2ll}, NullBytes()},
    {"regexp_extract", {Bytes("aaa"), Bytes("aa"), 2ll, 1ll}, Bytes("aa")},
    {"regexp_substr", {Bytes("aaa"), Bytes("aa"), 1ll, 2ll}, NullBytes()},
    {"regexp_extract", {Bytes(""), Bytes("abc"), 1ll, 2ll}, NullBytes()},
    {"regexp_substr", {Bytes("abc"), Bytes("abc"), 2ll, 1ll}, NullBytes()},
    {"regexp_extract", {Bytes("abc"), Bytes("c"), 3ll, 1ll}, Bytes("c")},
    {"regexp_substr", {Bytes("abc"), Bytes("c"), 4ll, 1ll}, NullBytes()},
    {"regexp_extract", {Bytes("abcabc"), Bytes("abc"), 4ll, 1ll},
       Bytes("abc")},
    {"regexp_substr", {Bytes("abcabcc"), Bytes("abc"), 1ll, 2ll},
       Bytes("abc")},
    {"regexp_extract", {Bytes("abcdefaazcddefg"), Bytes("a.c.*f"), 5ll, 1ll},
       Bytes("azcddef")},
    {"regexp_substr", {Bytes("abcabc"), Bytes("a(b)c"), 1ll, 2ll},
       Bytes("b")},
    {"regexp_extract", {Bytes("-2020-jack-class1"), Bytes("-[^.-]*"),
       1ll, 1ll}, Bytes("-2020")},
    {"regexp_substr", {Bytes("-2020-jack-class1"), Bytes("-[^.-]*"),
       2ll, 1ll}, Bytes("-jack")},
    {"regexp_extract", {Bytes("-2020-jack-class1"), Bytes("-[^.-]*"),
       7ll, 3ll}, NullBytes()},
    {"regexp_substr", {Bytes("щцф"), Bytes(".{2}"), 1ll, 3ll}, Bytes("ф")},
    {"regexp_extract", {Bytes("щцф"), Bytes(".{2}"), 2ll, 2ll}, Bytes("ф")},
    {"regexp_substr", {Bytes("щцф"), Bytes(".{2}"), 3ll, 1ll}, Bytes("ф")},
    {"regexp_extract", {Bytes("щцф"), Bytes(".{3}"), 1ll, 1ll},
       Bytes("\xD1\x89\xD1")},
    {"regexp_substr", {Bytes("щцф"), Bytes(".{3}"), 1ll, 2ll},
       Bytes("\x86\xD1\x84")},
    {"regexp_extract", {Bytes("щцф"), Bytes(".{3}"), 1ll, 3ll}, NullBytes()},
    {"regexp_substr", {Bytes("щцф"), Bytes(".{2}"), 3ll, 2ll}, NullBytes()},
    {"regexp_extract", {Bytes(""), Bytes("()"), 1ll, 2ll}, NullBytes()},
    {"regexp_substr", {Bytes("aa"), Bytes("a"), 1ll, int64max}, NullBytes()},
    {"regexp_extract", {Bytes("aa"), Bytes("a"), int64max, 1ll}, NullBytes()},
    {"regexp_substr", {Bytes("abc"), Bytes("abc"), 1ll, 0ll},
       NullBytes(), OUT_OF_RANGE},
    {"regexp_extract", {Bytes("abc"), Bytes("abc"), 0ll, 1ll},
       NullBytes(), OUT_OF_RANGE},
    {"regexp_substr", {Bytes("abc"), Bytes("((a))"), 1ll, 1ll},
       NullBytes(), OUT_OF_RANGE},
    {"regexp_extract", {Bytes("abc"), Bytes("(a)(b)"), 2ll, 1ll},
       NullBytes(), OUT_OF_RANGE},
    {"regexp_substr", {Bytes("aa"), Bytes("a"), 1ll, -1ll},
       NullBytes(), OUT_OF_RANGE},
    {"regexp_extract", {Bytes("aa"), Bytes("a"), -1ll, -1ll},
       NullBytes(), OUT_OF_RANGE},
    // Invalid utf-8 second byte.
    {"regexp_extract", {Bytes("\x61\xa7\x65\x71"), Bytes("a"), 1ll, 2ll},
       NullBytes()},
    {"regexp_substr", {Bytes("\x61\xa7\x65\x71"), Bytes("a"), 2ll, 2ll},
       NullBytes()},
  };

  if (include_feature_set) {
    for (auto& call : test_calls) {
      call.params = call.params.WrapWithFeature(
          FEATURE_V_1_3_ALLOW_REGEXP_EXTRACT_OPTIONALS);
    }
  }

  return test_calls;
}

}  // namespace zetasql
