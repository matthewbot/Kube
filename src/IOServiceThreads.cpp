#include "IOServiceThreads.h"

IOServiceThreads::IOServiceThreads(int threadcount) :
    work_io(threadcount),
    work_io_work(work_io)
{
    for (int i = 0; i < threadcount; i++) {
        threads.emplace_back([this]() {
            work_io.run();
        });
    }
}

IOServiceThreads::~IOServiceThreads() {
    work_io.stop();
    for (auto &thread : threads) {
        thread.join();
    }
}
