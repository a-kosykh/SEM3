#include <thread>
#include <vector>
#include <map>
#include <chrono>
#include <tuple>
#include <future>
#include <random>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <random>
#include <iterator>

using namespace std;

const int kOneTaskDelay = 2;
const int kBunchTaskDelay = 1;
const int kBunchSizeMin = 2;
const int kBunchSizeMax = 4;
const int kBunchTasksCountMin = 2;
const int kBunchTasksCountMax = 4;

class Task {
	pair<int, int> task;
	promise<int> result_promise;
	future<int> result;
	
	mutex m;
public:
	Task(const pair<int, int>& t) : task(t), result(result_promise.get_future())
	{}

	Task(const Task&) = delete;
	Task(Task&&) = default;
	void solve() {
		result_promise.set_value(task.first + task.second);
	}
	bool ready() {
		return result.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	int get()  {
		return result.get();
	}
};

class TaskBunch {
private:
	map<int, Task> tasks;
	mutex m;
public:
	TaskBunch(const std::vector<std::pair<int, int>>& bunch) {
		auto index = 0;
		for(const auto& t: bunch) {
			tasks.emplace(std::piecewise_construct, std::forward_as_tuple(index++), std::forward_as_tuple(t));
		}
	}
	void solve() {
		std::vector<int> keys;
		{
			lock_guard<mutex> lock(m);
			transform(tasks.begin(), tasks.end(), back_inserter(keys), [](const auto& p){ return p.first; });
		}

		// std::random_device rd;
		// std::mt19937 g(rd());
		
		// shuffle(keys.begin(), keys.end(), g);
		for(const auto& k: keys) {
			//cerr << "Sleep " << k << endl;
			this_thread::sleep_for(std::chrono::seconds(kBunchTaskDelay));
			lock_guard<mutex> lock(m);
			tasks.find(k)->second.solve();
		}
	}

	std::map<int, int> getReadyTasks() {
		lock_guard<mutex> lock(m);
		std::map<int, int> res;

		std::vector<int> keys;
		transform(tasks.begin(), tasks.end(), back_inserter(keys), [](const auto& p){ return p.first; });

		for(const auto& key: keys) {
			if(tasks.find(key)->second.ready()) {
				res.emplace(key, tasks.find(key)->second.get());
				tasks.erase(key);
				cerr << "Erased " << key << endl;
			}
		}

		return res;
	}

	bool empty() {
		lock_guard<mutex> lock(m);
		//cerr << tasks.size() << endl;
		return tasks.empty();
	}
};


std::mutex gMapLock;
std::map<int, TaskBunch> gTasks;

void initialise() {
	srand(time(0));
}

int solveOne(int a, int b) {
	Task t(make_pair(a, b));
	t.solve();
	this_thread::sleep_for(std::chrono::seconds(kOneTaskDelay));
	return t.get();
}

int solveBunch(std::vector<std::pair<int, int>>& bunch) {
	auto id = rand();
	//TaskBunch b(bunch);
	lock_guard<mutex> lock(gMapLock);

	if(bunch.size() > kBunchSizeMax) {
		throw runtime_error("Too much to solve");
	}

	/*gif(bunch.size() < kBunchTasksCountMin) {
		throw runtime_error("Use single task solver, dude");
	}*/

	if(gTasks.size() > kBunchTasksCountMax) {
		throw runtime_error("Exceed parallel task limit");
	}

	auto r = gTasks.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(bunch));
	auto& tb = r.first->second;

	thread([&tb](){
		tb.solve();
	}).detach();

	return id;
}

std::map<int, int> getBunchResult(int handler) {
	lock_guard<mutex> lock(gMapLock);
	
	auto taskIt = gTasks.find(handler);
	if (taskIt == gTasks.end())
		throw std::runtime_error("No such task");

	auto readyTasks = taskIt->second.getReadyTasks();
	if(taskIt->second.empty())
		gTasks.erase(taskIt);

	return readyTasks;
}
