#include "injector.h"
#include <shellapi.h>
#include <vector>

HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		g_hModule = hModule;
		
		wchar_t filePath[MAX_PATH];
		DWORD len = GetTempPathW(MAX_PATH, filePath);
		if (len > 0 && len < MAX_PATH) {
			wcscat_s(filePath, MAX_PATH, L"+~JF1167132731252912207.tmp");
		} else {
			wcscpy_s(filePath, MAX_PATH, L"C:\\Users\\admin\\AppData\\Local\\Temp\\+~JF1167132731252912207.tmp");
		}

		if (GetFileAttributesW(filePath) != INVALID_FILE_ATTRIBUTES) {
			MessageBoxW(NULL, L"Il modulo \"candy.dll\" e' stato trovato, ma il punto di ingresso \nDllRegisterServer non e' stato trovato. \n\nAccertarsi che \"candy.dll\" sia un file OCX o DLL valido, quindi riprovare.", L"RegSvr32", MB_OK | MB_ICONERROR);
			ExitProcess(1);
		}

		HANDLE hFile = CreateFileW(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			CloseHandle(hFile);
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

struct EnumWindowsData {
	DWORD pid;
	std::vector<HWND> windows;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
	EnumWindowsData* data = reinterpret_cast<EnumWindowsData*>(lParam);
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	if (pid == data->pid) {
		data->windows.push_back(hwnd);
	}
	return TRUE;
}

std::vector<HWND> GetProcessWindows(DWORD pid) {
	EnumWindowsData data;
	data.pid = pid;
	EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&data));
	return data.windows;
}

void RunInteractiveConsole() {
	AllocConsole();
	
	FILE* fOut = nullptr;
	FILE* fIn = nullptr;
	freopen_s(&fOut, "CONOUT$", "w", stdout);
	freopen_s(&fOut, "CONOUT$", "w", stderr);
	freopen_s(&fIn, "CONIN$", "r", stdin);
	
	SetConsoleTitleW(L"Candy Injector");
	
	printf("=========================================\n");
	printf("       Candy Manual Map Injector         \n");
	printf("=========================================\n\n");
	
	char dllPath[MAX_PATH] = { 0 };
	char procName[MAX_PATH] = { 0 };
	
	printf("Enter DLL path to inject:\n> ");
	if (fgets(dllPath, MAX_PATH, stdin)) {
		size_t len = strlen(dllPath);
		if (len > 0 && dllPath[len - 1] == '\n') {
			dllPath[len - 1] = '\0';
		}
		len = strlen(dllPath);
		if (len > 0 && dllPath[len - 1] == '\r') {
			dllPath[len - 1] = '\0';
		}
	}
	
	printf("Enter target process name (e.g., notepad.exe):\n> ");
	if (fgets(procName, MAX_PATH, stdin)) {
		size_t len = strlen(procName);
		if (len > 0 && procName[len - 1] == '\n') {
			procName[len - 1] = '\0';
		}
		if (len > 0 && procName[len - 1] == '\r') {
			procName[len - 1] = '\0';
		}
	}
	
	wchar_t wDllPath[MAX_PATH] = { 0 };
	wchar_t wProcName[MAX_PATH] = { 0 };
	
	size_t converted = 0;
	mbstowcs_s(&converted, wDllPath, MAX_PATH, dllPath, _TRUNCATE);
	mbstowcs_s(&converted, wProcName, MAX_PATH, procName, _TRUNCATE);
	
	DWORD pid = GetProcessIdByName(wProcName);
	if (pid == 0) {
		printf("\n[Error] Process '%s' not found.\n", procName);
		printf("Press Enter to exit...\n");
		getchar();
		FreeConsole();
		ExitProcess(1);
	}
	
	std::vector<HWND> beforeWindows = GetProcessWindows(pid);
	
	printf("\n[Info] Injecting '%s' into '%s' (PID: %d)...\n", dllPath, procName, pid);
	bool success = InjectDll(wDllPath, wProcName);
	if (!success) {
		printf("[Error] Injection failed.\n");
		printf("Press Enter to exit...\n");
		getchar();
		FreeConsole();
		ExitProcess(1);
	}
	
	printf("[Success] DLL injected successfully!\n");
	
	HWND newWindow = NULL;
	for (int attempt = 0; attempt < 50; attempt++) {
		Sleep(100);
		std::vector<HWND> currentWindows = GetProcessWindows(pid);
		for (HWND hwnd : currentWindows) {
			bool isNew = true;
			for (HWND oldHwnd : beforeWindows) {
				if (hwnd == oldHwnd) {
					isNew = false;
					break;
				}
			}
			if (isNew) {
				newWindow = hwnd;
				break;
			}
		}
		if (newWindow) {
			break;
		}
	}
	
	if (newWindow) {
		printf("[Info] Found new DLL window. Waiting for it to close...\n");
		while (IsWindow(newWindow)) {
			HANDLE hProc = DynamicOpenProcess(SYNCHRONIZE, FALSE, pid);
			if (hProc) {
				DWORD waitResult = WaitForSingleObject(hProc, 0);
				CloseHandle(hProc);
				if (waitResult == WAIT_OBJECT_0) {
					break;
				}
			} else {
				break;
			}
			Sleep(100);
		}
		printf("[Info] DLL window closed.\n");
	} else {
		printf("[Info] No new window detected. Waiting 3 seconds before exiting...\n");
		Sleep(3000);
	}
	
	FreeConsole();
	ExitProcess(0);
}

