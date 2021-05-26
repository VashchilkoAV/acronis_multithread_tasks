#include <iostream>
#include <atomic>

struct Node{
    std::atomic<Node*> prev;
    int payload;
    Node(int & val) {
        payload = val;
    }
};

class HPGuard{
public:
    Node * Protect(Node* node) {
        return node;
    };
};

class Backoff{
private:
    const int _initial;
    const int _step;
    const int _threshold;
    int _current;
public:
    Backoff(int init = 10, int step = 2, int threshold = 8000) : _initial(init), _step(step), _threshold(threshold), _current(init){}

    void operator()() {
        for (int i = 0; i < _current; i++) {

        }
        _current *= _threshold;
        if (_current > _threshold) {
            _current = _threshold;
        }
    }

    void Reset() {
        _current = _initial;
    }
};

class Stack{
public:
    bool Push(int value) {
        Node* oldTop = _top.load(std::memory_order_relaxed);
        Node* newNode = new Node(value);
        Backoff backoff;
        while(true) {
            newNode->prev.store(oldTop, std::memory_order_relaxed);
            if (_top.compare_exchange_weak(oldTop, newNode, std::memory_order_release, std::memory_order_relaxed)) {
                return true;
            }
            backoff();
        }

    }

    int Pop() {
        HPGuard guard;
        Backoff backoff;
        while(true) {
            Node * oldTop = guard.Protect(_top.load());
            if (oldTop == nullptr) {
                return -1;
            }

            Node * newTop = oldTop->prev;//.load(std::memory_order_relaxed);
            if (_top.compare_exchange_weak(oldTop, newTop, std::memory_order_acquire, std::memory_order_relaxed)) {
                return oldTop->payload;
            }
            backoff();
        }

    }

private:


    std::atomic<Node*> _top;
};

int main() {
    Stack stack;
    stack.Push(1);
    stack.Push(2);
    stack.Push(3);
    std::cout << stack.Pop() <<"\n";
    std::cout << stack.Pop() <<"\n";
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
