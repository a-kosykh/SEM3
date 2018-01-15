#include <iostream>  // wcin wcout
#include <vector>	 // vector
#include <string>	 // wstring
#include <iterator>  // iterator
#include <fstream>	 // wifstream
#include <sstream>	 // wstringstream
#include <codecvt>	 // codecvt_utf8
#include <algorithm> // transform
#include <map>

#define GIANTSPACE 10000
#define LESSERSPACE 5000

using namespace std;

wstring page() { // ������ ����� � wide string
	wifstream wif("avito.html");
	wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	wstringstream wss;
	wss << wif.rdbuf();
	return wss.str();
}
wstring refer = page(); // wstring <- func

class Adv { // ����� �������
private:
	wstring*		_avPage; // ������ �� ����� �����
	// ���� ������
	wstring			_title; 
	double			_price;
	wstring			_url;
	bool			_hasPhoto;
	size_t			_pos ;
	// �������� ������ ��������� ���������� �� �����
	void _getTitle() { // ��������� ��������
		wstring title;
		size_t temp = _avPage->find(L"line", _pos);
		size_t start = _avPage->find(L"</span>", temp) + 8;
		size_t end = _avPage->find(L"<span", start + 7);
		title = _avPage->substr(start, end - start - 1);
		_title = title;

	}
	void _getURL() { // ��������� ������
		wstring url;
		size_t start = _avPage->find(L"http", _pos);
		size_t end = _avPage->find(L"\" class", start);
		url = _avPage->substr(start, end - start);
		_url = url;
	}
	void _getPrice() { // ��������� ����
		wstring spaceDelete = L" ";
		wstring price;
		size_t end = _avPage->find(L" ���.", _pos); // ����� ����
		if ((end - _pos) < GIANTSPACE) {
			size_t start = _avPage->find(L"</span>", end - 15) + 8;
			price = _avPage->substr(start, end - start);
			while (price.find(spaceDelete, 0) != price.npos) { // �������� �������� �� ������
				size_t spaceN = price.find(spaceDelete, 0);
				price.erase(spaceN, 1);
			}
			double temp = _wtof(price.c_str());
			_price = temp;
		}
		else { // ���� ���� ��� � ���� �����, �� ��� ����� ����
			_price = 0;
		}
	}
	void _getPhoto() { // ��������� ���������� � ������� ����������
		if (_avPage->find(L"����", _pos - LESSERSPACE) < _pos) {
			_hasPhoto = true;
		}
		else {
			_hasPhoto = false;
		}
	}

public:
	Adv() { // ����������� �� ���������
		_avPage = &refer;
		_pos = 0;
		_price = 0;
		_title = L"";
		_hasPhoto = false;
		_url = L"";
	}
	Adv(size_t pos) { // ����������� � ���������� �������
		_avPage = &refer;
		_pos = pos;
		_getTitle();
		_getPrice();
		_getPhoto();
		_getURL();
	}
	Adv(const Adv &obj) { // ����������� �����������
		_pos = obj._pos;
		_price = obj._price;
		_title = obj._title;
		_hasPhoto = obj._hasPhoto;
		_url = obj._url;
	}
	wstring		getTitle() const { // ��������� ��������� �������� ����������
		return _title;
	}
	size_t		getPos() const { // ��������� ��������� ������� ��������
		return _pos;
	}
	double		getPrice() const { // ��������� ��������� ���� 
		return _price;
	}
	bool		getHasPhoto() const { // �������� �� ������� ����
		return _hasPhoto;
	}
	void		print() const { // ����� ���������� �� ����������
		wcout << _title << L"\n";
		wcout << _price << L"\n";
		wcout << _url << L"\n";
		wcout << L"Has photo: " << _hasPhoto << L"\n";
		wcout << L"---------------------------------------------\n\n";
	}
	friend bool operator< (const Adv& v1, const Adv& v2);
};

class AdsVector { // ����� ���� ����������
public:
	vector<Adv> vect; // ������ ����������

	AdsVector() {
		size_t pos = 0;
		wstring titleStart = L"title item-description-title"; // �������� �����

		while (pos != refer.npos) { // ���� ��������� ������� ���������� � ������ � �������� ��������
			pos = refer.find(titleStart, pos); // ����� ��������� �����
			if (pos == refer.npos) {
				break;
			}
			Adv obj(pos); // �������� ������� ������ ���������� �� �������
			vect.push_back(obj); // ����������� ������� � ������
			pos = pos + titleStart.length() + 1;
		}
	}
	
	void	print() const { // ����� ���������� �� ����������
		for (auto i = vect.begin(); i != vect.end(); ++i) {
			i->print();
		}
	} 
	
