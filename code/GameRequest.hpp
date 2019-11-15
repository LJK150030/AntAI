// #pragma once
// #include "Async/AbstractRequest.hpp"
// 
// //--------------------------------------------
// class Agent;
// typedef std::vector<eAgentActions> (*PathFunction)(const IntVec2 start, const IntVec2 end, bool team);
// typedef void (*SetPathFunction)(Agent* out_agent, std::vector<eAgentActions> path);
// 
// class RequestPath : public AbstractRequest
// {
// public:
// 	RequestPath();
// 	void SetPathCalcCB(PathFunction fnc);
// 	void SetPathSetterCB(SetPathFunction fnc);
// 	void SetStartCoord(IntVec2 start);
// 	void SetEndCoord(IntVec2 end);
// 	void SetAgent(Agent* agent);
// 	void Process() override;
// 	void Finish() override;
// 
// private:
// 	PathFunction m_pathFnc = nullptr;
// 	SetPathFunction m_setterFnc = nullptr;
// 	IntVec2 m_start;
// 	IntVec2 m_end;
// 	Agent* m_agent;
// 	std::vector<eAgentActions> m_calcPath;
// };
// 
// //--------------------------------------------
// 
// class RequestSetPath : public AbstractRequest
// {
// public:
// 	RequestSetPath();
// 	void SetPathSetterCB(SetPathFunction fnc);
// 	void SetAgent(Agent* agent);
// 	void SetPath(std::vector<eAgentActions> calc_path);
// 	void Process() override;
// 	void Finish() override;
// private:
// 	SetPathFunction m_setterFnc = nullptr;
// 	Agent* m_agent;
// 	std::vector<eAgentActions> m_calcPath;
// };
// 
// //--------------------------------------------
