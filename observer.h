#pragma once

#include <vector>
#include <list>
#include <queue>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

class Subscriber {
public:
    virtual ~Subscriber() = default;

    virtual void update() = 0;
    virtual void exit() = 0;
};

class Publisher {
public:
    virtual ~Publisher() = default;

    virtual void subscribe(const std::shared_ptr<Subscriber>& obs) = 0;
};

class CommandPrinterPublisher : public Publisher {
public:
    virtual ~CommandPrinterPublisher();
    void subscribe(const std::shared_ptr<Subscriber>& obs) override;
    void setBulk(std::vector<std::string>& bulk);
    std::vector<std::string> getBulk();
    void notify();
    void exit();

    std::queue<std::vector<std::string>> m_bulk;
    std::list<std::weak_ptr<Subscriber>> m_subs;
    std::mutex m_mutex;
};

static std::mutex stat_mtx;
class ConsolePrinter : public Subscriber, public std::enable_shared_from_this<ConsolePrinter> {
public:
    static std::shared_ptr<ConsolePrinter> create(CommandPrinterPublisher* publ);
    void update() override;
    void exit() override;

private:
    ConsolePrinter() = default;
    void run();
    std::thread* m_thread;
    std::queue< std::vector<std::string>> m_q;
    std::condition_variable m_qcv;
    std::mutex m_qmtx;
   // std::mutex m_mtx;
    bool m_quite = false;

    CommandPrinterPublisher *m_publisher{};
};

class FilePrinter : public Subscriber, public std::enable_shared_from_this<FilePrinter> {
public:
    static std::shared_ptr<FilePrinter> create(CommandPrinterPublisher* publ, int num);
    void update() override;
    void exit() override;

private:
    FilePrinter() = default;
    void run(int);
    std::vector<std::thread> m_threads;
    std::queue< std::vector<std::string>> m_q;
    std::condition_variable m_qcv;
    std::mutex m_qmtx;
    std::mutex m_mtx;
    int m_numThreads;
    bool m_quite = false;

    CommandPrinterPublisher *m_publisher{};
};