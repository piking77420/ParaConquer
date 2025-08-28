#include "SystemDialogue.hpp"

#include <cassert>

#include <Windows.h>      // For common windows data types and function headers
#define STRICT_TYPED_ITEMIDS
#include <Objbase.h>      // For COM headers
#include <Shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <Shlwapi.h>
#include <Knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <Propvarutil.h>  // for PROPVAR-related functions
#include <Propkey.h>      // for the Property key APIs/datatypes
#include <Propidl.h>      // for the Property System APIs
#include <Strsafe.h>      // for StringCchPrintfW
#include <Shtypes.h>      // for COMDLG_FILTERSPEC
#include <New>
#include <Shobjidl.h>  // For IFileDialogEvents

std::wstring GetFile(const wchar_t* _caption, DWORD _options)
{
    IFileOpenDialog* pFileOpen = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                      IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    std::wstring path;

    while (path.empty())
    {
        if (SUCCEEDED(hr))
        {
            DWORD dwOptions;
            if (SUCCEEDED(pFileOpen->GetOptions(&dwOptions)))
            {
                pFileOpen->SetOptions(dwOptions | _options);
            }
        
            hr = pFileOpen->Show(NULL);
            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath = NULL;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr))
                    {
                        //MessageBoxW(NULL, pszFilePath, _caption, MB_OK);
                        path = pszFilePath;
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
    }
    
    return path;
}


std::wstring SystemDialogue::SeletecFolder(const wchar_t* _caption)
{
    return GetFile(_caption, FOS_PICKFOLDERS);
}

std::wstring SystemDialogue::SeletecFile(const wchar_t* _caption)
{
    return GetFile(_caption, 0);
}

SystemDialogue::SystemDialogue()
{
    const HRESULT hr = CoInitialize(NULL);
    assert( hr == S_OK && "Faile to CoInitialize");
}

SystemDialogue::~SystemDialogue()
{
    CoUninitialize();
}
