#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace std;

struct Node {
    string name;

    Node(string n) : name{n} {}

    shared_ptr<Node> next;     // SERVER owns the chain
    weak_ptr<Node>   weakNext; // CLIENT traverses without ownership

    ~Node() { cout << "Node [" << name << "] destructor" << endl; }
};

class LinkedList {
public:
    LinkedList() = default;
    ~LinkedList() = default;

    void SERVER_buildCircularLinkedList() {
        vector<string> names = {"zero", "one", "two", "three", "four", "five"};
        numberOfNodes = static_cast<int>(names.size());

        serverRoot = make_shared<Node>(names[0]);
        shared_ptr<Node> node = serverRoot;

        for (int i = 1; i < numberOfNodes; i++) {
            node->next = make_shared<Node>(names[i]);
            node = node->next;
        }
        node->next = serverRoot; // close the circle

        // mirror shared_ptr chain into weak_ptrs for CLIENT
        node = serverRoot;
        for (int i = 0; i < numberOfNodes; i++) {
            node->weakNext = node->next;
            node = node->next;
        }

        clientRoot = serverRoot; // CLIENT holds only a weak_ptr
    }

    void SERVER_printLinkedList() {
        cout << "\nServer (step 2):\n";
        cout << string(33, '-') << "\n";
        if (!serverRoot) { cout << "[empty]\n"; return; }

        shared_ptr<Node> node = serverRoot;
        for (int i = 0; i < numberOfNodes; i++) {
            cout << "[" << node->name << "]"
                 << " : use_count: "             << node.use_count()
                 << "  address: "                << node.get()
                 << "  next (from shared_ptr): " << node->next.get()
                 << "\n";
            node = node->next;
        }
    }

    void CLIENT_printLinkedList() {
        cout << "\nClient (step 3):\n";
        cout << string(33, '-') << "\n";

        shared_ptr<Node> node = clientRoot.lock();
        if (!node) { cout << "[Nothing]\n"; return; }

        for (int i = 0; i < numberOfNodes; i++) {
            cout << "[" << node->name << "]"
                 << " : use_count: "          << node.use_count()
                 << "  address: "             << node.get()
                 << "  next (from weak_ptr): ";

            shared_ptr<Node> nextNode = node->weakNext.lock();
            cout << (nextNode ? nextNode.get() : nullptr) << "\n";

            node = nextNode;
            if (!node) break;
        }
    }

    void SERVER_deleteCircularLinkedList() {
        cout << "\nServer is deleting the shared_ptrs (step 4)\n";
        cout << string(33, '-') << "\n";
        if (!serverRoot) return;

        // null each ->next in order so ref-counts drop and destructors fire
        shared_ptr<Node> node = serverRoot->next;
        for (int i = 1; i < numberOfNodes; i++) {
            shared_ptr<Node> tmp = node->next;
            node->next = nullptr;
            node = tmp;
        }
        serverRoot->next = nullptr;
        serverRoot.reset();
    }

    void CLIENT_printAfterDelete() {
        cout << "\nClient after linked list deleted (step 5):\n";
        cout << string(33, '-') << "\n";

        shared_ptr<Node> node = clientRoot.lock();
        if (!node) {
            cout << "[Nothing]\n";
            return;
        }

        cout << "[" << node->name << "]"
             << " : use_count: "          << node.use_count()
             << "  address: "             << node.get()
             << "  next (from weak_ptr): "
             << (node->weakNext.lock() ? node->weakNext.lock().get() : nullptr)
             << "\n";

        for (int i = 1; i < numberOfNodes; i++) {
            cout << "Yipes! shared_ptr not available\n";
        }
    }

private:
    shared_ptr<Node> serverRoot; // SERVER's owning handle
    weak_ptr<Node>   clientRoot; // CLIENT's non-owning handle
    int numberOfNodes = 0;
};
