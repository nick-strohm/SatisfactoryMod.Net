/*
 * File: ClrHost.cpp
 * Author: MarkAtk
 * Date: 09.10.2018
 *
 * MIT License
 *
 * Copyright (c) 2018 Rage-MP-C-SDK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ClrHost.h"

#include <windows.h>
#include <iostream>
#include <filesystem>

#define RUNTIME_DIR_PATH "./dotnet/runtime/"
#define PLUGIN_DIR_PATH "./dotnet/plugins/"

#define PLUGIN_NAME "SatisfactoryMod.Net"
#define PLUGIN_CLASS_NAME "SatisfactoryMod.Net.PluginWrapper"

#define LIST_SEPARATOR ";"

ClrHost::ClrHost() {
	_runtimeHost = nullptr;
	_domainId = 0;
	_mainCallback = nullptr;

	_coreClrLib = nullptr;
	_initializeCoreCLR = nullptr;
	_shutdownCoreCLR = nullptr;
	_createDelegate = nullptr;
	_executeAssembly = nullptr;
}

ClrHost::~ClrHost() {
	unload();
}

bool ClrHost::load() {
	if (_coreClrLib == nullptr && !loadCoreClr()) {
		return false;
	}

	if ((_runtimeHost == nullptr || _domainId == 0) && !createAppDomain()) {
		return false;
	}

	if (_mainCallback == nullptr && getDelegate("PluginMain", (void **)&_mainCallback) == false) {
		return false;
	}

	return true;
}

void ClrHost::unload() {
	if (_runtimeHost == nullptr) {
		return;
	}

	_shutdownCoreCLR(_runtimeHost, _domainId, nullptr);

	_runtimeHost = nullptr;
	_domainId = 0;
	_mainCallback = nullptr;
}

MainMethod ClrHost::mainCallback() const {
	return _mainCallback;
}

bool ClrHost::loadCoreClr() {
	std::string coreClrDllPath = getAbsolutePath(RUNTIME_DIR_PATH);
	coreClrDllPath += "coreclr.dll";

	std::filesystem::path path = coreClrDllPath;

	if (!exists(path))
	{
		std::cerr << LOG_HEADER << "CoreCLR runtime is not installed" << std::endl;

		return false;
	}

	_coreClrLib = LoadLibraryEx(coreClrDllPath.c_str(), NULL, 0);
	if (_coreClrLib == NULL) {
		std::cerr << LOG_HEADER << "Unable to find CoreCLR dll or one of its dependencies" << std::endl;

		return false;
	}

	_initializeCoreCLR = (coreclr_initialize_ptr)GetProcAddress(_coreClrLib, "coreclr_initialize");
	_shutdownCoreCLR = (coreclr_shutdown_2_ptr)GetProcAddress(_coreClrLib, "coreclr_shutdown_2");
	_createDelegate = (coreclr_create_delegate_ptr)GetProcAddress(_coreClrLib, "coreclr_create_delegate");
	_executeAssembly = (coreclr_execute_assembly_ptr)GetProcAddress(_coreClrLib, "coreclr_execute_assembly");

	if (_initializeCoreCLR == nullptr || _shutdownCoreCLR == nullptr || _createDelegate == nullptr || _executeAssembly == nullptr) {
		std::cerr << LOG_HEADER << "Unable to find CoreCLR dll methods" << std::endl;

		return false;
	}

	return true;
}

bool ClrHost::createAppDomain() {
	std::string tpaList = "";

	for (auto &tpa : getTrustedAssemblies()) {
		tpaList += tpa;
		tpaList += LIST_SEPARATOR;
	}

	auto appPath = getAbsolutePath(PLUGIN_DIR_PATH);

	auto nativeDllPaths = appPath;
	nativeDllPaths += LIST_SEPARATOR;
	nativeDllPaths += getAbsolutePath(RUNTIME_DIR_PATH);

	auto rootDirectory = getAbsolutePath(".");

	const char *propertyKeys[] = {
		"TRUSTED_PLATFORM_ASSEMBLIES",
		"APP_PATHS",
		"APP_NI_PATHS",
		"NATIVE_DLL_SEARCH_DIRECTORIES",
		//"System.GC.Server",
		"System.Globalization.Invariant",
	};

	const char *propertyValues[] = {
		tpaList.c_str(),
		appPath.c_str(),
		appPath.c_str(),
		nativeDllPaths.c_str(),
		//"true",
		"true",
	};

	int result = _initializeCoreCLR(
		rootDirectory.c_str(),
		"host",
		sizeof(propertyKeys) / sizeof(propertyKeys[0]),
		propertyKeys,
		propertyValues,
		&_runtimeHost,
		&_domainId
	);

	if (result < 0) {
		std::cerr << LOG_HEADER << "Unable to create app domain: 0x" << std::hex << result << std::endl;

		return false;
	}

	// execute assembly to set a valid entry point which is needed by some libraries as mysql
	unsigned int exitCode = 0;
	auto libraryPath = getAbsolutePath(std::string(PLUGIN_DIR_PATH) + PLUGIN_NAME + ".dll");

	try
	{
		result = _executeAssembly(
			_runtimeHost,
			_domainId,
			0,
			nullptr,
			libraryPath.c_str(),
			&exitCode
		);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	
	if (result < 0) {
		std::cerr << "[.NET] Unable to execute assembly: 0x" << std::hex << result << std::endl;
	
		return false;
	}

	return true;
}

std::set<std::string> ClrHost::getTrustedAssemblies() {
	std::set<std::string> assemblies;

	const char * const tpaExtensions[] = { ".ni.dll", ".dll", ".ni.exe", ".exe", ".winmd" };

	std::string runtimeDirectory = getAbsolutePath(RUNTIME_DIR_PATH);

	for (int extIndex = 0; extIndex < sizeof(tpaExtensions) / sizeof(tpaExtensions[0]); extIndex++) {
		const char* ext = tpaExtensions[extIndex];
		size_t extLength = strlen(ext);

		std::string searchPath = runtimeDirectory;
		searchPath += "*";
		searchPath += ext;

		WIN32_FIND_DATA findData;
		HANDLE fileHandle = FindFirstFile(searchPath.c_str(), &findData);

		if (fileHandle == INVALID_HANDLE_VALUE) {
			continue;
		}

		do {
			std::string filePath = runtimeDirectory;
			filePath += findData.cFileName;

			// Ensure assemblies are unique in the list
			if (assemblies.find(filePath) != assemblies.end()) {
				continue;
			}

			assemblies.insert(filePath);
		} while (FindNextFile(fileHandle, &findData));

		FindClose(fileHandle);
	}

	return assemblies;
}

bool ClrHost::getDelegate(std::string methodName, void **callback) {
	if (_runtimeHost == nullptr || _domainId == 0) {
		std::cerr << LOG_HEADER << "Core CLR host not loaded" << std::endl;

		return false;
	}

	int result = _createDelegate(_runtimeHost, _domainId, PLUGIN_NAME, PLUGIN_CLASS_NAME, methodName.c_str(), callback);
	if (result < 0) {
		std::cerr << LOG_HEADER << "Unable to get '" << methodName << "'" << std::endl;

		return false;
	}

	return true;
}

std::string ClrHost::getAbsolutePath(std::string relativePath) {
	std::filesystem::path path = relativePath;
	std::filesystem::path absolutePath = std::filesystem::absolute(path);

	return absolutePath.u8string();
}

std::string ClrHost::getFilenameWithoutExtension(std::string filename) {
	auto pos = filename.rfind(".");
	if (pos == std::string::npos) {
		return filename;
	}

	return filename.substr(0, pos);
}