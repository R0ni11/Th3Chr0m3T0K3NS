#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>

bool UploadFile(const std::wstring& server,
    int port,
    const std::wstring& path,
    const std::wstring& filePath,
    const std::wstring& fieldName);

#endif