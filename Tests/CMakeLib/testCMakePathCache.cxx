/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmConfigure.h" // IWYU pragma: keep

#include <iostream>
#include <string>
#include <vector>

#include "cmCMakePath.h"
#include "cmPathCache.h"
#include "cmPathCacheControl.h"

#include "testCommon.h"

namespace {

void checkResult(bool success)
{
  if (!success) {
    std::cout << " => failed";
  }
  std::cout << std::endl;
}

bool testPathCaching()
{
  std::cout << "testPathCaching()";
  bool result = true;

  cmPathCacheControl::SetEnabled(true);

  std::string long_dir = "/a/very/long/directory/path/that/is/repeated/often";
  std::vector<cmCMakePath> paths;
  for (int i = 0; i < 100; ++i) {
    paths.emplace_back(long_dir + "/file" + std::to_string(i) + ".txt");
  }

  if (cmPathCache::instance().size() != 1) {
    result = false;
  }

  for (int i = 0; i < 100; ++i) {
    std::string expected =
      long_dir + "/file" + std::to_string(i) + ".txt";
    if (paths[i].String() != expected) {
      result = false;
      break;
    }
  }

  if (paths[10] != paths[10]) {
    result = false;
  }
  if (paths[10] == paths[11]) {
    result = false;
  }

  cmCMakePath p1 = long_dir + "/file10.txt";
  if (paths[10] != p1) {
    result = false;
  }

  cmPathCacheControl::SetEnabled(false);

  checkResult(result);
  return result;
}

} // namespace

int testCMakePathCache(int /*unused*/, char* /*unused*/[])
{
  return runTests({ testPathCaching }, false);
}
