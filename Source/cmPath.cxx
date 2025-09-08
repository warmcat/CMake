/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmPath.h"

#include "cmSystemTools.h"
#include "cmStringAlgorithms.h"

#include <functional>

std::vector<std::string> cmPath::BasePaths;
std::map<std::string, std::size_t> cmPath::BasePathMap;

namespace {
// This is a temporary solution to avoid code duplication.
// I will replace it with a more efficient implementation later.
cm::filesystem::path GetFilesystemPath(const cmPath& path)
{
  return cm::filesystem::path(path.GetString());
}
}

cmPath::cmPath(const char* path)
{
  if (path) {
    this->InternPath(path);
  }
}

cmPath::cmPath(const std::string& path)
{
  this->InternPath(path);
}

cmPath::cmPath(std::string&& path)
{
  this->InternPath(std::move(path));
}

bool cmPath::IsEmpty() const
{
  return this->BasePathIndex == static_cast<std::size_t>(-1) && this->RelativePath.empty();
}

bool cmPath::IsAbsolute() const
{
  return cmSystemTools::FileIsFullPath(this->GetString());
}

std::string cmPath::GetString() const
{
  if (this->BasePathIndex != static_cast<std::size_t>(-1)) {
    return cmSystemTools::JoinPath({BasePaths[this->BasePathIndex], this->RelativePath});
  }
  return this->RelativePath;
}

std::string cmPath::GenericString() const
{
    return GetFilesystemPath(*this).generic_string();
}

const char* cmPath::c_str() const
{
  // This is not efficient, but it's a temporary implementation
  // to get the basic structure in place.
  static std::string path_str;
  path_str = this->GetString();
  return path_str.c_str();
}

std::string cmPath::NativeString() const
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p.make_preferred();
    return p.string();
}

cmPath cmPath::Normal() const
{
    return cmPath(GetFilesystemPath(*this).lexically_normal().generic_string());
}

bool cmPath::HasRootName() const
{
    return GetFilesystemPath(*this).has_root_name();
}

bool cmPath::HasRootDirectory() const
{
    return GetFilesystemPath(*this).has_root_directory();
}

bool cmPath::HasRootPath() const
{
    return GetFilesystemPath(*this).has_root_path();
}

bool cmPath::HasFileName() const
{
    return GetFilesystemPath(*this).has_filename();
}

bool cmPath::HasExtension() const
{
    return GetFilesystemPath(*this).has_extension();
}

bool cmPath::HasStem() const
{
    return GetFilesystemPath(*this).has_stem();
}

bool cmPath::HasRelativePath() const
{
    return GetFilesystemPath(*this).has_relative_path();
}

bool cmPath::HasParentPath() const
{
    return GetFilesystemPath(*this).has_parent_path();
}

bool cmPath::IsRelative() const
{
    return GetFilesystemPath(*this).is_relative();
}

bool cmPath::IsPrefix(const cmPath& other) const
{
    auto prefix_it = GetFilesystemPath(*this).begin();
    auto prefix_end = GetFilesystemPath(*this).end();
    auto path_it = GetFilesystemPath(other).begin();
    auto path_end = GetFilesystemPath(other).end();

    while (prefix_it != prefix_end && path_it != path_end &&
           *prefix_it == *path_it) {
        ++prefix_it;
        ++path_it;
    }
    return (prefix_it == prefix_end) ||
           (prefix_it->empty() && path_it != path_end);
}

cmPath cmPath::GetRootName() const
{
    return cmPath(GetFilesystemPath(*this).root_name().string());
}

cmPath cmPath::GetRootDirectory() const
{
    return cmPath(GetFilesystemPath(*this).root_directory().string());
}

cmPath cmPath::GetRootPath() const
{
    return cmPath(GetFilesystemPath(*this).root_path().string());
}

cmPath cmPath::GetFileName() const
{
    return cmPath(GetFilesystemPath(*this).filename().string());
}

cmPath cmPath::GetExtension() const
{
    return cmPath(GetFilesystemPath(*this).extension().string());
}

cmPath cmPath::GetStem() const
{
    return cmPath(GetFilesystemPath(*this).stem().string());
}

