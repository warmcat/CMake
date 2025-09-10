/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmMemoryLog.h"
#include "cmSystemTools.h"
#include <iostream>
#include <string>

cmMemoryLog& cmMemoryLog::GetInstance()
{
  static cmMemoryLog instance;
  return instance;
}

void cmMemoryLog::Enable(const std::string& logPath)
{
  std::lock_guard<std::mutex> lock(this->Mutex);
  this->Enabled = true;
  this->LogPath = logPath;
}

bool cmMemoryLog::IsEnabled() const
{
  return this->Enabled;
}

void cmMemoryLog::LogAllocation(size_t size)
{
  if (!this->Enabled) {
    return;
  }
  std::lock_guard<std::mutex> lock(this->Mutex);
  this->TotalAllocations++;
  this->TotalMemoryAllocated += size;
}

void cmMemoryLog::LogDeallocation(size_t size)
{
  if (!this->Enabled) {
    return;
  }
  std::lock_guard<std::mutex> lock(this->Mutex);
  this->TotalDeallocations++;
  this->TotalMemoryDeallocated += size;
}

void cmMemoryLog::WriteLog()
{
  if (!this->Enabled || this->LogPath.empty()) {
    return;
  }

  std::lock_guard<std::mutex> lock(this->Mutex);

  cmsys::ofstream logFile(this->LogPath.c_str());
  if (!logFile) {
    cmSystemTools::Error("Could not open memory log file for writing: " + this->LogPath);
    return;
  }

  logFile << "--- CMake Memory Log ---" << std::endl;
  logFile << "Total Allocations: " << this->TotalAllocations << std::endl;
  logFile << "Total Deallocations: " << this->TotalDeallocations << std::endl;
  logFile << "Total Memory Allocated: " << this->TotalMemoryAllocated << " bytes" << std::endl;
  logFile << "Total Memory Deallocated: " << this->TotalMemoryDeallocated << " bytes" << std::endl;

  long long netAllocations = this->TotalAllocations - this->TotalDeallocations;
  long long netMemory = this->TotalMemoryAllocated - this->TotalMemoryDeallocated;

  logFile << "Net Allocations (outstanding): " << netAllocations << std::endl;
  logFile << "Net Memory (outstanding): " << netMemory << " bytes" << std::endl;
  logFile << "--- End CMake Memory Log ---" << std::endl;

  // Disable logging after writing to prevent issues during shutdown
  this->Enabled = false;
}
