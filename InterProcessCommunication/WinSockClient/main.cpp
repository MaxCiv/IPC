#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")	// библиотека поддержки Winsock API
/*
// данные сервера, к которым будет обращаться клиент
_TCHAR szServerIPAddr[20];	// IP адрес сервера
int nServerPort;			// номер порта сервера*/
_TCHAR szServerIPAddr[20] = _T("192.168.0.106");	// IP адрес сервера
int nServerPort = 5555;			// номер порта сервера
_TCHAR szBuffer[1024] = _T("msg");	// буфер для сообщений

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)((15 << 4) | 0));
	system("cls");

	// инициализация лога
	initlog(argv[0]);
	/*
	// считывание IP адреса и номера порта для подключения к серверу
	_tprintf(_T("The server IP Address: "));
	wscanf_s(_T("%19s"), szServerIPAddr, _countof(szServerIPAddr));
	_tprintf(_T("The server port number: "));
	wscanf_s(_T("%i"), &nServerPort);
	_getws_s(szBuffer, 1024);*/

	WSADATA wsaData;	//используется для инициализации библиотеки сокетов
	//Инициализация WinSock и проверка его запуска
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		double errorcode = WSAGetLastError();
		writelog(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		_tprintf(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("Windows Socket environment ready"));

	SOCKET hClientSocket;	// дескриптор клиентского сокета
	//создание сокета
	hClientSocket = socket(
		AF_INET,        // семейство адресов, AF_INET указывает на TCP/IP
		SOCK_STREAM,    // тип протокола, SOCK_STREAM указывает на TCP
		0);             // для AF_INET protocol равен 0

	// если сокет не создался, выходим
	if (hClientSocket == INVALID_SOCKET) {
		writelog(_T("Unable to create Server socket"));
		_tprintf(_T("Unable to create Server socket\n"));
		// очищаем то, что было инициализировано WSAStartup()
		WSACleanup();
		closelog();
		exit(2);
	}
	writelog(_T("Client socket created"));

	// адресная структура сервера
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;     // семейство адресов, должно быть AF_INET
	size_t   convtd;
	char *pMBBuffer = new char[20];
	// преобразование строки с широкими символами в строку с многобайтовыми символами
	wcstombs_s(&convtd, pMBBuffer, 20, szServerIPAddr, 20);
	serverAddr.sin_addr.s_addr = inet_addr(pMBBuffer);	// IP адрес - заданный в начале
	delete[] pMBBuffer;
	serverAddr.sin_port = htons(nServerPort);	// номер порта

	// подключение к серверу, в случае неудачи выходим
	if (connect(hClientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
		writelog(_T("Unable to connect to %s on port %d"), szServerIPAddr, nServerPort);
		_tprintf(_T("Unable to connect to %s on port %d"), szServerIPAddr, nServerPort);
		// освободить сокет и очистить то, что было инициализировано WSAStartup()
		closesocket(hClientSocket);
		WSACleanup();
		closelog();
		exit(3);
	}
	writelog(_T("Connect"));

	_tprintf(_T("Send (Q) to exit.\n"));

	// пока не пришла команда завершения выполняем передачу сообщений
	while (true) {
		_tprintf(_T("msg: "));
		_getws_s(szBuffer, 1024);	// считываем строку-сообщение

		int nLength = (wcslen(szBuffer) + 1) * sizeof(_TCHAR);	// длина всего сообщения

		// send() может не отправить все данные за один раз,
		// поэтому пробуем несколько итераций
		int nCntSend;
		_TCHAR *pBuffer = szBuffer;
		// пока не будут переданы все байты буфера, выполняем передачу
		while ((nCntSend = send(hClientSocket, (char *)pBuffer, nLength, 0)) != nLength) 
		{
			if (nCntSend == -1) {
				writelog(_T("Error sending the data to server"));
				_tprintf(_T("Error sending the data to server\n"));
				break;
			}
			if (nCntSend == nLength)	// всё передано - выходим из цикла
				break;
			// инкрементируем буфер на кол-во переданных байт
			pBuffer += nCntSend;
			nLength -= nCntSend;	// отнимаем от общей длины кол-во переданных байт
		}

		// если пришла команда завершения прекращаем передачу сообщений
		if (wcscmp(szBuffer, _T("Q")) == 0) {
			break;
		}
		
		// читаем ответ от сервера
		nLength = recv(hClientSocket, (char *)szBuffer, sizeof(szBuffer), 0);
		// выводим успешно прочитанное сообщение
		if (nLength > 0) {
			szBuffer[nLength] = '\0';	// последний символ - нулевой байт
			writelog(_T("Answer from server:  %s"), szBuffer);
			_tprintf(_T("Answer from server:  %s\n\n"), szBuffer);
		}
	}

	// освободить сокет и очистить то, что было инициализировано WSAStartup()
	closesocket(hClientSocket);
	WSACleanup();
	closelog();
	exit(0);
}