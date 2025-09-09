/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmConfigure.h" // IWYU pragma: keep

#include "cmCMakePath.h"

#include <string>
#include <utility>

#if defined(_WIN32)
#  include <cstdlib>
#endif

#include <cm/filesystem>
#include <cm/string_view>

#if defined(_WIN32)
#  include <cmext/string_view>

#  include "cmStringAlgorithms.h"
#endif

cmCMakePath::cmCMakePath()
{
  this->IsCached = cmPathCacheControl::IsEnabled();
  if (!this->IsCached) {
    this->IsPathStale = false;
  }
}

cmCMakePath::cmCMakePath(cmCMakePath&& path)
  : Path(std::move(path.Path))
  , DirId(path.DirId)
  , FileName(std::move(path.FileName))
  , IsCached(path.IsCached)
  , IsPathStale(path.IsPathStale)
{
}

cmCMakePath::cmCMakePath(cm::filesystem::path path)
{
  this->IsCached = cmPathCacheControl::IsEnabled();
  if (this->IsCached) {
    this->DirId =
      cmPathCache::instance().GetId(path.parent_path().generic_string());
    this->FileName = path.filename().generic_string();
  } else {
    this->Path = std::move(path);
    this->IsPathStale = false;
  }
}

cmCMakePath::cmCMakePath(cm::string_view source, format fmt)
  : cmCMakePath(FormatPath(source, fmt))
{
}

cmCMakePath::cmCMakePath(char const* source, format fmt)
  : cmCMakePath(FormatPath(cm::string_view{ source }, fmt))
{
}

#if defined(__SUNPRO_CC) && defined(__sparc)
cmCMakePath::cmCMakePath(std::string const& source, format fmt)
  : cmCMakePath(FormatPath(source, fmt))
{
}
cmCMakePath::cmCMakePath(std::string&& source, format fmt)
  : cmCMakePath(FormatPath(std::move(source), fmt))
{
}
#endif

void cmCMakePath::UpdatePath() const
{
  if (this->IsCached && this->IsPathStale) {
    this->Path = cmPathCache::instance().GetPath(this->DirId);
    this->Path /= this->FileName;
    this->IsPathStale = false;
  }
}

cmCMakePath& cmCMakePath::ReplaceWideExtension(cm::string_view extension)
{
  this->UpdatePath();
  auto file = this->Path.filename().string();
  if (!file.empty() && file != "." && file != "..") {
    auto pos = file.find('.', file[0] == '.' ? 1 : 0);
    if (pos != std::string::npos) {
      file.erase(pos);
    }
  }
  if (!extension.empty()) {
    if (extension[0] != '.') {
      file += '.';
    }
    file.append(std::string(extension));
  }
  this->Path.replace_filename(file);
  this->IsCached = false;
  return *this;
}

cmCMakePath cmCMakePath::GetWideExtension() const
{
  this->UpdatePath();
  auto file = this->Path.filename().string();
  if (file.empty() || file == "." || file == "..") {
    return cmCMakePath{};
  }

  auto pos = file.find('.', file[0] == '.' ? 1 : 0);
  if (pos != std::string::npos) {
    return cm::string_view(file.data() + pos, file.length() - pos);
  }

  return cmCMakePath{};
}

cmCMakePath cmCMakePath::GetNarrowStem() const
{
  this->UpdatePath();
  auto stem = this->Path.stem().string();
  if (stem.empty() || stem == "." || stem == "..") {
    return stem;
  }

  auto pos = stem.find('.', stem[0] == '.' ? 1 : 0);
  if (pos != std::string::npos) {
    return stem.substr(0, pos);
  }
  return stem;
}

cmCMakePath cmCMakePath::Absolute(cm::filesystem::path const& base) const
{
  this->UpdatePath();
  if (this->Path.is_relative()) {
    auto path = base;
    path /= this->Path;
    // filesystem::path::operator/= use preferred_separator ('\' on Windows)
    // so converts back to '/'
    return path.generic_string();
  }
  return *this;
}

bool cmCMakePath::IsPrefix(cmCMakePath const& path) const
{
  this->UpdatePath();
  path.UpdatePath();
  auto prefix_it = this->Path.begin();
  auto prefix_end = this->Path.end();
  auto path_it = path.Path.begin();
  auto path_end = path.Path.end();

  while (prefix_it != prefix_end && path_it != path_end &&
         *prefix_it == *path_it) {
    ++prefix_it;
    ++path_it;
  }
  return (prefix_it == prefix_end) ||
    (prefix_it->empty() && path_it != path_end);
}

std::string cmCMakePath::FormatPath(std::string path, format fmt)
{
#if defined(_WIN32)
  if (fmt == auto_format || fmt == native_format) {
    auto prefix = path.substr(0, 4);
    for (auto& c : prefix) {
      if (c == '\\') {
        c = '/';
      }
    }
    // remove Windows long filename marker
    if (prefix == "//?/"_s) {
      path.erase(0, 4);
    }
    if (cmHasPrefix(path, "UNC/"_s) || cmHasPrefix(path, "UNC\\"_s)) {
      path.erase(0, 2);
      path[0] = '/';
    }
  }
#else
  static_cast<void>(fmt);
#endif
  return path;
}

void cmCMakePath::GetNativePath(std::string& path) const
{
  this->UpdatePath();
  cm::filesystem::path tmp(this->Path);
  tmp.make_preferred();

  path = tmp.string();
}
void cmCMakePath::GetNativePath(std::wstring& path) const
{
  this->UpdatePath();
  cm::filesystem::path tmp(this->Path);
  tmp.make_preferred();

  path = tmp.wstring();

#if defined(_WIN32)
  // Windows long filename
  static std::wstring UNC(L"\\\\?\\UNC");
  static std::wstring PREFIX(L"\\\\?\\");

  if (this->IsAbsolute() && path.length() > _MAX_PATH - 12) {
    if (this->HasRootName() && path[0] == L'\\') {
      path = UNC + path.substr(1);
    } else {
      path = PREFIX + path;
    }
  }
#endif
}

