#ifndef _OGLDEV_UTIL_
#define _OGLDEV_UTIL_

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <time.h>
#include <string>
#include "ogldev_util.h"

bool ReadFile(const char* pFileName, string& outFile)
{
    ifstream f(pFileName);
    
    bool ret = false;
    
    if (f.is_open()) {
        string line;
        while (getline(f, line)) {
            outFile.append(line);
            outFile.append("\n");
        }
        
        f.close();
        
        ret = true;
    }
    else {
        //OGLDEV_FILE_ERROR(pFileName);
        std::cout << "123Error\n";
    }
    
    return ret;
}

void OgldevError(const char* pFileName, uint line, const char* pError)
{
#ifdef WIN32
    char msg[1000];
    _snprintf_s(msg, sizeof(msg), "%s:%d: %s", pFileName, line, pError);
    MessageBoxA(NULL, msg, NULL, 0);
#else
    fprintf(stderr, "%s:%d: %s\n", pFileName, line, pError);
#endif    
}


void OgldevFileError(const char* pFileName, uint line, const char* pFileError)
{
#ifdef WIN32
    char msg[1000];
    _snprintf_s(msg, sizeof(msg), "%s:%d: unable to open file `%s`", pFileName, line, pFileError);
    MessageBoxA(NULL, msg, NULL, 0);
#else
    fprintf(stderr, "%s:%d: unable to open file `%s`\n", pFileName, line, pFileError);
#endif    
}


long long GetCurrentTimeMillis()
{
#ifdef WIN32    
    return GetTickCount();

#else
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif    
}

#endif /* _OGLDEV_UTIL_ */