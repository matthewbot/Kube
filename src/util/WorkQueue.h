#ifndef WORKQUEUE_H
#define WORKQUEUE_H

#include <utility>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <boost/optional.hpp>

class WorkQueue {
public:
    WorkQueue();

    void post(std::function<void ()> func, int priority=0);
    boost::optional<int> getMinimumPriority() const;
    void stop();

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
};

#endif
