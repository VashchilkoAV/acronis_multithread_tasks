#include <iostream>
#include <atomic>
#include <vector>

constexpr unsigned THREADS_COUNT = 4;

struct Node{
    std::atomic<Node*> prev;
    int payload;
    Node(int & val) {
        payload = val;
    }
};

class HPGuard{
public:
    HPGuard(){}
    const unsigned limit = THREADS_COUNT+1;
    Node * Protect(Node* node) {
        localStorage.HP[0].store(node);
        return localStorage.HP[0].load();
    };

    void RetireNode(Node * node) {

    }

private:
    std::atomic<unsigned > index;
    std::vector<std::atomic<Node*>> storage;
    static thread_local class HPlocalStorage{
    public:
        HPlocalStorage() :
        _numThreads(THREADS_COUNT), _maxHPcount(1), _HPcount(THREADS_COUNT), _batchSize(THREADS_COUNT+1),
        _dcount(0), _dlist(std::vector<Node*>(THREADS_COUNT+1)), HP(std::vector<std::atomic<Node*>>(1)) {}

        unsigned _numThreads;
        unsigned _maxHPcount;
        unsigned _HPcount;
        unsigned _batchSize;

        std::vector<std::atomic<Node*>> HP;
        std::vector<Node*> _dlist;
        unsigned _dcount;
    } localStorage;


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
    Stack() {
        _top.store(nullptr);
    }

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
        HPGuard guard();
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
    std::cout << stack.Pop() <<"\n";
    std::cout << stack.Pop() <<"\n";
    std::cout << stack.Pop() <<"\n";
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
