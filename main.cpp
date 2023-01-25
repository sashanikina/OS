#include <iostream>
#include <string>
#include <windows.h>
#include <processthreadsapi.h>
#include <WinBase.h>

using namespace std;

#define pageSize 4096 
#define numberOfPages 13
#define numberOfWriters 7 
#define numberOfReaders 7 



int main() {
    string stringFileName = "C:\\Users\\asus\\Documents\\OS\\os_lab_4_1\\logFiles\\buffer.txt";
    wstring wstringFileName = wstring(stringFileName.begin(), stringFileName.end()); 
    HANDLE fileHandle = CreateFileA("buffer.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    string stringFileMappingName = "bufferMapping";
    wstring wstringFileMappingName = wstring(stringFileMappingName.begin(), stringFileMappingName.end());
    HANDLE fileMappingHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, pageSize * numberOfPages, L"bufferMapping");
    LPVOID fileMappingViewPointer = MapViewOfFile(fileMappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

    VirtualLock(fileMappingViewPointer, pageSize * numberOfPages);


    HANDLE readingSemaphores[numberOfPages];
    HANDLE writingSemaphores[numberOfPages];
    for (int i = 0; i < numberOfPages; ++i) {
        wstring wstringSemaphoreName = L"writingSemaphore" + to_wstring(i);
       
        writingSemaphores[i] = CreateSemaphore(NULL, 1, 1,wstringSemaphoreName.c_str());

        wstringSemaphoreName = L"readingSemaphore" + to_wstring(i);
        readingSemaphores[i] = CreateSemaphore(NULL, 0, 1, wstringSemaphoreName.c_str());
    }

    HANDLE processHandles[numberOfWriters + numberOfReaders];
    STARTUPINFOA startupInfos[numberOfWriters + numberOfReaders];
    PROCESS_INFORMATION processInfos[numberOfWriters + numberOfReaders];
    for (int i = 0; i < numberOfWriters; ++i) {
        string stringApplicationName = "C:\\Users\\asus\\Documents\\OS\\os_lab_4_1\\x64\\Debug\\writer.exe";
        ZeroMemory(&startupInfos[i], sizeof(startupInfos[i]));
        
        CreateProcessA(
            stringApplicationName.c_str(),
            NULL, 
            NULL, 
            NULL,
            TRUE,
            0, 
            NULL,
            NULL, 
            &startupInfos[i],
            &processInfos[i]
        );

        processHandles[i] = processInfos[i].hProcess;
    }
    
    for (int i = numberOfWriters; i < numberOfWriters + numberOfReaders; ++i) {
        string stringApplicationName = "C:\\Users\\asus\\Documents\\OS\\os_lab_4_1\\x64\\Debug\\reader.exe";
        
        ZeroMemory(&startupInfos[i], sizeof(startupInfos[i]));
        
        CreateProcessA(
            stringApplicationName.c_str(), 
            NULL, 
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL, 
            &startupInfos[i],
            &processInfos[i]
        );
        
        processHandles[i] = processInfos[i].hProcess;
    }

    WaitForMultipleObjects(
        numberOfWriters + numberOfReaders,
        processHandles,
        TRUE,
        INFINITE 
    );


    for (int i = 0; i < numberOfWriters + numberOfReaders; ++i) {
        CloseHandle(processHandles[i]);
    }
    for (int i = 0; i < numberOfPages; ++i) {
        CloseHandle(readingSemaphores[i]);
        CloseHandle(writingSemaphores[i]);
    }
    VirtualUnlock(fileMappingViewPointer, pageSize * numberOfPages);
    UnmapViewOfFile(fileMappingViewPointer);
    CloseHandle(fileMappingHandle);
    CloseHandle(fileHandle);

    cout << endl << "Press any key to exit..." << endl;
    getchar();
    return 0;
}

