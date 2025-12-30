/********************************************************************
	Minecraft: Pocket Edition - Decompilation Project
	Copyright (C) 2023 iProgramInCpp
	
	The following code is licensed under the BSD 1 clause license.
	SPDX-License-Identifier: BSD-1-Clause
 ********************************************************************/

// note: not an official file name

#include "common/Utils.hpp"
#include "compat/PlatformDefinitions.h"
#include <ostream>
#include <iostream>
#include <stdexcept>

#if defined(_WIN32) && !defined(_XBOX)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <psapi.h>

// Why are we not using GetTickCount64()? It's simple -- getTimeMs has the exact same problem as using regular old GetTickCount.
#pragma warning(disable : 28159)

#elif defined(_XBOX)

#else
#include <fstream>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>

#endif

#if MC_PLATFORM_MAC
#include <mach/mach.h>
#endif


// include zlib stuff
// cant get zlib to build on android, they include prebuilt one anyways. using that one
#include "zlib.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <iomanip>

int g_TimeSecondsOnInit = 0;

#if (!defined(USE_SDL) || defined(_WIN32)) && !defined(ANDROID) && !MC_PLATFORM_MAC

DIR* opendir(const char* name)
{
	size_t len = strlen(name);
	if (len == 0)
		return NULL;

	char buf[1024];
	if (len >= sizeof(buf) - 5)
		return NULL;

	strcpy(buf, name);

	if (name[len - 1] != '/')
		strcpy(&buf[len], "/*");
	else
		strcpy(&buf[len], "*");

	DIR* pDir = (DIR*)malloc(sizeof(DIR));
	if (!pDir)
		return pDir;

	memset(pDir, 0, sizeof * pDir);

	// Stupid Unicode bullshit
	//LPTSTR msBuff;
	//mbstowcs(&msBuff, buf, sizeof(buf));

	pDir->current = FindFirstFile(buf, &pDir->findData);
	if (pDir->current == INVALID_HANDLE_VALUE)
	{
		free(pDir);
		return NULL;
	}

	return pDir;
}

