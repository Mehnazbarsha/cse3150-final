#include <iostream>
#include "LinkedList.h"

using namespace std;

int main() {
    LinkedList list;

    // Step 1: SERVER builds the circular list
    list.SERVER_buildCircularLinkedList();

    // Step 2: SERVER prints via shared_ptrs
    list.SERVER_printLinkedList();

    // Step 3: CLIENT prints via weak_ptrs
    list.CLIENT_printLinkedList();

    // Step 4: SERVER deletes the list
    list.SERVER_deleteCircularLinkedList();

    // Step 5: CLIENT tries to print
    list.CLIENT_printAfterDelete();

    return 0;
}
