#include <iostream>
#include <atomic>
#include <cassert>

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

class spin_lock_TAS {
    std::atomic<unsigned> m_spin;
public:
    spin_lock_TAS() : m_spin(0) {}
    ~spin_lock_TAS() {assert(m_spin.load() == 0);}

    void Lock() {
        Backoff backoff;
        unsigned expected;
        do {
            expected = 0;
            backoff();
        }
        while(!m_spin.compare_exchange_weak(expected, 1, std::memory_order_acquire, std::memory_order_relaxed));
    }

    void unlock() {
        m_spin.store(0, std::memory_order_release);
    }
};

class spin_lock_TTAS {
    std::atomic<unsigned> m_spin;
public:
    spin_lock_TAS() : m_spin(0) {}
    ~spin_lock_TAS() {assert(m_spin.load() == 0);}

    void Lock() {
        Backoff backoff;
        unsigned expected;
        do {
            while(m_spin.load(std::memory_order_relaxed)) {
                Backoff();
            }
            expected = 0;
        } while(!m_spin.compare_exchange_weak(expected, 1, std::memory_order_acquire, std::memory_order_relaxed));
    }

    void unlock() {
        m_spin.store(0, std::memory_order_release);
    }
};

class ticket_lock{
    std::atomic_size_t now_serving = {0};
    std::atomic_size_t next_ticket = {0};
public:
    void lock() {
        Backoff backoff;
        const auto ticket = next_ticket.fetch_add(1, std::memory_order_relaxed);
        while(now_serving.load(std::memory_order_relaxed) != ticket) {
            backoff();
        }
    }

    void unlock() {
        const auto successor = now_serving.load(std::memory_order_relaxed) + 1;
        now_serving.store(successor, std::memory_order_release);
    }
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
