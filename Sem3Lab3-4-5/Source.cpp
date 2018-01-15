#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
//#include <iomanip>
#include <sys/stat.h>
#include <time.h>
#include <exception>
namespace fs = std::experimental::filesystem;

// ������� ������ ���� �� ����
void demo_perms(fs::perms p)
{
	std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-") // ������ ����� ����������
		<< ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-") // ������ � ���� ����������
		<< ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-") // ������ ����� ����������
		<< ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-") // ������ ����� �������
		<< ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-") // ������ � ���� �������
		<< ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-") // ������ ����� �������
		<< ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-") // ������ ����� �������
		<< ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-") // ������ � ���� �������
		<< ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-"); // ������ ����� �������
}

// ����� ���������� � ���������� ����� (����)
void ls(std::string path) {
	std::stringstream ss;
	std::string temp;
	/*if (!(fs::is_directory(path))) {
		std::cout << path << " === is not a DIR!\n\n";
		return;
	}*/
	// ���� ������� ������/���������� � �����
	for (auto &i : fs::directory_iterator(path)) {
		// ���� �����/���������� �� �������� �� ��������� ������
		ss << i;
		temp = ss.str();

		// �������� ����������� ���� � ����� �������� �����
		temp.erase(0, path.length() + 1);
		std::cout << temp << "  --- ";

		// ���������� �������, ������� ������� ����� �����
		demo_perms(fs::status(i).permissions());
		std::cout << " --- ";

		// ������ �����
		if (!(fs::is_directory(i))) {
			std::cout << fs::file_size(i) << " Bytes  --- ";
		}
		else {
			// ���� �����
			std::cout << "DIR  --- ";
		}

		// ����� �������� ����� ��� ����������
		temp = path + '/' + temp;
		struct stat t_stat;
		stat(temp.c_str(), &t_stat);
		struct tm * timeinfo = localtime(&t_stat.st_ctime);
		printf("%s", asctime(timeinfo));

		// ������� ��������� ������
		ss.str("");
	}
	std::cout << "\n";
}


// ����������� (���, ����)
void cp(std::string what, std::string to) {
	// �������� �� ������������� �����/�����
	if (!(fs::exists(what))) {
		std::cout << "=== " << what << " === Doesn't exist!\nExiting...\n";
		return;
	}
	if (!(fs::exists(to))) {
		std::cout << "=== " << to << " === Doesn't exist!\nExiting...\n";
		return;
	}

	std::string tempDir = what.substr(what.find_last_of("/"));
	// ���� ����������
	if (fs::exists(to + tempDir)) {
		// �����
		if (fs::is_directory(what)) {
			// �������� �� ����������� ����������
			try {
				fs::remove_all(to + tempDir);
				fs::copy(what, to + tempDir, fs::copy_options::recursive);
				std::cout << "Copied! Replaced!\n\n";
			}
			catch (std::exception &e) {
				std::cout << e.what() << "\n\n";
			}
		}
		// ����
		else {
			// ��������� �� ����������� ����������
			try {
				fs::copy(what, to + tempDir, fs::copy_options::overwrite_existing);
				std::cout << "Copied!\n\n";
			}
			catch (std::exception &e) {
				std::cout << e.what() << "\n\n";
			}
		}
	}
	// ���� �� ����������
	else {
		fs::copy(what, to + tempDir, fs::copy_options::recursive);
		std::cout << "Copied!\n\n";
	}
}

// ����� ����� (����, �����)
void find_word(std::string path, std::string word) {
	if (!(fs::exists(path))) {
		std::cout << "=== " << path << " === doesn't exist\n";
		return;
	}

	// ��������� ����������
	std::string line;
	std::stringstream ss;
	std::string temp;
	bool isFound = false;

	// ���� ������� �����, ����������� ����������� �����
	if (fs::is_directory(path)) {
		// ���� ������� ������/���������� � �����
		for (auto &i : fs::directory_iterator(path)) {
			// ����������� � ������ �������� �����/�����
			ss << i;
			temp = ss.str();
			// ������� ������
			ss.str("");

			// ���� ����� -- ����������� ������ �������
			if (fs::is_directory(temp)) {
				find_word(temp, word);
			}
			// ���� ���� -- ������ ������ �����
			else {
				std::ifstream ifs;
				ifs.open(temp);

				while (std::getline(ifs, line)) {
					if (line.find(word) != std::string::npos) {
						isFound = true;
						std::cout << temp << "\n";
					}
				}
				ifs.close();
			}
		}
	}
	// ���� ������ ����, ����������� ����� ����� �� �����
	else {
		std::ifstream ifs;
		ifs.open(path);

		while (std::getline(ifs, line)) {
			if (line.find(word) != std::string::npos) {
				isFound = true;
				std::cout << path << "\n";
			}
		}
	}

	// ���� ����� �� ������� -- ���������
	if (!(isFound)) {
		std::cout << word << " is not found\n";
	}

}


int main() {
	//ls("E:/test1");
	//cp("E:/Trash/labtest/toBeCopied", "E:/Trash/labtest/test2");
	find_word("E:/", "password");

	system("pause");
	return 0;
}