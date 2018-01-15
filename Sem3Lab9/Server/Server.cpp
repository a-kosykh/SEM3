#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#define PORT 8801

typedef boost::system::error_code boostError;
typedef boost::asio::ip::tcp tcp;
namespace fs = boost::filesystem;

std::string make_string(boost::asio::streambuf& streambuf)
{
	return{ boost::asio::buffers_begin(streambuf.data()),
		boost::asio::buffers_end(streambuf.data()) };
}

int main() {
	try {
		//----Установка i/o службы и ассептора--------
		boost::asio::io_service io;
		tcp::socket sock(io);
		tcp::acceptor acc(io, tcp::endpoint(tcp::v4(), PORT));
		std::cout << "Port: " << PORT << "\n" << "Waiting for connection . . .\n";
		acc.accept(sock);
		std::cout << "Client connected!\n";
		boostError ec;
		//---------------------------
		if (ec) { 
			std::cerr << ec.message(); 
			system("pause");
			std::exit(1);
		}
		else std::cout << "Connection established\n";
		//----Ожидание пути файла от клиента---------
		boost::asio::streambuf read_buffer;
		auto bytes_transferred = boost::asio::read_until(sock, read_buffer,'\n');
		std::cout << "File path: " << make_string(read_buffer) << std::endl;
		std::string filePath = make_string(read_buffer);
		read_buffer.consume(bytes_transferred);
		filePath.pop_back();
		//----Открытие файла-------------------------
		std::ifstream ifs(filePath.c_str(), std::ios::binary);
		if (!ifs) { 
			std::cerr << "Can't read file\n"; 
			system("pause");
			std::exit(1);
		}
		//----Получение размера файла----------------
		std::uintmax_t fileSize = fs::file_size(fs::path(filePath));
		std::cout << "fileSize = " << fileSize << std::endl;
		//----Отправление клиенту названия файла и его размер
		std::size_t sentFileName = boost::asio::write(sock, boost::asio::buffer("FileName: " + fs::path(filePath).filename().string() + "\r\n"));
		std::size_t sentFileSize = boost::asio::write(sock, boost::asio::buffer("FileSize: " + boost::lexical_cast<std::string>(fileSize) + "\r\n\r\n"));
		std::uintmax_t sentFileBody = 0;
		//----Буффер файла---------------------------
		char fileBuf[100000];
		std::size_t fileBufSize = sizeof(fileBuf);
		//----Считывание файла в буффер и отправление клиенту по частям
		while (ifs) {
			ifs.read(fileBuf, fileBufSize);
			sentFileBody += boost::asio::write(sock, boost::asio::buffer(fileBuf, ifs.gcount()));
		}
		if (sentFileBody != fileSize) { 
			std::cerr << "Write failed\n"; 
			system("pause");
			std::exit(1);
		}
		std::cout << "File name = " << sentFileName << std::endl
			<< "Size = " << sentFileSize << std::endl;
		//----Успешное отправление файла
		std::cout << "File sent successfully!" << std::endl;
		ifs.close();
		sock.shutdown(tcp::socket::shutdown_both);
		sock.close();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	system("pause");
	return 0;
}