#pragma once

#include <string>

void cmPathCounter_SetCWD(const std::string& cwd);
void cmPathCounter_CheckPath(const std::string& path);
void cmPathCounter_Print();
void cmPathCounter_SetContext(const std::string& context);
