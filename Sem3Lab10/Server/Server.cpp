#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/ip/tcp.hpp>

using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> socket_ptr;
typedef boost::shared_ptr<string> string_ptr;
typedef map<socket_ptr, string_ptr> clientMap;
typedef boost::shared_ptr<clientMap> clientMap_ptr;
typedef boost::shared_ptr< list<socket_ptr> > clientList_ptr;
typedef boost::shared_ptr< queue<clientMap_ptr> > messageQueue_ptr;

// ������������� ��������� ����������
io_service service;
tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 8001));
boost::mutex mtx;
clientList_ptr clientList(new list<socket_ptr>);
messageQueue_ptr messageQueue(new queue<clientMap_ptr>);

const int bufSize = 256; // ������������ ������ ��������� 
enum sleepLen // ����� ��� ��� �������
{
	sml = 100,
	lon = 200
};

//------��������� �������----------------
bool clientSentExit(string_ptr);
bool clientSentInvalid(string_ptr);
void disconnectClient(socket_ptr);
void acceptorLoop();
void requestLoop();
void responseLoop();

int main()
{
	// ������������� �������
	boost::thread_group threads;

	// ����� �������� ������������
	threads.create_thread(boost::bind(acceptorLoop));
	boost::this_thread::sleep(boost::posix_time::millisec(sleepLen::sml)); // ��� ������ �� 100��

	// ����� �������� ���������
	threads.create_thread(boost::bind(requestLoop));
	boost::this_thread::sleep(boost::posix_time::millisec(sleepLen::sml)); // ��� ������ �� 100��

	// ����� ������ ��������
	threads.create_thread(boost::bind(responseLoop));
	boost::this_thread::sleep(boost::posix_time::millisec(sleepLen::sml)); // ��� ������ �� 100��

	threads.join_all();

	puts("Press any key to continue...");
	getc(stdin);
	return EXIT_SUCCESS;
}

void acceptorLoop()
{
	cout << "Waiting for clients..." << endl;

	// ����������� ���� �������� ��������
	for (;;)
	{
		// �������� ���������� ������
		socket_ptr clientSock(new tcp::socket(service));
		// ��������
		acceptor.accept(*clientSock);

		cout << "New client joined! ";
		// ���������� ������ ��� ���������� ������� � ������
		mtx.lock();
		clientList->emplace_back(clientSock);
		mtx.unlock(); // �������������

		cout << clientList->size() << " total clients" << endl;
	}
}

// �������� ���������
void requestLoop()
{
	for (;;)
	{
		if (!clientList->empty())
		{
			mtx.lock(); // ���������� ������ �� ����� ������������ ������ �������� 
			for (auto& clientSock : *clientList)
			{
				if (clientSock->available())
				{
					char readBuf[bufSize] = { 0 };
					// ������, ������������� �� ������ ���������
					int bytesRead = clientSock->read_some(buffer(readBuf, bufSize));
					// ���������� ���������
					string_ptr msg(new string(readBuf, bytesRead));
					// ���� ��������� exit, ������ �������
					if (clientSentExit(msg) || clientSentInvalid(msg))
					{
						disconnectClient(clientSock);
						break;
					}
					// ���������� �������/��������� � �������
					clientMap_ptr cm(new clientMap);
					cm->insert(pair<socket_ptr, string_ptr>(clientSock, msg));
					messageQueue->push(cm);
					// ����� ��������� �� ������
					cout << "Mess: " << *msg << endl;
				}
			}
			mtx.unlock();
		}

		boost::this_thread::sleep(boost::posix_time::millisec(sleepLen::lon));
	}
}

bool clientSentExit(string_ptr message) // ���� �� ��������� exit
{
	if (message->find("exit") != string::npos)
		return true;
	else
		return false;
}
bool clientSentInvalid(string_ptr message) {
	if (message->find("\n") == string::npos) {
		return true;
	}
	else
		return false;
}

void disconnectClient(socket_ptr clientSock) // ���������� ����������� �������
{
	auto position = find(clientList->begin(), clientList->end(), clientSock);

	clientSock->shutdown(tcp::socket::shutdown_both);
	clientSock->close();

	clientList->erase(position);

	cout << "Client Disconnected! " << clientList->size() << " total clients" << endl;
}

void responseLoop() // ���� ����������� ��������� ��������� �������
{
	for (;;)
	{
		if (!messageQueue->empty())
		{
			// ��������� �� �������
			auto message = messageQueue->front();
			// ���������� ������ ��� ����������� ��������� ���� ��������
			mtx.lock();
			for (auto& clientSock : *clientList)
			{
				clientSock->write_some(buffer(*(message->begin()->second), bufSize));
			}
			mtx.unlock();
			// �������������-���������� � �������� ��������� �� �������
			mtx.lock();
			messageQueue->pop();
			mtx.unlock();
		}

		boost::this_thread::sleep(boost::posix_time::millisec(sleepLen::lon)); // ��� ������ �� 200��
	}
}