	void	sortPriceL() { // ���������� ��������� �� ���� (Low to high)
		for (auto i = vect.begin(); i != vect.end(); i++) {
			for (auto j = vect.begin(); j < i; j++) {
				if (i->getPrice() < j->getPrice()) {
					iter_swap(i, j);
				}
			}
		}
	}
	void	sortPriceH() { // High to low
		for (auto i = vect.begin(); i != vect.end(); i++) {
			for (auto j = vect.begin(); j < i; j++) {
				if (i->getPrice() > j->getPrice()) {
					iter_swap(i, j);
				}
			}
		}
	}

	void	findAdPrice() { // ���������� � ����� ���������� � ����� ���� ��� ���� ���������
		wstring choice;
		double pr;
		
		wcout << L"Higher(H) or Lower(L) ";
		wcin >> choice;
		transform(choice.begin(), choice.end(), choice.begin(), towlower);
		wcout << L"than (enter price): ";
		wcin >> pr;
		for (auto i = vect.begin(); i != vect.end(); ++i) {
			if (choice == L"h") {
				if (i->getPrice() >= pr) {
					i->print();
				}
			}
			else {
				if (i->getPrice() <= pr) {
					i->print();
				}
			}
		}
		cin.clear();
	}
	void	findAdTitle() { // ���������� �� ������ ����������
		wstring temp;
		
		wstring title;
		wcout << L"enter title: ";
		wcin >> title;
		transform(title.begin(), title.end(), title.begin(),towlower);
		
		for (auto i = vect.begin(); i != vect.end(); ++i) {
			temp = i->getTitle();
			transform(temp.begin(), temp.end(), temp.begin(), towlower);
			if (temp.find(title, 0) != temp.npos) {
				i->print();
			}
		}
		cin.clear();
	}
	void	findAdPhoto() { // ���������� �� ������� ����
		for (auto i = vect.begin(); i != vect.end(); ++i) {
			if (i->getHasPhoto()) {
				i->print();
			}
		}
	}
	void	findIfEngLetters() { // ���������� �� ������� ����. �������� � ������ �����.
		for (auto i = vect.begin(); i != vect.end(); ++i) {
			wstring temp = i->getTitle();
			for (unsigned int j = 0; j < temp.length(); ++j) {
				if (((temp[j] > 'a') && (temp[j] < 'z')) || ((temp[j] > 'A') && (temp[j] < 'z'))) {
					i->print();
					break;
				 }
			}
		}
	}

	void	sortByPrice(unsigned int i) {
		if (i == 0)
			sort(vect.begin(), vect.end());
		else
			sort(vect.rbegin(), vect.rend());
	}
};

bool operator<(const Adv & v1, const Adv & v2)
{
	return v1.getPrice() < v2.getPrice();
}


int main() {
	unsigned int start = clock();
	wcout.imbue(locale("rus_rus.866"));
	wcin.imbue(locale("rus_rus.866"));

	AdsVector allAdverts;
	unsigned int choice = 0;


	while (choice != 9) {
		unsigned int j = 0;
		
		wcout << L"1: ����� ���� ���������\n2: ���������� �� ���� (�� �����������) -- bubble\n3: ���������� �� ���� (�� ��������) -- bubble\n4: ���������� �� ����\n5: ���������� �� ��������\n6: ���������� �� ������� ����\n7: ���������� �� ������� ���� ����������� ��������\n8: ���������� �� ���� (std::sort)\n9: �����\n";
		wcout << L"�������: ";
		unsigned int end = clock();
		unsigned int result = end - start;
		wcin >> choice;

		switch (choice) {
		case 1:
			allAdverts.print();
			break;
		case 2:
			allAdverts.sortPriceL();
			wcout << "Sorted! Enter (1) to print!\n";
			break;
		case 3:
			allAdverts.sortPriceH();
			wcout << "Sorted! Enter (1) to print!\n";
			break;
		case 4:
			allAdverts.findAdPrice();
			break;
		case 5:
			allAdverts.findAdTitle();
			break;
		case 6:
			allAdverts.findAdPhoto();
			break;
		case 7:
			allAdverts.findIfEngLetters();
			break;
		case 8:
			wcout << "0: From Low to High\n1: From High to low\n";
			wcout << "Choice: ";
			wcin >> j;
			allAdverts.sortByPrice(j);
			wcout << "Sorted! Enter (1) to print!\n";
			break;
		case 9:
			return 0;
		default:
			break;
		}
	}



	wcout << "\n";
	system("pause");
	return 0;
}

