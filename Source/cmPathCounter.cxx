#include "cmPathCounter.h"
#include <iostream>
#include <string>
#include "cmSystemTools.h"

static long long CWDCount = 0; // Number of strings containing CWD
static long long CWDSize = 0;
static long long CWDOccurrencesInStrings = 0; // Total number of CWD substrings
static std::string CWD;

void cmPathCounter_SetCWD(const std::string& cwd)
{
  CWD = cwd;
}

void cmPathCounter_CheckPath(const std::string& path)
{
  if (CWD.empty()) {
    return;
  }

  size_t occurrences = 0;
  size_t pos = path.find(CWD, 0);
  while(pos != std::string::npos)
  {
    occurrences++;
    pos = path.find(CWD, pos + 1);
  }

  if (occurrences > 0) {
    CWDCount++;
    CWDSize += path.size();
    CWDOccurrencesInStrings += occurrences;
  }
}

void cmPathCounter_Print()
{
  std::cout << "PathCounter CWD: " << CWD << std::endl;
  std::cout << "PathCounter CWD-containing strings: " << CWDCount << std::endl;
  std::cout << "PathCounter CWD total occurrences: " << CWDOccurrencesInStrings << std::endl;
  std::cout << "PathCounter CWD total size: " << CWDSize << std::endl;
}
