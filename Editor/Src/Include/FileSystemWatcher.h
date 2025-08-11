#pragma once

#include <thread>
#include <unordered_map>
#include <mutex>

#include "EditorHeader.hpp"

#include "Resources/ResourceManager.hpp";

BEGIN_EDITOR_PCCORE

class FileSystemWatcher
{
public:
	FileSystemWatcher() = default;

	~FileSystemWatcher();

	void LauchWatcher(const char* _projectBaseAssetPath);

	//bool ResourceDirty(std::filesystem::path* _outItemFile);

private:
	std::thread worker;

	//std::filesystem::path m_ProjectBaseAssetPath;

	std::atomic<bool> m_Watch = false;

	std::mutex resourceChangeMutex;



	void WorkerMainLoop();

};

END_EDITOR_PCCORE