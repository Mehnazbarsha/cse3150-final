#include <iostream>
#include <sstream>
#include <cassert>
#include <memory>
#include <string>
#include "LinkedList.h"

using namespace std;

struct CaptureOutput {
    ostringstream buf;
    streambuf* old;
    CaptureOutput()  { old = cout.rdbuf(buf.rdbuf()); }
    ~CaptureOutput() { cout.rdbuf(old); }
    string str()     { return buf.str(); }
};

void test_server_print() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    CaptureOutput cap;
    list.SERVER_printLinkedList();
    string out = cap.str();

    for (const string name : {"zero","one","two","three","four","five"})
        assert(out.find("[" + name + "]") != string::npos);

    assert(out.find("shared_ptr") != string::npos);
    cout << "PASS test_server_print\n";
}

void test_client_print_alive() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();
    { CaptureOutput q; list.SERVER_printLinkedList(); }

    CaptureOutput cap;
    list.CLIENT_printLinkedList();
    string out = cap.str();

    for (const string name : {"zero","one","two","three","four","five"})
        assert(out.find("[" + name + "]") != string::npos);

    assert(out.find("weak_ptr") != string::npos);
    assert(out.find("Yipes") == string::npos);
    cout << "PASS test_client_print_alive\n";
}

void test_destructors_fire() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    CaptureOutput cap;
    list.SERVER_deleteCircularLinkedList();
    string out = cap.str();

    for (const string name : {"one","two","three","four","five"})
        assert(out.find("Node [" + name + "] destructor") != string::npos);

    cout << "PASS test_destructors_fire\n";
}

void test_client_sees_expired() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    { CaptureOutput q; list.SERVER_printLinkedList(); }
    { CaptureOutput q; list.CLIENT_printLinkedList(); }
    { CaptureOutput q; list.SERVER_deleteCircularLinkedList(); }

    CaptureOutput cap;
    list.CLIENT_printAfterDelete();
    string out = cap.str();

    bool hasYipes   = out.find("Yipes")     != string::npos;
    bool hasNothing = out.find("[Nothing]") != string::npos;
    assert(hasYipes || hasNothing);
    cout << "PASS test_client_sees_expired\n";
}

void test_circular_structure() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    CaptureOutput cap;
    list.SERVER_printLinkedList();
    string out = cap.str();

    vector<string> addresses, nexts;
    size_t pos = 0;

    while ((pos = out.find("address: ", pos)) != string::npos) {
        pos += 9;
        size_t end = out.find_first_of(" \n", pos);
        addresses.push_back(out.substr(pos, end - pos));
    }
    pos = 0;
    while ((pos = out.find("next (from shared_ptr): ", pos)) != string::npos) {
        pos += 24;
        size_t end = out.find_first_of(" \n", pos);
        nexts.push_back(out.substr(pos, end - pos));
    }

    assert(!addresses.empty() && !nexts.empty());
    assert(nexts.back() == addresses.front());
    cout << "PASS test_circular_structure\n";
}

int main() {
    cout << "\n=== Unit Tests: LinkedList (weak_ptr lab) ===\n\n";
    test_server_print();
    test_client_print_alive();
    test_destructors_fire();
    test_client_sees_expired();
    test_circular_structure();
    cout << "\nAll tests passed!\n";
    return 0;
}