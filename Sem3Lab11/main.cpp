#include <iostream>
#include <fstream>
#include <list>
#include <thread>
#include "solver.h"

#define MAX_THREADS 4
#define MAX_PACKET_SIZE 4

using namespace std;

template<typename R>
bool is_ready(std::future<R> const& f)
{
	return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

struct Problems {
	std::pair<int, int> probPair;
	Problems() {
		probPair.first = 0;
		probPair.second = 0;
	}
	friend istream &operator >> (istream &is, Problems &f);
	ostream &operator<<(ostream &os) {
		return os << probPair.first << " " << probPair.second;
	}
};

istream & operator >> (istream & is, Problems & f)
{
	return is >> f.probPair.first >> f.probPair.second;
}

int main(void) {
	initialise();

	std::vector<std::thread> threads_vector;
	std::vector<std::pair<int,int>> problem_vector;
	
	fstream problemsF("F1.txt",std::ios::in);
	if (!problemsF.is_open()) {
		throw std::runtime_error("Can't open!");
	}
	Problems obj;
	while (problemsF >> obj) {
		problem_vector.push_back(obj.probPair);
	}
	problemsF.close();

	//----------------------T A S K 1------------------------------
	
	/*std::vector<std::future<int>> ans;
	
	for (unsigned int i = 0; i < problem_vector.size(); ++i) {
		ans.emplace_back(std::async(solveOne, problem_vector.at(i).first, problem_vector.at(i).second));
		cout << "done" << endl;
		if (ans.size() % MAX_THREADS == 0) {
			for (unsigned int f = ans.size(); f != ans.size() - MAX_THREADS; --f) {
				for (;;) {
					if (is_ready(ans.at(f - 1)))
						break;
				}
			}
		}
	}
	int a = 2;
	ofstream fileOut("F2.txt");
	for (auto& fut : ans) {
		fileOut << fut.get() << endl;
	}
	fileOut.close();*/
	
	//---------------------------------------------------------------
	
	//---------------T A S K 2---------------------------------------
	
	std::vector<std::map<int, int>> answers;
	std::vector<std::pair<int,int>> tempVector;
	std::vector<std::future<int>> resId;
	int a = 0;
	int counter = 0;
	
	for (unsigned int i = 0; i < problem_vector.size(); ++i) {
		
		tempVector.push_back(problem_vector.at(i));
		if (tempVector.size() == MAX_PACKET_SIZE || i == problem_vector.size() - 1) {
			_sleep(200);
			resId.emplace_back(std::async(solveBunch, tempVector));
			tempVector.clear();
			a++;
		}
		
		if (a == MAX_THREADS) {

			for (unsigned int f = resId.size(); f != resId.size() - MAX_THREADS; --f) {
				for (;;) {
					if (is_ready(resId.at(f - 1)))
						break;
				}
			}
			list<int> tempId;
			for (unsigned int j = resId.size(); j != resId.size() - MAX_THREADS; --j) {
				int id = resId.at(j-1).get();
				tempId.push_front(id);
			}
			for (auto it = tempId.begin(); it != tempId.end(); ++it) {
				size_t sizeAnswers = 0;
				while (sizeAnswers < 4) {
					map<int, int> temp = getBunchResult(*it);
					sizeAnswers += temp.size();
					answers.push_back(temp);
				}
			}
			counter += 16;
			a = 0;
		}
	}

	// если остались
	if (a != 0) {
		for (unsigned int f = resId.size(); f != resId.size() - a; --f) {
			for (;;) {
				if (is_ready(resId.at(f - 1)))
					break;
			}
		}
		list<int> tempId;
		for (unsigned int j = resId.size(); j != resId.size() - a; --j) {
			int id = resId.at(j - 1).get();
			tempId.push_front(id);
		}
		for (auto it = tempId.begin(); it != tempId.end(); ++it) {
			a--;
			size_t sizeAnswers = 0;
			if (a == 0) {
				size_t toGet = problem_vector.size() % MAX_PACKET_SIZE;
				if (toGet == 0) {
					toGet = 4;
				}
				while (sizeAnswers < toGet) {
					map<int, int> temp = getBunchResult(*it);
					sizeAnswers += temp.size();
					answers.push_back(temp);
				}
			}
			else {
				while (sizeAnswers < 4) {
					map<int, int> temp = getBunchResult(*it);
					sizeAnswers += temp.size();
					answers.push_back(temp);
				}
			}
		}
	}

	// открытие файла
	ofstream fileOut("F2.txt");
	// запись из вектора "ответы" в файл
	unsigned int tk = 0;
	for (unsigned int i = 0; i < answers.size(); ++i) {
		size_t sizeTemp = answers.at(i).size();
		if (sizeTemp < MAX_PACKET_SIZE && sizeTemp != 0) {
			for (unsigned int j = 0; j < sizeTemp; ++j) {
				fileOut << answers.at(i).at(tk) << endl;
				tk++;
			}
			if (tk == 4) {
				tk = 0;
			}
		}
		else {
			for (unsigned int j = 0; j < sizeTemp; ++j) {
				fileOut << answers.at(i).at(j) << endl;
			}
		}
	}
	fileOut.close();
	
	
	//-----------------------------------------------------
	system("pause");
	return 0;
}