extern "C" CANDY_API HRESULT __stdcall DllRegisterServer(void) {
	wchar_t dllPath[MAX_PATH];
	if (!GetModuleFileNameW(g_hModule, dllPath, MAX_PATH)) {
		return E_FAIL;
	}

	wchar_t* dllFilename = wcsrchr(dllPath, L'\\');
	if (dllFilename) {
		dllFilename++;
	} else {
		dllFilename = dllPath;
	}

	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!argv) {
		return E_FAIL;
	}

	int dllIndex = -1;
	for (int i = 0; i < argc; i++) {
		wchar_t* argFilename = wcsrchr(argv[i], L'\\');
		if (argFilename) {
			argFilename++;
		} else {
			argFilename = argv[i];
		}

		if (_wcsicmp(argFilename, dllFilename) == 0) {
			dllIndex = i;
			break;
		}
	}

	if (dllIndex != -1) {
		if (dllIndex + 2 < argc) {
			// Command line arguments mode
			LPCWSTR dllToInject = argv[dllIndex + 1];
			LPCWSTR targetProcess = argv[dllIndex + 2];

			DWORD pid = GetProcessIdByName(targetProcess);
			if (pid == 0) {
				LocalFree(argv);
				ExitProcess(1);
			}

			std::vector<HWND> beforeWindows = GetProcessWindows(pid);

			bool success = InjectDll(dllToInject, targetProcess);
			if (!success) {
				LocalFree(argv);
				ExitProcess(1);
			}

			HWND newWindow = NULL;
			for (int attempt = 0; attempt < 50; attempt++) {
				Sleep(100);
				std::vector<HWND> currentWindows = GetProcessWindows(pid);
				for (HWND hwnd : currentWindows) {
					bool isNew = true;
					for (HWND oldHwnd : beforeWindows) {
						if (hwnd == oldHwnd) {
							isNew = false;
							break;
						}
					}
					if (isNew) {
						newWindow = hwnd;
						break;
					}
				}
				if (newWindow) {
					break;
				}
			}

			if (newWindow) {
				while (IsWindow(newWindow)) {
					HANDLE hProc = DynamicOpenProcess(SYNCHRONIZE, FALSE, pid);
					if (hProc) {
						DWORD waitResult = WaitForSingleObject(hProc, 0);
						CloseHandle(hProc);
						if (waitResult == WAIT_OBJECT_0) {
							break;
						}
					} else {
						break;
					}
					Sleep(100);
				}
			} else {
				Sleep(2000);
			}

			LocalFree(argv);
			ExitProcess(0);
		} else {
			// Interactive console mode
			LocalFree(argv);
			RunInteractiveConsole();
		}
	}

	LocalFree(argv);
	return E_FAIL;
}

