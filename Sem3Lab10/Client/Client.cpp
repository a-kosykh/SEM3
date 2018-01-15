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

//----инициализация сервисных переменных--------------
io_service service;
messageQueue_ptr messageQueue(new queue<string_ptr>);
const int inputSize = 256;
string_ptr promptCpy;
//----------------------------------------------------

//------Прототипы функций-----------------------------
bool isOwnMessage(string_ptr); // проверка на возвращение от сервера собственного сообщения 
void displayLoop(socket_ptr); // цикл вывода сообщений на экран
void readLoop(socket_ptr, string_ptr); // ожидание сообщений от сервера
void writeLoop(socket_ptr, string_ptr); // цикл записи
string* buildPrompt(); // возвращает указатель на имя пользователя и добавляет двоеточие после

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	try
	{
		// ввод ip-адреса сервера
		cout << "Enter server ip: ";
		string addr;
		cin >> addr;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		
		// конечный адрес
		tcp::endpoint ep(ip::address::from_string(addr), 8001);
		
		// инициализация группы циклов
		boost::thread_group threads;
		
		// инициализация сокета
		socket_ptr sock(new tcp::socket(service));
		
		// получение имени пользователя
		string_ptr prompt(buildPrompt());
		promptCpy = prompt;
		
		// соединение с сервером
		sock->connect(ep);
		cout << "Successfully connected to chat server!" << endl;
		
		// запуск потоков (вывод сообщений на экран, ожидание ответа от сервера, отправление сообщений на сервер)
		threads.create_thread(boost::bind(displayLoop, sock));
		threads.create_thread(boost::bind(readLoop, sock, prompt));
		threads.create_thread(boost::bind(writeLoop, sock, prompt));

		threads.join_all();
	}
	// кэтчер иисключений
	catch (std::exception& e)
	{
		cerr << e.what() << endl;
	}

	std::system("pause");
	return 0;
}

// функция определение имени пользователя (хэдер сообщения)
string* buildPrompt()
{
	// максимальное количество символов
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	const int inputSize = 256;
	char inputBuf[inputSize] = { 0 };
	char nameBuf[inputSize] = { 0 };
	string* prompt = new string(": ");
	// ввод имени
	cout << "Your username: ";
	cin.getline(nameBuf, inputSize);
	// добавление к имени двоеточия и смена регистра символов
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
	// цикл ожидания сообщений с сервера
	for (;;)
	{
		if (sock->available()) // если сокет доступен
		{
			// считывание в readBud inputSize байт
			bytesRead = sock->read_some(buffer(readBuf, inputSize));
			string_ptr msg(new string(readBuf, bytesRead));
			// отправление сообщения в очередь
			messageQueue->push(msg);
		}
		// сон потока
		boost::this_thread::sleep(boost::posix_time::millisec(1000));
	}
}

void writeLoop(socket_ptr sock, string_ptr prompt)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	char inputBuf[inputSize] = { 0 };
	string inputMsg;
	// цикл отправления сообщения
	for (;;)
	{
		// считывание из ввода inputSize байт
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
		// добавление хэдера
		inputMsg = *prompt + (string)inputBuf + '\n';
		
		// если сообщение не пустое, inputSize байт отправляется на сервер
		if (!inputMsg.empty())
		{
			sock->write_some(buffer(inputMsg, inputSize));
			if (invalid || temp == "exit") {
				std::exit(1);
			}
		}
		// очистка буфера
		inputMsg.clear();
		memset(inputBuf, 0, inputSize);
	}
}

void displayLoop(socket_ptr sock)
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	for (;;) // цикл вывода сообщений от сервера на экран
	{
		if (!messageQueue->empty()) // если очередь сообщений не пустая
		{
			if (!isOwnMessage(messageQueue->front())) // если сообщение не своё
			{
				// вывод времени получения сообщения
				cout << "\n";
				boost::posix_time::ptime messTime = boost::posix_time::second_clock::local_time();
				stringstream ss;
				ss << messTime.date() << " " << messTime.time_of_day();
				cout << ss.str();
				ss.clear();
				// вывод сообщения
				cout << "\n" + *(messageQueue->front());
			}
			// удаление сообщения из очереди
			messageQueue->pop();
		}
		// сон потока
		boost::this_thread::sleep(boost::posix_time::millisec(500));
	}
}

bool isOwnMessage(string_ptr message) // проверка на собственное сообщение
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	if (message->find(*promptCpy) != string::npos)
		return true;
	else
		return false;
}