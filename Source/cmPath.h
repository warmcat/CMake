/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#pragma once

#include "cmConfigure.h" // IWYU pragma: keep

#include <string>
#include <vector>
#include <map>

#include <cm/filesystem>
#include <cm/string_view>

class cmPath
{
public:
  cmPath() = default;
  cmPath(const char* path);
  cmPath(const std::string& path);
  cmPath(std::string&& path);
  cmPath(cm::string_view path);

  bool IsEmpty() const;
  bool IsAbsolute() const;

  std::string GetString() const;
  std::string GenericString() const;
  const char* c_str() const;
  std::string String() const { return this->GetString(); }
  std::string NativeString() const;

  cmPath Normal() const;

  bool HasRootName() const;
  bool HasRootDirectory() const;
  bool HasRootPath() const;
  bool HasFileName() const;
  bool HasExtension() const;
  bool HasStem() const;
  bool HasRelativePath() const;
  bool HasParentPath() const;

  bool IsRelative() const;
  bool IsPrefix(const cmPath& other) const;

  cmPath GetRootName() const;
  cmPath GetRootDirectory() const;
  cmPath GetRootPath() const;
  cmPath GetFileName() const;
  cmPath GetExtension() const;
  cmPath GetStem() const;
  cmPath GetRelativePath() const;
  cmPath GetParentPath() const;

  cmPath& RemoveFileName();
  cmPath& ReplaceFileName(const cmPath& replacement);
  cmPath& RemoveExtension();
  cmPath& ReplaceExtension(const cmPath& replacement);
  cmPath& RemoveWideExtension();
  cmPath& ReplaceWideExtension(const cmPath& replacement);

  cmPath GetWideExtension() const;
  cmPath GetNarrowStem() const;

  cmPath Relative(const cmPath& base) const;
  cmPath Absolute(const cmPath& base) const;

  void SetPath(const std::string& path);
  void SetPath(std::string&& path);

  cmPath& operator=(const std::string& path) { this->SetPath(path); return *this; }
  cmPath& operator=(const char* path) { if(path) this->SetPath(path); else *this = cmPath(); return *this; }

  static void AddBasePath(const std::string& path);

  cmPath& operator/=(const cmPath& other);
  cmPath& operator+=(const cmPath& other);

  cmPath Append(const cmPath& other) const;

private:
  void InternPath(const std::string& path);

  std::size_t BasePathIndex = -1;
  std::string RelativePath;

  static std::vector<std::string> BasePaths;
  static std::map<std::string, std::size_t> BasePathMap;
};

bool operator==(const cmPath& lhs, const cmPath& rhs);
bool operator!=(const cmPath& lhs, const cmPath& rhs);
size_t hash_value(const cmPath& path);