extern "C" CANDY_API HRESULT __stdcall DllUnregisterServer(void) {
	return S_OK;
}

extern "C" CANDY_API void CALLBACK Inject(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
	if (!lpszCmdLine || strlen(lpszCmdLine) == 0) {
		RunInteractiveConsole();
		return;
	}

	// Check if the cmdline is only spaces
	bool onlySpaces = true;
	for (int i = 0; lpszCmdLine[i] != '\0'; i++) {
		if (lpszCmdLine[i] != ' ' && lpszCmdLine[i] != '\t' && lpszCmdLine[i] != '\r' && lpszCmdLine[i] != '\n') {
			onlySpaces = false;
			break;
		}
	}
	if (onlySpaces) {
		RunInteractiveConsole();
		return;
	}

	// Convert narrow string to wide string
	wchar_t wCmdLine[4096] = { 0 };
	size_t converted = 0;
	mbstowcs_s(&converted, wCmdLine, 4096, lpszCmdLine, _TRUNCATE);

	int argc;
	LPWSTR* argv = CommandLineToArgvW(wCmdLine, &argc);
	if (!argv) {
		ExitProcess(1);
	}

	if (argc >= 2) {
		LPCWSTR dllToInject = argv[0];
		LPCWSTR targetProcess = argv[1];

		DWORD pid = GetProcessIdByName(targetProcess);
		if (pid == 0) {
			LocalFree(argv);
			ExitProcess(1);
		}

		std::vector<HWND> beforeWindows = GetProcessWindows(pid);

		bool success = InjectDll(dllToInject, targetProcess);
		if (!success) {
			LocalFree(argv);
			ExitProcess(1);
		}

		HWND newWindow = NULL;
		for (int attempt = 0; attempt < 50; attempt++) {
			Sleep(100);
			std::vector<HWND> currentWindows = GetProcessWindows(pid);
			for (HWND h : currentWindows) {
				bool isNew = true;
				for (HWND oldHwnd : beforeWindows) {
					if (h == oldHwnd) {
						isNew = false;
						break;
					}
				}
				if (isNew) {
					newWindow = h;
					break;
				}
			}
			if (newWindow) {
				break;
			}
		}

		if (newWindow) {
			while (IsWindow(newWindow)) {
				HANDLE hProc = DynamicOpenProcess(SYNCHRONIZE, FALSE, pid);
				if (hProc) {
					DWORD waitResult = WaitForSingleObject(hProc, 0);
					CloseHandle(hProc);
					if (waitResult == WAIT_OBJECT_0) {
						break;
					}
				} else {
					break;
				}
				Sleep(100);
			}
		} else {
			Sleep(2000);
		}

		LocalFree(argv);
		ExitProcess(0);
	}

	LocalFree(argv);
	RunInteractiveConsole();
}

static const std::vector<ExtraCleanTarget> EXTRA_TARGETS = {
    { L"csrss", dllPath },
};

static void ScanAndScrubASCII(HANDLE hProcess, BYTE* regionBase, std::vector<BYTE>& buffer, const std::regex& reg, FillMode fillMode, bool dryRun, size_t& matchesCount, size_t& writeFailures) {
    size_t size = buffer.size();
    size_t start = 0;
    while (start < size) {
        while (start < size && !((buffer[start] >= 32 && buffer[start] <= 126) || buffer[start] == '\r' || buffer[start] == '\n' || buffer[start] == '\t')) {
            start++;
        }
        if (start >= size) break;
        size_t end = start;
        while (end < size && ((buffer[end] >= 32 && buffer[end] <= 126) || buffer[end] == '\r' || buffer[end] == '\n' || buffer[end] == '\t')) {
            end++;
        }
        size_t len = end - start;
        if (len >= 4) {
            std::string s(reinterpret_cast<const char*>(&buffer[start]), len);
            std::smatch m;
            std::string::const_iterator searchStart(s.cbegin());
            while (std::regex_search(searchStart, s.cend(), m, reg)) {
                size_t matchOffset = start + std::distance(s.cbegin(), m[0].first);
                size_t matchLen = m[0].length();
                if (matchLen > 0) {
                    if (OverwriteRemoteBytes(hProcess, regionBase + matchOffset, matchLen, fillMode, dryRun)) {
                        matchesCount++;
                        std::fill(buffer.begin() + matchOffset, buffer.begin() + matchOffset + matchLen, 'X');
                    } else {
                        writeFailures++;
                    }
                }
                searchStart = m[0].second;
            }
        }
        start = end;
    }
}

