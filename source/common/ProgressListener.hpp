#pragma once

#include <string>

class ProgressListener
{
public:
	virtual void progressStartNoAbort(const std::string& title);
	virtual void progressStart(const std::string& title);
	virtual void progressStage(const std::string& status);
	virtual void progressStagePercentage(int);

public:
	int m_percentage = 0;
	std::string m_status = "";
	std::string m_title = "";
};
