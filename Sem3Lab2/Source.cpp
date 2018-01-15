#include <iostream>                                                                      
#include <utility>
using namespace std;

struct Node {
	int data;
	Node* next;

	Node() {
		data = NULL;
		next = nullptr;
	}
	Node(int&& value) {
		data = value;
		next = nullptr;
	}
	Node(const Node&) = delete;
	Node(Node&&) = delete;
	Node& operator=(Node&&) = delete;
};

class Stack {
private:
	Node* _first;
	Node* _head;
public:
	Stack() {
		_first = nullptr;
		_head = nullptr;
	}
	Stack(const Stack&) = delete;
	Stack& operator= (Stack&) = delete;
	~Stack() {
		while (_first != nullptr) {
			pop_destroy();
		}
	}
	Stack& push_back(int value) {
		Node* temp = new Node;
		temp->data = value;
		temp->next = nullptr;

		if (_first == nullptr) {
			_first = temp;
			_head = temp;
		}
		else {
			_head->next = temp;
			_head = _head->next;
		}

		cout << "Pushed " << value << " on the stack\n";

		temp = nullptr;
		return *this;
	}
	Stack& pop_emplace(int&& value) {
		getHead().data = std::forward<int>(value);
	}
	Stack& pop_destroy() {
		if (_first == nullptr) {
			cout << "No nodes to pop.\n";
		}
		else {
			// only one node left
			if (_first == _head) {
				cout << "Popped " << _first->data << " off the stack\n";
				delete _first;
				_first = nullptr;
				_head = nullptr;
				return *this;
			}

			Node* temp = _first;
			while (temp->next != _head) {
				temp = temp->next;
			}
			cout << "Popped " << _head->data << " off the stack\n";
			delete _head;
			_head = temp;
			_head->next = nullptr;
		}
		return *this;
	}
	Node& getHead() {
		return *this->_head;
	}
	Stack& operator=(Stack&& obj) {
		if (this != &obj) {
			this->_first = obj._first;
			this->_head = obj._head;
			obj._first = nullptr;
			obj._head = nullptr;
		}
		return *this;
	}
	void print_stack() {
		Node *temp = _first;
		cout << "Stack: ";
		if (_first == nullptr) {
			cout << "empty\n";
			return;
		}
		do {
			cout << temp->data << " ";
			temp = temp->next;
		} while (temp->next != nullptr);
		if (_first != _head) {
			cout << _head->data;
		}
		cout << "\n";
	}
};

int main() {
	cout << "---working with 's' object---\n";
	Stack s;
	s.pop_destroy();
	s.push_back(1);
	s.push_back(2);
	s.pop_destroy();
	s.push_back(3).push_back(4);
	s.print_stack();
	cout << "get HEAD of 's': " << s.getHead().data << "\n";

	cout << "\n---moving to obj1---\n";

	Stack obj1;
	obj1 = std::move(s);
	obj1.print_stack();
	obj1.pop_destroy();
	obj1.pop_emplace(8);
	obj1.print_stack();

	cout << "\n--check if 's' is empty---\n";
	s.pop_destroy();
	s.print_stack();

	cout << "\n";
	system("pause");
	return 0;
}