#include <time.h>
#include <iostream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <functional>
#include <fstream>

#define CLOCK_START clock_t start, end; double cpu_time_used; start = clock();
#define CLOCK_FINISH end = clock(); cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

const unsigned int numberOfTests = 5;
const std::vector<size_t> contSize = { 10, 100, 1000, 10000, 100000, 500000 };

struct S {
	double a;
	double b;
	S(double one, double two) {
		a = one;
		b = two;
	}
	bool operator==(const S& anotherS) const
	{
		return (a == anotherS.a && b == anotherS.b);
	}
};

namespace std
{
	template <>
	struct hash<S>
	{
		size_t operator()(const S& k) const
		{
			// Compute individual hash values for two data members and combine them using XOR and bit shifting
			return ((hash<double>()(k.a) ^ (hash<double>()(k.b) << 1)) >> 1);
		}
	};
}



//---VECTOR-------------------------------------
double vectorIndexAcc(std::vector<double> vec);
double vectorPushBack(std::vector<double> vec);
double vectorMovPushBack(std::vector<double> vec);

//---DEQUE--------------------------------------
double dequeIndexAcc(std::deque<double> dq);
double dequePushBack(std::deque<double> dq);
double dequeMovPushBack(std::deque<double> dq);

//---MultiMap-----------------------------------
double multimapInsertSame(std::multimap<double, double> mm, double size);

//---SET----------------------------------------
double setFind(std::set<double> st, unsigned int option);

//---HASHER-------------------------------------
double badHasher();
double unOrderMapFind(std::unordered_map<double, double> unordmap);


