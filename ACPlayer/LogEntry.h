#pragma once
#include <string>
#include <sstream>
#include <iomanip>

class LogEntry
{
public:
	std::string timeStamp;
	std::string type;
	std::string title;
	std::string details;

	LogEntry(const std::string& timeStamp, const std::string& type, const std::string& title, const std::string& details);

	LogEntry(const std::string& type, const std::string& title, const std::string& details);

	std::string toCsv() const;

	void print() const;
private:
	std::string getCurrentTimeStamp();
};

