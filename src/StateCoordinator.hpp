#pragma once
#include "StateBase.hpp"

enum class TransitionType : uint8_t { Enter = 1, Update = 2, Exit = 3 };

template<class TContext, uint8_t SZ>
class StateCoordinator
{
public:
    explicit StateCoordinator(TContext* context) : currentState(nullptr), activeState(nullptr), context(context), currentTransition(0), pendingTransitions(0), transitionOccurred(false), transitionBuffer{}
    {

    }

    StateBase<TContext>* getActiveState() const
    {
        return activeState;
    }

    bool isCurrentState(StateBase<TContext>& target) const
    {
        return currentState == &target;
    }

    void stageActiveTransition(TransitionType type, StateBase<TContext>* target)
    {
        stageTransition(type, activeState, target);
    }

    void stageCurrentTransition(StateBase<TContext>* target)
    {
        stageTransition(static_cast<TransitionType>(0), currentState, target);
    }

    void clearPendingTransitions()
    {
        transitionOccurred = true;
        currentTransition = pendingTransitions;
    }

    void execute();

private:
    void stageTransition(TransitionType type, StateBase<TContext>*& source, StateBase<TContext>* target);

    StateBase<TContext>* currentState;
    StateBase<TContext>* activeState;
    TContext* const context;

    uint8_t currentTransition;
    uint8_t pendingTransitions;
    bool transitionOccurred;
   
    struct
    {
        TransitionType type;
        StateBase<TContext>** source;
        StateBase<TContext>* target;
    } transitionBuffer[SZ];
};

template<class TContext, uint8_t SZ>
void StateCoordinator<TContext, SZ>::stageTransition(TransitionType type, StateBase<TContext>*& source, StateBase<TContext>* target)
{
    auto newPendingTransitions = (newPendingTransitions + 1) % SZ;

    if (newPendingTransitions != currentTransition)
    {
        transitionBuffer[pendingTransitions] = { type, &source, target };
        pendingTransitions = newPendingTransitions;
    }
}

template<class TContext, uint8_t SZ>
void StateCoordinator<TContext, SZ>::execute()
{
    transitionOccurred = false;

    while (currentTransition != pendingTransitions && transitionOccurred == false)
    {
        auto transition = transitionBuffer[currentTransition];
        currentTransition = (currentTransition + 1) % SZ;
        
        *transition.source = transition.target;

        if (*transition.source == nullptr)
            continue;

        switch (transition.type)
        {
        case TransitionType::Enter:
            (*transition.source)->enter(context);
            break;
        case TransitionType::Update:
            (*transition.source)->update(context);
            break;
        case TransitionType::Exit:
            (*transition.source)->exit(context);
            break;
        }
    }
}