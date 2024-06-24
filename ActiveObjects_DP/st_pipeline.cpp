#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <functional>

using namespace std;

bool is_prime(unsigned int n) {
    if (n < 2)
        return false;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

class busyQueue {

private:
    std::queue<void *> queue;
    mutex queue_mutex;
    condition_variable queue_cond;

public:
    void enqueue(void *f) {
        unique_lock <mutex> lock(queue_mutex);
        queue.push(f);
        queue_cond.notify_one();
    }

    void *dequeue() {
        unique_lock <mutex> lock(queue_mutex);
        while (queue.empty()) {
            queue_cond.wait(lock);
        }
        auto f = queue.front();
        queue.pop();
        return f;
    }
};

class ActiveObject {

private:
    thread my_AO;
    busyQueue my_tasks;
    bool active;

public:
    ActiveObject() : active(true) {
        my_AO = thread([this] {
            while (active) {
                auto task = reinterpret_cast<function<void()> *>(my_tasks.dequeue());
                (*task)();
                delete task;

            }
        });
    }

    ~ActiveObject() {
        stop();
    }

    void CreateActiveObject(void *f) {
        my_tasks.enqueue(f);
    }

    void stop() {
        if (active) {
            auto func = new function<void()>([this]() {active = false;});
            CreateActiveObject(reinterpret_cast<void *>(func));
            my_AO.join();
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        cerr << "Usage: ./st_pipeline N [Seed](Optional)\n";
        exit(EXIT_FAILURE);
    }

    int N = stoi(argv[1]);
    unsigned int seed = (argc == 3) ? stoul(argv[2]) : time(nullptr);
    srand(seed);
    vector <unique_ptr<ActiveObject>> pipeline;

    for (int i = 0; i < 4; i++) {
        pipeline.push_back(make_unique<ActiveObject>());
    }

    for (int i = 0; i < N; i++) {
        pipeline[0]->CreateActiveObject(reinterpret_cast<void *>(new function<void()>([&pipeline](){
            unsigned int Ao1 = rand() % 900000 + 100000;
            this_thread::sleep_for(chrono::milliseconds(1));
            pipeline[1]->CreateActiveObject(reinterpret_cast<void *>(new function<void()>([Ao1, &pipeline](){
                cout << Ao1 << endl << (is_prime(Ao1) ? "true" : "false") <<endl;
                unsigned int Ao2 = Ao1 + 11;
                pipeline[2]->CreateActiveObject(reinterpret_cast<void *>(new function<void()>([Ao2, &pipeline](){
                    cout << Ao2 << endl << (is_prime(Ao2) ? "true" : "false") << endl;
                    unsigned int Ao3 = Ao2 - 13;
                    pipeline[3]->CreateActiveObject(reinterpret_cast<void *>(new function<void()>([Ao3](){
                        cout << Ao3 << endl << (is_prime(Ao3) ? "true" : "false") << endl;
                        unsigned int Ao4 = Ao3 + 2;
                        cout << Ao4 << endl;
                    })));
                })));
            })));
        })));
    }

    for (auto &ao: pipeline) {
        ao->stop();
    }

    return 0;
}