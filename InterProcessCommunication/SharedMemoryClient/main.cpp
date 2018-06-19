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
	Sleep(200);

	HANDLE hMapFile;
	LPCTSTR pBuf;
	// открываем созданный ранее сервером мьютекс
	mutex = OpenMutex( 
		MUTEX_ALL_ACCESS, // полный доступ к мьютексу
		FALSE,			  // без наследования
		TEXT("SyncMutex")); // имя объекта

	if (mutex == NULL) {
		double errorcode = GetLastError();
		writelog(_T("OpenMutex error, GLE=%d"), errorcode);
		_tprintf(_T("OpenMutex error, GLE=%d\n"), errorcode);
		closelog();
		exit(1);
	}

	writelog(_T("OpenMutex successfully opened the mutex"));
	_tprintf(_T("OpenMutex successfully opened the mutex.\n"));

	// открываем объект "проекция файла"
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS, // доступ к чтению/записи
		FALSE,		// имя не наследуется
		szName);	// имя "проецируемого" объекта

	if (hMapFile == NULL) {
		double errorcode = GetLastError();
		writelog(_T("OpenFileMapping failed, GLE=%d"), errorcode);
		_tprintf(_T("OpenFileMapping failed, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}

	// создаем представление файла в адресном пространстве процесса
	// и возвращаем указатель на представление данных файла
		pBuf = (LPTSTR)MapViewOfFile(hMapFile,	// дескриптор "проецируемого" объекта
			FILE_MAP_ALL_ACCESS, // разрешение чтения/записи
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

	// цикл чтения из памяти
	while (true) {
		writelog(_T("Wait For Mutex"));
		// ждем освобождения мьютекса
		WaitForSingleObject(mutex, INFINITE);
		writelog(_T("Get Mutex"));

		_tprintf(_T("Read message: %s\n"), pBuf); // выводим содержание памяти
		writelog(_T("Read message: %s"), pBuf);
		Sleep(300);
		ReleaseMutex(mutex);	// освобождаем мьютекс
		writelog(_T("Release Mutex"));

		// В ответ на команду "exit" завершаем цикл обмена данными
		if (!_tcsncmp(pBuf, L"exit", 4)) {
			writelog(_T("Processing exit code."));
			_tprintf(_T("Client exiting.\n"));
			break;
		}
	}
	// освобождение памяти и закрытие дескрипторов handle
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);

	closelog();
	Sleep(1000);
	exit(0);
}