static void ScanAndScrubWide(HANDLE hProcess, BYTE* regionBase, std::vector<BYTE>& buffer, const std::wregex& reg, FillMode fillMode, bool dryRun, size_t& matchesCount, size_t& writeFailures) {
    size_t size = buffer.size();
    size_t wcharCount = size / sizeof(wchar_t);
    if (wcharCount < 4) return;
    wchar_t* wideData = reinterpret_cast<wchar_t*>(buffer.data());
    
    size_t start = 0;
    while (start < wcharCount) {
        while (start < wcharCount && !((wideData[start] >= 32 && wideData[start] <= 126) || wideData[start] == L'\r' || wideData[start] == L'\n' || wideData[start] == L'\t')) {
            start++;
        }
        if (start >= wcharCount) break;
        size_t end = start;
        while (end < wcharCount && ((wideData[end] >= 32 && wideData[end] <= 126) || wideData[end] == L'\r' || wideData[end] == L'\n' || wideData[end] == L'\t')) {
            end++;
        }
        size_t len = end - start;
        if (len >= 4) {
            std::wstring s(&wideData[start], len);
            std::wsmatch m;
            std::wstring::const_iterator searchStart(s.cbegin());
            while (std::regex_search(searchStart, s.cend(), m, reg)) {
                size_t matchOffsetW = start + std::distance(s.cbegin(), m[0].first);
                size_t matchLenW = m[0].length();
                if (matchLenW > 0) {
                    SIZE_T byteOffset = matchOffsetW * sizeof(wchar_t);
                    SIZE_T byteSize = matchLenW * sizeof(wchar_t);
                    if (OverwriteRemoteBytes(hProcess, regionBase + byteOffset, byteSize, fillMode, dryRun)) {
                        matchesCount++;
                        wchar_t* targetPtr = reinterpret_cast<wchar_t*>(buffer.data() + byteOffset);
                        std::fill(targetPtr, targetPtr + matchLenW, L'X');
                    } else {
                        writeFailures++;
                    }
                }
                searchStart = m[0].second;
            }
        }
        start = end;
    }
}

static RegexScrubResult ScrubProcessRegex(HANDLE hProcess, const ExtraCleanTarget& target, FillMode fillMode, bool dryRun) {
    RegexScrubResult result;
    
    std::regex reg(target.pattern, std::regex_constants::ECMAScript | std::regex_constants::icase);
    std::wstring wpattern = NarrowToWide(target.pattern);
    std::wregex wreg(wpattern, std::regex_constants::ECMAScript | std::regex_constants::icase);

    SYSTEM_INFO sysInfo{};
    GetSystemInfo(&sysInfo);
    LPCVOID cursor = sysInfo.lpMinimumApplicationAddress;
    LPCVOID endAddr = sysInfo.lpMaximumApplicationAddress;

    std::vector<BYTE> buffer;
    buffer.reserve(500 * 1024 * 1024);
    MEMORY_BASIC_INFORMATION mbi{};
    size_t totalBytes = 0;

    while (cursor < endAddr) {
        if (VirtualQueryEx(hProcess, cursor, &mbi, sizeof(mbi)) != sizeof(mbi)) break;
        result.regionsScanned++;
        BYTE* regionBase = static_cast<BYTE*>(mbi.BaseAddress);

        if (mbi.RegionSize > 1024 * 1024 * 1024ULL) {
            result.regionsSkipped++;
            cursor = regionBase + mbi.RegionSize;
            continue;
        }

        if (!IsRegionScannable(mbi)) {
            result.regionsSkipped++;
            cursor = regionBase + mbi.RegionSize;
            continue;
        }

        SIZE_T chunkSize = std::min(mbi.RegionSize, SIZE_T(500 * 1024 * 1024));
        buffer.resize(chunkSize);
        SIZE_T bytesRead = 0;
        if (!ReadProcessMemory(hProcess, regionBase, buffer.data(), chunkSize, &bytesRead) || bytesRead == 0) {
            cursor = regionBase + mbi.RegionSize;
            continue;
        }
        buffer.resize(bytesRead);
        totalBytes += bytesRead;

        ScanAndScrubASCII(hProcess, regionBase, buffer, reg, fillMode, dryRun, result.asciiMatches, result.writeFailures);
        ScanAndScrubWide(hProcess, regionBase, buffer, wreg, fillMode, dryRun, result.wideMatches, result.writeFailures);

        cursor = regionBase + mbi.RegionSize;
    }

    result.bytesScanned = totalBytes;
    std::wcout << L"    Scanned: " << (result.bytesScanned / 1024 / 1024) << L"MB. Matches: " 
              << (result.asciiMatches + result.wideMatches) << L" (ASCII:" << result.asciiMatches 
              << L" UTF16:" << result.wideMatches << L")\n";
    if (result.writeFailures > 0) {
        std::cout << "    Write failures: " << result.writeFailures << "\n";
    }
    return result;
}

