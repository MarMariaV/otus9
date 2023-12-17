#include "bulk_class.h"
#include <sstream>

Bulk::Bulk(int N): m_N(N)
{
	createConsolePrinter();
	createFilePrinter(2);
}

Bulk::~Bulk()
{
}

void Bulk::exit() {
	commandPrintPubl.exit();
}

void Bulk::setBulk(const std::string& bulk, int size) {
	std::stringstream ss(bulk);
	std::string str; 
	while (getline(ss, str, '\n')) {
		if (str != "") setCmd(str);
	}
}

void Bulk::setCmd(const std::string & cmd)
{
	if (cmd == "{")
	{
		if (!m_inclBlock)
		{
			printBulk();
		}
		++m_inclBlock;
		return;
	}

	if (cmd == "}")
	{
		if (m_inclBlock > 0) --m_inclBlock;
		if (!m_inclBlock)
		{
			printBulk();
		}
		return;
	}

    q_cmd.push_back(cmd);

	if (q_cmd.size() == m_N && !m_inclBlock)
	{
		printBulk();
	}
}

void Bulk::createConsolePrinter()
{
	consolePrint = ConsolePrinter::create(&commandPrintPubl);
}

void Bulk::createFilePrinter(int num)
{
	filePrint = FilePrinter::create(&commandPrintPubl, num);
}

void Bulk::printBulk()
{
	if (q_cmd.empty()) return;

    commandPrintPubl.setBulk(q_cmd);

    q_cmd.clear();
}
