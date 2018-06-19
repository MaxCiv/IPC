#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")	// ���������� ��������� Winsock API

// ������ �������, � ������� ����� ���������� �������
_TCHAR szServerIPAddr[20];	// IP ����� �������
int nServerPort;			// ����� ����� �������

// ��������� ���������� � �������, ������� ����� ������������ � ������� ����������� ������
struct CLIENT_INFO
{
	SOCKET hClientSocket;	// ���������� ����������� ������
	struct sockaddr_in clientAddr;	// ���������, ������� ����� ���������
									// ��������� ������, ������������� ����������
};

// ������� ������ �������
BOOL WINAPI ClientThread(LPVOID lpData) {
	// �������� ���������� � ������� �� ���������
	CLIENT_INFO *pClientInfo = (CLIENT_INFO *)lpData;
	_TCHAR szBuffer[1024];	// ����� ��� ���������
	int nLength;	// ���������� ��� ����� ���������� ���������� ����

	while (true) {
		// ����� ������ �� �������
		nLength = recv(pClientInfo->hClientSocket,
			(char *)szBuffer,	// ��������� �� �����, � ������� ���������� ������
			sizeof(szBuffer),	// ������ ������������ ������ � ������
			0);	// �����

		wchar_t* sin_addr = new wchar_t[20];
		size_t   convtd;
		// �������������� �����-�������� ������ � ������ ������� ��������
		mbstowcs_s(&convtd, sin_addr, 20, inet_ntoa(pClientInfo->clientAddr.sin_addr), 20);

		// ������������ ������� ����������� ���������
		if (nLength > 0) {
			szBuffer[nLength] = '\0';	// ��������� ������ - ������� ����
			writelog(_T("[%s:%d]: %s"), sin_addr, pClientInfo->clientAddr.sin_port, szBuffer);
			_tprintf(_T("[%s:%5d]: %s\n"), sin_addr, pClientInfo->clientAddr.sin_port, szBuffer);

			// ���� ������ (Q), ��������� ���������� ����� � ������� ��������� � ����� � �������
			if (wcscmp(szBuffer, _T("Q")) == 0) {
				writelog(_T("Connection [%s:%d] closed."), sin_addr, pClientInfo->clientAddr.sin_port);
				_tprintf(_T("Connection [%s:%5d] closed.\n"), sin_addr, pClientInfo->clientAddr.sin_port);
				closesocket(pClientInfo->hClientSocket);
				delete pClientInfo;
				return TRUE;
			}

			// send() ����� �� ��������� ��� ������ �� ���� ���,
			// ������� ������� ��������� ��������
			int nCntSend;
			_TCHAR *pBuffer = szBuffer;
			// ���� �� ����� �������� ��� ����� ������, ��������� ��������
			while ((nCntSend = send(pClientInfo->hClientSocket, (char *)pBuffer, nLength, 0) != nLength))
			{
				if (nCntSend == -1) {
					writelog(_T("Error sending the data to %s:%d"), sin_addr, pClientInfo->clientAddr.sin_port);
					_tprintf(_T("Error sending the data to %s:%5d\n"), sin_addr, pClientInfo->clientAddr.sin_port);
					break;
				}
				if (nCntSend == nLength)	// �� �������� - ������� �� �����
					break;
				// �������������� ����� �� ���-�� ���������� ����
				pBuffer += nCntSend;
				nLength -= nCntSend;	// �������� �� ����� ����� ���-�� ���������� ����
			}
		}
		else {
			writelog(_T("Error reading the data from %s:%d"), sin_addr, pClientInfo->clientAddr.sin_port);
			_tprintf(_T("Error reading the data from %s:%5d\n"), sin_addr, pClientInfo->clientAddr.sin_port);
		}
		delete[] sin_addr;
	}

	return TRUE;
}

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// ������������� ����
	initlog(argv[0]);

	// ���������� IP ������ � ������ ����� ��� �������
	_tprintf(_T("Enter address (127.0.0.1) for local connection.\n"));
	_tprintf(_T("The server IP address: "));
	wscanf_s(_T("%19s"), szServerIPAddr, _countof(szServerIPAddr));
	_tprintf(_T("The server port number: "));
	wscanf_s(_T("%i"), &nServerPort);

	WSADATA wsaData;	//������������ ��� ������������� ���������� �������
	// ������������� WinSock � �������� ��� �������
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		double errorcode = WSAGetLastError();
		writelog(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		_tprintf(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("Windows Socket environment ready"));

	SOCKET hServerSocket;	// ���������� ��� ������
	//�������� ������
	hServerSocket = socket(
		AF_INET,        // ��������� �������, AF_INET ��������� �� TCP/IP
		SOCK_STREAM,    // ��� ���������, SOCK_STREAM ��������� �� TCP
		0               // ��� AF_INET protocol ����� 0
	);

	// ���� ����� �� ��������, �������
	if (hServerSocket == INVALID_SOCKET) {
		writelog(_T("Unable to create Server socket"));
		_tprintf(_T("Unable to create Server socket"));
		// ������� ��, ��� ���� ���������������� WSAStartup()
		WSACleanup();
		closelog();
		exit(2);
	}
	writelog(_T("Server socket created"));

	// �������� ��������� �������
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;     // ��������� �������, ������ ���� AF_INET
	serverAddr.sin_addr.s_addr = INADDR_ANY;	// ��������� ������� ����� ������
	serverAddr.sin_port = htons(nServerPort);	// ����� �����

	// ����������� ����� � ����������� IP � ������ �����, ��� ������� �������
	if (bind(hServerSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		writelog(_T("Unable to bind to %s on port %d"), szServerIPAddr, nServerPort);
		_tprintf(_T("Unable to bind to %s on port %d"), szServerIPAddr, nServerPort);
		// ���������� ����� � �������� ��, ��� ���� ���������������� WSAStartup()
		closesocket(hServerSocket);
		WSACleanup();
		closelog();
		exit(3);
	}
	writelog(_T("Bind"));

	// ������������� �����-������ � ��������� �������������, 
	// ��� ��� �� ����� ����� ����������� �������, ��� ������� �������
	if (listen(hServerSocket, SOMAXCONN) == SOCKET_ERROR) {
		writelog(_T("Unable to put server in listen state"));
		_tprintf(_T("Unable to put server in listen state"));
		// ���������� ����� � �������� ��, ��� ���� ���������������� WSAStartup()
		closesocket(hServerSocket);
		WSACleanup();
		closelog();
		exit(4);
	}
	writelog(_T("Ready for connection"));
	_tprintf(_T("\nReady for connection\n"));
	// ������� ���������� � �������
	writelog(_T("IP: %s\tPort: %d"), szServerIPAddr, nServerPort);
	_tprintf(_T("IP: %s\tPort: %d\n"), szServerIPAddr, nServerPort);

	// ����������� ����, � ������� � ���������� ������ ������������ ����������
	while (true) {
		// ������� ���������, � ������� ����� ���������� � �������
		CLIENT_INFO *pClientInfo = new CLIENT_INFO;
		int nSize = sizeof(pClientInfo->clientAddr);	// ������ ������

		// ����� ����� accept () ���������� �������, �� ������ ���������� �����
		pClientInfo->hClientSocket = accept(hServerSocket, // ��������� �����
			(struct sockaddr *) &pClientInfo->clientAddr, // ��������� ������, ����������� 
														  // ����������� � ���������� �������
			&nSize);	// ������ ��������� ������

		// ���� ���������� ������ �������� ������, �������� ����� �������� �����
		if (pClientInfo->hClientSocket == INVALID_SOCKET) {
			writelog(_T("accept() failed"));
			_tprintf(_T("accept() failed\n"));
			continue;
		}

		HANDLE hClientThread;	// ���������� ������ �������
		DWORD dwThreadId;		// id ������

		wchar_t* sin_addr = new wchar_t[20];
		size_t   convtd;
		// �������������� �����-�������� ������ � ������ ������� ��������
		mbstowcs_s(&convtd, sin_addr, 20, inet_ntoa(pClientInfo->clientAddr.sin_addr), 20);
		writelog(_T("Client connected from %s:%d"), sin_addr, pClientInfo->clientAddr.sin_port);
		_tprintf(_T("Client connected from %s:%5d\n"), sin_addr, pClientInfo->clientAddr.sin_port);
		delete[] sin_addr;

		// ��������� ����� �������
		hClientThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)ClientThread,	// ������� ������
			(LPVOID)pClientInfo,	// �������� ��������� ���� � ������� ��� ��������
			0,
			&dwThreadId);	// id ������

		if (hClientThread == NULL) {
			writelog(_T("Unable to create client thread"));
			_tprintf(_T("Unable to create client thread\n"));
		}
		else {
			CloseHandle(hClientThread);	// ��������� ����������
		}
	}

	// ���������� ����� � �������� ��, ��� ���� ���������������� WSAStartup()
	closesocket(hServerSocket);
	WSACleanup();
	closelog();
	exit(0);
}