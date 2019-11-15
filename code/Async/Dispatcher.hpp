#pragma once

#include "Async/AbstractRequest.hpp"
#include "Async/Worker.hpp"

#include <queue>
#include <mutex>
#include <thread>
#include <vector>


class Dispatcher {
public:
	static bool Init(uint workers);
	static bool Stop();
	static void AddRequest(AbstractRequest* request);
	static bool AddWorker(Worker* worker);
	static void JobProcessForCategory( JobCategory category );

private:
	static std::vector<std::queue<AbstractRequest*>> m_requests;
	static std::queue<Worker*> m_workers;
	static std::mutex m_requestsMutex;
	static std::mutex m_workersMutex;
	static std::vector<Worker*> m_allWorkers;
	static std::vector<std::thread*> m_threads;
};

