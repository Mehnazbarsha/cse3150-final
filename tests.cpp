// Unit tests

#include <iostream>
#include <sstream>
#include <cassert>
#include <memory>
#include <string>
#include "LinkedList.h"

using namespace std;

// ── Helper: redirect cout to a string ─────────────────────────────────
struct CaptureOutput {
    ostringstream buf;
    streambuf* old;
    CaptureOutput()  { old = cout.rdbuf(buf.rdbuf()); }
    ~CaptureOutput() { cout.rdbuf(old); }
    string str()     { return buf.str(); }
};

// ── Test 1: Nodes are created and server can print them ────────────────
void test_server_print() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    CaptureOutput cap;
    list.SERVER_printLinkedList();
    string out = cap.str();

    // All six node names must appear in server output
    for (const string name : {"zero","one","two","three","four","five"}) {
        assert(out.find("[" + name + "]") != string::npos &&
               "test_server_print: missing node name");
    }
    assert(out.find("shared_ptr") != string::npos);
    cout << "PASS test_server_print\n";
}

// ── Test 2: Client can print the list while it's alive ─────────────────
void test_client_print_alive() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();
    list.SERVER_printLinkedList();   // quiet, redirected nowhere

    CaptureOutput cap;
    list.CLIENT_printLinkedList();
    string out = cap.str();

    for (const string name : {"zero","one","two","three","four","five"}) {
        assert(out.find("[" + name + "]") != string::npos &&
               "test_client_print_alive: missing node name");
    }
    assert(out.find("weak_ptr") != string::npos);
    assert(out.find("Yipes") == string::npos && "client should not see expired ptrs yet");
    cout << "PASS test_client_print_alive\n";
}

// ── Test 3: Destructors fire when server deletes the list ───────────────
void test_destructors_fire() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    CaptureOutput cap;
    list.SERVER_deleteCircularLinkedList();
    string out = cap.str();

    // At least nodes one..five should print their destructors
    for (const string name : {"one","two","three","four","five"}) {
        assert(out.find("Node [" + name + "] destructor") != string::npos &&
               ("test_destructors_fire: missing destructor for " + name).c_str());
    }
    cout << "PASS test_destructors_fire\n";
}

// ── Test 4: Client sees expired weak_ptrs after deletion ───────────────
void test_client_sees_expired() {
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    // Run steps quietly
    { CaptureOutput q; list.SERVER_printLinkedList(); }
    { CaptureOutput q; list.CLIENT_printLinkedList(); }
    { CaptureOutput q; list.SERVER_deleteCircularLinkedList(); }

    CaptureOutput cap;
    list.CLIENT_printAfterDelete();
    string out = cap.str();

    // Must report expired ptrs OR nothing
    bool hasYipes   = out.find("Yipes")    != string::npos;
    bool hasNothing = out.find("[Nothing]") != string::npos;
    assert((hasYipes || hasNothing) && "test_client_sees_expired: expected expired-ptr report");
    cout << "PASS test_client_sees_expired\n";
}

// ── Test 5: Circular structure — last node's next == root address ────────
void test_circular_structure() {
    // We verify circularity via the server print output:
    // the last line's "next" address should equal the first line's "address".
    LinkedList list;
    list.SERVER_buildCircularLinkedList();

    CaptureOutput cap;
    list.SERVER_printLinkedList();
    string out = cap.str();

    // Extract all "address: 0x..." tokens
    vector<string> addresses;
    size_t pos = 0;
    while ((pos = out.find("address: ", pos)) != string::npos) {
        pos += 9;
        size_t end = out.find_first_of(" \n", pos);
        addresses.push_back(out.substr(pos, end - pos));
    }
    // Extract all "next (from shared_ptr): 0x..." tokens
    vector<string> nexts;
    pos = 0;
    while ((pos = out.find("next (from shared_ptr): ", pos)) != string::npos) {
        pos += 24;
        size_t end = out.find_first_of(" \n", pos);
        nexts.push_back(out.substr(pos, end - pos));
    }

    assert(!addresses.empty() && !nexts.empty());
    // last node's next should equal root's address
    assert(nexts.back() == addresses.front() && "test_circular_structure: list is not circular");
    cout << "PASS test_circular_structure\n";
}

// ──────────────────────────────────────────────────────────────────────
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
