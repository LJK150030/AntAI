#pragma once
#include "Async/AbstractRequest.hpp"

#include <string>

typedef void (*LogFunction)(std::string text);

//-----------------------------------------------------------------

class RequestPrint : public AbstractRequest
{
public:
	void SetValue(int value);
	void SetOutput(LogFunction fnc);
	void Process() override;
	void Finish() override;

private:
	int m_value = 0;
	LogFunction m_outFnc = nullptr;
};

//-----------------------------------------------------------------
