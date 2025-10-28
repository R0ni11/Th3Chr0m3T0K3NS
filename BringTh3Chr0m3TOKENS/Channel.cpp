#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include <vector>
#include "helper.hpp"

#pragma comment(lib, "winhttp.lib")

using namespace std;

bool UploadFile(const wstring& server,
    int port,
    const wstring& path,
    const wstring& filePath,
    const wstring& fieldName) {

    HINTERNET hSession = nullptr;
    HINTERNET hConnect = nullptr;
    HINTERNET hRequest = nullptr;
    BOOL bResults = FALSE;

    try {
        hSession = WinHttpOpen(L"WinHTTP File Upload/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) {
            throw runtime_error("Failed to open HTTP session");
        }

        hConnect = WinHttpConnect(hSession, server.c_str(), port, 0);
        if (!hConnect) {
            throw runtime_error("Failed to connect to server");
        }

        DWORD flags = WINHTTP_FLAG_SECURE;
        hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(),
            nullptr, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (!hRequest) {
            throw runtime_error("Failed to open HTTP request");
        }

        /*
        DWORD requestFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
            SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
            SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
            SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;

        WinHttpSetOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS,
            &requestFlags, sizeof(requestFlags));
        */

        vector<BYTE> fileData = ReadFile(filePath);
        wstring fileName = GetFileName(filePath);

        wstring boundary = L"----WebKitFormBoundary" + to_wstring(GetTickCount64());

        string formData;

        formData += "--" + WideToMultiByte(boundary) + "\r\n";
        formData += "Content-Disposition: form-data; name=\"" + WideToMultiByte(fieldName) +
            "\"; filename=\"" + WideToMultiByte(fileName) + "\"\r\n";
        formData += "Content-Type: application/octet-stream\r\n\r\n";

        formData.insert(formData.end(), fileData.begin(), fileData.end());
        formData += "\r\n";
        formData += "--" + WideToMultiByte(boundary) + "--\r\n";

        wstring contentType = L"Content-Type: multipart/form-data; boundary=" + boundary;
        bResults = WinHttpAddRequestHeaders(hRequest, contentType.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
        if (!bResults) {
            throw runtime_error("Failed to add request headers");
        }

        bResults = WinHttpSendRequest(hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            (LPVOID)formData.data(),
            formData.size(),
            formData.size(), 0);
        if (!bResults) {
            DWORD error = GetLastError();
            throw runtime_error("Failed to send request. Error: " + to_string(error));
        }

        bResults = WinHttpReceiveResponse(hRequest, nullptr);
        if (!bResults) {
            throw runtime_error("Failed to receive response");
        }

        DWORD dwSize = 0;
        DWORD dwDownloaded = 0;
        string response;

        do {
            dwSize = 0;
            if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) {
                throw runtime_error("Error querying data availability");
            }

            if (dwSize == 0) break;

            vector<BYTE> buffer(dwSize);
            if (!WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded)) {
                throw runtime_error("Error reading response data");
            }

            response.append(buffer.begin(), buffer.end());
        } while (dwSize > 0);

        printf("Upload successful for file: %ls\n", filePath.c_str());

    }
    catch (const exception& e) {
        printf("Error: %s\n", e.what());
        bResults = FALSE;
    }

    if (hRequest) WinHttpCloseHandle(hRequest);
    if (hConnect) WinHttpCloseHandle(hConnect);
    if (hSession) WinHttpCloseHandle(hSession);

    return bResults;
}