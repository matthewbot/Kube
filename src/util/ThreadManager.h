#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include "util/WorkerThread.h"
#include "util/WorkQueue.h"
#include <atomic>

class ThreadManager {
public:
    ThreadManager();

    bool runMain(std::chrono::milliseconds time) {
	return main.runSomeWork(time);
    }

    void stopMain() {
	main.stop();
    }
    
    void postWork(std::function<void ()> func, int priority=0);
    void postWork(std::function<void (WorkerThread &)> func, int priority=0);
    void postWorkAll(const std::function<void ()> &func, int priority=0);
    void postMain(std::function<void ()> func, int priority=0) {
	main.post(std::move(func), priority);
    }
    
private:
    std::vector<WorkerThread> threads;
    std::atomic<unsigned int> next_num;
    WorkQueue main;

    WorkerThread *getNextThread();
};

#endif