int cmCMakePath::Compare(cmCMakePath const& path) const noexcept
{
  if (this->IsCached && path.IsCached) {
    if (this->DirId == path.DirId) {
      return this->FileName.compare(path.FileName);
    }
  }

  this->UpdatePath();
  path.UpdatePath();
  return this->Path.compare(path.Path);
}

bool cmCMakePath::IsEmpty() const noexcept
{
  if (this->IsCached) {
    return this->FileName.empty() &&
      cmPathCache::instance().GetPath(this->DirId).empty();
  }
  return this->Path.empty();
}

bool cmCMakePath::HasRootPath() const
{
  this->UpdatePath();
  return this->Path.has_root_path();
}

bool cmCMakePath::HasRootName() const
{
  this->UpdatePath();
  return this->Path.has_root_name();
}

bool cmCMakePath::HasRootDirectory() const
{
  this->UpdatePath();
  return this->Path.has_root_directory();
}

bool cmCMakePath::HasRelativePath() const
{
  this->UpdatePath();
  return this->Path.has_relative_path();
}

bool cmCMakePath::HasParentPath() const
{
  this->UpdatePath();
  return this->Path.has_parent_path();
}

bool cmCMakePath::HasFileName() const
{
  this->UpdatePath();
  return this->Path.has_filename();
}

bool cmCMakePath::HasStem() const
{
  this->UpdatePath();
  return this->Path.has_stem();
}

bool cmCMakePath::HasExtension() const
{
  this->UpdatePath();
  return this->Path.has_extension();
}

bool cmCMakePath::IsAbsolute() const
{
  this->UpdatePath();
  return this->Path.is_absolute();
}

bool cmCMakePath::IsRelative() const
{
  this->UpdatePath();
  return this->Path.is_relative();
}

cmCMakePath cmCMakePath::Normal() const
{
  this->UpdatePath();
  auto path = this->Path.lexically_normal();
  // filesystem::path:lexically_normal use preferred_separator ('\') on
  // Windows) so convert back to '/'
  return path.generic_string();
}

cmCMakePath cmCMakePath::Relative(cmCMakePath const& base) const
{
  this->UpdatePath();
  return this->Relative(base.Path);
}

cmCMakePath cmCMakePath::Relative(cm::filesystem::path const& base) const
{
  this->UpdatePath();
  auto path = this->Path.lexically_relative(base);
  // filesystem::path:lexically_relative use preferred_separator ('\') on
  // Windows) so convert back to '/'
  return path.generic_string();
}

cmCMakePath cmCMakePath::Proximate(cmCMakePath const& base) const
{
  this->UpdatePath();
  return this->Proximate(base.Path);
}

cmCMakePath cmCMakePath::Proximate(cm::filesystem::path const& base) const
{
  this->UpdatePath();
  auto path = this->Path.lexically_proximate(base);
  // filesystem::path::lexically_proximate use preferred_separator ('\') on
  // Windows) so convert back to '/'
  return path.generic_string();
}

cmCMakePath cmCMakePath::Absolute(cmCMakePath const& base) const
{
  this->UpdatePath();
  return this->Absolute(base.Path);
}

cmCMakePath cmCMakePath::GetRootName() const
{
  this->UpdatePath();
  return this->Path.root_name();
}

cmCMakePath cmCMakePath::GetRootDirectory() const
{
  this->UpdatePath();
  return this->Path.root_directory();
}

cmCMakePath cmCMakePath::GetRootPath() const
{
  this->UpdatePath();
  return this->Path.root_path();
}

cmCMakePath cmCMakePath::GetFileName() const
{
  this->UpdatePath();
  return this->Path.filename();
}

cmCMakePath cmCMakePath::GetExtension() const
{
  this->UpdatePath();
  return this->Path.extension();
}

cmCMakePath cmCMakePath::GetStem() const
{
  this->UpdatePath();
  return this->Path.stem();
}

cmCMakePath cmCMakePath::GetRelativePath() const
{
  this->UpdatePath();
  return this->Path.relative_path();
}

cmCMakePath cmCMakePath::GetParentPath() const
{
  this->UpdatePath();
  return this->Path.parent_path();
}

void cmCMakePath::swap(cmCMakePath& other) noexcept
{
  std::swap(this->Path, other.Path);
  std::swap(this->DirId, other.DirId);
  std::swap(this->FileName, other.FileName);
  std::swap(this->IsCached, other.IsCached);
  std::swap(this->IsPathStale, other.IsPathStale);
}

std::string cmCMakePath::String() const
{
  this->UpdatePath();
  return this->Path.string();
}

std::wstring cmCMakePath::WString() const
{
  this->UpdatePath();
  return this->Path.wstring();
}

cmCMakePath::string_type cmCMakePath::Native() const
{
  this->UpdatePath();
  string_type path;
  this->GetNativePath(path);
  return path;
}

std::string cmCMakePath::NativeString() const
{
  this->UpdatePath();
  std::string path;
  this->GetNativePath(path);
  return path;
}

std::wstring cmCMakePath::NativeWString() const
{
  this->UpdatePath();
  std::wstring path;
  this->GetNativePath(path);
  return path;
}

std::string cmCMakePath::GenericString() const
{
  this->UpdatePath();
  return this->Path.generic_string();
}

std::wstring cmCMakePath::GenericWString() const
{
  this->UpdatePath();
  return this->Path.generic_wstring();
}
