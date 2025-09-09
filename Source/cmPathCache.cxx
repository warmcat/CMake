/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmPathCache.h"

#include <utility>

cmPathCache& cmPathCache::instance()
{
  static cmPathCache inst;
  return inst;
}

size_t cmPathCache::GetId(const std::string& path)
{
  std::lock_guard<std::mutex> lock(this->Mutex);

  auto it = this->PathToId.find(path);
  if (it != this->PathToId.end()) {
    return it->second;
  }

  size_t newId = this->IdToPath.size();
  this->IdToPath.push_back(path);
  this->PathToId[path] = newId;
  return newId;
}

const std::string& cmPathCache::GetPath(size_t id) const
{
  // This read-only access is assumed to be safe without a lock because
  // the vector is only ever appended to, and we are accessing an existing
  // element. This is a performance optimization.
  return this->IdToPath[id];
}
