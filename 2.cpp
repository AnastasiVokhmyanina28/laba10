#include <windows.h>
#include <shobjidl.h>
#include <string>
#include <iostream>
using namespace std;

bool OpenDlg(wstring& path)
{
	bool flag = false;
	IFileDialog* dlg;
	if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&dlg)))
	{
		if(SUCCEEDED(dlg->Show(NULL)))
		{
			IShellItem* item;
			if(SUCCEEDED(dlg->GetResult(&item)))
			{
				PWSTR filepath;
				if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &filepath)))
				{
					path = filepath;
					CoTaskMemFree(filepath);
					item->Release();
					dlg->Release();
					flag = true;
					return flag;
				}else
					return flag;
			}else
				return flag;
		}else
			return flag;
	}else
		return flag;
	
}

bool SaveDlg(wstring& path)
{
	bool flag = false;
	IFileDialog* dlg;
	if (SUCCEEDED(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, (void**)&dlg)))
	{
		if(SUCCEEDED(dlg->Show(NULL)))
		{
			IShellItem* item;
			if(SUCCEEDED(dlg->GetResult(&item)))
			{
				PWSTR filepath;
				if(SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &filepath)))
				{
					path = filepath;
					CoTaskMemFree(filepath);
					item->Release();
					dlg->Release();
					flag = true;
					return flag;
				}else
					return flag;
			}else
				return flag;
		}else
			return flag;
	}else
		return flag;	
}

void Copy(wstring src, wstring dst)
{
	
	const int BUFSIZE = 1024;
	char* bufread; 
	char* bufwrite;
	char *t;
	bufread = new char [BUFSIZE];
	bufwrite = new char [BUFSIZE];
	HANDLE FileSrc, FileOst, EvtRead, EvtWrite, Events[2];
	OVERLAPPED WriteF, ReadF;
	LARGE_INTEGER FSize, ReadPos, WritePos;
	DWORD BytesRead, BytesWrite;
	FileSrc = CreateFileW(src.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	FileOst = CreateFileW(dst.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

		if ((FileSrc != INVALID_HANDLE_VALUE) && (FileOst != INVALID_HANDLE_VALUE))
		{
			GetFileSizeEx(FileSrc, &FSize);
			ITaskbarList3* taskbar1;
			HWND wnd = GetConsoleWindow();
			if (SUCCEEDED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_ALL, IID_ITaskbarList3, (void**)&taskbar1)))
			{
				taskbar1->SetProgressState(wnd, TBPF_NORMAL); 
			}else
				taskbar1 = NULL;

			ReadPos.QuadPart = 0;
			WritePos.QuadPart = 0;

			EvtRead = CreateEvent(NULL, false, false, NULL);
			EvtWrite = CreateEvent(NULL, false, false, NULL);

			Events[0] = EvtRead;
			Events[1] = EvtWrite;

			ZeroMemory(&ReadF, sizeof(ReadF));

			ReadF.Offset = 0;
			ReadF.OffsetHigh = 0;
			ReadF.hEvent = EvtRead;

			ReadFile(FileSrc, bufread, BUFSIZE, NULL, &ReadF);
			WaitForSingleObject(EvtRead, INFINITE);
			GetOverlappedResult(FileSrc, &ReadF, &BytesRead, false);

			while (BytesRead > 0)
			{
				WritePos.QuadPart = ReadPos.QuadPart;
				ReadPos.QuadPart += BytesRead;
				//swap(bufread, bufwrite);
				t = bufread;
				bufread=bufwrite;
				bufwrite=t;

				ZeroMemory(&ReadF, sizeof(ReadF));
				ReadF.Offset = ReadPos.LowPart;
				ReadF.OffsetHigh = ReadPos.HighPart;
				ReadF.hEvent = EvtRead;

				ZeroMemory(&WriteF, sizeof(WriteF));
				WriteF.Offset = WritePos.LowPart;
				WriteF.OffsetHigh = WritePos.HighPart;
				WriteF.hEvent = EvtWrite;

				ReadFile(FileSrc, bufread, BUFSIZE, NULL, &ReadF);
				WriteFile(FileOst, bufwrite, BytesRead, NULL, &WriteF);
				WaitForMultipleObjects(2, Events, true, INFINITE);
				GetOverlappedResult(FileSrc, &ReadF, &BytesRead, false);

				if (taskbar1 != NULL)
				{
					taskbar1 -> SetProgressValue (wnd, WritePos.QuadPart, FSize.QuadPart);
				}

			}
			CloseHandle(EvtRead);
			CloseHandle(EvtWrite);
			if (taskbar1 != NULL) 
				taskbar1 -> Release();
		}
		CloseHandle(FileSrc);
		CloseHandle(FileOst);
		delete[] bufread;
		delete[] bufwrite;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
	{
		wstring src, dst;
		if (OpenDlg(src) && SaveDlg(dst))
		{
			cout<<"Исходный файл: ";
			wcout << src << endl;
			cout<<"Целевой файл: ";
			wcout << dst << endl;
			Copy (src, dst);
			
		}else
			cout << "Ошибка OpenDlg();" << endl;
	}else
		cout << "Ошибка CoInitializeEx();" << endl;
	CoUninitialize();
	system("pause");
}

