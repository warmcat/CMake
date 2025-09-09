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

bool testPathCaching()
{
  std::cout << "testPathCaching()" << std::endl;

  cmPathCacheControl::SetEnabled(true);

  std::string long_dir = "/a/very/long/directory/path/that/is/repeated/often";
  std::vector<cmCMakePath> paths;
  for (int i = 0; i < 100; ++i) {
    paths.emplace_back(long_dir + "/file" + std::to_string(i) + ".txt");
  }

  if (cmPathCache::instance().size() != 1) {
    std::cout << "  => failed: cache size is "
              << cmPathCache::instance().size() << ", expected 1" << std::endl;
    return false;
  }

  for (int i = 0; i < 100; ++i) {
    std::string expected = long_dir + "/file" + std::to_string(i) + ".txt";
    if (paths[i].String() != expected) {
      std::cout << "  => failed: path[" << i << "] is " << paths[i].String()
                << ", expected " << expected << std::endl;
      return false;
    }
  }

  if (paths[10] != paths[10]) {
    std::cout << "  => failed: paths[10] != paths[10]" << std::endl;
    return false;
  }
  if (paths[10] == paths[11]) {
    std::cout << "  => failed: paths[10] == paths[11]" << std::endl;
    return false;
  }

  cmCMakePath p1 = long_dir + "/file10.txt";
  if (paths[10] != p1) {
    std::cout << "  => failed: paths[10] != p1" << std::endl;
    return false;
  }

  cmPathCacheControl::SetEnabled(false);

  std::cout << " => passed" << std::endl;
  return true;
}

} // namespace

int testCMakePathCache(int /*unused*/, char* /*unused*/[])
{
  return runTests({ testPathCaching }, false);
}
