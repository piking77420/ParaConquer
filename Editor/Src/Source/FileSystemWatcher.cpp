#include "FileSystemWatcher.h"

using namespace PC_EDITOR_CORE;

#include <Windows.h>
#include <Winbase.h>
#include <Cassert>

#include "Thread/ThreadUtils.hpp"


DWORD BytesReturned;

size_t GetSizeOfNotify(const FILE_NOTIFY_INFORMATION* notify)
{
	const FILE_NOTIFY_INFORMATION* end = notify;

	while (end->NextEntryOffset != 0)
	{
		end = (FILE_NOTIFY_INFORMATION*)((char*)end + end->NextEntryOffset);
	}
	return end - notify;
}

bool IsValidFormat(const wchar_t** _formats, size_t _formatCount, const wchar_t* _string, const wchar_t** _format)
{
	const size_t stringSize = std::wcslen(_string);

	for (size_t i = 0; i < _formatCount; i++)
	{
		const size_t formatLength = std::wcslen(_formats[i]);

		if (stringSize >= formatLength &&
			std::wcsncmp(_string + stringSize - formatLength, _formats[i], formatLength) == 0)
		{
			*_format = _formats[i];
			return true;
		}
	}

	return false; 
}


void FileSystemWatcher::LauchWatcher(const FileWatcherCreateInfo& _fileWatcherCreateInfo)
{
	m_WatchRoot = _fileWatcherCreateInfo.watchRoot;
	m_ExpectFormat = _fileWatcherCreateInfo.expectFormat;
	m_FileWatcherEvents = _fileWatcherCreateInfo.fileWatcherEvents;

	m_Worker.watch = true;
	m_Worker.asyncObj = std::make_unique<uint8_t[]>(sizeof(OVERLAPPED));
	m_Worker.systemBuffer = std::make_unique<uint8_t[]>(sizeof(FILE_NOTIFY_INFORMATION) * 1024);
	m_Worker.fileNameBuffer.resize(1024);
	
	// Lauch Thread
	m_Worker.thread = std::thread([&, _fileWatcherCreateInfo]()
		{
			PC_CORE::Utils::SetThreadName(_fileWatcherCreateInfo.watcherName);
			WorkerMainLoop();
		});
}

FileSystemWatcher::~FileSystemWatcher()
{
	Stop();
}
void FileSystemWatcher::Stop()
{
	if (m_Worker.watch)
	{
		m_Worker.watch = false;
		CancelIoEx(m_Worker.fileHandle, reinterpret_cast<OVERLAPPED*>(&m_Worker.asyncObj[0]));
		m_Worker.thread.join();
	}
}
void FileSystemWatcher::WorkerMainLoop()
{
	m_Worker.fileHandle = CreateFile(m_WatchRoot, // pointer to the file name
		FILE_LIST_DIRECTORY,                // access (read/write) mode
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,  // share mode
		NULL,                               // security descriptor
		OPEN_EXISTING,                      // how to create
		FILE_FLAG_BACKUP_SEMANTICS,         // file attributes
		NULL                                // file with attributes to copy
	);

	assert(m_Worker.fileHandle != 0);

	std::array<FILE_NOTIFY_INFORMATION, 1024>& fileNotificationBuffer = *reinterpret_cast<std::array<FILE_NOTIFY_INFORMATION, 1024>*>(m_Worker.systemBuffer.get());
	std::vector<int> fileActions;

	while (m_Worker.watch)
	{
		DWORD flag = FILE_NOTIFY_CHANGE_CREATION |
			FILE_NOTIFY_CHANGE_FILE_NAME |
			FILE_NOTIFY_CHANGE_DIR_NAME |
			FILE_NOTIFY_CHANGE_LAST_WRITE;

		DWORD byteReturn;
		BOOL b = ReadDirectoryChangesW(m_Worker.fileHandle, fileNotificationBuffer.data(), fileNotificationBuffer.size() * sizeof(FILE_NOTIFY_INFORMATION),
			TRUE, flag, &byteReturn, reinterpret_cast<OVERLAPPED*>(&m_Worker.asyncObj[0]), NULL);

		FILE_NOTIFY_INFORMATION* pNotify = (FILE_NOTIFY_INFORMATION*)fileNotificationBuffer.data();

		// Parsing Actions
		for (;;)
		{
			const size_t nameLen = pNotify->FileNameLength / sizeof(WCHAR);
			wcsncpy(m_Worker.fileNameBuffer.data(), pNotify->FileName, nameLen);
			m_Worker.fileNameBuffer[nameLen] = L'\0';

			const wchar_t* format = nullptr;
			bool isFormatHandle = IsValidFormat(m_ExpectFormat.data(), m_ExpectFormat.size(), m_Worker.fileNameBuffer.data(), &format);

			if (isFormatHandle)
			{
				RecordFileModifycation(pNotify, m_Worker.fileNameBuffer.data(), nameLen, format);
			}

			if (pNotify->NextEntryOffset) {
				pNotify = (FILE_NOTIFY_INFORMATION*)((char*)pNotify + pNotify->NextEntryOffset);
			}
			else {
				break;
			}
		}		
	}
}

void FileSystemWatcher::RecordFileModifycation(void* _notifyPtr, const wchar_t* _filePath, size_t _fileNameLenght, const wchar_t* _format)
{
	PERF_REGION_SCOPED;

	const FILE_NOTIFY_INFORMATION* pNotify = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(_notifyPtr);
	const std::string file = std::string(_filePath, _filePath + _fileNameLenght);

	switch (pNotify->Action)
	{
	case FILE_ACTION_MODIFIED:
	{
		ZoneTextF("This File Has Been Modify %ls", _filePath);
		PC_LOG("This File Has Been Modify {}", file);
	}
		break;
	case FILE_ACTION_ADDED:
	{
		ZoneTextF("This File Has Been Added %ls", _filePath);
		PC_LOG("This File Has Been Added {}", file);

	}
		break;
	case FILE_ACTION_REMOVED:
	{
		ZoneTextF("This File Has Been Removed %ls", _filePath);
		PC_LOG("This File Has Been Removed {}", file);

	}
		break;
	case FILE_ACTION_RENAMED_OLD_NAME:
	{
		ZoneTextF("This old name is %ls", _filePath);
		PC_LOG("This old name is {}", file);

	}
		break;
	case FILE_ACTION_RENAMED_NEW_NAME:
	{
		ZoneTextF("This new name is %ls", _filePath);
		PC_LOG("This new name is {}", file);

	}
		break;
	default:
		assert(false);
	}

	m_Worker.filePendingActions[std::wstring(_filePath)].push_back(pNotify->Action);
}

void FileSystemWatcher::HandleModifcation()
{

	for (auto& it : m_Worker.filePendingActions)
	{
		const auto& actions = it.second;

		switch (actions.size())
		{
		
		case 1: 
		{
			switch (actions[0])
			{
			case FILE_ACTION_MODIFIED:
			{
				if (m_FileWatcherEvents.onFileModify)
				{
					FileModifyEventData fileModifyEventData(it.first.c_str());
					m_FileWatcherEvents.onFileModify(fileModifyEventData);
				}
			}
			break;
			case FILE_ACTION_ADDED:
			case FILE_ACTION_REMOVED:
			case FILE_ACTION_RENAMED_OLD_NAME:
			case FILE_ACTION_RENAMED_NEW_NAME:
				break;
			default:
				assert(false);
			}
			break;
		}
		default:
			break;
		}

	}

}


