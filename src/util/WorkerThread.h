#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "util/WorkQueue.h"
#include <boost/any.hpp>
#include <unordered_map>
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

    template <typename T>
    T *getLocal(const std::string &name) {
	auto iter = worker_locals.find(name);
	if (iter == std::end(worker_locals)) {
	    return nullptr;
	}

        return boost::any_cast<T *>(iter->second);
    }

    template <typename T>
    void putLocal(std::string name, T val) {
	auto iter = worker_locals.find(name);
	if (iter == std::end(worker_locals)) {
	    worker_locals.emplace(std::move(name),
				  boost::any(std::move(val)));
	} else {
	    iter->second = std::move(val);
	}
    }

    template <typename T>
    T &cacheLocal(const std::string &name) {
	auto iter = worker_locals.find(name);
	if (iter == std::end(worker_locals)) {
	    iter = worker_locals.emplace(name,
					 boost::any(T())).first;
	}

	return boost::any_cast<T &>(iter->second);
    }

private:
    WorkQueue queue;
    std::unordered_map<std::string, boost::any> worker_locals;
    std::thread thread;
};

#endif
