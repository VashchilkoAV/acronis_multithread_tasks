#include <iostream>
#include <atomic>
#include <cassert>
#include <chrono>
#include <vector>
#include <mutex>
#include <thread>

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

    void lock() {
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
    spin_lock_TTAS() : m_spin(0) {}
    ~spin_lock_TTAS() {assert(m_spin.load() == 0);}

    void lock() {
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

class Timer {
public:
    Timer() {
        begin = std::chrono::steady_clock::now();
    }

    ~Timer() {
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
        std::cout << elapsed_ms.count() << " ms" << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::_V2::steady_clock, std::chrono::duration<long int, std::ratio<1, 1000000000>>>
            begin;


};

template<typename Lock=spin_lock_TAS>
class Tester {
private:
    const int thread_cnt = 100;
    Lock lock;

public:


    void test1() {

        using namespace std::chrono;
        using time_point = time_point<std::chrono::steady_clock>;


        for (int thread_cnt = 1; thread_cnt < 9; ++thread_cnt) {

            Timer* t = new Timer;

            int i = 0;
            const long long limit = 1000000;
            std::vector<std::vector<std::pair<time_point, time_point>>> vec(thread_cnt);

            auto task = [&](int num) {
                steady_clock clock;
                while (true) {

                    auto knock = clock.now();
                    std::lock_guard<Lock> _lock(lock);
                    auto enter = clock.now();
                    vec[num].push_back(std::make_pair(knock, enter));
                    if (i < limit) {
                        ++i;
                    } else {
                        break;
                    }

                }
            };

            std::vector<std::thread> workers(thread_cnt);

            for (int i = 0; i < thread_cnt; ++i) {
                workers[i] = std::thread(task, i);
            }

            for (auto& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }

            std::cout << thread_cnt << " threads\n";
            std::cout << "time elapsed" << std::endl;
            delete t;

            assert(i == limit);

            double avg;
            int cnt = 0;
            for (auto& arr: vec) {
                for (auto& item: arr) {
                    ++cnt;
                    avg += duration_cast<milliseconds>(item.second - item.first).count();
                }
            }

            std::cout << "avg = " << avg / static_cast<double>(cnt) << std::endl <<std::endl;

        }


    } // test if lock works correctly

    /*
    void test2() {
        using namespace std::chrono;

        using time_point = time_point<std::chrono::steady_clock>;

        std::chrono::steady_clock clock;

        std::vector<std::vector<std::pair<time_point, time_point>>> vec(thread_cnt);

        auto task = [&](int num) {
            for (int i = 0; i < 2; ++i) {
//                int my_moment_of_request = clock();
                auto knock = clock.now();
                std::lock_guard<TTAS> _lock(lock);
                auto enter = clock.now();
                vec[num].push_back(std::make_pair(knock, enter));
            }
        };


        std::vector<std::thread> workers(thread_cnt);


        for (int i = 0; i < thread_cnt; ++i) {
            workers[i] = std::thread(task, i);
        }

        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

//        for (int i = 0; i < thread_cnt; ++i) {
//            std::cout << "thread " << i << std::endl;
//            for (auto& item: vec[i]) {
//                std::cout << "I requested lock at " << item.first.time_since_epoch() << " and got lock at "
//                          << item.second << std::endl;
//            }
//        }

    }// show fairness/unfairness
     */



    void test3() {

        using namespace std::chrono;
        using time_point = time_point<std::chrono::steady_clock>;


        for (int thread_cnt = 100; thread_cnt < 110; ++thread_cnt) {

            std::vector<std::vector<std::pair<time_point, time_point>>> vec(thread_cnt);
            Timer* t = new Timer;

            auto task = [&](int num) {
                steady_clock clock;

                for (int i = 0; i < 10; ++i) {
                    auto knock = clock.now();
                    std::lock_guard<Lock> _lock(lock);
                    auto enter = clock.now();
                    vec[num].push_back(std::make_pair(knock, enter));
                    for (int j = 0; j < 1000; ++j);
                }

            };

            std::vector<std::thread> workers(thread_cnt);

            for (int i = 0; i < thread_cnt; ++i) {
                workers[i] = std::thread(task, i);
            }

            for (auto& worker : workers) {
                if (worker.joinable()) {
                    worker.join();
                }
            }

            std::cout << thread_cnt << " threads\n";

            std::cout << "time elapsed" << std::endl;
            delete t;

            double avg;
            int cnt = 0;
            for (auto& arr: vec) {
                for (auto& item: arr) {
                    ++cnt;
                    avg += duration_cast<milliseconds>(item.second - item.first).count();
                }
            }

//            std::cout << avg << "   " << cnt << std::endl;
            std::cout << "avg = " << avg / static_cast<double>(cnt) << std::endl << std::endl;

        }

    }

};

int main() {
    Tester<spin_lock_TAS> tester;
    tester.test1();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
