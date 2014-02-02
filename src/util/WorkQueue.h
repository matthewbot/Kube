#ifndef WORKQUEUE_H
#define WORKQUEUE_H

#include "util/Optional.h"
#include <vector>
#include <utility>
#include <chrono>
#include <mutex>
#include <condition_variable>

class WorkQueue {
public:
    WorkQueue();

    void post(std::function<void ()> func, int priority=0);
    Optional<int> getMinimumPriority() const;
    void stop();

    void sync() const;

    void runAllWork();
    bool runSomeWork(std::chrono::milliseconds time);
    
private:
    mutable std::mutex mutex;
    mutable std::condition_variable cond;

    struct Item {
	std::function<void ()> func;
	int priority;

	Item(std::function<void ()> func, int priority) :
	    func(std::move(func)),
	    priority(priority) { }
	
	bool operator<(const Item &other) const {
	    return priority < other.priority;
	}
    };
    std::vector<Item> item_heap;
    bool stop_flag;
    bool idle_flag;

    void runItemWithoutLock(std::unique_lock<std::mutex> &lock);
};

#endif
