#include "WorkerThread.h"

#include <algorithm>
#include <functional>

WorkerThread::WorkerThread() :
    thread(&WorkQueue::runAllWork, &queue) { }

WorkerThread::~WorkerThread() {
    if (thread.joinable()) {
        stop();
    }
}

void WorkerThread::stop() {
    queue.stop();
    thread.join();
}
