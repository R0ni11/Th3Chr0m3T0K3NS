#ifndef HELPER_HPP
#define HELPER_HPP

#include <windows.h>
#include <string>
#include <vector>

std::vector<BYTE> ReadFile(const std::wstring& filePath);
std::wstring GetFileName(const std::wstring& filePath);
std::string WideToMultiByte(const std::wstring& wide);
std::wstring MultiByteToWide(const std::string& multiByte);
std::wstring GetCurrentUserLocalAppDataPath();
std::wstring FindLatestLogFile(const std::wstring& directory);
std::wstring GetChromeLocalStoragePath();

#endif