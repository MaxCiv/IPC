#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")	// библиотека поддержки Winsock API

// данные сервера, к которым будут обращаться клиенты
_TCHAR szServerIPAddr[20];	// IP адрес сервера
int nServerPort;			// номер порта сервера

// структура информации о клиенте, которая будет передаваться в функцию клиентского потока
struct CLIENT_INFO
{
	SOCKET hClientSocket;	// дескриптор клиентского сокета
	struct sockaddr_in clientAddr;	// структура, которая будет содержать
									// параметры сокета, инициирующего соединение
};

// функция потока клиента
BOOL WINAPI ClientThread(LPVOID lpData) {
	// получаем информацию о клиенте из параметра
	CLIENT_INFO *pClientInfo = (CLIENT_INFO *)lpData;
	_TCHAR szBuffer[1024];	// буфер для сообщений
	int nLength;	// переменная для числа фактически переданных байт

	while (true) {
		// прием данных от клиента
		nLength = recv(pClientInfo->hClientSocket,
			(char *)szBuffer,	// указатель на буфер, в который передаются данные
			sizeof(szBuffer),	// размер передаваемых данных в байтах
			0);	// флаги

		wchar_t* sin_addr = new wchar_t[20];
		size_t   convtd;
		// преобразование много-байтовой строки в строку широких символов
		mbstowcs_s(&convtd, sin_addr, 20, inet_ntoa(pClientInfo->clientAddr.sin_addr), 20);

		// обрабатываем успешно прочитанное сообщение
		if (nLength > 0) {
			szBuffer[nLength] = '\0';	// последний символ - нулевой байт
			writelog(_T("[%s:%d]: %s"), sin_addr, pClientInfo->clientAddr.sin_port, szBuffer);
			_tprintf(_T("[%s:%5d]: %s\n"), sin_addr, pClientInfo->clientAddr.sin_port, szBuffer);

			// если пришло (Q), закрываем клиентский сокет и удаляем структуру с инфой о клиенте
			if (wcscmp(szBuffer, _T("Q")) == 0) {
				writelog(_T("Connection [%s:%d] closed."), sin_addr, pClientInfo->clientAddr.sin_port);
				_tprintf(_T("Connection [%s:%5d] closed.\n"), sin_addr, pClientInfo->clientAddr.sin_port);
				closesocket(pClientInfo->hClientSocket);
				delete pClientInfo;
				return TRUE;
			}

			// send() может не отправить все данные за один раз,
			// поэтому пробуем несколько итераций
			int nCntSend;
			_TCHAR *pBuffer = szBuffer;
			// пока не будут переданы все байты буфера, выполняем передачу
			while ((nCntSend = send(pClientInfo->hClientSocket, (char *)pBuffer, nLength, 0) != nLength))
			{
				if (nCntSend == -1) {
					writelog(_T("Error sending the data to %s:%d"), sin_addr, pClientInfo->clientAddr.sin_port);
					_tprintf(_T("Error sending the data to %s:%5d\n"), sin_addr, pClientInfo->clientAddr.sin_port);
					break;
				}
				if (nCntSend == nLength)	// всё передано - выходим из цикла
					break;
				// инкрементируем буфер на кол-во переданных байт
				pBuffer += nCntSend;
				nLength -= nCntSend;	// отнимаем от общей длины кол-во переданных байт
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

	// инициализация лога
	initlog(argv[0]);

	// считывание IP адреса и номера порта для сервера
	_tprintf(_T("Enter address (127.0.0.1) for local connection.\n"));
	_tprintf(_T("The server IP address: "));
	wscanf_s(_T("%19s"), szServerIPAddr, _countof(szServerIPAddr));
	_tprintf(_T("The server port number: "));
	wscanf_s(_T("%i"), &nServerPort);

	WSADATA wsaData;	//используется для инициализации библиотеки сокетов
	// инициализация WinSock и проверка его запуска
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		double errorcode = WSAGetLastError();
		writelog(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		_tprintf(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("Windows Socket environment ready"));

	SOCKET hServerSocket;	// переменная для сокета
	//создание сокета
	hServerSocket = socket(
		AF_INET,        // семейство адресов, AF_INET указывает на TCP/IP
		SOCK_STREAM,    // тип протокола, SOCK_STREAM указывает на TCP
		0               // для AF_INET protocol равен 0
	);

	// если сокет не создался, выходим
	if (hServerSocket == INVALID_SOCKET) {
		writelog(_T("Unable to create Server socket"));
		_tprintf(_T("Unable to create Server socket"));
		// очищаем то, что было инициализировано WSAStartup()
		WSACleanup();
		closelog();
		exit(2);
	}
	writelog(_T("Server socket created"));

	// адресная структура сервера
	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;     // семейство адресов, должно быть AF_INET
	serverAddr.sin_addr.s_addr = INADDR_ANY;	// принимаем запросы любых систем
	serverAddr.sin_port = htons(nServerPort);	// номер порта

	// привязываем сокет к конкретному IP и номеру порта, при неудаче выходим
	if (bind(hServerSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		writelog(_T("Unable to bind to %s on port %d"), szServerIPAddr, nServerPort);
		_tprintf(_T("Unable to bind to %s on port %d"), szServerIPAddr, nServerPort);
		// освободить сокет и очистить то, что было инициализировано WSAStartup()
		closesocket(hServerSocket);
		WSACleanup();
		closelog();
		exit(3);
	}
	writelog(_T("Bind"));

	// устанавливаем сокет-сервер в состоянии прослушивания, 
	// так что он может ждать подключения клиента, при неудаче выходим
	if (listen(hServerSocket, SOMAXCONN) == SOCKET_ERROR) {
		writelog(_T("Unable to put server in listen state"));
		_tprintf(_T("Unable to put server in listen state"));
		// освободить сокет и очистить то, что было инициализировано WSAStartup()
		closesocket(hServerSocket);
		WSACleanup();
		closelog();
		exit(4);
	}
	writelog(_T("Ready for connection"));
	_tprintf(_T("\nReady for connection\n"));
	// вывести информацию о сервере
	writelog(_T("IP: %s\tPort: %d"), szServerIPAddr, nServerPort);
	_tprintf(_T("IP: %s\tPort: %d\n"), szServerIPAddr, nServerPort);

	// бесконечный цикл, в котором к серверному сокету подключаются клиентские
	while (true) {
		// создаем структуру, в которой будет информация о клиенте
		CLIENT_INFO *pClientInfo = new CLIENT_INFO;
		int nSize = sizeof(pClientInfo->clientAddr);	// размер адреса

		// когда вызов accept () завершится успешно, он вернет клиентский сокет
		pClientInfo->hClientSocket = accept(hServerSocket, // слушающий сокет
			(struct sockaddr *) &pClientInfo->clientAddr, // структура адреса, заполненная 
														  // информацией о клиентской системе
			&nSize);	// размер структуры адреса

		// если дескриптор сокета содержит ошибку, начинаем новую итерацию цикла
		if (pClientInfo->hClientSocket == INVALID_SOCKET) {
			writelog(_T("accept() failed"));
			_tprintf(_T("accept() failed\n"));
			continue;
		}

		HANDLE hClientThread;	// дескриптор потока клиента
		DWORD dwThreadId;		// id потока

		wchar_t* sin_addr = new wchar_t[20];
		size_t   convtd;
		// преобразование много-байтовой строки в строку широких символов
		mbstowcs_s(&convtd, sin_addr, 20, inet_ntoa(pClientInfo->clientAddr.sin_addr), 20);
		writelog(_T("Client connected from %s:%d"), sin_addr, pClientInfo->clientAddr.sin_port);
		_tprintf(_T("Client connected from %s:%5d\n"), sin_addr, pClientInfo->clientAddr.sin_port);
		delete[] sin_addr;

		// запускаем поток клиента
		hClientThread = CreateThread(NULL, 0,
			(LPTHREAD_START_ROUTINE)ClientThread,	// функция потока
			(LPVOID)pClientInfo,	// передаем структуру инфы о клиенте как параметр
			0,
			&dwThreadId);	// id потока

		if (hClientThread == NULL) {
			writelog(_T("Unable to create client thread"));
			_tprintf(_T("Unable to create client thread\n"));
		}
		else {
			CloseHandle(hClientThread);	// закрываем дескриптор
		}
	}

	// освободить сокет и очистить то, что было инициализировано WSAStartup()
	closesocket(hServerSocket);
	WSACleanup();
	closelog();
	exit(0);
}