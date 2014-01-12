#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include "util/WorkQueue.h"
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
    void sync() const { queue.sync(); }

    template <typename T>
    T *getLocal(const std::string &name) {
	auto iter = worker_locals.find(name);
	if (iter == std::end(worker_locals)) {
	    return nullptr;
	}

        return static_cast<T *>(iter->second);
    }

    template <typename T>
    void putLocal(std::string name, T val) {
	auto iter = worker_locals.find(name);
	if (iter == std::end(worker_locals)) {
	    worker_locals.emplace(std::move(name),
				  new T(std::move(val)));
	} else {
	    *static_cast<T *>(iter->second) = std::move(val);
	}
    }

    template <typename T>
    T &cacheLocal(const std::string &name) {
	auto iter = worker_locals.find(name);
	if (iter == std::end(worker_locals)) {
	    iter = worker_locals.emplace(name,
					 new T()).first;
	}

	return *static_cast<T *>(iter->second);
    }

private:
    WorkQueue queue;
    std::unordered_map<std::string, void *> worker_locals;
    std::thread thread;
};

#endif
