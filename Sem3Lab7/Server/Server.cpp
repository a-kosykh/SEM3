#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>

// Подключение библиотеки ws2_32
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 64
#define DEFAULT_PORT "27015"

int main()
{
	// инициализация сетевых настроек (для windows)
	WSADATA wsaData;
	// для обработки ошибок
	int iResult = 0;
	
	// инициализация сокетов
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	// инициализация стрктур хранения информации о сокете
	struct addrinfo *result = NULL;
	struct addrinfo hints;


	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN]; // принятый пакет
	int recvbuflen = DEFAULT_BUFLEN; // максимальный размер пакета

	// Инициализация сокета 
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed with error: " << iResult << "\n";
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // домен (семейство адресов. в данном случае интернетдомен)
	hints.ai_socktype = SOCK_STREAM; // предварительная установка соединения
	hints.ai_protocol = IPPROTO_TCP; // протокол TCP
	hints.ai_flags = AI_PASSIVE; // флаг передачи

	// подготовка параметров соединения (передача универсальному объекту 
	// iResult нуль, в случае верной работы функции)
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed with error: " << iResult << "\n";
		WSACleanup();
		return 1;
	}

	// Создание "слушащего" сокета
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Связывание "слушащего" сокета с адресом
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "bind failed with error: " << WSAGetLastError() << "\n";
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);
	// проверка прослушивания
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		std::cout << "listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	// сервер запущен
	system("cls");
	std::cout << "Server is running...\n";
	
	// ожидание сокета клиента
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		std::cout << "accept failed with error: " << WSAGetLastError() << "\n";
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// закрытие прослушиващего сокета
	closesocket(ListenSocket);

	
	// ожидание пакета от клиента
	do {
		iResult = recv(ClientSocket, recvbuf, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			std::cout << "Bytes received: " << iResult << "\n";
			std::cout << "Received: ";
			std::cout.write(recvbuf, iResult) << "\n";
			// Отправление пакета обратно клиенту
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				std::cout << "send failed with error: " << WSAGetLastError() << "\n";
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			std::cout << "Bytes sent: " << iSendResult << "\n";
		}
		else if (iResult == 0)
			std::cout << "Connection closing...\n";
		else {
			std::cout << "recv failed with error: " <<  WSAGetLastError() << "\n";
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// заверешение работы клиентского сокета
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " << WSAGetLastError() << "\n";
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	
	// закрытие сокета клиента
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}