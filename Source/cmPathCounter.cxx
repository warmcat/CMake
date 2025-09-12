#include "cmPathCounter.h"
#include <iostream>
#include <string>
#include "cmSystemTools.h"

static long long CWDCount = 0;
static long long CWDSize = 0;
static std::string CWD;

void cmPathCounter_SetCWD(const std::string& cwd)
{
  CWD = cwd;
}

void cmPathCounter_CheckPath(const std::string& path)
{
  if (!CWD.empty() && path.find(CWD) != std::string::npos) {
    CWDCount++;
    CWDSize += path.size();
  }
}

void cmPathCounter_Print()
{
  std::cout << "PathCounter CWD: " << CWD << std::endl;
  std::cout << "PathCounter CWD Count: " << CWDCount << std::endl;
  std::cout << "PathCounter CWD Size: " << CWDSize << std::endl;
}
