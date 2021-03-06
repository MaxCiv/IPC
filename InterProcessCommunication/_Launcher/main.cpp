#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "logger.h"

HANDLE hConsoleHandle;

/*enum ConsoleColor
{
        Black         = 0,
        Blue          = 1,
        Green         = 2,
        Cyan          = 3,
        Red           = 4,
        Magenta       = 5,
        Brown         = 6,
        LightGray     = 7,
        DarkGray      = 8,
        LightBlue     = 9,
        LightGreen    = 10,
        LightCyan     = 11,
        LightRed      = 12,
        LightMagenta  = 13,
        Yellow        = 14,
        White         = 15
};*/

void usage() {
	printf("Choose programm to start:\n");
	printf("1  - Anonymous Pipe Server and Client\n");
	printf("2  - Named Pipe Local Server\n");
	printf("22 - Named Pipe Local Client\n");
	printf("3  - Named Pipe Network Server\n");
	printf("33 - Named Pipe Network Client\n");
	printf("4  - Mailslot Server\n");
	printf("44 - Mailslot Client\n");
	printf("5  - Shared Memory Server\n");
	printf("55 - Shared Memory Client\n");
	printf("6  - WinSock Server\n");
	printf("66 - WinSock Client\n");
	printf("7  - Completion Port Server\n");
	printf("77 - Completion Port Clients (x5)\n");
	printf("0  - Exit\n");
}

