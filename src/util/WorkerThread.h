#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "util/WorkQueue.h"
#include <utility>
#include <thread>

class WorkerThread {
public:
    WorkerThread();
    ~WorkerThread();
    
    void post(std::function<void ()> func, int priority=0) {
	queue.post(std::move(func), priority);
    }  
    boost::optional<int> getMinimumPriority() const {
	return queue.getMinimumPriority();
    }
    
private:
    WorkQueue queue;
    std::thread thread;
};

#endif
