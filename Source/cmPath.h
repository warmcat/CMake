/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <memory>
#include <string>

class cmGlobalGenerator;

class cmPath
{
public:
  cmPath() = default;
  cmPath(const std::string& path, cmGlobalGenerator* gg);
  cmPath(const char* path, cmGlobalGenerator* gg);

  cmPath(const cmPath& other) = default;
  cmPath(cmPath&& other) = default;
  cmPath& operator=(const cmPath& other) = default;
  cmPath& operator=(cmPath&& other) = default;

  const std::string& ToString() const;
  const char* c_str() const;
  operator const std::string&() const;

  bool operator==(const cmPath& other) const;
  bool operator!=(const cmPath& other) const { return !(*this == other); }
  bool operator<(const cmPath& other) const;
  bool IsEmpty() const;

private:
  void RebuildFullPath() const;

  std::shared_ptr<const std::string> BasePath;
  std::string RelativePath;

  mutable std::string FullPath;
  mutable bool FullPathDirty = true;
};
