#include "observer.h"

void CommandPrinterPublisher::subscribe(const std::shared_ptr<Subscriber>& obs) {
    m_subs.emplace_back(obs);
}

CommandPrinterPublisher::~CommandPrinterPublisher() {

}

void CommandPrinterPublisher::exit() {
    for (auto subs : m_subs) {
        subs.lock()->exit();
    }
    /*auto iter = m_subs.begin();
    while (iter != m_subs.end()) {
        auto ptr = iter->lock();
        if (ptr) {
            ptr->exit();
            ++iter;
        }
        else {
            m_subs.erase(iter++);
        }
    }*/
}

void CommandPrinterPublisher::setBulk(std::vector<std::string>& bulk) {
    //std::lock_guard<std::mutex> lock(m_mutex);
    m_mutex.lock();
    m_bulk.push(bulk);
    m_mutex.unlock();
    notify();
    m_mutex.lock();
    m_bulk.pop();
    m_mutex.unlock();
}

std::vector<std::string> CommandPrinterPublisher::getBulk() {
    //std::lock_guard<std::mutex> lock(m_mutex);
    if (m_bulk.empty()) return std::vector<std::string>();
    m_mutex.lock();
    const auto temp = m_bulk.front();
    m_mutex.unlock();
    return temp;
}

void CommandPrinterPublisher::notify() {
    auto iter = m_subs.begin();
    while(iter != m_subs.end()) {
        auto ptr = iter->lock();
        if (ptr) {
            ptr->update();
            ++iter;
        } else {
            m_subs.erase(iter++);
        }
    }
}

std::shared_ptr<ConsolePrinter> ConsolePrinter::create(CommandPrinterPublisher* publ) {
    auto ptr = std::shared_ptr<ConsolePrinter>{new ConsolePrinter{}};
    ptr->m_publisher = publ;
    ptr->m_publisher->subscribe(ptr->shared_from_this());
    ptr->m_thread = new std::thread(&ConsolePrinter::run, ptr);
    return ptr;
}

void ConsolePrinter::run() {
    while (!m_quite || !m_q.empty()) {
        std::unique_lock<std::mutex> lock(m_qmtx);
        m_qcv.wait(lock, [this]()->bool { return !m_q.empty() || m_quite; });
        if (!m_q.empty()) {
            stat_mtx.lock();
            auto bulk = std::move(m_q.front());

            std::cout << "bulk: ";
            for (const auto& cmd : bulk) {
                std::cout << cmd << (&cmd != &bulk.back() ? ", " : "");
            }
            std::cout << "\n";
            m_q.pop();
            stat_mtx.unlock();
        }
    }
}

void ConsolePrinter::update() {
    const auto& bulk = m_publisher->getBulk();
    if (!bulk.size()) return;
    stat_mtx.lock();
    m_q.push(bulk);
    stat_mtx.unlock();


    //std::lock_guard<std::mutex> lock(m_mtx);
    //const auto& bulk = m_publisher->getBulk();
    //if (!bulk.size()) return;
    //m_q.push(bulk);
}

void ConsolePrinter::exit() {
    m_quite = true;
    m_qcv.notify_all();
    m_thread->join();
}




std::shared_ptr<FilePrinter> FilePrinter::create(CommandPrinterPublisher* publ, int num) {
    auto ptr = std::shared_ptr<FilePrinter>{new FilePrinter{}};
    ptr->m_publisher = publ;
    ptr->m_numThreads = num;
    ptr->m_publisher->subscribe(ptr->shared_from_this());
    for (int i = 0; i < ptr->m_numThreads; ++i) {
        ptr->m_threads.emplace_back(&FilePrinter::run, ptr, i);
    }
    return ptr;
}

void FilePrinter::exit() {
    m_quite = true;
    for (int i = 0; i < m_numThreads; ++i) {
        m_qcv.notify_all();
        m_threads[i].join();
    }
}

void FilePrinter::run(int id) {
    while (!m_quite || !m_q.empty()) {
        std::unique_lock<std::mutex> lock(m_qmtx);
        m_qcv.wait(lock, [this]()->bool { return !m_q.empty() || m_quite; });
        if (!m_q.empty()) {
            m_mtx.lock();
            auto bulk = std::move(m_q.front());
            auto time = std::chrono::system_clock::now();
            std::ofstream fout;
            fout.open("bulk" + std::to_string(time.time_since_epoch().count()) + "_" + std::to_string(id)+ ".log", std::ios_base::out);
            fout << "bulk: ";
            for (const auto& cmd : bulk) {
                fout << cmd << (&cmd != &bulk.back() ? ", " : "");
            }
            fout.close();
            m_q.pop();
            m_mtx.unlock();
        }
    }
}

void FilePrinter::update() {
    const auto& bulk = m_publisher->getBulk();
    if (!bulk.size()) return;
    m_mtx.lock();
    m_q.push(bulk);
    m_mtx.unlock();
}