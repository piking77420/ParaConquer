#include "SystemDialogue.hpp"

#include <cassert>

#if _WIN32
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
#endif


#if _WIN32
std::wstring GetFile(const wchar_t* _caption, const wchar_t* _basePath, DWORD _options)
{
    std::wstring path;


    IFileOpenDialog* pFileOpen = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                                  IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));


    if (SUCCEEDED(hr))
    {
        DWORD dwOptions;
        if (SUCCEEDED(pFileOpen->GetOptions(&dwOptions)))
        {
            pFileOpen->SetOptions(dwOptions | _options);
        }

        if (_basePath != nullptr)
        {
            IShellItem* pFolder = nullptr;
            hr = SHCreateItemFromParsingName(_basePath, nullptr, IID_PPV_ARGS(&pFolder));
            if (SUCCEEDED(hr))
            {
                // Set the initial folder
                pFileOpen->SetFolder(pFolder);
                pFolder->Release();
            }
        }

        hr = pFileOpen->Show(nullptr);
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem;
            hr = pFileOpen->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath = nullptr;
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
    }

    pFileOpen->Release();



    return path;
}
#else 
 
#endif

std::wstring SystemDialogue::SeletecFolder(const wchar_t* _caption)
{
#if _WIN32
    return GetFile(_caption, nullptr, FOS_PICKFOLDERS);
#else 
return {};
#endif
}

std::wstring SystemDialogue::SeletecFile(const wchar_t* _caption, const wchar_t* _basePath)
{

    #if _WIN32
    return GetFile(_caption, _basePath, 0);
    #else 
    return {};
    #endif
    
}

SystemDialogue::SystemDialogue()
{
    #if _WIN32
    const HRESULT hr = CoInitialize(nullptr);
    assert(hr == S_OK && "Faile to CoInitialize");
    #else 
    #endif
}

SystemDialogue::~SystemDialogue()
{
    #if _WIN32
    CoUninitialize();
    #else 
    #endif
}
