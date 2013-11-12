#ifndef IOSERVICETHREADS_H
#define IOSERVICETHREADS_H

#include <boost/asio.hpp>

#include <vector>
#include <thread>

class IOServiceThreads {
public:
    IOServiceThreads(int threadcount);
    ~IOServiceThreads();

    template <typename Handler>
    void postWork(Handler handler) { work_io.post(handler); }

    template <typename Handler>
    void postMain(Handler handler) { main_io.post(handler); }
    
    boost::asio::io_service &getMainIO() { return main_io; }
    boost::asio::io_service &getWorkIO() { return work_io; }

    void runMain() { main_io.run(); }
    
private:
    boost::asio::io_service main_io;
    boost::asio::io_service work_io;
    boost::asio::io_service::work work_io_work;
    std::vector<std::thread> threads;
};

#endif
