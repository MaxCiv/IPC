#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")	// ���������� ��������� Winsock API

// ������ �������, � ������� ����� ���������� ������
_TCHAR szServerIPAddr[20] = _T("192.168.0.106");	// IP ����� �������
int nServerPort = 5555;			// ����� ����� �������
_TCHAR szBuffer[1024] = _T("msg");	// ����� ��� ���������

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// ������������� ����
	initlog(argv[0]);

	WSADATA wsaData;	//������������ ��� ������������� ���������� �������
	//������������� WinSock � �������� ��� �������
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		double errorcode = WSAGetLastError();
		writelog(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		_tprintf(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("Windows Socket environment ready"));

	SOCKET hClientSocket;	// ���������� ����������� ������
	//�������� ������
	hClientSocket = socket(
		AF_INET,        // ��������� �������, AF_INET ��������� �� TCP/IP
		SOCK_STREAM,    // ��� ���������, SOCK_STREAM ��������� �� TCP
		0);             // ��� AF_INET protocol ����� 0

	// ���� ����� �� ��������, �������
	if (hClientSocket == INVALID_SOCKET) {
		writelog(_T("Unable to create Server socket"));
		_tprintf(_T("Unable to create Server socket\n"));
		// ������� ��, ��� ���� ���������������� WSAStartup()
		WSACleanup();
		closelog();
		exit(2);
	}
	writelog(_T("Client socket created"));

	// �������� ��������� �������
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;     // ��������� �������, ������ ���� AF_INET
	size_t   convtd;
	char *pMBBuffer = new char[20];
	// �������������� ������ � �������� ��������� � ������ � �������������� ���������
	wcstombs_s(&convtd, pMBBuffer, 20, szServerIPAddr, 20);
	serverAddr.sin_addr.s_addr = inet_addr(pMBBuffer);	// IP ����� - �������� � ������
	delete[] pMBBuffer;
	serverAddr.sin_port = htons(nServerPort);	// ����� �����

	// ����������� � �������, � ������ ������� �������
	if (connect(hClientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
		writelog(_T("Unable to connect to %s on port %d"), szServerIPAddr, nServerPort);
		_tprintf(_T("Unable to connect to %s on port %d"), szServerIPAddr, nServerPort);
		// ���������� ����� � �������� ��, ��� ���� ���������������� WSAStartup()
		closesocket(hClientSocket);
		WSACleanup();
		closelog();
		exit(3);
	}
	writelog(_T("Connect"));

	_tprintf(_T("Send (Q) to exit.\n"));

	const auto client_id = argv[1];
	swprintf_s(szBuffer, _T("Client #%s"), client_id);

	int i = 0;

	// ���� �� ������ ������� ���������� ��������� �������� ���������
	while (true) {
		struct tm newtime;
		__time64_t long_time;
		// �������� ����� ��� 64-bit integer
		_time64(&long_time);
		// �������������� � ��������� ���
		_localtime64_s(&newtime, &long_time);
		_tprintf(_T("[%02d:%02d:%02d]: %s\n"), newtime.tm_hour,
			newtime.tm_min, newtime.tm_sec, szBuffer);

		int nLength = (wcslen(szBuffer) + 1) * sizeof(_TCHAR);	// ����� ����� ���������

		// send() ����� �� ��������� ��� ������ �� ���� ���,
		// ������� ������� ��������� ��������
		int nCntSend;
		_TCHAR *pBuffer = szBuffer;
		// ���� �� ����� �������� ��� ����� ������, ��������� ��������
		while ((nCntSend = send(hClientSocket, (char *)pBuffer, nLength, 0)) != nLength) 
		{
			if (nCntSend == -1) {
				writelog(_T("Error sending the data to server"));
				_tprintf(_T("Error sending the data to server\n"));
				break;
			}
			if (nCntSend == nLength)	// �� �������� - ������� �� �����
				break;
			// �������������� ����� �� ���-�� ���������� ����
			pBuffer += nCntSend;
			nLength -= nCntSend;	// �������� �� ����� ����� ���-�� ���������� ����
		}

		// ���� ������ ������� ���������� ���������� �������� ���������
		if (wcscmp(szBuffer, _T("Q")) == 0) {
			break;
		}

		if (i++ > 250)
			break;
		Sleep(20);
	}

	// ���������� ����� � �������� ��, ��� ���� ���������������� WSAStartup()
	closesocket(hClientSocket);
	WSACleanup();
	closelog();
	exit(0);
}