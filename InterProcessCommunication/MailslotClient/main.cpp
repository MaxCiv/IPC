#include <windows.h>
#include <stdio.h>
#include "logger.h"

#define BUFSIZE 512

//LPTSTR SlotName = TEXT("\\\\IE11Win7\\mailslot\\sample_mailslot");	// доступ к удаленной машине
//LPTSTR SlotName = TEXT("\\\\*\\mailslot\\sample_mailslot");	// широковещательна€ рассылка
LPTSTR SlotName = TEXT("\\\\.\\mailslot\\sample_mailslot");		// доступ к локальной машине

BOOL WriteSlot(HANDLE hSlot, LPTSTR lpszMessage)
{
	BOOL fResult;
	DWORD cbWritten;

	writelog(_T("Text to send: %s"), lpszMessage);
	_tprintf(_T("Text to send: %s\n"), lpszMessage);

	// запись сообщени€ в почтовый слот
	fResult = WriteFile(hSlot,	// дескриптор слота
		lpszMessage,	// сообщение
		(DWORD)(lstrlen(lpszMessage) + 1) * sizeof(TCHAR),
		&cbWritten,
		(LPOVERLAPPED)NULL);

	if (!fResult) {
		double errorcode = GetLastError();
		writelog(_T("WriteFile failed, GLE=%d."), errorcode);
		_tprintf(_T("WriteFile failed, GLE=%d.\n"), errorcode);
		return FALSE;
	}
	writelog(_T("Slot written to successfully"));
	_tprintf(_T("Slot written to successfully\n\n"));

	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// инициализаци€ лога
	initlog(argv[0]);

	HANDLE hFile;
	// открытие почтового слота
	hFile = CreateFile(SlotName,	// им€ слота
		GENERIC_WRITE,	// дл€ записи
		FILE_SHARE_READ,// дл€ чтени€ файла сервером
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,	// открыть существующий слот
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		double errorcode = GetLastError();
		writelog(_T("CreateFile failed, GLE=%d."), errorcode);
		_tprintf(_T("CreateFile failed, GLE=%d."), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("Mailslot created."));
	_tprintf(_T("Mailslot created. Enter (exit) to exit.\n"));

	while (TRUE) {
		_TCHAR chBuf[BUFSIZE]; // Ѕуфер дл€ передачи данных через канал

		// ѕриглашение дл€ ввода
		_tprintf(_T("msg: "));

		// ¬водим текстовую строку
		_fgetts(chBuf, BUFSIZE, stdin);

		// ¬ ответ на команду "exit" завершаем цикл обмена данными
		if (!_tcsncmp(chBuf, L"exit", 4)) {
			writelog(_T("Processing exit code."));
			_tprintf(_T("Client exiting.\n"));
			break;
		}

		WriteSlot(hFile, chBuf);
	}

	CloseHandle(hFile);

	closelog();
	Sleep(1000);
	exit(0);
}