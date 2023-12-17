#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "observer.h"

class Bulk
{
public:
	explicit Bulk(int N);
	~Bulk();

	void setBulk(const std::string&, int);
	void setCmd(const std::string &);
	void createConsolePrinter();
	void createFilePrinter(int num);
	void exit();

private:
	size_t m_N;
    size_t m_inclBlock = 0;
    std::vector<std::string> q_cmd;

    CommandPrinterPublisher commandPrintPubl;
 	std::shared_ptr<ConsolePrinter > consolePrint;
	std::shared_ptr<FilePrinter > filePrint;

	void printBulk();
};

