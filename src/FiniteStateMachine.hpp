#pragma once
#include "StateCoordinator.hpp"

namespace FiniteSystem
{
    template <class TContext>
    struct State : StateBase<TContext>
    {
        State(StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : StateBase<TContext>(enter, update, exit)
        {

        }
    };

    template<class TContext>
    class StateMachine
    {
    public:
        explicit StateMachine(TContext* context) : targetState(nullptr), coordinator(context)
        {

        }

        bool isCurrentState(State<TContext>& target) const
        {
            return coordinator.isCurrentState(target);
        }

        void update();
        void transitionTo(State<TContext>& target, bool immediate = false);
    
    private:
        State<TContext>* targetState;
        StateCoordinator<TContext, 4> coordinator;
    };
}

template<class TContext>
using FState = FiniteSystem::State<TContext>;

template<class TContext>
using FSM = FiniteSystem::StateMachine<TContext>;

template<class TContext>
void FiniteSystem::StateMachine<TContext>::transitionTo(FiniteSystem::State<TContext>& target, bool immediate)
{
    targetState = &target;

    auto activeState = coordinator.getActiveState();
    
    coordinator.clearPendingTransitions();
    
    if (activeState && activeState->is(StateStatus::Exiting) == false)
        coordinator.stageActiveTransition(TransitionType::Exit, activeState);

    coordinator.stageCurrentTransition(targetState);
    coordinator.stageActiveTransition(TransitionType::Enter, targetState);

    if (immediate)
        coordinator.execute();
}

template<class TContext>
void FiniteSystem::StateMachine<TContext>::update()
{
    coordinator.stageActiveTransition(TransitionType::Update, targetState);
    
    coordinator.execute();
}