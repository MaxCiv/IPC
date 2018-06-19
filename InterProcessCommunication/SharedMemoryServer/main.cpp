#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "logger.h"

#define BUF_SIZE 256
TCHAR szName[] = _T("MyFileMappingObject");
HANDLE mutex;

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// инициализация лога
	initlog(argv[0]);

	HANDLE hMapFile;
	LPCTSTR pBuf;
	// создаём мьютекс
	mutex = CreateMutex(NULL, false, TEXT("SyncMutex"));
	writelog(_T("Mutex created"));
	// создаём объект "проекция файла"
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,	// использование файла подкачки
		NULL,					// защита по умолчанию
		PAGE_READWRITE,			// доступ к чтению/записи
		0,						// макс. размер объекта
		BUF_SIZE,				// размер буфера
		szName);	// имя отраженного в памяти объекта

	// если не удалось выделить память выходим
	if (hMapFile == NULL || hMapFile == INVALID_HANDLE_VALUE) {
		double errorcode = GetLastError();
		writelog(_T("CreateFileMapping failed, GLE=%d"), errorcode);
		_tprintf(_T("CreateFileMapping failed, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("FileMappingObject created"));

	// создаем представление файла в адресном пространстве процесса
	// и возвращаем указатель на представление данных файла
	pBuf = (LPTSTR)MapViewOfFile(
		hMapFile, // дескриптор проецируемого в памяти объекта
		FILE_MAP_ALL_ACCESS, // разрешение чтения/записи(режим доступа)
		0, // старшее слово смещения файла, где начинается отображение
		0, // младшее слово смещения файла, где начинается отображение
		BUF_SIZE); // число отображаемых байтов файла
	// при неудаче выходим
	if (pBuf == NULL) {
		double errorcode = GetLastError();
		writelog(_T("MapViewOfFile failed, GLE=%d"), errorcode);
		_tprintf(_T("MapViewOfFile failed, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}

	_tprintf(_T("Server is running. Enter (exit) to exit.\n"));

	// цикл записи в память
	while (true) {
		_TCHAR chBuf[BUF_SIZE]; // Буфер для передачи данных через канал

		// Приглашение для ввода
		_tprintf(_T("msg: "));

		writelog(_T("Wait For Mutex"));
		// ждем освобождения мьютекса
		WaitForSingleObject(mutex, INFINITE);
		writelog(_T("Get Mutex"));

		// вводим текстовую строку
		_fgetts(chBuf, BUF_SIZE, stdin);

		// записываем в память сообщение
		CopyMemory((PVOID)pBuf, chBuf, sizeof(chBuf));
		_tprintf(_T("Write message: %s\n"), pBuf);
		writelog(_T("Write message: %s"), pBuf);
		ReleaseMutex(mutex);	// освобождаем мьютекс
		writelog(_T("Release Mutex"));

		// В ответ на команду "exit" завершаем цикл обмена данными
		if (!_tcsncmp(pBuf, L"exit", 4)) {
			writelog(_T("Processing exit code."));
			_tprintf(_T("Server exiting.\n"));
			break;
		}
	}
	// освобождение памяти и закрытие дескрипторов handle
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
	CloseHandle(mutex);

	closelog();
	Sleep(3000);
	exit(0);
}