dirent* readdir(DIR* dir)
{
	if (dir->current == INVALID_HANDLE_VALUE)
		return NULL;

	static dirent de;

	if (!dir->returnedFirstFileData)
	{
		dir->returnedFirstFileData = true;
	}
	else
	{
		if (!FindNextFile(dir->current, &dir->findData))
			return NULL;
	}

	LPTSTR fileName = dir->findData.cFileName;

	// Stupid Unicode bullshit
	//char* fileName;
	//wcstombs(fileName, fileNameMs, 255);

	strcpy(de.d_name, fileName);
	de.d_type = (dir->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? DT_DIR : DT_REG;

	return &de;
}

void closedir(DIR* dir)
{
	if (dir->current != INVALID_HANDLE_VALUE)
		FindClose(dir->current);

	free(dir);
}

#else

#include <sys/types.h>
#include <dirent.h>

#endif

bool createFolderIfNotExists(const char* pDir)
{
	if (!XPL_ACCESS(pDir, 0))
		return true;

	return XPL_MKDIR(pDir, 0755) == 0;
}

bool createFoldersIfNotExists(const char* pDir)
{
	if (!pDir || !*pDir)
		return false;

	std::string path = pDir;

	size_t last_slash = path.find_last_of("/\\");
	if (last_slash == std::string::npos)
		return true;

	std::string parent_path = path.substr(0, last_slash);
	if (parent_path.empty())
		return true;

	std::string current_path;
	bool is_absolute = (path[0] == '/' || (path.length() >= 2 && path[1] == ':'));

	size_t start = 0;
	if (is_absolute && path[0] == '/')
	{
		current_path = "/";
		start = 1;
	}
	else if (is_absolute && path[1] == ':')
	{
		current_path = path.substr(0, 2);  // e.g., "C:"
		start = 2;
		if (path.length() > 2 && (path[2] == '/' || path[2] == '\\'))
			start = 3;  // Skip "C:/" or "C:\"
	}

	while (start < parent_path.length())
	{
		size_t end = parent_path.find_first_of("/\\", start);
		if (end == std::string::npos)
			end = parent_path.length();

		if (!current_path.empty() && current_path.back() != '/' && current_path.back() != '\\')
			current_path += "/";

		current_path += parent_path.substr(start, end - start);
		start = end + 1;

		while (start < parent_path.length() && (parent_path[start] == '/' || parent_path[start] == '\\'))
			++start;

		// Check if the current path exists
		if (XPL_ACCESS(current_path.c_str(), 0) == 0)
		{
			struct stat st;
			if (stat(current_path.c_str(), &st) == 0)
			{
				if (!S_ISDIR(st.st_mode))
					return false;
			}
			continue;  // Exists and is a directory
		}

		// Create the directory
		if (XPL_MKDIR(current_path.c_str(), 0755) != 0)
			return false;
	}

	return true;
}

bool DeleteDirectory(const std::string& name2, bool unused)
{
	std::string name = name2;
	if (name.empty())
		return false;

	if (name[name.size() - 1] == '/')
		name.resize(name.size() - 1);

	DIR* dir = opendir(name.c_str());
	if (!dir)
		return false;

	while (true)
	{
		dirent* de = readdir(dir);
		if (!de)
			break;

		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
			continue;

		const std::string dirPath = name + "/" + de->d_name;
		remove(dirPath.c_str());
	}

	closedir(dir);

#ifdef _WIN32
	return RemoveDirectoryA(name.c_str());
#else
	return remove(name.c_str()) == 0;
#endif
}

const char* GetTerrainName()
{
	return "terrain.png";
}

const char* GetItemsName()
{
	return "gui/items.png";
}

const char* GetGUIBlocksName()
{
	return "gui/gui_blocks.png";
}

#ifdef _WIN32
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
	// We're on a version of Windows that's older than XP, it's time to go big or go home...
	// This is a less-accurate way of getting the time we need, but it may work as a failsafe.
	{
		// https://stackoverflow.com/questions/10905892/equivalent-of-gettimeofday-for-windows

		// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
		// This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
		// until 00:00:00 January 1, 1970
		static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

		SYSTEMTIME  system_time;
		FILETIME    file_time;
		uint64_t    time;

		GetSystemTime(&system_time);
		SystemTimeToFileTime(&system_time, &file_time); // this can fail. if it does, nothing will tick
		time = ((uint64_t)file_time.dwLowDateTime);
		time += ((uint64_t)file_time.dwHighDateTime) << 32;

		tp->tv_sec = (long)((time - EPOCH) / 10000000L);
		tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
		return 0;
	}
}
#endif

double getTimeS()
{
#ifdef _WIN32
	// Let's shoot our shot, but if we have to change teams mid-game, we're fucked because g_TimeSecondsOnInit won't be set.
	{
		static LARGE_INTEGER Frequency = { 0 };
		static LARGE_INTEGER CounterStart = { 0 }; // a more accurate verison of g_TimeSecondsOnInit specifically for Win32

		// https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-large_integer-r1
		// "If your compiler has built-in support for 64-bit integers, use the QuadPart member to store the 64-bit integer. Otherwise, use the LowPart and HighPart members to store the 64-bit integer."
		if (Frequency.QuadPart == 0)
			QueryPerformanceFrequency(&Frequency);

		LARGE_INTEGER liTime;
		if (QueryPerformanceCounter(&liTime))
		{
			if (CounterStart.QuadPart == 0)
				CounterStart = liTime;

			LARGE_INTEGER liElapsedSeconds;
			LARGE_INTEGER liElapsedMicroseconds;
			liElapsedSeconds.QuadPart = liTime.QuadPart - CounterStart.QuadPart;
			
			//
			// We now have the elapsed number of ticks, along with the
			// number of ticks-per-second. We use these values
			// to convert to the number of elapsed microseconds.
			// To guard against loss-of-precision, we convert
			// to microseconds *before* dividing by ticks-per-second.
			//

			liElapsedMicroseconds.QuadPart = (liElapsedSeconds.QuadPart * 100000) / Frequency.QuadPart;

			return double(liElapsedMicroseconds.QuadPart) * (1.0 / 100000.0);
		}
	}
#endif

	// Variant implemented by Mojang. This does not work on MSVC.
	{
		timeval tv;

		gettimeofday(&tv, NULL);

		if (g_TimeSecondsOnInit == 0)
			g_TimeSecondsOnInit = tv.tv_sec;

		return double(tv.tv_sec - g_TimeSecondsOnInit) + double(tv.tv_usec) / 1000000.0;
	}
}

