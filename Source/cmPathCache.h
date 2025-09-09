/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#pragma once

#include <cstddef>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class cmPathCache
{
public:
  static cmPathCache& instance();

  cmPathCache(const cmPathCache&) = delete;
  cmPathCache& operator=(const cmPathCache&) = delete;

  size_t GetId(const std::string& path);
  const std::string& GetPath(size_t id) const;

private:
  cmPathCache() = default;

  std::mutex Mutex;
  std::unordered_map<std::string, size_t> PathToId;
  std::vector<std::string> IdToPath;
};
