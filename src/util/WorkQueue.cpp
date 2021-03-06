#include "WorkQueue.h"
#include <algorithm>
#include <cassert>

WorkQueue::WorkQueue() : stop_flag(false), idle_flag(false) { }

void WorkQueue::post(std::function<void ()> func, int priority) {
    assert(func);
    std::unique_lock<std::mutex> lock{mutex};
    item_heap.emplace_back(std::move(func), priority);
    std::push_heap(std::begin(item_heap), std::end(item_heap));
    lock.unlock();
    cond.notify_all();
}

Optional<int> WorkQueue::getMinimumPriority() const {
    std::unique_lock<std::mutex> lock{mutex};
    
    return !item_heap.empty() ?
	Optional<int>{item_heap.front().priority} :
	None;
}

void WorkQueue::stop() {
    std::unique_lock<std::mutex> lock{mutex};
    stop_flag = true;
    lock.unlock();
    cond.notify_all();
}

void WorkQueue::sync() const {
    std::unique_lock<std::mutex> lock{mutex};
    cond.wait(lock, [&]{ return idle_flag && item_heap.empty(); });
}

void WorkQueue::runAllWork() {
    std::unique_lock<std::mutex> lock{mutex};
    while (true) {
	cond.wait(lock, [&]{ return stop_flag || !item_heap.empty(); });

	if (stop_flag) {
	    return;
	}

	runItemWithoutLock(lock);
    }
}

bool WorkQueue::runSomeWork(std::chrono::milliseconds time) {
    auto stop_time = std::chrono::steady_clock::now() + time;
    
    std::unique_lock<std::mutex> lock{mutex};
    while (true) {
	bool timeout = !cond.wait_until(lock, stop_time,
					[&]{ return stop_flag || !item_heap.empty(); });
	if (timeout) {
	    return true;
	}
	
	if (stop_flag) {
	    return false;
	}

	runItemWithoutLock(lock);
    }
}

void WorkQueue::runItemWithoutLock(std::unique_lock<std::mutex> &lock) {
    Item item = std::move(item_heap.front());
    std::pop_heap(std::begin(item_heap), std::end(item_heap));
    item_heap.pop_back();

    idle_flag = false;
    lock.unlock();
    item.func();
    lock.lock();
    idle_flag = true;
    cond.notify_all();
}
