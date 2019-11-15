// #include "Game/GameRequest.hpp"
// #include "Game/App.hpp"
// #include "Engine/Async/Dispatcher.hpp"
// 
// RequestPath::RequestPath()
// {
// 	m_calcPath = std::vector<eAgentActions>();
// }
// 
// //-----------------------------------------------------------------
// void RequestPath::SetPathCalcCB(PathFunction fnc)
// {
// 	m_pathFnc = fnc;
// }
// 
// void RequestPath::SetPathSetterCB(SetPathFunction fnc)
// {
// 	m_setterFnc = fnc;
// }
// 
// void RequestPath::SetStartCoord(const IntVec2 start)
// {
// 	m_start = start;
// }
// 
// void RequestPath::SetEndCoord(const IntVec2 end)
// {
// 	m_end = end;
// }
// 
// void RequestPath::SetAgent(Agent* agent)
// {
// 	m_agent = agent;
// }
// 
// void RequestPath::Process()
// {
// 	m_calcPath = m_pathFnc(m_start, m_end, m_agent->GetTeam());
// }
// 
// void RequestPath::Finish()
// {
// 	RequestSetPath* path_rq = new RequestSetPath();
// 	path_rq->SetCategory(JOB_MAIN);
// 	path_rq->SetPathSetterCB(&SetAgentPath);
// 	path_rq->SetAgent(m_agent);
// 	path_rq->SetPath(m_calcPath);
// 	Dispatcher::AddRequest(path_rq);
// }
// 
// //-----------------------------------------------------------------
// RequestSetPath::RequestSetPath()
// {
// 	m_calcPath = std::vector<eAgentActions>();
// }
// 
// void RequestSetPath::SetPathSetterCB(SetPathFunction fnc)
// {
// 	m_setterFnc = fnc;
// }
// 
// void RequestSetPath::SetAgent(Agent* agent)
// {
// 	m_agent = agent;
// }
// 
// void RequestSetPath::SetPath(std::vector<eAgentActions> calc_path)
// {
// 	m_calcPath = calc_path;
// }
// 
// void RequestSetPath::Process()
// {
// 	SetAgentPath(m_agent, m_calcPath);
// }
// 
// void RequestSetPath::Finish()
// {
// }
// 
