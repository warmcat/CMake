/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include <iostream>
#include <string>
#include <vector>

#include <cmext/string_view>

#include "cmListView.h"

#include "testCommon.h"

namespace {

bool testCase(const char* name, cm::string_view input,
              const std::vector<cm::string_view>& expected)
{
  std::cout << "Test case: " << name << "..." << std::endl;
  cmListView list(input);
  std::vector<cm::string_view> actual;
  for (cm::string_view item : list) {
    actual.push_back(item);
  }

  if (actual == expected) {
    return true;
  }

  std::cout << "  Expected: ";
  for (cm::string_view item : expected) {
    std::cout << "'" << item << "' ";
  }
  std::cout << std::endl;

  std::cout << "  Actual:   ";
  for (cm::string_view item : actual) {
    std::cout << "'" << item << "' ";
  }
  std::cout << std::endl;

  return false;
}

} // namespace

int testCmListView(int /*unused*/, char* /*unused*/[])
{
  int failed = 0;

  if (!testCase("Empty list", "", {})) {
    failed++;
  }
  if (!testCase("One element", "a", { "a" })) {
    failed++;
  }
  if (!testCase("Two elements", "a;b", { "a", "b" })) {
    failed++;
  }
  if (!testCase("Multiple elements", "a;b;c;d", { "a", "b", "c", "d" })) {
    failed++;
  }
  if (!testCase("Empty elements", "a;;b", { "a", "", "b" })) {
    failed++;
  }
  if (!testCase("Leading semicolon", ";a;b", { "", "a", "b" })) {
    failed++;
  }
  if (!testCase("Trailing semicolon", "a;b;", { "a", "b", "" })) {
    failed++;
  }
  if (!testCase("Leading and trailing semicolons", ";a;b;",
                { "", "a", "b", "" })) {
    failed++;
  }
  if (!testCase("All empty", ";;;", { "", "", "", "" })) {
    failed++;
  }
  if (!testCase("Escaped semicolon", "a\\;b;c", { "a;b", "c" })) {
    failed++;
  }
  if (!testCase("Bracket argument", "a[;]b;c", { "a[;]b", "c" })) {
    failed++;
  }
  if (!testCase("Nested bracket argument", "a[[;]]b;c", { "a[[;]]b", "c" })) {
    failed++;
  }
  if (!testCase("Bracket argument with no semicolon", "a[b]c", { "a[b]c" })) {
    failed++;
  }
  if (!testCase("Single trailing semicolon", "a;", { "a", "" })) {
    failed++;
  }
  if (!testCase("Single leading semicolon", ";", { "", "" })) {
    failed++;
  }
  if (!testCase("Single escaped semicolon", "\\;", { ";" })) {
    failed++;
  }

  return failed;
}
