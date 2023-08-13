#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// Représentation simplifiée d'un bucket
struct Bucket {
    // ... 
};

class ThreadPool {

    std::vector<std::thread> workers;
    std::queue<Bucket*> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;

public:

    ThreadPool(size_t threads) {
        for(size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while(true) {
                    Bucket* task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                        if(this->stop && this->tasks.empty()) return;
                        task = tasks.front();
                        tasks.pop();
                    }
                    // Exécutez l'opération sur le bucket
                    // ...
                }
            });
        }
    }

    void enqueueTask(Bucket* task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if(stop) throw std::runtime_error("enqueue on stopped ThreadPool");
            tasks.push(task);
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for(std::thread &worker: workers) {
            worker.join();
        }
    }
};

int main() {
    ThreadPool pool(4); // Pool avec 4 threads

    // Simuler l'ajout de buckets
    std::vector<Bucket> buckets(100);
    for(Bucket &bucket : buckets) {
        pool.enqueueTask(&bucket);
    }

    return 0;
}