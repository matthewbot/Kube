#include "WorkerThread.h"

#include <algorithm>

WorkerThread::WorkerThread() :
    thread(&WorkQueue::runAllWork, &queue) { }

WorkerThread::~WorkerThread() {
    queue.stop();
    thread.join();
}
