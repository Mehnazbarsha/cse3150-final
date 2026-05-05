# Lab: Node Cycles and weak_ptrs

## Overview
This lab demonstrates C++ `shared_ptr` and `weak_ptr` semantics using a
circular linked list with a SERVER/CLIENT ownership model.

**Main Idea**  
- The **SERVER** owns every node via `shared_ptr` — nodes stay alive as long as
  the SERVER holds its pointers.  
- The **CLIENT** accesses nodes via `weak_ptr` — it can traverse the list while
  it exists, but cannot prevent nodes from being destroyed.

## Files

| File | Description |
|------|-------------|
| `LinkedList.h` | `Node` struct + `LinkedList` class (all implementation) |
| `main.cpp` | Runs the 5-step demo |
| `tests.cpp` | 5 unit tests (assert-based) |
| `Makefile` | Build targets: `all`, `run`, `test`, `clean` |

## Build and Run

**Requirements:** g++ with C++17 support.

```bash
# Build everything
make

# Run the demo
make run

# Run unit tests
make test

# Clean
make clean
```

Or manually:

```bash
g++ -std=c++17 -Wall -o main main.cpp && ./main
g++ -std=c++17 -Wall -o tests tests.cpp && ./tests
```

