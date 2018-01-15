#include <iostream>
#include <queue>
#include <string>
#include <cstdlib>
#include <numeric>
#include <sstream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

typedef boost::shared_ptr<tcp::socket> socket_ptr;
typedef boost::shared_ptr<string> string_ptr;
typedef boost::shared_ptr< queue<string_ptr> > messageQueue_ptr;

//----������������� ��������� ����������--------------
io_service service;
messageQueue_ptr messageQueue(new queue<string_ptr>);
const int inputSize = 256;
string_ptr promptCpy;
//----------------------------------------------------

//------��������� �������-----------------------------
bool isOwnMessage(string_ptr); // �������� �� ����������� �� ������� ������������ ��������� 
void displayLoop(socket_ptr); // ���� ������ ��������� �� �����
void readLoop(socket_ptr, string_ptr); // �������� ��������� �� �������
void writeLoop(socket_ptr, string_ptr); // ���� ������
string* buildPrompt(); // ���������� ��������� �� ��� ������������ � ��������� ��������� �����

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	try
	{
		// ���� ip-������ �������
		cout << "Enter server ip: ";
		string addr;
		cin >> addr;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		
		// �������� �����
		tcp::endpoint ep(ip::address::from_string(addr), 8001);
		
		// ������������� ������ ������
		boost::thread_group threads;
		
		// ������������� ������
		socket_ptr sock(new tcp::socket(service));
		
		// ��������� ����� ������������
		string_ptr prompt(buildPrompt());
		promptCpy = prompt;
		
		// ���������� � ��������
		sock->connect(ep);
		cout << "Successfully connected to chat server!" << endl;
		
		// ������ ������� (����� ��������� �� �����, �������� ������ �� �������, ����������� ��������� �� ������)
		threads.create_thread(boost::bind(displayLoop, sock));
		threads.create_thread(boost::bind(readLoop, sock, prompt));
		threads.create_thread(boost::bind(writeLoop, sock, prompt));

		threads.join_all();
	}
	// ������ �����������
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
	}

	std::system("pause");
	return 0;
}

// ������� ����������� ����� ������������ (����� ���������)
string* buildPrompt()
{
	// ������������ ���������� ��������
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	const int inputSize = 256;
	char inputBuf[inputSize] = { 0 };
	char nameBuf[inputSize] = { 0 };
	string* prompt = new string(": ");
	// ���� �����
	cout << "Your username: ";
	cin.getline(nameBuf, inputSize);
	// ���������� � ����� ��������� � ����� �������� ��������
	*prompt = (string)nameBuf + *prompt;
	boost::algorithm::to_lower(*prompt);
	return prompt;
}

void readLoop(socket_ptr sock, string_ptr prompt)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	int bytesRead = 0;
	char readBuf[1024] = { 0 };
	// ���� �������� ��������� � �������
	for (;;)
	{
		if (sock->available()) // ���� ����� ��������
		{
			// ���������� � readBud inputSize ����
			bytesRead = sock->read_some(buffer(readBuf, inputSize));
			string_ptr msg(new string(readBuf, bytesRead));
			// ����������� ��������� � �������
			messageQueue->push(msg);
		}
		// ��� ������
		boost::this_thread::sleep(boost::posix_time::millisec(1000));
	}
}

void writeLoop(socket_ptr sock, string_ptr prompt)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	char inputBuf[inputSize] = { 0 };
	string inputMsg;
	// ���� ����������� ���������
	for (;;)
	{
		// ���������� �� ����� inputSize ����
		bool invalid = false;
		std::string temp;
		std::getline(cin, temp);
		if (temp.size() > inputSize)
		{
			cout << endl << "[-]" << endl;
			temp = "exit";
			invalid = true;
		}
		std::copy(temp.begin(), temp.end(), inputBuf);
		// ���������� ������
		inputMsg = *prompt + (string)inputBuf + '\n';
		
		// ���� ��������� �� ������, inputSize ���� ������������ �� ������
		if (!inputMsg.empty())
		{
			sock->write_some(buffer(inputMsg, inputSize));
			if (invalid || temp == "exit") {
				std::exit(1);
			}
		}
		// ������� ������
		inputMsg.clear();
		memset(inputBuf, 0, inputSize);
	}
}

void displayLoop(socket_ptr sock)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	for (;;) // ���� ������ ��������� �� ������� �� �����
	{
		if (!messageQueue->empty()) // ���� ������� ��������� �� ������
		{
			if (!isOwnMessage(messageQueue->front())) // ���� ��������� �� ���
			{
				// ����� ������� ��������� ���������
				cout << "\n";
				boost::posix_time::ptime messTime = boost::posix_time::second_clock::local_time();
				stringstream ss;
				ss << messTime.date() << " " << messTime.time_of_day();
				cout << ss.str();
				ss.clear();
				// ����� ���������
				cout << "\n" + *(messageQueue->front());
			}
			// �������� ��������� �� �������
			messageQueue->pop();
		}
		// ��� ������
		boost::this_thread::sleep(boost::posix_time::millisec(500));
	}
}

bool isOwnMessage(string_ptr message) // �������� �� ����������� ���������
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	if (message->find(*promptCpy) != string::npos)
		return true;
	else
		return false;
}