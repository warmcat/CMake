/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmPathCacheControl.h"

bool cmPathCacheControl::Enabled = false;

bool cmPathCacheControl::IsEnabled()
{
  return Enabled;
}

void cmPathCacheControl::SetEnabled(bool enabled)
{
  Enabled = enabled;
}
