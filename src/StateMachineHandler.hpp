#pragma once
#include "StateBase.hpp"

template<class TContext, uint8_t SZ>
class StateMachineHandler
{
public:
    explicit StateMachineHandler(TContext* context) : currentState(nullptr), activeState(nullptr), context(context), transitionStackTop(0), transitionStack{}
    {

    }

    StateBase<TContext>* getCurrentState() const
    {
        return currentState;
    }

    StateBase<TContext>* getActiveState() const
    {
        return activeState;
    }

    bool isInState(StateBase<TContext>& state) const
    {
        return currentState == &state;
    }

    void stageTransition(StateBase<TContext>* const state)
    {
        stageTransition(static_cast<StateStatus>(0), currentState, state);
    }

    void stageTransition(StateStatus status, StateBase<TContext>* dst)
    {
        stageTransition(status, activeState, dst);
    }

    void clearStagedTransitions()
    {
        transitionStackTop = 0;
    }

    void execute();

private:
    struct Transition
    {
        StateStatus status;
        StateBase<TContext>** src;
        StateBase<TContext>* dst;
    };

    void stageTransition(StateStatus status, StateBase<TContext>*& src, StateBase<TContext>* dst)
    {
        if (transitionStackTop >= SZ)
            return;

        transition[transitionStackTop++] = { status, &src, dst };
    }

    StateBase<TContext>* currentState;
    StateBase<TContext>* activeState;
    TContext* const context;

    uint8_t transitionStackTop;
    Transition transitionStack[SZ];
};

template<class TContext, uint8_t SZ>
void StateMachineHandler<TContext, SZ>::execute()
{
    while (transitionStackTop > 0)
    {
        auto transition = transitionStack[--transitionStackTop];
        
        *transition.src = transition.dst;

        if (*transition.src == nullptr)
            continue;

        switch (transition.status)
        {
        case StateStatus::Entering:
            (*transition.src)->enter(context);
            break;
        case StateStatus::Updating:
            (*transition.src)->update(context);
            break;
        case StateStatus::Exiting:
            (*transition.src)->exit(context);
            break;
        }
    }
}