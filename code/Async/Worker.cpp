#include "Async/Worker.hpp"
#include "Async/Dispatcher.hpp"

#include <chrono>


Worker::Worker(): m_request(nullptr), m_isRunning(true), m_isReady(false)
{
}

void Worker::Run()
{
	while (m_isRunning) 
	{
		if (m_isReady) 
		{
			// Execute the request.
			m_isReady = false;
			m_request->Process();
			m_request->Finish();

			//work finished request and will clean up
			delete m_request;
			m_request = nullptr;

		}

		// Add self to Dispatcher queue and execute next request or wait.
		if (Dispatcher::AddWorker(this)) 
		{
			// Use loop to deal with spurious wake-ups.
			while (!m_isReady && m_isRunning) 
			{
				std::unique_lock<std::mutex> ulock(m_mutex);
				if (m_conditionVariable.wait_for(ulock, 
					std::chrono::seconds(1)) == 
					std::cv_status::timeout) 
				{
					// We timed out, but we keep waiting unless the worker is
					// stopped by the dispatcher.
				}
			}
		}
	}
}

void Worker::Stop()
{
	m_isRunning = false;
}


void Worker::SetRequest(AbstractRequest* request)
{
	this->m_request = request;
	m_isReady = true;
}


void Worker::GetCondition(std::condition_variable* &cv)
{
	cv = &(this)->m_conditionVariable;
}