int getTimeMs()
{
	return int(getTimeS() * 1000.0);
}

int64_t getTimeNano()
{
	return int64_t(getTimeS() * 1000.0);
}

std::string getFormattedDate(int64_t time)
{
	std::time_t seconds = time / 1000;
	std::tm* tm_ptr = std::gmtime(&seconds);
	std::ostringstream a;
	a << std::put_time(tm_ptr, "%m/%d/%Y %I:%M %p");
	return a.str();
}

time_t getRawTimeS()
{
	timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec;
}

time_t getEpochTimeS()
{
	return time(0);
}

int64_t getMillis() {
#ifdef _WIN32
	// Windows version using GetSystemTimeAsFileTime
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);

	uint64_t time = ((uint64_t)ft.dwLowDateTime) | ((uint64_t)ft.dwHighDateTime << 32);

	// Convert from 100-nanosecond intervals since Jan 1, 1601 to milliseconds since Jan 1, 1970
	return (int64_t)((time - 116444736000000000ULL) / 10000ULL);
#else
	// POSIX version
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000 + (int64_t)(tv.tv_usec / 1000);
#endif
}

#ifdef _WIN32

HINSTANCE g_hInstance = NULL;
HWND g_hWnd = NULL;

void SetInstance(HINSTANCE hinst)
{
	g_hInstance = hinst;
}

HINSTANCE GetInstance()
{
	return g_hInstance;
}

void SetHWND(HWND hwnd)
{
	g_hWnd = hwnd;
}

HWND GetHWND()
{
	return g_hWnd;
}

void CenterWindow(HWND hWnd)
{
	RECT r, desk;
	GetWindowRect(hWnd, &r);
	GetWindowRect(GetDesktopWindow(), &desk);

	int wa, ha, wb, hb;

	wa = (r.right - r.left) / 2;
	ha = (r.bottom - r.top) / 2;

	wb = (desk.right - desk.left) / 2;
	hb = (desk.bottom - desk.top) / 2;

	SetWindowPos(hWnd, NULL, wb - wa, hb - ha, r.right - r.left, r.bottom - r.top, 0);
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	PIXELFORMATDESCRIPTOR pfd;

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);

	/* set the pixel format for the DC */
	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hwnd, hDC);
}

#endif

void sleepMs(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else
	usleep(1000 * ms);
#endif
}

// zlib stuff
uint8_t* ZlibInflateToMemory(uint8_t* pInput, size_t compressedSize, size_t decompressedSize)
{
	int ret;
	z_stream strm;
	memset(&strm, 0, sizeof strm);

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;

	// initialize the inflation state machine
	ret = inflateInit(&strm);
	if (ret != Z_OK)
		return nullptr;

	uint8_t* pDestBuff = new uint8_t[decompressedSize + 1]; //room for extra null at the end;

	pDestBuff[decompressedSize] = 0; //add the extra null, if we decompressed a text file this can be useful
	strm.avail_in = compressedSize;
	strm.next_in = pInput;
	strm.avail_out = decompressedSize;
	strm.next_out = pDestBuff;

	ret = inflate(&strm, Z_NO_FLUSH);
	if (!(ret == Z_OK || ret == Z_STREAM_END))
	{
		SAFE_DELETE_ARRAY(pDestBuff);
		return nullptr;
	}

	(void)inflateEnd(&strm);

	return pDestBuff;
}

uint8_t* ZlibDeflateToMemory(uint8_t* pInput, size_t sizeBytes, size_t* compressedSizeOut)
{
	return ZlibDeflateToMemoryLvl(pInput, sizeBytes, compressedSizeOut, Z_DEFAULT_COMPRESSION);
}

