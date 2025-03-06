#include "file_system_watcher.h"

using namespace PC_EDITOR_CORE;

#include <windows.h>
#include <Winbase.h>
#include <cassert>

static wchar_t filename[MAX_PATH];
static FILE_NOTIFY_INFORMATION Buffer[1024];
DWORD BytesReturned;

void FileSystemWatcher::LauchWatcher(const char* _projectBaseAssetPath)
{
	m_ProjectBaseAssetPath = _projectBaseAssetPath;
	m_Watch = true;
	//worker = std::thread(&FileSystemWatcher::WorkerMainLoop, this);
}

bool FileSystemWatcher::ResourceDirty(std::filesystem::path* _outItemFile)
{
	
	return false;
}

FileSystemWatcher::~FileSystemWatcher()
{
	if (m_Watch)
	{
		m_Watch = false;
		//worker.join();
	}

}



void FileSystemWatcher::WorkerMainLoop()
{
	HANDLE baseHandle = CreateFile(m_ProjectBaseAssetPath.c_str(), // pointer to the file name
		FILE_LIST_DIRECTORY,                // access (read/write) mode
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,  // share mode
		NULL,                               // security descriptor
		OPEN_EXISTING,                      // how to create
		FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
		NULL                                // file with attributes to copy
	);

	assert(baseHandle != 0);


	while (m_Watch)
	{
		auto f = FILE_NOTIFY_CHANGE_SECURITY |
			FILE_NOTIFY_CHANGE_CREATION |
			FILE_NOTIFY_CHANGE_LAST_ACCESS |
			FILE_NOTIFY_CHANGE_LAST_WRITE |
			FILE_NOTIFY_CHANGE_SIZE |
			FILE_NOTIFY_CHANGE_ATTRIBUTES |
			FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_FILE_NAME;

		BOOL b = ReadDirectoryChangesW(baseHandle, &Buffer, sizeof(Buffer), TRUE, f, &BytesReturned, NULL, NULL);

		int offset = 0;
		FILE_NOTIFY_INFORMATION* pNotify;
		pNotify = (FILE_NOTIFY_INFORMATION*)((char*)Buffer + offset);
		wcsncpy(filename, pNotify->FileName, pNotify->FileNameLength / 2);

		filename[pNotify->FileNameLength / 2] = NULL;

		switch (Buffer[0].Action)
		{
		case FILE_ACTION_MODIFIED:
			//watcherObj.OnFileChange(filename);
			break;
		case FILE_ACTION_ADDED:
			//watcherObj.OnFileAdded(filename);
			break;
		case FILE_ACTION_REMOVED:
			//watcherObj.OnFileRemoved(filename);
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			//watcherObj.OnFileRenamed(filename);
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			//watcherObj.OnFileRenamed(filename);
			break;
		}

	}
}


