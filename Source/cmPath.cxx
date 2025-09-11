/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#include "cmPath.h"

#include "cmGlobalGenerator.h"
#include "cmStringAlgorithms.h"
#include "cmSystemTools.h"

cmPath::cmPath(const std::string& path, cmGlobalGenerator* gg)
  : FullPathDirty(true)
{
  if (gg) {
    this->BasePath = gg->GetSharedBasePath(path, this->RelativePath);
  } else {
    this->RelativePath = path;
  }
}

cmPath::cmPath(const char* path, cmGlobalGenerator* gg)
  : cmPath(std::string(path), gg)
{
}

const std::string& cmPath::ToString() const
{
  this->RebuildFullPath();
  return this->FullPath;
}

const char* cmPath::c_str() const
{
  this->RebuildFullPath();
  return this->FullPath.c_str();
}

cmPath::operator const std::string&() const
{
  return this->ToString();
}

bool cmPath::operator==(const cmPath& other) const
{
  if (this->BasePath == other.BasePath &&
      this->RelativePath == other.RelativePath) {
    return true;
  }
  return this->ToString() == other.ToString();
}

bool cmPath::operator<(const cmPath& other) const
{
  if (this->BasePath == other.BasePath) {
    return this->RelativePath < other.RelativePath;
  }
  return this->ToString() < other.ToString();
}

bool cmPath::IsEmpty() const
{
  return !this->BasePath && this->RelativePath.empty();
}

void cmPath::RebuildFullPath() const
{
  if (this->FullPathDirty) {
    if (this->BasePath) {
      this->FullPath = *this->BasePath;
      if (!this->FullPath.empty() && this->FullPath.back() != '/' &&
          this->FullPath.back() != '\\') {
        this->FullPath += '/';
      }
      this->FullPath += this->RelativePath;
    } else {
      this->FullPath = this->RelativePath;
    }
    this->FullPathDirty = false;
  }
}