cmPath cmPath::GetRelativePath() const
{
    return cmPath(GetFilesystemPath(*this).relative_path().string());
}

cmPath cmPath::GetParentPath() const
{
    return cmPath(GetFilesystemPath(*this).parent_path().string());
}

cmPath& cmPath::RemoveFileName()
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p.remove_filename();
    this->SetPath(p.generic_string());
    return *this;
}

cmPath& cmPath::ReplaceFileName(const cmPath& replacement)
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p.replace_filename(GetFilesystemPath(replacement));
    this->SetPath(p.generic_string());
    return *this;
}

cmPath& cmPath::RemoveExtension()
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p.replace_extension();
    this->SetPath(p.generic_string());
    return *this;
}

cmPath& cmPath::ReplaceExtension(const cmPath& replacement)
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p.replace_extension(GetFilesystemPath(replacement));
    this->SetPath(p.generic_string());
    return *this;
}

cmPath& cmPath::RemoveWideExtension()
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    if (p.has_extension()) {
        p.replace_extension();
    }
    this->SetPath(p.generic_string());
    return *this;
}

cmPath& cmPath::ReplaceWideExtension(const cmPath& replacement)
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p.replace_extension(GetFilesystemPath(replacement).string());
    this->SetPath(p.generic_string());
    return *this;
}

cmPath cmPath::GetWideExtension() const
{
    auto file = GetFilesystemPath(*this).filename().string();
    if (file.empty() || file == "." || file == "..") {
        return cmPath();
    }

    auto pos = file.find('.', file[0] == '.' ? 1 : 0);
    if (pos != std::string::npos) {
        return cm::string_view(file.data() + pos, file.length() - pos);
    }

    return cmPath();
}

cmPath cmPath::GetNarrowStem() const
{
    auto stem = GetFilesystemPath(*this).stem().string();
    if (stem.empty() || stem == "." || stem == "..") {
        return stem;
    }

    auto pos = stem.find('.', stem[0] == '.' ? 1 : 0);
    if (pos != std::string::npos) {
        return stem.substr(0, pos);
    }
    return stem;
}


cmPath cmPath::Relative(const cmPath& base) const
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    return cmPath(p.lexically_relative(GetFilesystemPath(base)).generic_string());
}

cmPath cmPath::Absolute(const cmPath& base) const
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    if (p.is_relative()) {
        cm::filesystem::path base_p = GetFilesystemPath(base);
        base_p /= p;
        return cmPath(base_p.generic_string());
    }
    return *this;
}

void cmPath::SetPath(const std::string& path)
{
  this->InternPath(path);
}

void cmPath::SetPath(std::string&& path)
{
  this->InternPath(std::move(path));
}

void cmPath::AddBasePath(const std::string& path)
{
  if (BasePathMap.find(path) == BasePathMap.end()) {
    BasePathMap[path] = BasePaths.size();
    BasePaths.push_back(path);
  }
}

void cmPath::InternPath(const std::string& path)
{
  for (const auto& pair : BasePathMap) {
    if (cmHasPrefix(path, pair.first)) {
      this->BasePathIndex = pair.second;
      this->RelativePath = path.substr(pair.first.length());
      if (!this->RelativePath.empty() && (this->RelativePath[0] == '/' || this->RelativePath[0] == '\\')) {
        this->RelativePath = this->RelativePath.substr(1);
      }
      return;
    }
  }

  this->BasePathIndex = -1;
  this->RelativePath = path;
}

cmPath& cmPath::operator/=(const cmPath& other)
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p /= GetFilesystemPath(other);
    this->SetPath(p.generic_string());
    return *this;
}

cmPath& cmPath::operator+=(const cmPath& other)
{
    cm::filesystem::path p = GetFilesystemPath(*this);
    p += GetFilesystemPath(other).string();
    this->SetPath(p.generic_string());
    return *this;
}

bool operator==(const cmPath& lhs, const cmPath& rhs)
{
  return lhs.GetString() == rhs.GetString();
}

bool operator!=(const cmPath& lhs, const cmPath& rhs)
{
  return !(lhs == rhs);
}

size_t hash_value(const cmPath& path)
{
    return std::hash<std::string>()(path.GetString());
}
