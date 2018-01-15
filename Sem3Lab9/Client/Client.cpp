#include <iostream>
#include <string>
#include <fstream>
#include <numeric>
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <windows.h>

#define DEFAULT_ADDRESS "192.168.0.2"
#define DEFAULT_PORT 8801

typedef boost::asio::ip::tcp tcp;
typedef boost::system::error_code boostError;

boost::smatch res;
boost::regex regFileName("FileName: *(.+?)\r\n");
boost::regex regFileSize("FileSize: *([0-9]+?)\r\n");

int main() {
	std::cout << "Client started...\n";
	try {
		//----Установка i/o службы и указание адреса сервера
		boost::asio::io_service io;
		tcp::socket sock(io);
		boostError ec;
		
		std::string addr;
		std::cout << "enter ip-address: ";
		std::cin >> addr;
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		tcp::endpoint endPoint(boost::asio::ip::address::from_string(addr), DEFAULT_PORT);
		std::cout << "Connecting to " << addr << ":" << DEFAULT_PORT << "\n";
		sock.connect(endPoint, ec);
		if (ec) { 
			std::cerr << ec.message(); 
			system("pause"); 
			std::exit(1); 
		}
		else std::cout << "Connection established\n";
		//----Ввод пути файла на сервере-----------------------
		std::cout << "Input file path\n";
		std::string filePath;
		std::getline(std::cin, filePath);
		//----Отправление пути на сервер-----------------------
		auto bytes = boost::asio::write(sock, boost::asio::buffer(filePath + '\n'));
		std::cout << "Bytes transferred = " << bytes << "\n";
		//----Ожидание размера и названия файла от сервера
		boost::asio::streambuf sBuf;
		std::istream istr(&sBuf);
		std::size_t l = boost::asio::read_until(sock, sBuf, boost::regex("(?:\r\n){2,}"), ec);
		if (ec) { 
			std::cerr << "read_until failed\nl = " << 
				l << "\n"; 
			std::cin.get(); 
			return __LINE__; 
		}
		//----Сохранение данных в хэдер и проверка на валидность
		std::string headers, tmp;
		while (std::getline(istr, tmp) && tmp != "\r") {
			headers += (tmp + '\n');
		}
		std::cout << "Headers:\n" << headers << std::endl;
		if (!boost::regex_search(headers, res, regFileSize)) {
			std::cerr << "regFileSize not found\n";
			std::cin.get();
			return __LINE__;
		}
		std::size_t fileSize = boost::lexical_cast<std::size_t>(res[1]);
		std::cout << "fileSize = " << fileSize << std::endl;
		if (!boost::regex_search(headers, res, regFileName)) {
			std::cerr << "regFileName not found\n";
			std::cin.get();
			return __LINE__;
		}
		std::string fileName = res[1];
		std::cout << "fileName = " << fileName << std::endl;
		//----Указание пути сохранения файла--------------
		std::string destPath = "E:\\";
		std::cout << "Destanation path: --" << destPath << "\n";
		std::ofstream ofs((destPath + fileName).c_str(), std::ios::binary);
		if (!ofs) { 
			std::cerr << "Can't create file\n"; 
			std::cin.get(); 
			return __LINE__; 
		}
		//----Получение файла. Цикл записи в буфер частями до 
		//----тех пор, пока обновляющийся размер не совпадет с раннее полученным
		std::size_t len = 0;
		if (sBuf.size()) {
			len += sBuf.size();
			ofs << &sBuf;
		}
		while (len != fileSize) {
			len += boost::asio::read(sock, sBuf, boost::asio::transfer_at_least(1));
			ofs << &sBuf;
		}
		//----Завершение работы программы--------------------
		std::cout << "OK" << std::endl;
		ofs.close();
		istr.rdbuf(NULL);
		sock.shutdown(tcp::socket::shutdown_both);
		sock.close();
	}
	catch (std::exception& ex) {
		std::cerr << "Exception: " << ex.what() << std::endl;
	}
	system("pause");
	return 0;
}