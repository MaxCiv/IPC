#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <conio.h>
#include "logger.h"

HANDLE hSlot;	// дескриптор почтового слота
LPTSTR SlotName = _T("\\\\.\\mailslot\\sample_mailslot");

BOOL WINAPI MakeSlot(LPTSTR lpszSlotName)
{
	// Создать почтовый слот
	hSlot = CreateMailslot(lpszSlotName,	// имя
		0,								// максимальный размер не ограничен
		MAILSLOT_WAIT_FOREVER,			// интервал тайм-аута чтения (ждать бесконечно долго)
		(LPSECURITY_ATTRIBUTES)NULL);	// информация о безопасности

	if (hSlot == INVALID_HANDLE_VALUE) {
		double errorcode = GetLastError();
		writelog(_T("CreateMailslot failed, GLE=%d."), errorcode);
		_tprintf(_T("CreateMailslot failed, GLE=%d."), errorcode);
		return FALSE;
	}
	writelog(_T("Mailslot created."));
	_tprintf(_T("Mailslot created.\n"));

	return TRUE;
}

BOOL ReadSlot()
{
	DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPTSTR lpszBuffer;
	TCHAR achID[80];
	DWORD cAllMessages;
	HANDLE hEvent;
	OVERLAPPED ov;
		
	cbMessage = cMessage = cbRead = 0;
	// создаём или открываем событие чтения почтового слота
	hEvent = CreateEvent(NULL, FALSE, FALSE, _T("ExampleSlot"));
	// если событие не создалось - выходим
	if (NULL == hEvent)
		return FALSE;
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;

	// определение состояния слота
	fResult = GetMailslotInfo(hSlot,	// дескриптор слота
		(LPDWORD)NULL,					// нет максимального размера сообщения
		&cbMessage,						// адрес размера следующего сообщения
		&cMessage,						// адрес количества сообщений
		(LPDWORD)NULL);					// адрес времени ожидания

	// при ошибке получения информации о слоте выходим
	if (!fResult) {
		double errorcode = GetLastError();
		writelog(_T("GetMailslotInfo failed, GLE=%d."), errorcode);
		_tprintf(_T("GetMailslotInfo failed, GLE=%d."), errorcode);
		return FALSE;
	}

	// если сообщений нет - выходим
	if (cbMessage == MAILSLOT_NO_MESSAGE) {
		writelog(_T("Waiting for a message..."));
		_tprintf(_T("\tWaiting for a message...\r"));
		return TRUE;
	}

	cAllMessages = cMessage;

	// получить все сообщения
	while (cMessage != 0) {	// если количество сообщений 0 - прекращаем чтение
		// создаём строку с номером сообщения
		StringCchPrintf((LPTSTR)achID,
			80,
			_T("Message #%d of %d\n"),
			cAllMessages - cMessage + 1,
			cAllMessages);

		// выделяем память для сообщений
		lpszBuffer = (LPTSTR)GlobalAlloc(GPTR,
			lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage);
		if (NULL == lpszBuffer)
			return FALSE;
		lpszBuffer[0] = '\0';

		// чтение сообщений из слота
		fResult = ReadFile(hSlot,	// дескриптор слота
			lpszBuffer,	// сообщение
			cbMessage,	// размер сообщения
			&cbRead,
			&ov);

		if (!fResult) {
			double errorcode = GetLastError();
			writelog(_T("ReadFile failed, GLE=%d."), errorcode);
			_tprintf(_T("ReadFile failed, GLE=%d./n"), errorcode);
			GlobalFree((HGLOBAL)lpszBuffer);	// освобождаем память
			return FALSE;
		}

		// объединяем сообщение и строку с номером сообщения
		StringCbCat(lpszBuffer,
			lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage,
			(LPTSTR)achID);

		// выводим общее сообщение
		writelog(_T("Contents of the mailslot: %s"), lpszBuffer);
		_tprintf(_T("\nContents of the mailslot: %s\n"), lpszBuffer);

		GlobalFree((HGLOBAL)lpszBuffer);	// освобождаем память

		// получаем информацию о состоянии слота для следующей итерации чтения
		fResult = GetMailslotInfo(hSlot,	// дескриптор слота
			(LPDWORD)NULL,					// нет максимального размера сообщения
			&cbMessage,						// адрес размера следующего сообщения
			&cMessage,						// адрес количества сообщений
			(LPDWORD)NULL);					// адрес времени ожидания

		if (!fResult) {
			double errorcode = GetLastError();
			writelog(_T("GetMailslotInfo failed, GLE=%d."), errorcode);
			_tprintf(_T("GetMailslotInfo failed, GLE=%d./n"), errorcode);
			return FALSE;
		}
	}
	// закрываем событие чтения слота
	CloseHandle(hEvent);
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// инициализация лога
	initlog(argv[0]);

	MakeSlot(SlotName);

	while (TRUE) {
		ReadSlot();
		Sleep(2000);
	}

	closelog();
	exit(0);
}