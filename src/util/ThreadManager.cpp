#include "ThreadManager.h"
#include <algorithm>

ThreadManager::ThreadManager() :
    threads(std::thread::hardware_concurrency()),
    next_num(0) { }

void ThreadManager::postWork(std::function<void ()> func, int priority) {
    getNextThread()->post(std::move(func), priority);
}

void ThreadManager::postWork(std::function<void (WorkerThread &)> func, int priority) {
    auto threadptr = getNextThread();
    threadptr->post(std::bind(std::move(func), std::ref(*threadptr)), priority);
}

void ThreadManager::postWorkAll(const std::function<void()> &func, int priority) {
    for (auto &thread : threads) {
	thread.post(func, priority);
    }
}

WorkerThread *ThreadManager::getNextThread() {
    unsigned int num;
    unsigned int next;
    do {
	num = next_num.load();
	next = num+1;
	if (next >= threads.size()) {
	    next = 0;
	}
    } while (!next_num.compare_exchange_weak(num, next));

    return &threads[num];
}