uint8_t* ZlibDeflateToMemoryLvl(uint8_t* pInput, size_t sizeBytes, size_t* compressedSizeOut, int level)
{
	z_stream strm;
	memset(&strm, 0, sizeof strm);
	int ret;

	// initialize deflate state
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	// initialize deflation state machine
	ret = deflateInit(&strm, level);
	if (ret != Z_OK)
		return nullptr;

	// padding bytes in case our compressed output is larger than the raw input for some reason
	const int ZLIB_PADDING_BYTES = (1024 * 5);
	
	uint8_t* pOut = new uint8_t[sizeBytes + ZLIB_PADDING_BYTES];
	strm.avail_in = sizeBytes;
	strm.next_in = pInput;
	strm.avail_out = sizeBytes + ZLIB_PADDING_BYTES;
	strm.next_out = pOut;

	ret = deflate(&strm, Z_FINISH);
	assert(ret != Z_STREAM_ERROR);

	deflateEnd(&strm);
	*compressedSizeOut = strm.total_out;

	return pOut;
}

void compressZlib(const std::string& input, std::string& output) {
	uLongf compressedSize = compressBound(input.size());
	std::vector<char> buffer(compressedSize);

	int res = compress2(
		reinterpret_cast<Bytef*>(buffer.data()), &compressedSize,
		reinterpret_cast<const Bytef*>(input.data()), input.size(),
		Z_BEST_COMPRESSION
	);

	if (res != Z_OK) {
		throw std::runtime_error("Zlib compression failed.");
	}

	output.assign(buffer.data(), compressedSize);
}

void decompressZlib(const std::string& input, std::string& output, size_t expectedSize = 0) {
	size_t sizeHint = expectedSize > 0 ? expectedSize : input.size() * 4;
	std::vector<char> buffer(sizeHint);

	uLongf destLen = sizeHint;
	int res = uncompress(
		reinterpret_cast<Bytef*>(buffer.data()), &destLen,
		reinterpret_cast<const Bytef*>(input.data()), input.size()
	);

	if (res != Z_OK) {
		throw std::runtime_error("Zlib decompression failed.");
	}

	output.assign(buffer.data(), destLen);
}

std::vector<uint8_t> compressZlibStream(const uint8_t* inputData, size_t inputSize, bool useGzip, int compressionLevel) {
	z_stream strm{};
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	int windowBits = useGzip ? 16 + MAX_WBITS : MAX_WBITS;

	if (deflateInit2(&strm, compressionLevel, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY) != Z_OK)
		throw std::runtime_error("deflateInit2 failed");

	strm.avail_in = static_cast<uInt>(inputSize);
	strm.next_in = const_cast<Bytef*>(inputData);

	std::vector<uint8_t> output;
	const size_t CHUNK_SIZE = 4096;
	uint8_t outChunk[CHUNK_SIZE];

	int ret;
	do {
		strm.avail_out = CHUNK_SIZE;
		strm.next_out = outChunk;

		ret = deflate(&strm, Z_FINISH);
		if (ret == Z_STREAM_ERROR) {
			deflateEnd(&strm);
			throw std::runtime_error("deflate failed");
		}

		size_t have = CHUNK_SIZE - strm.avail_out;
		output.insert(output.end(), outChunk, outChunk + have);
	} while (strm.avail_out == 0);

	deflateEnd(&strm);
	return output;
}



std::vector<uint8_t> decompressZlibStream(const uint8_t* inputData, size_t inputSize, bool useGzip) {
	if (!inputData || inputSize == 0)
		throw std::runtime_error("invalid input for decompression");

	z_stream strm{};
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	strm.avail_in = static_cast<uInt>(inputSize);
	strm.next_in = const_cast<Bytef*>(inputData);

	// Enable gzip header support if requested
	int windowBits = useGzip ? 16 + MAX_WBITS : MAX_WBITS;

	if (inflateInit2(&strm, windowBits) != Z_OK)
		throw std::runtime_error("inflateInit2 failed");

	std::vector<uint8_t> output;
	const size_t CHUNK_SIZE = 4096;
	uint8_t outChunk[CHUNK_SIZE];

	int ret;
	do {
		strm.avail_out = CHUNK_SIZE;
		strm.next_out = outChunk;

		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR || ret == Z_NEED_DICT) {
			inflateEnd(&strm);
			throw std::runtime_error("inflate failed");
		}

		size_t have = CHUNK_SIZE - strm.avail_out;
		output.insert(output.end(), outChunk, outChunk + have);
	} while (ret != Z_STREAM_END);

	inflateEnd(&strm);
	return output;
}


