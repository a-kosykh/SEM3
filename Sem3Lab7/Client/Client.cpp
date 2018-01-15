#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream> 


#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int main(int argc, char **argv)
{
	// ������������� ���������� (�������, ����������, ������...)
	system("cls");
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL, *ptr = NULL, hints;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// �������� ���� ��������� � �������
	if (argc != 2) {
		std::cout << "Enter address as arg\n";
		return 1;
	}

	// ������������� ������ (������ 2.2)
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		std::cout << "WSAStartup failed with error: " << iResult;
		return 1;
	}

	// ��������� ������
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// �������� ���������� (������ ���������, ����������)
	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed with error: " << iResult << "\n";
		WSACleanup();
		return 1;
	}

	// ������� ����������� � ������ �� ���, ���� ��� �� �������� �������
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// �������� ������ ��� ����������
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error: " << WSAGetLastError() << "\n";
			WSACleanup();
			return 1;
		}

		// ���������� � �������
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Unable to connect to server!\n";
		WSACleanup();
		return 1;
	}

	// ���� ��������� ��� �����������
	std::string message;
	std::cout << "Enter message: ";
	std::getline(std::cin, message);
	if ((int)message.length() > DEFAULT_BUFLEN) {
		std::cout << "Overflow!\n";
		return 1;
	}
	// ����������� ��������� � ������� ������
	iResult = send(ConnectSocket, message.c_str(), (int)message.length(), 0);
	if (iResult == SOCKET_ERROR) {
		std::cout << "send failed with error: " << WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	std::cout << "Message: "  << message << " sent\n";

	std::cout << "Bytes Sent: " << iResult << "\n";

	// ���������� ������ ���������� �� ������� send
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error: " <<  WSAGetLastError() << "\n";
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// ������� �������� ���������� � �����
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::cout << "\nEchoed: "; 
			std::cout.write(recvbuf, iResult) << "\n";
			std::cout << "Bytes received: " << iResult << "\n";
		}
		else if (iResult == 0)
			std::cout << "Connection closed\n";
		else
			std::cout << "recv failed with error: " << WSAGetLastError() << "\n";

	} while (iResult > 0);

	// �������� ������ �����������
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}