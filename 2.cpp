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

int main()
{
	setlocale(LC_ALL, "Russian");

	if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
	{
		wstring path;
		if (OpenDlg(path))
		{
			wcout << path << endl;
		}else
			cout << "Îøèáêà OpenDlg();" << endl;
	}else
		cout << "Îøèáêà CoInitializeEx();" << endl;
	CoUninitialize();
	system("pause");
}

