#pragma once
#include "StateMachineHandler.hpp"

template<class TContext>
class FiniteStateMachine
{
public:
    struct State : StateBase<TContext>
    {
        State(StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : StateBase<TContext>(enter, update, exit)
        {

        }
    };

    explicit FiniteStateMachine(TContext* context) : handler(context)
    {

    }

    bool isInState(State& state) const
    {
        return handler.isInState(state);
    }

    void update();
    void transitionTo(State& state, bool immediate = false);

private:
    static constexpr uint8_t QUEUE_SIZE = 4;

    StateMachineHandler<TContext, QUEUE_SIZE + 1> handler;
};

template<class TContext>
using FSM = FiniteStateMachine<TContext>;

template<class TContext>
void FiniteStateMachine<TContext>::transitionTo(FiniteStateMachine<TContext>::State& state, bool immediate)
{
    auto act = handler.getActiveState();

    handler.setNextState(&state);
    
    handler.beginTransitionQueue();

    if (act && act->is(StateStatus::Exiting) == false)
        handler.queueTransition(StateStatus::Exiting, act);

    handler.queueTransition(StateStatus::Entering, &state);
    handler.endTransitionQueue(&state);
    
    if (immediate)
        handler.execute();
}

template<class TContext>
void FiniteStateMachine<TContext>::update()
{
    handler.queueTransition(StateStatus::Updating, handler.getNextState());
    handler.execute();
}