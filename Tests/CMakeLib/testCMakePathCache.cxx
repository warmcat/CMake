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

bool testSubProject()
{
  std::cout << "testSubProject()" << std::endl;

  cmPathCacheControl::SetEnabled(true);

  cmCMakePath buildDir("/home/agreen/projects/cmake/build/Tests/SubProject");
  std::cout << "  buildDir: " << buildDir.String() << std::endl;

  cmCMakePath subDir = "foo";
  std::cout << "  subDir: " << subDir.String() << std::endl;

  buildDir /= subDir;
  std::cout << "  buildDir after append: " << buildDir.String() << std::endl;

  std::string expected =
    "/home/agreen/projects/cmake/build/Tests/SubProject/foo";
  if (buildDir.String() != expected) {
    std::cout << "  => failed: expected " << expected << ", got "
              << buildDir.String() << std::endl;
    return false;
  }

  cmPathCacheControl::SetEnabled(false);

  std::cout << " => passed" << std::endl;
  return true;
}

bool testSubProjectFile()
{
  std::cout << "testSubProjectFile()" << std::endl;

  cmPathCacheControl::SetEnabled(true);

  cmCMakePath buildDir(
    "/home/agreen/projects/cmake/build/Tests/SubProject/foo");
  std::cout << "  buildDir: " << buildDir.String() << std::endl;

  cmCMakePath fileName = "Makefile";
  std::cout << "  fileName: " << fileName.String() << std::endl;

  buildDir /= fileName;
  std::cout << "  buildDir after append: " << buildDir.String() << std::endl;

  std::string expected =
    "/home/agreen/projects/cmake/build/Tests/SubProject/foo/Makefile";
  if (buildDir.String() != expected) {
    std::cout << "  => failed: expected " << expected << ", got "
              << buildDir.String() << std::endl;
    return false;
  }

  cmPathCacheControl::SetEnabled(false);

  std::cout << " => passed" << std::endl;
  return true;
}

bool testOutDir()
{
  std::cout << "testOutDir()" << std::endl;

  cmPathCacheControl::SetEnabled(true);

  cmCMakePath top("/home/agreen/projects/cmake/build");
  cmCMakePath testc1_lib = top;
  testc1_lib /= "archive/Release/libtestc1.a";

  std::cout << "  top: " << top.String() << std::endl;
  std::cout << "  testc1_lib: " << testc1_lib.String() << std::endl;

  cmCMakePath relative = testc1_lib.Relative(top);
  std::cout << "  relative: " << relative.String() << std::endl;

  std::string expected = "archive/Release/libtestc1.a";
  if (relative.String() != expected) {
    std::cout << "  => failed: expected " << expected << ", got "
              << relative.String() << std::endl;
    return false;
  }

  cmPathCacheControl::SetEnabled(false);

  std::cout << " => passed" << std::endl;
  return true;
}

} // namespace

int testCMakePathCache(int /*unused*/, char* /*unused*/[])
{
  return runTests(
    { testPathCaching, testSubProject, testSubProjectFile, testOutDir }, false);
}
