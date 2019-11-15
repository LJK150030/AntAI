#include "Async/Dispatcher.hpp"

std::vector<std::queue<AbstractRequest*>> Dispatcher::m_requests;
std::queue<Worker*> Dispatcher::m_workers;
std::mutex Dispatcher::m_requestsMutex;
std::mutex Dispatcher::m_workersMutex;
std::vector<Worker*> Dispatcher::m_allWorkers;
std::vector<std::thread*> Dispatcher::m_threads;


TODO("need to pass in active threads into this, stall them and wait for instructions")
bool Dispatcher::Init(uint workers)
{
	std::thread* t = nullptr;
	Worker* worker = nullptr;

	for (int worker_idx = 0; worker_idx < static_cast<int>(workers); ++worker_idx) 
	{
		worker = new Worker;
		m_allWorkers.push_back(worker);
		t = new std::thread(&Worker::Run, worker);
		m_threads.push_back(t);
	}


	for(int category_idx = 0; category_idx < NUM_JOB_CATEGORIES; ++category_idx)
	{
		m_requests.emplace_back();
	}

	//DebuggerPrintf("Initialised Dispatcher \n");
	return true;
}


bool Dispatcher::Stop()
{
	//stop worker threads
	for (uint worker_idx = 0; worker_idx < m_allWorkers.size(); ++worker_idx) 
	{
		m_allWorkers[worker_idx]->Stop();
	}
	//DebuggerPrintf("Stopped workers.\n");


	//join worker threads and delete thread reference
	for (int thread_idx = 0; thread_idx < m_threads.size(); ++thread_idx) 
	{
		m_threads[thread_idx]->join();
		delete m_threads[thread_idx];
		m_threads[thread_idx] = nullptr;

		//DebuggerPrintf("Joined threads. \n");
	}

	//delete worker reference
	for (uint worker_idx = 0; worker_idx < m_allWorkers.size(); ++worker_idx) 
	{
		delete m_allWorkers[worker_idx];
		m_allWorkers[worker_idx] = nullptr;
	}

	return true;
}


void Dispatcher::AddRequest(AbstractRequest* request)
{
	m_workersMutex.lock();

	// Check whether there's a worker available in the workers queue, else add
	// the request to the requests queue.
	if (!m_workers.empty() && request->GetCategory() == JOB_GENERAL) {
		Worker* worker = m_workers.front();
		worker->SetRequest(request);

		std::condition_variable* cv;
		worker->GetCondition(cv);
		cv->notify_one();

		m_workers.pop();
		m_workersMutex.unlock();
	}
	else {
		m_workersMutex.unlock();

		m_requestsMutex.lock();
		m_requests[static_cast<int>(request->GetCategory())].push(request);
		m_requestsMutex.unlock();
	}


}


// --- ADD WORKER ---
bool Dispatcher::AddWorker(Worker* worker)
{
	bool wait = true;
	m_requestsMutex.lock();

	// If a request is waiting in the requests queue, assign it to the worker.
	// Else add the worker to the workers queue.
	// Returns true if the worker was added to the queue and has to wait for its condition variable.
	if (!m_requests.empty() && !m_requests[JOB_GENERAL].empty()) {
		AbstractRequest* request = m_requests[JOB_GENERAL].front();
		worker->SetRequest(request);

		m_requests[JOB_GENERAL].pop();

		wait = false;

		m_requestsMutex.unlock();
	}
	else {
		m_requestsMutex.unlock();

		// These are the only important lines for dispatcher
		m_workersMutex.lock();
		m_workers.push(worker);
		m_workersMutex.unlock();
	}

	return wait;
}

void Dispatcher::JobProcessForCategory(JobCategory category)
{
	m_requestsMutex.lock();
	int category_idx = static_cast<int>(category);

	while(!m_requests[category_idx].empty()) 
	{
		AbstractRequest* request = m_requests[category_idx].front();
		m_requests[category_idx].pop();
		// Execute the request.
		request->Process();
		request->Finish();
	}

	m_requestsMutex.unlock();
}
