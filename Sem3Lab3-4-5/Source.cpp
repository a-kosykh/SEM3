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

// функция вывода прав на файл
void demo_perms(fs::perms p)
{
	std::cout << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-") // чтение файла владельцом
		<< ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-") // запись в файл владельцом
		<< ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-") // запуск файла владельцом
		<< ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-") // чтение файла группой
		<< ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-") // запись в файл группой
		<< ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-") // запуск файла группой
		<< ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-") // чтение файла другими
		<< ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-") // запись в файл другими
		<< ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-"); // запуск файла другими
}

// вывод информации о содержимом папки (путь)
void ls(std::string path) {
	std::stringstream ss;
	std::string temp;
	/*if (!(fs::is_directory(path))) {
		std::cout << path << " === is not a DIR!\n\n";
		return;
	}*/
	// цикл обходов файлов/директорий в папке
	for (auto &i : fs::directory_iterator(path)) {
		// ввод файла/директории по итерации во временную строку
		ss << i;
		temp = ss.str();

		// Удаление предыдущего пути и вывод названия файла
		temp.erase(0, path.length() + 1);
		std::cout << temp << "  --- ";

		// Вызывается функция, которая выводит права файла
		demo_perms(fs::status(i).permissions());
		std::cout << " --- ";

		// Размер файла
		if (!(fs::is_directory(i))) {
			std::cout << fs::file_size(i) << " Bytes  --- ";
		}
		else {
			// если папка
			std::cout << "DIR  --- ";
		}

		// Время создания файла или директории
		temp = path + '/' + temp;
		struct stat t_stat;
		stat(temp.c_str(), &t_stat);
		struct tm * timeinfo = localtime(&t_stat.st_ctime);
		printf("%s", asctime(timeinfo));

		// очистка строчного потока
		ss.str("");
	}
	std::cout << "\n";
}


// копирование (что, куда)
void cp(std::string what, std::string to) {
	// проверка на существование файла/папки
	if (!(fs::exists(what))) {
		std::cout << "=== " << what << " === Doesn't exist!\nExiting...\n";
		return;
	}
	if (!(fs::exists(to))) {
		std::cout << "=== " << to << " === Doesn't exist!\nExiting...\n";
		return;
	}

	std::string tempDir = what.substr(what.find_last_of("/"));
	// если существует
	if (fs::exists(to + tempDir)) {
		// папка
		if (fs::is_directory(what)) {
			// проверка на возможность перезаписи
			try {
				fs::remove_all(to + tempDir);
				fs::copy(what, to + tempDir, fs::copy_options::recursive);
				std::cout << "Copied! Replaced!\n\n";
			}
			catch (std::exception &e) {
				std::cout << e.what() << "\n\n";
			}
		}
		// файл
		else {
			// провеерка на возможность перезаписи
			try {
				fs::copy(what, to + tempDir, fs::copy_options::overwrite_existing);
				std::cout << "Copied!\n\n";
			}
			catch (std::exception &e) {
				std::cout << e.what() << "\n\n";
			}
		}
	}
	// если не существует
	else {
		fs::copy(what, to + tempDir, fs::copy_options::recursive);
		std::cout << "Copied!\n\n";
	}
}

// поиск слова (путь, слово)
void find_word(std::string path, std::string word) {
	if (!(fs::exists(path))) {
		std::cout << "=== " << path << " === doesn't exist\n";
		return;
	}

	// временные переменные
	std::string line;
	std::stringstream ss;
	std::string temp;
	bool isFound = false;

	// если указана папка, запускается рекурсивный поиск
	if (fs::is_directory(path)) {
		// цикл обходов файлов/директорий в папке
		for (auto &i : fs::directory_iterator(path)) {
			// отправление в строку название файла/папки
			ss << i;
			temp = ss.str();
			// очистка потока
			ss.str("");

			// если папка -- рекурсивный запуск функции
			if (fs::is_directory(temp)) {
				find_word(temp, word);
			}
			// если файл -- запуск чтение файла
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
	// если указан файл, запускается поиск слова по файлу
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

	// если слово не найдено -- сообщение
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