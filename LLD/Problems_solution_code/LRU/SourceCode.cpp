#include<bits/stdc++.h>
using namespace std;


void INIT() {
	freopen("input.txt" , "r" , stdin);
	freopen("output.txt" , "w" , stdout);
}
class Node {
public:
	int key;
	Node* prev;
	Node* next;
	Node(int key): key(key), prev(nullptr), next(nullptr) {}
};
class DLL {
public:
	Node* head, *tail;
	DLL(): head(nullptr), tail(nullptr) {
		head = new Node(-1);
		tail = new Node(-1);
		head->next = tail;
		tail->prev = head;
	}
	void deleteNode(Node* node) {
		if (node->next == nullptr)return;
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	void moveTohead(Node* node) {
		deleteNode(node);
		head->next->prev = node;
		node->next = head->next;
		head->next = node;
		node->prev = head;
		cout << node->key << " is new head\n";

	}
};

class LRU {
	int capacity;
	unordered_map < int, Node*>cache;
	DLL dll;
public:
	LRU(int capacity): capacity(capacity) {
		cache.clear();
	}
	void get(int key) {
		if (cache.find(key) != cache.end()) {
			cout << "key " << key << " found\n";
			dll.moveTohead(cache[key]);
			return;
		}
		cout << "key " << key << " not found\n";
		put(key);
	}
	void put(int key) {
		capacity--;
		if (capacity < 0) {
			cache.erase(dll.tail->prev->key);
			dll.deleteNode(dll.tail->prev);
			cout << dll.tail->prev->key << " is deleted\n";
			capacity++;
		}
		Node* node = new Node(key);
		dll.moveTohead(node);
		cache[key] = node;
		// cout << key << " is updated\n";
	}
	void print() {
		cout << "Printing keys: ";
		Node *start = dll.head->next;
		while (start != dll.tail) {
			cout << start->key << ' ';
			start = start->next;
		}
		cout << endl;
		// delete start;
	}
};

int main() {
	INIT();
// ------  Write Your Code From Here -----------
	LRU lru(3);
	lru.get(1);
	lru.put(2);
	lru.get(3);
	lru.get(1);
	lru.get(4);
	lru.print();

	return 0;
}
