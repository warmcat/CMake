/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file LICENSE.rst or https://cmake.org/licensing for details.  */

#include "cmMemoryLog.h"
#include "cmSystemTools.h"
#include "cmsys/FStream.hxx"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring> // For memset

static cmMemoryLog g_MemoryLogInstance;

cmMemoryLog& cmMemoryLog::GetInstance()
{
  return g_MemoryLogInstance;
}

cmMemoryLog::cmMemoryLog()
{
  memset(this->AllocationHistogram, 0, sizeof(this->AllocationHistogram));
  this->LargeAllocations = 0;
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
  if (size < HISTOGRAM_MAX_SIZE) {
    this->AllocationHistogram[size]++;
  } else {
    this->LargeAllocations++;
  }
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
  if (!this->Enabled) {
    return;
  }

  // Disable logging to prevent deadlocks from allocations during file I/O.
  this->Enabled = false;

  if (this->LogPath.empty()) {
    return;
  }

  std::lock_guard<std::mutex> lock(this->Mutex);

  cmsys::ofstream logFile(this->LogPath.c_str());
  if (!logFile) {
    // We can't use cmSystemTools::Error because it might allocate.
    // Just write to stderr directly.
    fprintf(stderr, "Could not open memory log file for writing: %s\n", this->LogPath.c_str());
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
  logFile << std::endl;

  logFile << "--- Top 20 Allocation Sizes (under " << HISTOGRAM_MAX_SIZE << " bytes) ---" << std::endl;
  logFile << "Size (bytes) | Count" << std::endl;
  logFile << "-------------------------------" << std::endl;

  std::vector<std::pair<size_t, long long>> sorted_histogram;
  for (size_t i = 0; i < HISTOGRAM_MAX_SIZE; ++i) {
    if (this->AllocationHistogram[i] > 0) {
      sorted_histogram.push_back({i, this->AllocationHistogram[i]});
    }
  }
  std::sort(sorted_histogram.begin(), sorted_histogram.end(),
            [](const auto& a, const auto& b) {
    return a.second > b.second;
  });

  int count = 0;
  for (auto const& [size, num] : sorted_histogram) {
    if (count >= 20) {
      break;
    }
    logFile << size << " | " << num << std::endl;
    count++;
  }

  logFile << std::endl;
  logFile << "Allocations >= " << HISTOGRAM_MAX_SIZE << " bytes: " << this->LargeAllocations << std::endl;

  logFile << "--- End CMake Memory Log ---" << std::endl;
}
