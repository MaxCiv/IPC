#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")	// ���������� ��������� Winsock API
/*
// ������ �������, � ������� ����� ���������� ������
_TCHAR szServerIPAddr[20];	// IP ����� �������
int nServerPort;			// ����� ����� �������*/
_TCHAR szServerIPAddr[20] = _T("192.168.0.106");	// IP ����� �������
int nServerPort = 5555;			// ����� ����� �������
_TCHAR szBuffer[1024] = _T("msg");	// ����� ��� ���������

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// ������������� ����
	initlog(argv[0]);
	/*
	// ���������� IP ������ � ������ ����� ��� ����������� � �������
	_tprintf(_T("The server IP Address: "));
	wscanf_s(_T("%19s"), szServerIPAddr, _countof(szServerIPAddr));
	_tprintf(_T("The server port number: "));
	wscanf_s(_T("%i"), &nServerPort);
	_getws_s(szBuffer, 1024);*/

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

	// ���� �� ������ ������� ���������� ��������� �������� ���������
	while (true) {
		_tprintf(_T("msg: "));
		_getws_s(szBuffer, 1024);	// ��������� ������-���������

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
		
		// ������ ����� �� �������
		nLength = recv(hClientSocket, (char *)szBuffer, sizeof(szBuffer), 0);
		// ������� ������� ����������� ���������
		if (nLength > 0) {
			szBuffer[nLength] = '\0';	// ��������� ������ - ������� ����
			writelog(_T("Answer from server:  %s"), szBuffer);
			_tprintf(_T("Answer from server:  %s\n\n"), szBuffer);
		}
	}

	// ���������� ����� � �������� ��, ��� ���� ���������������� WSAStartup()
	closesocket(hClientSocket);
	WSACleanup();
	closelog();
	exit(0);
}