int main() {

	std::vector<double> _vector;
	std::deque<double> _deque;
	std::multimap<double, double> _multiMap;
	std::set<double> _set;
	std::unordered_map<double, double> _unorderMap;

	//// VECTOR INDEX TEST
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_vector.push_back(i);
	//	}
	//	for (unsigned int j = 0; j < numberOfTests; ++j) {
	//		sum += vectorIndexAcc(_vector);
	//	}
	//	std::cout << "Vector get by index. size: " << it
	//		<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//	_vector.clear();
	//}
	//std::cout << "---------------------" << std::endl;
	//// VECTOR PUSH_BACK TIME
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_vector.push_back(i);
	//	}
	//	for (unsigned int j = 0; j < numberOfTests; ++j) {
	//		sum += vectorPushBack(_vector);
	//	}
	//	std::cout << "Vector push_back. size: " << it
	//		<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//	_vector.clear();
	//}
	//std::cout << "---------------------" << std::endl;
	//// VECTOR PUSH_BACK MOVEABLE ITEM TEST
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_vector.push_back(i);
	//	}
	//	for (unsigned int j = 0; j < numberOfTests; ++j) {
	//		sum += vectorMovPushBack(_vector);
	//	}
	//	std::cout << "Vector push_back moveable. size: " << it
	//		<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//	_vector.clear();
	//}

	//std::cout << "---------------------" << std::endl;
	//// DEQUE GET BY INDEX TEST
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_deque.push_back(i);
	//	}
	//	for (unsigned int j = 0; j < numberOfTests; ++j) {
	//		sum += dequeIndexAcc(_deque);
	//	}
	//	std::cout << "Deque get by index. size: " << it
	//		<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//	_deque.clear();
	//}

	//std::cout << "---------------------" << std::endl;
	//// DEQUE PUSH_BACK TEST
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_deque.push_back(i);
	//	}
	//	for (unsigned int j = 0; j < numberOfTests; ++j) {
	//		sum += dequePushBack(_deque);
	//	}
	//	std::cout << "Deque push_back test. size: " << it
	//		<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//	_deque.clear();
	//}

	//std::cout << "---------------------" << std::endl;
	//// DEQUE PUSH_BACK MOVEABLE ITEM TEST
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_deque.push_back(i);
	//	}
	//	for (unsigned int j = 0; j < numberOfTests; ++j) {
	//		sum += dequeMovPushBack(_deque);
	//	}
	//	std::cout << "Deque push_back mov item. size: " << it
	//		<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//	_deque.clear();
	//}
	//
	//std::ofstream ofs("forTable.csv");
	//std::string temp;
	//for (const auto& it : contSize) {
	//	ofs << it << '\n';
	//}
	//ofs << "\n";
	//std::cout << "---------------------" << std::endl;
	//// MultiMap insert TEST
	//for (const auto& it : contSize) {
	//	double time = 0;
	//	time = multimapInsertSame(_multiMap, it);
	//	std::cout << "MultiMap insert TEST. size: " << it
	//		<< ". Average CPU time used: " << time << std::endl;
	//	ofs << time << "\n";
	//	_multiMap.clear();
	//}
	//ofs << "\n";
	//ofs.close();

	//// SET FIND
	//for (unsigned int k = 1; k < 3; ++k) {
	//	std::cout << "---------------------" << std::endl;
	//	for (const auto& it : contSize) {
	//		double sum = 0;
	//		for (unsigned int i = 0; i < it; ++i) {
	//			_set.insert(i);
	//		}
	//		for (unsigned int j = 0; j < numberOfTests; ++j) {
	//			sum += setFind(_set, k);
	//		}
	//		if (k == 1) {
	//			std::cout << "set.find() test.";
	//		}
	//		else {
	//			std::cout << "algorithm::find test.";
	//		}
	//		std::cout << " size: " << it
	//			<< ". Average CPU time used: " << sum / numberOfTests << std::endl;
	//		_set.clear();
	//	}
	//}
	//// UNORDERMAP BUCKET SIZE CHECK
	//std::cout << "---------------------" << std::endl;
	//for (const auto& it : contSize) {
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_unorderMap.insert(std::pair<double, double> {i, rand() % 100});
	//	}
	//	std::cout << "Unordered MultiMap. size: " << it
	//		<< ". Buckets: " << _unorderMap.bucket_count() << std::endl;
	//	std::cout << "Number of elements in bucket #"
	//		<< (int)(_unorderMap.bucket_count() / 2) - 1 << ": "
	//		<< _unorderMap.bucket_size((int)(_unorderMap.bucket_count() / 2) - 1)
	//		<< std::endl;
	//	_unorderMap.clear();
	//}

	//// BAD HASHER CHECKER
	//std::cout << "---------------------" << std::endl;
	//for (const auto& it : contSize) {
	//	double sum = 0;
	//	for (unsigned int i = 0; i < it; ++i) {
	//		_unorderMap.insert(std::pair<double, double> {badHasher(), rand() % 100});
	//	}
	//	for (unsigned int i = 0; i < numberOfTests; ++i) {
	//		sum += unOrderMapFind(_unorderMap);
	//	}
	//	std::cout << "Unordered Map. find - bad hasher. size: "
	//		<< it << ". Average time: " << sum / numberOfTests << std::endl;
	//	_unorderMap.clear();
	//}
	//// GOOD HASHER
	//S obj1(6, 12);
	//S obj2(45, 21);
	//S obj3(183, 54);
	//S obj4(6, 12);
	//std::unordered_map<S, double> unMAP = {
	//	{ obj1, 54 },
	//	{ obj2, 77 },
	//	{ obj3, 66 },
	//	{ obj4, 122 }
	//};

	// BUCKET SIZE CHECK
	
	std::cout << "---------------------" << std::endl;
	for (const auto& it : contSize) {
		std::cout << "UnorderMap size: " << it
			<< ".\nBucket sizes. 5 or 10 per current size" << std::endl;
		for (unsigned int i = 0; i < it; ++i) {
			_unorderMap.insert(std::pair<double, double> {i, rand() % 100});
			if (it == 10 && _unorderMap.size() % 10 == 0) {
				std::cout << _unorderMap.bucket_count() << std::endl;
			}
			if (it == 100 && _unorderMap.size() % 10 == 0) {
				std::cout << _unorderMap.bucket_count() << std::endl;
			}
			if (it == 1000 && _unorderMap.size() % 100 == 0) {
				std::cout << _unorderMap.bucket_count() << std::endl;
			}
			if (it == 10000 && _unorderMap.size() % 1000 == 0) {
				std::cout << _unorderMap.bucket_count() << std::endl;
			}
			if (it == 100000 && _unorderMap.size() % 100 == 0) {
				//o1 << _unorderMap.bucket_count() << '\n';
			}
			if (it == 500000 && _unorderMap.size() % 100000 == 0) {
				std::cout << _unorderMap.bucket_count() << std::endl;
			}
		}
		std::cout << "---------------------" << std::endl;
		_unorderMap.clear();
	}
	
	// VECTOR CAPACITY CHECK
	_vector.shrink_to_fit();
	std::ofstream o1("a.csv");
	std::cout << "---------------------" << std::endl;
	for (const auto& it : contSize) {
		std::cout << "Vector size: " << it
			<< ".\nCapacity. 5 or 10 per current size" << std::endl;
		for (unsigned int i = 0; i < it; ++i) {
			_vector.push_back(i);
			if (it == 10 && _vector.size() % 10 == 0) {
				std::cout << _vector.capacity() << std::endl;
			}
			if (it == 100 && _vector.size() % 10 == 0) {
				std::cout << _vector.capacity() << std::endl;
			}
			if (it == 1000 && _vector.size() % 100 == 0) {
				std::cout << _vector.capacity() << std::endl;
			}
			if (it == 10000 && _vector.size() % 1000 == 0) {
				std::cout << _vector.capacity() << std::endl;
			}
			if (it == 100000 && _vector.size() % 100 == 0) {
				o1 << _vector.capacity() << "\n";
			}
			if (it == 500000 && _vector.size() % 100000 == 0) {
				std::cout << _vector.capacity() << std::endl;
			}
		}
		std::cout << "---------------------" << std::endl;
		_unorderMap.clear();
	}
	o1.close();
	system("pause");
	return 0;
}

