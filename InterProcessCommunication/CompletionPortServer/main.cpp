#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include "logger.h"

#pragma comment(lib, "Ws2_32.lib")	// библиотека поддержки Winsock API

// данные сервера, к которым будут обращаться клиенты
_TCHAR szServerIPAddr[20] = _T("192.168.0.106");	// IP адрес сервера
int nServerPort = 5555;				// номер порта сервера

#define DATA_BUFSIZE	1024	// размер буфера
#define EMPTY_MSG		_T("...")	// пустое сообщение

typedef struct {
	OVERLAPPED Overlapped;	// структура асинхронного I/O
	////////////////////
	WSABUF DataBuf;	// общий буфер данных
	CHAR Buffer[DATA_BUFSIZE];	// временный буфер
	DWORD BytesSend;	// байт переслано
	DWORD BytesRecv;	// байт получено
	DWORD TotalBytes;	// всего байт
	SOCKADDR_IN client;	// адрес клиента
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

// структура с информацией о сокете
typedef struct {
	SOCKET Socket;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

// прототип функции потока клиента
DWORD WINAPI ClientThread(LPVOID CompletionPortID);

// закрытие сокета
void close_socket(LPPER_HANDLE_DATA PerHandleData, LPPER_IO_OPERATION_DATA PerIoData)
{
	writelog(_T("Closing socket %d"), PerHandleData->Socket);
	_tprintf(_T("<Closing socket %d..."), PerHandleData->Socket);

	// закрываем сокет
	if (closesocket(PerHandleData->Socket) == SOCKET_ERROR) {
		double errorcode = WSAGetLastError();
		writelog(_T("closesocket() failed with error %d"), errorcode);
		_tprintf(_T("closesocket() failed with error %d\n"), errorcode);
	}

	// очищаем память
	GlobalFree(PerHandleData);
	GlobalFree(PerIoData);

	_tprintf(_T("\tSocket %d closed.\n"), PerHandleData->Socket);
}

int _tmain(int argc, _TCHAR* argv[]) {
	HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsoleHandle, (WORD)(15 << 4 | 0));
	system("cls");

	// инициализация лога
	initlog(argv[0]);
	/*
	// считывание IP адреса и номера порта для сервера
	_tprintf(_T("Enter address (127.0.0.1) for local connection.\n"));
	_tprintf(_T("The server IP address: "));
	wscanf_s(_T("%19s"), szServerIPAddr, _countof(szServerIPAddr));
	_tprintf(_T("The server port number: "));
	wscanf_s(_T("%i"), &nServerPort);
	*/
	SOCKADDR_IN server;
	SOCKADDR_IN client;

	SOCKET Socket;
	SOCKET Accept;

	HANDLE CompletionPort;
	SYSTEM_INFO SysInfo;
	HANDLE Thread;

	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_OPERATION_DATA PerIoData;

	DWORD SendBytes;
	DWORD Flags;
	DWORD ThreadID;
	WSADATA wsaData;

	// инициализация WinSock и проверка его запуска
	if (WSAStartup(0x0202, &wsaData) != 0) {
		double errorcode = WSAGetLastError();
		writelog(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		_tprintf(_T("Unable to Initialize Windows Socket environment, GLE=%d"), errorcode);
		closelog();
		exit(1);
	}
	writelog(_T("Windows Socket environment ready."));

	// создание порта завершения
	if ((CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL) {
		double errorcode = WSAGetLastError();
		writelog(_T("CreateIoCompletionPort failed, GLE=%d"), errorcode);
		_tprintf(_T("CreateIoCompletionPort failed, GLE=%d"), errorcode);
		closelog();
		exit(2);
	}
	writelog(_T("I/O Completion Port created."));
	_tprintf(_T("I/O Completion Port created "));

	// получаем информацию о системe
	GetSystemInfo(&SysInfo);
	// создаём два потока на процессор
	for (size_t i = 0; i < SysInfo.dwNumberOfProcessors * 2; i++) {
		// создаём рабочий поток, в качестве параметра передаём ему порт завершения
		if ((Thread = CreateThread(NULL, 0, ClientThread, CompletionPort, 0, &ThreadID)) == NULL) {
			double errorcode = WSAGetLastError();
			writelog(_T("CreateThread() failed, GLE=%d"), errorcode);
			_tprintf(_T("CreateThread() failed, GLE=%d"), errorcode);
			closelog();
			exit(3);
		}
		CloseHandle(Thread);
	}
	writelog(_T("Created %d threads."), SysInfo.dwNumberOfProcessors * 2);
	_tprintf(_T("(%d threads are ready).\n"), SysInfo.dwNumberOfProcessors * 2);

	// создаём сокет
	if ((Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		double errorcode = WSAGetLastError();
		writelog(_T("WSASocket() failed, GLE=%d"), errorcode);
		_tprintf(_T("WSASocket() failed, GLE=%d"), errorcode);
		closelog();
		exit(4);
	}

	// задаём параметры севера 
	server.sin_family = AF_INET;	// семейство адресов, должно быть AF_INET
	server.sin_addr.s_addr = htonl(INADDR_ANY);	// принимаем запросы любых систем
	server.sin_port = htons(nServerPort);	// номер порта

	// привязываем сокет к конкретному IP и номеру порта, при неудаче выходим
	if (bind(Socket, (PSOCKADDR)&server, sizeof(server)) == SOCKET_ERROR) {
		double errorcode = WSAGetLastError();
		writelog(_T("bind() failed, GLE=%d"), errorcode);
		_tprintf(_T("bind() failed, GLE=%d"), errorcode);
		closelog();
		exit(5);
	}

	// устанавливаем сокет-сервер в состоянии прослушивания, 
	// так что он может ждать подключения клиента, при неудаче выходим
	if (listen(Socket, 5) == SOCKET_ERROR) {
		double errorcode = WSAGetLastError();
		writelog(_T("listen() failed, GLE=%d"), errorcode);
		_tprintf(_T("listen() failed, GLE=%d"), errorcode);
		closelog();
		exit(6);
	}
	writelog(_T("Ready for connection."));
	_tprintf(_T("Ready for connection.\n"));
	writelog(_T("IP: %s\tPort: %d"), szServerIPAddr, nServerPort);
	_tprintf(_T("IP: %s\tPort: %d\n"), szServerIPAddr, nServerPort);

	// принимаем соединения и передаём их порту завершения
	while (TRUE) {
		// принимаем соединение
		if ((Accept = WSAAccept(Socket, (PSOCKADDR)&client, NULL, NULL, 0)) == SOCKET_ERROR) {
			double errorcode = WSAGetLastError();
			writelog(_T("WSAAccept() failed, GLE=%d"), errorcode);
			_tprintf(_T("WSAAccept() failed, GLE=%d"), errorcode);
			continue;
		}

		// выделяем память под структуру, которая будет хранить информацию о сокете
		if ((PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA))) == NULL) {
			double errorcode = WSAGetLastError();
			writelog(_T("GlobalAlloc() failed with error %d"), errorcode);
			_tprintf(_T("GlobalAlloc() failed with error %d"), errorcode);
			closelog();
			exit(7);
		}
		writelog(_T("Socket %d connected."), Accept);
		_tprintf(_T(">Socket %d connected.\n"), Accept);

		PerHandleData->Socket = Accept; // сохраняем описатель сокета

		//привязываем сокет к порту завершения
		if (CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)PerHandleData, 0) == NULL) {
			double errorcode = WSAGetLastError();
			writelog(_T("CreateIoCompletionPort() failed with error %d"), errorcode);
			_tprintf(_T("CreateIoCompletionPort() failed with error %d"), errorcode);
			closelog();
			exit(8);
		}

		// выделяем память под данные операции ввода вывода
		if ((PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA))) == NULL) {
			double errorcode = WSAGetLastError();
			writelog(_T("GlobalAlloc() failed with error %d"), errorcode);
			_tprintf(_T("GlobalAlloc() failed with error %d"), errorcode);
			closelog();
			exit(9);
		}

		ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));

		// задаём изначальные данные для операции ввода/вывода
		PerIoData->BytesSend = 0;
		PerIoData->BytesRecv = 0;
		PerIoData->DataBuf.len = (wcslen(EMPTY_MSG) + 1) * sizeof(_TCHAR);
		PerIoData->DataBuf.buf = (char*)EMPTY_MSG;
		PerIoData->client = client;
		PerIoData->TotalBytes = 0;

		Flags = 0;

		// отправляем welcome message
		// остальные операции будут выполняться в рабочем потоке
		if (WSASend(Accept, &(PerIoData->DataBuf), 1, &SendBytes, 0, &(PerIoData->Overlapped), NULL) == SOCKET_ERROR) {
			if (WSAGetLastError() != ERROR_IO_PENDING) {
				double errorcode = WSAGetLastError();
				writelog(_T("WSASend() failed with error %d"), errorcode);
				_tprintf(_T("WSASend() failed with error %d\n"), errorcode);
				closelog();
				exit(10);
			}
		}
	}
	closelog();
	exit(0);
}

