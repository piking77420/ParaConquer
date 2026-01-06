#pragma once

#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <string>

#include "EditorHeader.hpp"

#include "Resources/ResourceManager.hpp"

constexpr size_t MAX_SYS_PATH = 1024;
constexpr size_t SYSTEM_FILE_NOTIFCATION_BUFFER_SIZE = 1024;

BEGIN_EDITOR_PCCORE
    struct FileModifyEventData
    {
        const wchar_t* path;
    };

    struct FileModifyName
    {
        const wchar_t* oldName;
        const wchar_t* newName;
        const wchar_t* oldFormat;
        const wchar_t* newFormat;
    };

    struct FileAdd
    {
        const wchar_t* oldName;
        const wchar_t* newName;
        const wchar_t* oldFormat;
        const wchar_t* newFormat;
    };

    struct FileRemove
    {
        const wchar_t* oldName;
        const wchar_t* newName;
        const wchar_t* oldFormat;
        const wchar_t* newFormat;
    };


    struct FileWatcherEvents
    {
        std::function<void(const FileModifyEventData&)> onFileModify;
        std::function<void(const FileModifyName&)> onFileModifyName;
        std::function<void(const FileAdd&)> onFileAdd;
        std::function<void(const FileRemove&)> onFileRemove;
    };

    struct FileWatcherCreateInfo
    {
        const char* watcherName;
        const char* watchRoot;
        std::vector<const wchar_t*> expectFormat;
        FileWatcherEvents fileWatcherEvents;
    };

    class FileSystemWatcher
    {
    public:
        void Stop();

        void LauchWatcher(const FileWatcherCreateInfo& _fileWatcherCreateInfo);

        FileSystemWatcher() = default;

        ~FileSystemWatcher();

    private:
        struct Worker
        {
            std::thread thread;

            void* fileHandle;

            std::atomic<bool> watch = false;

            std::unique_ptr<uint8_t[]> asyncObj;

            std::wstring fileNameBuffer;

            std::unique_ptr<uint8_t[]> systemBuffer;

            std::unordered_map<std::wstring, std::vector<int>> filePendingActions;
        } m_Worker;


        const char* m_WatchRoot;

        FileWatcherEvents m_FileWatcherEvents;

        std::vector<const wchar_t*> m_ExpectFormat;

        void WorkerMainLoop();

        void RecordFileModifycation(void* _notifyPtr, const wchar_t* _fileName, size_t _fileNameLenght,
                                    const wchar_t* _format);

        void HandleModifcation();
    };

END_EDITOR_PCCORE