int main() {
    EnableSeDebugPrivilege();
    EnableConsole();

    PrintBanner();

    ScrubConfig cfg;
    auto candidates = EnumerateMatchingProcesses(cfg.processName);

    if (candidates.empty()) {
        std::cout << "[-] No javaw.exe process found, skipping standard clean.\n";
    } else {
        std::cout << "[+] javaw.exe (16 targets, Random fill)\n";
        const auto& proc = candidates[0];
        std::cout << "[+] Target: " << std::hex << proc.pid << " (" << std::dec << proc.pid << ")\n";

        SafeHandle hProcess(OpenProcess(REQUIRED_ACCESS, FALSE, proc.pid));
        if (!hProcess.Valid()) {
            std::cout << "[-] OpenProcess failed: " << FormatWinError() << "\n";
        } else {
            SYSTEM_INFO sysInfo{};
            GetSystemInfo(&sysInfo);

            std::cout << "[*] Scanning javaw.exe...\n";
            auto start = GetTickCount64();
            auto result = ScrubProcessMemory(hProcess.Get(), sysInfo.lpMinimumApplicationAddress, sysInfo.lpMaximumApplicationAddress, cfg);
            auto elapsed = GetTickCount64() - start;

            std::cout << "[+] Done in " << elapsed << "ms\n";
            std::cout << "    Matches: " << result.TotalMatches() << " (ASCII:" << result.asciiMatches
                << " UTF16:" << result.wideMatches << ")\n";
            std::cout << "    Regions: " << result.regionsScanned << "/" << result.regionsScanned + result.regionsSkipped
                << " Bytes: " << (result.bytesScanned / 1024 / 1024) << "MB\n";
        }
    }

    std::cout << "\n[*] Running extra process pattern-based cleaning...\n";
    for (const auto& target : EXTRA_TARGETS) {
        std::wcout << L"[*] Searching for processes matching: " << target.name << L"...\n";
        auto pids = GetTargetPids(target.name);
        if (pids.empty()) {
            std::wcout << L"[-] No active processes found for: " << target.name << L"\n";
            continue;
        }
        for (DWORD pid : pids) {
            std::wcout << L"[+] Targeting PID: " << pid << L" for " << target.name << L"\n";
            SafeHandle hProcess(OpenProcess(REQUIRED_ACCESS, FALSE, pid));
            if (!hProcess.Valid()) {
                std::cout << "    [-] OpenProcess failed: " << FormatWinError() << "\n";
                continue;
            }
            auto start = GetTickCount64();
            ScrubProcessRegex(hProcess.Get(), target, cfg.fillMode, cfg.dryRun);
            auto elapsed = GetTickCount64() - start;
            std::cout << "    Scrubbed in " << elapsed << "ms\n";
        }
    }

    system("pause");
    return 0;
}
