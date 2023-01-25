#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

int main()
{
	HANDLE pipe;
	string pipe_name, event_name;
	char* data = new char[512];
	OVERLAPPED Overlapped;
	DWORD written;

	ZeroMemory(&Overlapped, sizeof(Overlapped));

	Overlapped.hEvent = CreateEvent(NULL, TRUE, 0, NULL);

	pipe_name = "\\\\.\\pipe\\mypipe";
	pipe = CreateNamedPipeA(pipe_name.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_WAIT, 1, 256, 256, 0, NULL);
	BOOL isConnect = ConnectNamedPipe(pipe, &Overlapped);
	if(!isConnect){
		cout << "You can input data.\n\n";
		fgets(data, 512, stdin);
		WriteFile(pipe, data, strlen(data) + 1, &written, &Overlapped);
		WaitForSingleObject(Overlapped.hEvent, INFINITE);

		delete[] data;
		CloseHandle(Overlapped.hEvent);
		DisconnectNamedPipe(pipe);
	}
	else {
		cout << "Error creating pipe " << GetLastError() << endl;
	}
	CloseHandle(pipe);
	system("pause");
	return 0;
}