#pragma once
#include <stack>
#include <functional>

namespace Elite
{
	class Blackboard;
}

namespace GOAP
{
	class FSM;

	class FSMState final
	{
	public:
		FSMState() = default;
		FSMState(std::function<void(GOAP::FSM*, Elite::Blackboard*)> _state) : m_State(_state) {};

		void Update(GOAP::FSM* pFsm, Elite::Blackboard* pBlackboard) { m_State(pFsm, pBlackboard); }
	private:
		std::function<void(FSM*, Elite::Blackboard*)> m_State;
	};

	class FSM final
	{
	public:
		void Update(Elite::Blackboard* pBlackboard) { if (!m_StateStack.empty()) m_StateStack.top().Update(this, pBlackboard); }
		void PushState(FSMState& state) { m_StateStack.push(state); }
		void PopState() { m_StateStack.pop(); }
		void ClearStack() { m_StateStack = std::stack<FSMState>{}; }
	private:
		std::stack<FSMState> m_StateStack;
	};
}