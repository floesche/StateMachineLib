#pragma once
#include "StateBase.hpp"

template<class TContext>
class StateMachineHandler
{
public:
    explicit StateMachineHandler(TContext* context) : currState(nullptr), nextState(nullptr), context(context), transitionOccurred(false)
    {

    }

    void completeTransition()
    {
        currState = nextState;
        nextState = nullptr;
    }

    bool trySetNextState(StateBase<TContext> const& state, bool force)
    {
        transitionOccurred = force || nextState == nullptr || nextState != &state;

        if (transitionOccurred)
            nextState = &state;

        return transitionOccurred;
    }

    bool isInState(StateBase<TContext> const& state) const
    {
        return currState == &state;
    }

    bool isTransitioning(bool reset = false)
    {
        auto result = transitionOccurred;

        if (result && reset)
            transitionOccurred = false;

        return result;
    }

    StateBase<TContext> const* getCurrentState() const
    {
        return currState;
    }

    StateBase<TContext> const* getNextState() const
    {
        return nextState;
    }

    bool execute(StateBase<TContext> const* state, StateAction action, bool applyStateChange = false);

private:
    StateBase<TContext> const* currState;
    StateBase<TContext> const* nextState;
    TContext* const context;
    bool transitionOccurred;
};

template<class TContext>
bool StateMachineHandler<TContext>::execute(StateBase<TContext> const* state, StateAction action, bool applyStateChange)
{
    if (state == nullptr)
        return false;
    
    state->execute(action, context);

    if (transitionOccurred && applyStateChange)
        currState = state;
    
    return transitionOccurred;
}