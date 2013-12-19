#include "WorkerThread.h"

#include <algorithm>
#include <functional>

WorkerThread::WorkerThread() :
    thread(&WorkQueue::runAllWork, &queue) { }

WorkerThread::~WorkerThread() {
    queue.stop();
    thread.join();
}
