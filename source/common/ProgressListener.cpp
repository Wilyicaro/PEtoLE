#include "ProgressListener.hpp"

void ProgressListener::progressStartNoAbort(const std::string& title)
{
	//@Note: No difference for now
	progressStart(title);
}

void ProgressListener::progressStart(const std::string& title)
{
	m_title = title;
}

void ProgressListener::progressStage(const std::string& status)
{
	m_status = status;
}

void ProgressListener::progressStagePercentage(int percentage)
{
	m_percentage = percentage;
}
