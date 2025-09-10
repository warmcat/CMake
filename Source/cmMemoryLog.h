/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */
#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <map>

class cmMemoryLog
{
public:
  static cmMemoryLog& GetInstance();

  void Enable(const std::string& logPath);
  bool IsEnabled() const;
  void LogAllocation(size_t size);
  void LogDeallocation(size_t size);
  void WriteLog();

public:
  cmMemoryLog() = default;
  ~cmMemoryLog() = default;
private:
  cmMemoryLog(const cmMemoryLog&) = delete;
  cmMemoryLog& operator=(const cmMemoryLog&) = delete;

  bool Enabled = false;
  std::string LogPath;

  long long TotalAllocations = 0;
  long long TotalDeallocations = 0;
  long long TotalMemoryAllocated = 0;
  long long TotalMemoryDeallocated = 0;

  std::map<size_t, long long> AllocationHistogram;

  std::mutex Mutex;
};
