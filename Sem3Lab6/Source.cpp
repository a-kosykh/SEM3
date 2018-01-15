#include <iostream>
#include <memory>
#include <utility>

template <typename T>
void swapValues(std::unique_ptr<T> &uni, std::shared_ptr<T> &sharPtr) {
	std::swap(*uni, *sharPtr);
	std::cout << "Swapped!\n";
}


template <typename T>
class LinkedList {
public:
	LinkedList();
	unsigned int getLength() const;
	void print() const;
	void push_back(T data);
	void push_front(T data);
	void pop_back();
	void pop_front();


private:
	struct ListNode {
		T data;
		std::shared_ptr<ListNode> next;
		std::weak_ptr<ListNode> prev;

		ListNode(T const &data) : data(data) {}
	};
	unsigned int length;
	std::shared_ptr<ListNode> head;
	std::shared_ptr<ListNode> tail;
};

template <typename T>
LinkedList<T>::LinkedList()
	: length(0) {}



template <typename T>
unsigned int LinkedList<T>::getLength() const {
	return length;
}

template <typename T>
void LinkedList<T>::print() const {
	std::shared_ptr<ListNode> h(head);
	std::cout << "List (length = " << length << "): ";
	if (length == 0) {
		std::cout << "empty\n";
	}
	else {
		while (h) {
			std::cout << h->data << " ";
			h = h->next;
		}
		std::cout << std::endl;
	}
}

template <typename T>
void LinkedList<T>::push_back(T data) {
	std::shared_ptr<ListNode> node_ptr(std::make_shared<ListNode>(data));
	node_ptr->prev = tail;

	if (tail) {
		tail->next = node_ptr;
		tail = std::move(node_ptr);
	}
	else {
		tail = std::move(node_ptr);
		head = tail;
	}

	length++;
}

template <typename T>
void LinkedList<T>::push_front(T data) {
	std::shared_ptr<ListNode> node_ptr(std::make_shared<ListNode>(data));
	node_ptr->next = head;

	if (head) {
		head->prev = node_ptr;
		head = std::move(node_ptr);
	}
	else {
		head = std::move(node_ptr);
		tail = head;
	}

	length++;
}


template<typename T>
void LinkedList<T>::pop_back() {
	if (!tail) {
		return;
	}
	auto ptr = tail;
	auto shared_prev = (tail->prev).lock();
	if (shared_prev) {
		tail = shared_prev;
		shared_prev->next = ptr->next;
	}
	else {
		head.reset();
		tail.reset();
	}
	length--;
}

template<typename T>
void LinkedList<T>::pop_front()
{
	if (!head) {
		return;
	}
	auto ptr = head;
	if (head->next) {
		head = head->next;
		ptr->next->prev = ptr->prev;
	}
	else {
		head.reset();
		tail.reset();
	}
	length--;
}



int main() {

	std::unique_ptr<int> uniP(new int(10));
	std::shared_ptr<int> shaP(new int(1000));
	std::cout << *uniP << "\n"; // 10
	std::cout << *shaP << "\n"; // 1000
	swapValues(uniP, shaP);
	std::cout << *uniP << "\n"; // 1000
	std::cout << *shaP << "\n\n"; // 10
	/////////////////////////////////////////////////

	LinkedList<int> obj;
	obj.push_back(1);
	obj.push_back(2);
	obj.push_back(3);
	obj.push_back(4);
	obj.push_back(5);
	obj.print(); // 1 2 3 4 5

	obj.pop_front();
	obj.print(); // 2 3 4 5

	obj.pop_back();
	obj.print(); // 2 3 4

	obj.push_front(666);
	obj.print(); // 666 2 3 4

				 /////////////////////////////////////////////////
	std::cout << "\n";
	LinkedList<int> obj1;
	obj1.push_back(3);
	obj1.print(); // 3

	obj1.pop_back();
	obj1.print(); // empty

	system("pause");
	return 0;
}