#pragma once
#include "StateMachineHandler.hpp"

template<class TContext>
class FiniteStateMachine
{
public:
    struct State : StateBase<TContext>
    {
        constexpr State(StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : StateBase<TContext>(enter, update, exit)
        {

        }
    };

    explicit FiniteStateMachine(TContext* context) : handler(context)
    {

    }

    bool isInState(State const& state) const
    {
        return handler.isInState(state);
    }

    void transitionTo(State const& state, bool force = false, bool immediate = false);
    void update();

private:
    void handleTransition();

    StateMachineHandler<TContext> handler;
};

template<class TContext>
using FSM = FiniteStateMachine<TContext>;

template<class TContext>
void FiniteStateMachine<TContext>::handleTransition()
{
    while (handler.isTransitioning(true))
    {
        if (handler.execute(handler.getCurrentState(), StateAction::Exit, true))
            continue;

        if (handler.execute(handler.getNextState(), StateAction::Enter, true))
            continue;

        handler.completeTransition();
    }
}

template<class TContext>
void FiniteStateMachine<TContext>::transitionTo(FiniteStateMachine<TContext>::State const& state, bool force, bool immediate)
{
    if (handler.trySetNextState(state, force) && immediate)
        handleTransition();
}

template<class TContext>
void FiniteStateMachine<TContext>::update()
{
    handleTransition();

    handler.execute(handler.getCurrentState(), StateAction::Update);
}