void writeBE16(std::ostream& os, uint16_t val) {
	os.put(static_cast<char>((val >> 8) & 0xFF));
	os.put(static_cast<char>(val & 0xFF));
}

void writeIntBE(FILE* file, int value) {
	uint8_t bytes[4] = {
		(uint8_t)(value >> 24),
		(uint8_t)(value >> 16),
		(uint8_t)(value >> 8),
		(uint8_t)value
	};
	fwrite(bytes, 1, 4, file);
}

int readIntBE(FILE* file) {
	uint8_t bytes[4] = { 0, 0, 0, 0 };
	if (fread(bytes, 1, 4, file) != 4) return 0;
	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

bool w_get(const std::string& url, std::vector<unsigned char>& outData)
{
	std::string host, path;
	size_t schemePos = url.find("://");
	std::string cleanUrl = (schemePos != std::string::npos) ? url.substr(schemePos + 3) : url;

	size_t pathPos = cleanUrl.find('/');
	if (pathPos != std::string::npos) {
		host = cleanUrl.substr(0, pathPos);
		path = cleanUrl.substr(pathPos);
	}
	else {
		host = cleanUrl;
		path = "/";
	}

	return url.find("https") != std::string::npos ? https_get(host, path, outData) : http_get(host, path, outData);
}

bool http_get(const std::string& host, const std::string& path, std::vector<unsigned char>& outData)
{
	// @TODO implement with libcurl
	return false;
}

bool https_get(const std::string& host, const std::string& path, std::vector<unsigned char>& outData)
{
	// @TODO implement with libcurl
	return false;
}

int64_t getProcessMemoryUsage()
{
	try {
#ifdef _WIN32
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
			return static_cast<int64_t>(pmc.WorkingSetSize / 1024);
		else
			return -1;
#elif MC_PLATFORM_MAC
		task_basic_info info;
		mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
		if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
			return static_cast<int64_t>(info.resident_size / 1024);
		else
			return -1;
#else
		std::ifstream statm("/proc/self/statm");
		if (!statm.is_open())
			return -1;
		uint64_t size, resident, shared, text, lib, data, dt;
		statm >> size >> resident >> shared >> text >> lib >> data >> dt;
		statm.close();
		long pageSize = sysconf(_SC_PAGESIZE);
		if (pageSize == -1)
			return -1;
		return static_cast<int64_t>(resident * pageSize / 1024);
#endif
	}
	catch (const std::exception& e)
	{
		return -1;
	}
}

int64_t getAvailableSystemMemory() {
	try {
#ifdef _WIN32
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		if (GlobalMemoryStatusEx(&memInfo))
			return static_cast<int64_t>(memInfo.ullAvailPhys / 1024);
		return -1;
#elif MC_PLATFORM_MAC
		vm_size_t pageSize;
		vm_statistics64_data_t vmStats;
		mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
		host_page_size(mach_host_self(), &pageSize);
		if (host_statistics64(mach_host_self(), HOST_VM_INFO64, (host_info64_t)&vmStats, &count) == KERN_SUCCESS)
		{
			int64_t freePages = vmStats.free_count + vmStats.inactive_count;
			return static_cast<int64_t>(freePages * pageSize / 1024);
		}
		return -1;
#else
		std::ifstream meminfo("/proc/meminfo");
		std::string key;
		int64_t value;
		std::string unit;
		while (meminfo >> key >> value >> unit)
		{
			if (key == "MemAvailable:")
				return value;
		}
#endif
	}
	catch (const std::exception& e)
	{
		return -1;
	}
}
