#pragma once
#include "Async/AbstractRequest.hpp"

#include <condition_variable>
#include <mutex>


class Worker {
public:
	Worker();
	void Run();
	void Stop();
	void SetRequest(AbstractRequest* request);
	void GetCondition(std::condition_variable* & cv);

private:
	std::condition_variable m_conditionVariable;
	std::mutex m_mutex;
	AbstractRequest* m_request;
	bool m_isRunning;
	bool m_isReady;
};