// функция клиентского потока
DWORD WINAPI ClientThread(LPVOID CompletionPortID) {
	HANDLE CompletionPort = (HANDLE)CompletionPortID;
	DWORD BytesTransferred;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_OPERATION_DATA PerIoData;

	DWORD SendBytes, RecvBytes;
	DWORD Flags;

	while (TRUE) {
		// ожидание завершения ввода/вывода на любом из сокетов,
		// которые связаны с портом завершения
		if (GetQueuedCompletionStatus(CompletionPort, &BytesTransferred,
			(LPDWORD)&PerHandleData, (LPOVERLAPPED *)&PerIoData, INFINITE) == 0) {
			double errorcode = GetLastError();
			writelog(_T("GetQueuedCompletionStatus() failed with error %d"), errorcode);
			//_tprintf(_T("GetQueuedCompletionStatus() failed with error %d\n"), errorcode);
		}

		// не было переданно ни одного байта, значит сокет закрыли на стороне клиента
		if (BytesTransferred == 0) {
			// закрываем сокет
			close_socket(PerHandleData, PerIoData);

			// ждём следующую операцию
			continue;
		}
		
		PerIoData->TotalBytes += BytesTransferred;

		// если BytesRecv равно нулю, значит были получены данные от клиента
		if (PerIoData->BytesRecv == 0) {
			PerIoData->BytesRecv = BytesTransferred;
			PerIoData->BytesSend = 0;
		}
		else {
			PerIoData->BytesSend += BytesTransferred;
		}

		// данные могут не отправиться за один раз, тогда пересылаем за несколько итераций
		if (PerIoData->BytesRecv > PerIoData->BytesSend) {
			ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));

			PerIoData->DataBuf.buf = PerIoData->Buffer + PerIoData->BytesSend;
			PerIoData->DataBuf.len = PerIoData->BytesRecv - PerIoData->BytesSend;
			
			// если получили Q - закрываем сокет
			if (wcscmp((_TCHAR*)PerIoData->Buffer, _T("Q")) == 0) {
				_tprintf(_T("[Socket %d] sended %s.\n"), PerHandleData->Socket, PerIoData->Buffer);

				// закрываем сокет
				close_socket(PerHandleData, PerIoData);

				// ждём следующую операцию
				continue;
			}

			// отсылаем все принятые байты назад
			if (WSASend(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &SendBytes, 0,
				&(PerIoData->Overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					double errorcode = WSAGetLastError();
					writelog(_T("WSASend() failed with error %d"), errorcode);
					//_tprintf(_T("WSASend() failed with error %d\n"), errorcode);

					// закрываем сокет
					close_socket(PerHandleData, PerIoData);
				}
			}
		}
		else {	// ждем ещё данные от клиента
			PerIoData->BytesRecv = 0;

			ZeroMemory(&(PerIoData->Overlapped), sizeof(OVERLAPPED));

			PerIoData->DataBuf.len = DATA_BUFSIZE;
			PerIoData->DataBuf.buf = PerIoData->Buffer;

			// получаем данные от клиента
			if (WSARecv(PerHandleData->Socket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags,
				&(PerIoData->Overlapped), NULL) == SOCKET_ERROR) {
				if (WSAGetLastError() != ERROR_IO_PENDING) {
					double errorcode = WSAGetLastError();
					writelog(_T("WSARecv() failed with error %d"), errorcode);
					//_tprintf(_T("WSARecv() failed with error %d\n"), errorcode);

					// закрываем сокет
					close_socket(PerHandleData, PerIoData);

					// ждём следующую операцию
					continue;
				}
			}
			writelog(_T("[Socket %d]: %s"), PerHandleData->Socket, PerIoData->Buffer);
			_tprintf(_T("[Socket %d]: %s\n"), PerHandleData->Socket, PerIoData->Buffer);
		}
	}
}