double vectorIndexAcc(std::vector<double> vec)
{
	CLOCK_START
		vec.at((size_t)vec.size() / 2);
	CLOCK_FINISH
		return cpu_time_used;
}

double vectorPushBack(std::vector<double> vec)
{
	CLOCK_START
		double item = { 666666.0 };
	vec.push_back(item);
	CLOCK_FINISH
		return cpu_time_used;
}

double vectorMovPushBack(std::vector<double> vec)
{
	CLOCK_START

		double item = { 666666.0 };
	vec.push_back(std::move(item));

	CLOCK_FINISH
		return cpu_time_used;
}

double dequeIndexAcc(std::deque<double> dq)
{
	CLOCK_START

		dq.at((size_t)dq.size() / 2);

	CLOCK_FINISH
		return cpu_time_used;
}

double dequePushBack(std::deque<double> dq)
{
	CLOCK_START

		double item = { 666666.0 };
	dq.push_back(item);

	CLOCK_FINISH
		return cpu_time_used;
}

double dequeMovPushBack(std::deque<double> dq)
{
	CLOCK_START

		double item = { 666666.0 };
	dq.push_back(std::move(item));

	CLOCK_FINISH
		return cpu_time_used;
}

double multimapInsertSame(std::multimap<double, double> mm, double size)
{
	CLOCK_START

		for (unsigned int i = 0; i < size; ++i) {
			mm.insert(std::pair<double, double> {size, rand() % 30});
		}

	CLOCK_FINISH
		return cpu_time_used;
}

double setFind(std::set<double> st, unsigned int option)
{
	CLOCK_START
		if (option == 1) {
			st.find((double)st.size() / 2);
		}
		else {
			find(st.begin(), st.end(), (double)st.size() / 2);
		}
		CLOCK_FINISH
			return cpu_time_used;
}

double badHasher()
{
	return rand() % 100;
}

double unOrderMapFind(std::unordered_map<double, double> unordmap)
{
	CLOCK_START
		unordmap.find(5);
	CLOCK_FINISH
		return cpu_time_used;
}