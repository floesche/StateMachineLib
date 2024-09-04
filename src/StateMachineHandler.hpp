#pragma once
#include "StateBase.hpp"

template<class TContext, uint8_t SZ>
class StateMachineHandler
{
public:
    explicit StateMachineHandler(TContext* context) : currentState(nullptr), activeState(nullptr), nextState(nullptr), context(context), queueHead(0), queueTail(0), transitionOccurred(0), transitionQueue{}
    {

    }

    StateBase<TContext>* getNextState() const
    {
        return nextState;
    }

    StateBase<TContext>* getActiveState() const
    {
        return activeState;
    }

    bool isInState(StateBase<TContext>& state) const
    {
        return currentState == &state;
    }

    void setNextState(StateBase<TContext>* state)
    {
        nextState = state;
    }

    void queueTransition(StateStatus status, StateBase<TContext>* dst)
    {
        queueTransition(status, activeState, dst);
    }

    void endTransitionQueue(StateBase<TContext>* const state)
    {
        queueTransition(static_cast<StateStatus>(0), currentState, state);
    }

    void beginTransitionQueue()
    {
        transitionOccurred = true;
        queueHead = queueTail;
    }

    void execute();

private:
    struct Transition
    {
        StateStatus status;
        StateBase<TContext>** src;
        StateBase<TContext>* dst;
    };

    void queueTransition(StateStatus status, StateBase<TContext>*& src, StateBase<TContext>* dst);

    StateBase<TContext>* currentState;
    StateBase<TContext>* activeState;
    StateBase<TContext>* nextState;
    TContext* const context;

    uint8_t queueHead;
    uint8_t queueTail;
    bool transitionOccurred;
    Transition transitionQueue[SZ];
};

template<class TContext, uint8_t SZ>
void StateMachineHandler<TContext, SZ>::queueTransition(StateStatus status, StateBase<TContext>*& src, StateBase<TContext>* dst)
{
    auto nextQueueTail = (queueTail + 1) % SZ;

    if (nexQueueTail != queueHead)
    {
        transitionQueue[queueTail] = { status, &src, dst };
        queueTail = nextQueueTail;
    }
}

template<class TContext, uint8_t SZ>
void StateMachineHandler<TContext, SZ>::execute()
{
    transitionOccurred = false;

    while (queueHead != queueTail && transitionOccurred == false)
    {
        auto transition = transitionQueue[queueHead];
        queueHead = (queueHead + 1) % SZ;
        
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