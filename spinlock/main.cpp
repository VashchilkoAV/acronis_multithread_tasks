#include <iostream>
#include <atomic>
#include <cassert>


class spin_lock_TAS {
    std::atomic<unsigned> m_spin;
public:
    spin_lock_TAS() : m_spin(0) {}
    ~spin_lock_TAS() {assert(m_spin.load() == 0);}

    void Lock() {
        unsigned expected;
        do {expected = 0;}
        while(!m_spin.compare_exchange_weak(expected, 1));
    }

    void unlock() {
        m_spin.store(0);
    }
};

class spin_lock_TTAS {
    std::atomic<unsigned> m_spin;
public:
    spin_lock_TAS() : m_spin(0) {}
    ~spin_lock_TAS() {assert(m_spin.load() == 0);}

    void Lock() {
        unsigned expected;
        do {
            while(m_spin.load());
            expected = 0;
        } while(!m_spin.compare_exchange_weak(expected, 1));
    }

    void unlock() {
        m_spin.store(0);
    }
};

class ticket_lock{
    std::atomic_size_t now_serving = {0};
    std::atomic_size_t next_ticket = {0};
public:
    void lock() {
        const auto ticket = next_ticket.fetch_add(1);
        while(now_serving.load() != ticket);
    }

    void unlock() {
        const auto successor = now_serving.load() + 1;
        now_serving.store(successor);
    }
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
