#include "helper.hpp"
#include <fstream>
#include <iostream>
#include <shlobj.h>
#include <knownfolders.h>
#include <shlwapi.h>

using namespace std;

vector<BYTE> ReadFile(const wstring& filePath) {
    ifstream file(filePath, ios::binary | ios::ate);
    if (!file.is_open()) {
        throw runtime_error("Cannot open file");
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<BYTE> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw runtime_error("Error reading file");
    }

    return buffer;
}

wstring GetFileName(const wstring& filePath) {
    size_t lastSlash = filePath.find_last_of(L"\\/");
    if (lastSlash != wstring::npos) {
        return filePath.substr(lastSlash + 1);
    }
    return filePath;
}

string WideToMultiByte(const wstring& wide) {
    int size = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, nullptr, 0, nullptr, nullptr);
    string multiByte(size - 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, &multiByte[0], size, nullptr, nullptr);
    return multiByte;
}

wstring MultiByteToWide(const string& multiByte) {
    int size = MultiByteToWideChar(CP_UTF8, 0, multiByte.c_str(), -1, nullptr, 0);
    wstring wide(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, multiByte.c_str(), -1, &wide[0], size);
    return wide;
}

wstring GetCurrentUserLocalAppDataPath() {
    PWSTR path = nullptr;
    wstring result;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path))) {
        result = path;
        CoTaskMemFree(path);
    }

    return result;
}

wstring FindLatestLogFile(const wstring& directory) {
    wstring searchPath = directory + L"\\*.log";
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);

    wstring latestFile;
    FILETIME latestTime = { 0, 0 };

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                if (CompareFileTime(&findFileData.ftLastWriteTime, &latestTime) > 0) {
                    latestTime = findFileData.ftLastWriteTime;
                    latestFile = directory + L"\\" + findFileData.cFileName;
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);
        FindClose(hFind);
    }

    return latestFile;
}

wstring GetChromeLocalStoragePath() {
    wstring localAppData = GetCurrentUserLocalAppDataPath();
    if (localAppData.empty()) {
        throw runtime_error("Could not get Local AppData path");
    }

    wstring chromePath = localAppData + L"\\Google\\Chrome\\User Data\\Default\\Local Storage\\leveldb";

    if (GetFileAttributes(chromePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        chromePath = localAppData + L"\\Google\\Chrome Beta\\User Data\\Default\\Local Storage\\leveldb";
        if (GetFileAttributes(chromePath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            throw runtime_error("Chrome Local Storage path not found");
        }
    }

    return chromePath;
}