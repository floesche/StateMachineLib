#pragma once
#include "StateMachineHandler.hpp"

#ifndef FSM_TRANSITION_STACK_SIZE
#define FSM_TRANSITION_STACK_SIZE 10
#endif

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
    StateMachineHandler<TContext, FSM_TRANSITION_STACK_SIZE> handler;
};

template<class TContext>
using FSM = FiniteStateMachine<TContext>;

template<class TContext>
void FiniteStateMachine<TContext>::transitionTo(FiniteStateMachine<TContext>::State& state, bool immediate)
{
    handler.clearStagedTransitions();
    
    handler.stageTransition(&state);
    
    handler.stageTransition(StateStatus::Entering, &state);

    auto act = handler.getActiveState();

    if (act && act->is(StateStatus::Exiting) == false)
        handler.stageTransition(StateStatus::Exiting, act);

    if (immediate)
        handler.execute();
}

template<class TContext>
void FiniteStateMachine<TContext>::update()
{
    handler.execute();

    handler.stageTransition(StateStatus::Updating, handler.getCurrentState());
}