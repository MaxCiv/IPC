#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <strsafe.h>
#include "logger.h"

#define BUFSIZE 512

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// инициализация лога
	initlog(argv[0]);
	_tprintf(_T("Client is started!\n\n"));

	HANDLE hPipe = INVALID_HANDLE_VALUE; // Идентификатор канала
	LPTSTR lpszPipename = _T("\\\\.\\pipe\\$$MyPipe$$"); // Имя создаваемого канала Pipe
	_TCHAR chBuf[BUFSIZE]; // Буфер для передачи данных через канал
	DWORD readbytes, writebytes; // Число байт прочитанных и переданных

	writelog(_T("Try to use WaitNamedPipe..."));
	_tprintf(_T("Try to use WaitNamedPipe...\n"));
	// Пытаемся открыть именованный канал, если надо - ожидаем его освобождения
	while (1) 
	{
		// Создаем канал с процессом-сервером:
		hPipe = CreateFile(
			lpszPipename, // имя канала,
			GENERIC_READ // текущий клиент имеет доступ на чтение,
			| GENERIC_WRITE, // текущий клиент имеет доступ на запись,
			0, // тип доступа,
			NULL, // атрибуты защиты,
			OPEN_EXISTING, // открывается существующий файл,
			0, // атрибуты и флаги для файла,
			NULL); // доступа к файлу шаблона.

		// Продолжаем работу, если канал создать удалось 
		if (hPipe != INVALID_HANDLE_VALUE)
			break;

		// Выход, если ошибка связана не с занятым каналом. 
		double errorcode = GetLastError();
		if (errorcode != ERROR_PIPE_BUSY) {
			writelog(_T("Could not open pipe. GLE=%d"), errorcode);
			_tprintf(_T("Could not open pipe. GLE=%d\n"), errorcode);
			closelog();
			exit(1);
		}

		// Если все каналы заняты, ждём 20 секунд 
		if (!WaitNamedPipe(lpszPipename, 20000)) {
			double errorcode = GetLastError();
			writelog(_T("Could not open pipe: 20 second wait timed out, GLE = %d"), errorcode);
			_tprintf(_T("ould not open pipe: 20 second wait timed out, GLE = %d\n"), errorcode);
			closelog();
			exit(2);
		}
	}

	// Выводим сообщение о создании канала
	writelog(_T("Successfully connected!"));
	_tprintf(_T("Successfully connected!\n\nInput (exit) to exit.\n"));
	// Цикл обмена данными с серверным процессом
	while (1) 
	{
		_TCHAR chBufN[BUFSIZE]; // Буфер для передачи данных через канал

		swprintf(chBuf, BUFSIZE, L"[%5d]: ", GetCurrentProcessId());

		_tprintf(_T("%s"), chBuf);

		// Вводим текстовую строку
		_fgetts(chBufN, BUFSIZE, stdin);

		swprintf(chBuf, BUFSIZE, L"[%5d]: %s", GetCurrentProcessId(), chBufN);

		// Заносим строку в протокол
		writelog(_T("Client sended: %s"), chBuf);

		// В ответ на команду "exit" завершаем цикл обмена данными с серверным процессом
		if (!_tcsncmp(chBufN, L"exit", 4)) {
			// Передаем введенную строку серверному процессу в качестве команды
			if (!WriteFile(hPipe, chBufN, (lstrlen(chBufN) + 1) * sizeof(TCHAR), &writebytes, NULL))
			{
				double errorcode = GetLastError();
				writelog(_T("Connection refused, GLE = %d"), errorcode);
				_tprintf(_T("Connection refused, GLE = %d\n"), errorcode);
				break;
			}
			writelog(_T("Processing exit code."));
			break;
		}

		// Передаем введенную строку серверному процессу в качестве команды
		if (!WriteFile(hPipe, chBuf, (lstrlen(chBuf) + 1)*sizeof(TCHAR), &writebytes, NULL)) {
			double errorcode = GetLastError();
			writelog(_T("Connection refused, GLE = %d"), errorcode);
			_tprintf(_T("Connection refused, GLE = %d\n"), errorcode);
			break;
		}
		// Получаем эту же команду обратно от сервера
		if (ReadFile(hPipe, chBuf, BUFSIZE*sizeof(TCHAR), &readbytes, NULL)) {
			writelog(_T("Received from server: %s"), chBuf);
			_tprintf(_T("Received from server: %s\n"), chBuf);
		} 
		// Если произошла ошибка, выводим ее код и завершаем работу приложения
		else {
			double errorcode = GetLastError();
			writelog(_T("ReadFile: Error %ld"), errorcode);
			_tprintf(_T("ReadFile: Error %ld\n"), errorcode);
			_getch();
			break;
		}
	}

	// Закрываем идентификатор канала
	CloseHandle(hPipe);

	closelog();
	_tprintf(TEXT("Press ENTER to terminate connection and exit\n"));
	_getch();
	return 0;
}