/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#pragma once

class cmPathCacheControl
{
public:
  static bool IsEnabled();
  static void SetEnabled(bool enabled);

private:
  static bool Enabled;
};