void SetColorMode(int i) {
	switch (i)
	{
	case 0: SetConsoleTextAttribute(hConsoleHandle, (WORD)((0 << 4) | 15));	// белый
		break;
	case 1: SetConsoleTextAttribute(hConsoleHandle, (WORD)((0 << 4) | 10));	// зеленый
		break;
	case 2: SetConsoleTextAttribute(hConsoleHandle, (WORD)((0 << 4) | 12));	// красный
		break;
	case 3: SetConsoleTextAttribute(hConsoleHandle, (WORD)((0 << 4) | 8));	// темно-серый
		break;
	default: SetConsoleTextAttribute(hConsoleHandle, (WORD)((0 << 4) | 15));// белый
		break;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetColorMode(0);
	system("cls");

	// инициализация лога
	initlog(argv[0]); 
	
	usage();
	int compl_clients = 1;	// кол-во запущенных клиентов порта завершения

	int choise = 0;
	while (1) {
		SetColorMode(1);
		printf(">");
		SetColorMode(0);
		scanf("%d", &choise);
		if (choise == 1) {

			printf("Starting ");
			SetColorMode(1);
			printf("AnonymousPipeServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

			//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Anonymous Pipe Server";
			TCHAR czCommandLine[] = L"AnonymousPipeServer.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process AnonymousPipeServer.exe\n");
				writelog(_T("Error: Create process AnonymousPipeServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting AnonymousPipeServer.exe."));

		}
		else if (choise == 2) {

			printf("Starting ");
			SetColorMode(1);
			printf("NamedPipeLocalServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Named Pipe Local Server";
			TCHAR czCommandLine[] = L"NamedPipeLocalServer.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process NamedPipeLocalServer.exe\n");
				writelog(_T("Error: Create process NamedPipeLocalServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting NamedPipeLocalServer.exe."));

		}
		else if (choise == 22) {

			printf("Starting ");
			SetColorMode(1);
			printf("NamedPipeLocalClient.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Named Pipe Local Client";
			TCHAR czCommandLine[] = L"NamedPipeLocalClient.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process NamedPipeLocalClient.exe\n");
				writelog(_T("Error: Create process NamedPipeLocalClient.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting NamedPipeLocalClient.exe."));

		}
		else if (choise == 3) {

			printf("Starting ");
			SetColorMode(1);
			printf("NamedPipeNetworkServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Named Pipe Network Server";
			TCHAR czCommandLine[] = L"NamedPipeNetworkServer.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process NamedPipeNetworkServer.exe\n");
				writelog(_T("Error: Create process NamedPipeNetworkServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting NamedPipeNetworkServer.exe."));

		}
		else if (choise == 33) {

			printf("Starting ");
			SetColorMode(1);
			printf("NamedPipeNetworkClient.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Named Pipe Network Client";
			TCHAR czCommandLine[] = L"NamedPipeNetworkClient.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process NamedPipeLocalClient.exe\n");
				writelog(_T("Error: Create process NamedPipeLocalClient.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting NamedPipeLocalClient.exe."));

		}
		else if (choise == 4) {

			printf("Starting ");
			SetColorMode(1);
			printf("MailslotServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Mailslot Server";
			TCHAR czCommandLine[] = L"MailslotServer.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process MailslotServer.exe\n");
				writelog(_T("Error: Create process MailslotServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting MailslotServer.exe."));

		}
		else if (choise == 44) {

			printf("Starting ");
			SetColorMode(1);
			printf("MailslotClient.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Mailslot Client";
			TCHAR czCommandLine[] = L"MailslotClient.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process MailslotClient.exe\n");
				writelog(_T("Error: Create process MailslotClient.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting MailslotClient.exe."));

		}
		else if (choise == 5) {

			printf("Starting ");
			SetColorMode(1);
			printf("SharedMemoryServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Shared Memory Server";
			TCHAR czCommandLine[] = L"SharedMemoryServer.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process SharedMemoryServer.exe\n");
				writelog(_T("Error: Create process SharedMemoryServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting SharedMemoryServer.exe."));

		}
		else if (choise == 55) {

			printf("Starting ");
			SetColorMode(1);
			printf("SharedMemoryClient.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Shared Memory Client";
			TCHAR czCommandLine[] = L"SharedMemoryClient.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process SharedMemoryClient.exe\n");
				writelog(_T("Error: Create process SharedMemoryClient.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting SharedMemoryClient.exe."));

		}
		else if (choise == 6) {

			printf("Starting ");
			SetColorMode(1);
			printf("WinSockServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"WinSock Server";
			TCHAR czCommandLine[] = L"WinSockServer.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process WinSockServer.exe\n");
				writelog(_T("Error: Create process WinSockServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting WinSockServer.exe."));

		}
		else if (choise == 66) {

			printf("Starting ");
			SetColorMode(1);
			printf("WinSockClient.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"WinSock Client";
			TCHAR czCommandLine[] = L"WinSockClient.exe";

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process WinSockClient.exe\n");
				writelog(_T("Error: Create process WinSockClient.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting WinSockClient.exe."));

		}
		else if (choise == 7) {

			printf("Starting ");
			SetColorMode(1);
			printf("CompletionPortServer.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Completion Port Server";
			TCHAR czCommandLine[] = L"CompletionPortServer.exe";

			compl_clients = 1; // при запуске нового сервера кол-во клиентов обнуляется

			if (!CreateProcess(NULL, // имя исполняемого модуля
				czCommandLine, //командная строка
				NULL, //атрибуты безопасности процесса
				NULL, //атрибуты безопасности потока
				NULL, //флаг наследования описателя
				CREATE_NEW_CONSOLE, //флаги создания
				NULL, //новый блок окружения
				NULL, //имя текущей директории
				&si, // STARTUPINFO
				&pi)) //PROCESS_INFORMATION
			{
				printf("Error: Create process CompletionPortServer.exe\n");
				writelog(_T("Error: Create process CompletionPortServer.exe"));
				break;
			}
			printf("Successed.\tProcess ID: %d\n", pi.dwProcessId);
			writelog(_T("Starting CompletionPortServer.exe."));

		}
		else if (choise == 77) {

			printf("Starting ");
			SetColorMode(1);
			printf("CompletionPortClient.exe...\n");
			SetColorMode(0);
			//структура, которая описывает внешний вид основного окна и содержит
			// дескрипторы стандартных устройств нового процесса
			STARTUPINFO si = { sizeof(si) };
			PROCESS_INFORMATION pi;	// информация о процессе-клиенте

									//обнуляем поля STARTUPINFO и задаем нужные значения
			ZeroMemory(&si, sizeof(STARTUPINFO));
			si.cb = sizeof(STARTUPINFO);
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_NORMAL;
			si.lpTitle = L"Completion Port Client";
			TCHAR czCommandLine[] = L"CompletionPortClient.exe 1000";

			int i = compl_clients + 5;
			for (; compl_clients < i; compl_clients++)
			{
				swprintf_s(czCommandLine, _T("CompletionPortClient.exe %d"), compl_clients);

				if (!CreateProcess(NULL, // имя исполняемого модуля
					czCommandLine, //командная строка
					NULL, //атрибуты безопасности процесса
					NULL, //атрибуты безопасности потока
					NULL, //флаг наследования описателя
					CREATE_NEW_CONSOLE, //флаги создания
					NULL, //новый блок окружения
					NULL, //имя текущей директории
					&si, // STARTUPINFO
					&pi)) //PROCESS_INFORMATION
				{
					printf("Error: Create process CompletionPortClient.exe\n");
					writelog(_T("Error: Create process CompletionPortClient.exe"));
					break;
				}
				printf("Successed (");
				SetColorMode(3);
				printf("%d", compl_clients);
				SetColorMode(0);
				printf(").\tProcess ID: %d\n", pi.dwProcessId);
				writelog(_T("Starting CompletionPortClient.exe."));
			}
		}
		else if (choise == 0) {
			SetColorMode(2);
			printf("Shutting down...\n");
			SetColorMode(0);
			break;
		}
		else {
			SetColorMode(2);
			printf("Invalid choise, try again.\n");
			SetColorMode(0);
			usage();
		}

	}

	// закрываем лог
	closelog();
	Sleep(100);
	return 0;
}