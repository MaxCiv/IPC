#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <conio.h>
#include "logger.h"

HANDLE hSlot;	// ���������� ��������� �����
LPTSTR SlotName = _T("\\\\.\\mailslot\\sample_mailslot");

BOOL WINAPI MakeSlot(LPTSTR lpszSlotName)
{
	// ������� �������� ����
	hSlot = CreateMailslot(lpszSlotName,	// ���
		0,								// ������������ ������ �� ���������
		MAILSLOT_WAIT_FOREVER,			// �������� ����-���� ������ (����� ���������� �����)
		(LPSECURITY_ATTRIBUTES)NULL);	// ���������� � ������������

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
	// ������ ��� ��������� ������� ������ ��������� �����
	hEvent = CreateEvent(NULL, FALSE, FALSE, _T("ExampleSlot"));
	// ���� ������� �� ��������� - �������
	if (NULL == hEvent)
		return FALSE;
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;

	// ����������� ��������� �����
	fResult = GetMailslotInfo(hSlot,	// ���������� �����
		(LPDWORD)NULL,					// ��� ������������� ������� ���������
		&cbMessage,						// ����� ������� ���������� ���������
		&cMessage,						// ����� ���������� ���������
		(LPDWORD)NULL);					// ����� ������� ��������

	// ��� ������ ��������� ���������� � ����� �������
	if (!fResult) {
		double errorcode = GetLastError();
		writelog(_T("GetMailslotInfo failed, GLE=%d."), errorcode);
		_tprintf(_T("GetMailslotInfo failed, GLE=%d."), errorcode);
		return FALSE;
	}

	// ���� ��������� ��� - �������
	if (cbMessage == MAILSLOT_NO_MESSAGE) {
		writelog(_T("Waiting for a message..."));
		_tprintf(_T("\tWaiting for a message...\r"));
		return TRUE;
	}

	cAllMessages = cMessage;

	// �������� ��� ���������
	while (cMessage != 0) {	// ���� ���������� ��������� 0 - ���������� ������
		// ������ ������ � ������� ���������
		StringCchPrintf((LPTSTR)achID,
			80,
			_T("Message #%d of %d\n"),
			cAllMessages - cMessage + 1,
			cAllMessages);

		// �������� ������ ��� ���������
		lpszBuffer = (LPTSTR)GlobalAlloc(GPTR,
			lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage);
		if (NULL == lpszBuffer)
			return FALSE;
		lpszBuffer[0] = '\0';

		// ������ ��������� �� �����
		fResult = ReadFile(hSlot,	// ���������� �����
			lpszBuffer,	// ���������
			cbMessage,	// ������ ���������
			&cbRead,
			&ov);

		if (!fResult) {
			double errorcode = GetLastError();
			writelog(_T("ReadFile failed, GLE=%d."), errorcode);
			_tprintf(_T("ReadFile failed, GLE=%d./n"), errorcode);
			GlobalFree((HGLOBAL)lpszBuffer);	// ����������� ������
			return FALSE;
		}

		// ���������� ��������� � ������ � ������� ���������
		StringCbCat(lpszBuffer,
			lstrlen((LPTSTR)achID) * sizeof(TCHAR) + cbMessage,
			(LPTSTR)achID);

		// ������� ����� ���������
		writelog(_T("Contents of the mailslot: %s"), lpszBuffer);
		_tprintf(_T("\nContents of the mailslot: %s\n"), lpszBuffer);

		GlobalFree((HGLOBAL)lpszBuffer);	// ����������� ������

		// �������� ���������� � ��������� ����� ��� ��������� �������� ������
		fResult = GetMailslotInfo(hSlot,	// ���������� �����
			(LPDWORD)NULL,					// ��� ������������� ������� ���������
			&cbMessage,						// ����� ������� ���������� ���������
			&cMessage,						// ����� ���������� ���������
			(LPDWORD)NULL);					// ����� ������� ��������

		if (!fResult) {
			double errorcode = GetLastError();
			writelog(_T("GetMailslotInfo failed, GLE=%d."), errorcode);
			_tprintf(_T("GetMailslotInfo failed, GLE=%d./n"), errorcode);
			return FALSE;
		}
	}
	// ��������� ������� ������ �����
	CloseHandle(hEvent);
	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// ������������� ����
	initlog(argv[0]);

	MakeSlot(SlotName);

	while (TRUE) {
		ReadSlot();
		Sleep(2000);
	}

	closelog();
	exit(0);
}