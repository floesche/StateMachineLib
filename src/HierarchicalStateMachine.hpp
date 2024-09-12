#pragma once
#include "StateCoordinator.hpp"

namespace HierarchicalSystem
{
    template<class TContext>
    struct State : StateBase<TContext>
    {
        State* const parent;

        State(State* parent, StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : StateBase<TContext>(enter, update, exit), parent(parent)
        {

        }
    };

    template<class TContext, uint8_t depth>
    class StateMachine
    {
    public:
        explicit StateMachine(TContext* context, bool handleAncestorReentry = false) : handleAncestorReentry(handleAncestorReentry), targetState(nullptr), coordinator(context)
        {

        }

        bool isCurrentState(State<TContext>& target) const
        {
            return coordinator.isCurrentState(target);
        }

        void update();
        void transitionTo(State<TContext>& target, bool immediate = false);

    private:
        static State<TContext>* findLeastCommonAncestorState(State<TContext>* const s1, State<TContext>* const s2);

        void stageTransitionsTopDown(State<TContext>* state, State<TContext>* const ancestor, TransitionType type);
        void stageTransitionsBottomUp(State<TContext>* state, State<TContext>* const ancestor, TransitionType type);

        bool const handleAncestorReentry;
        State<TContext>* targetState;
        StateCoordinator<TContext, (depth * 3) + 1> coordinator;
    };
}

template<class TContext>
using HState = HierarchicalSystem::State<TContext>;

template<class TContext, uint8_t depth>
using HSM = HierarchicalSystem::StateMachine<TContext, depth>;

template<class TContext, uint8_t depth>
HierarchicalSystem::State<TContext>* HierarchicalSystem::StateMachine<TContext, depth>::findLeastCommonAncestorState(HierarchicalSystem::State<TContext>* const s1, HierarchicalSystem::State<TContext>* const s2)
{
    if (s1 == nullptr || s2 == nullptr)
        return nullptr;

    if (s1 == s2)
        return s1->parent;

    if (s1->parent == s2)
        return s2;

    if (s2->parent == s1)
        return s1;

    auto c1 = s1, c2 = s2;

    while (c1 != c2)
    {
        c1 = c1 ? c1->parent : s2;
        c2 = c2 ? c2->parent : s1;
    }

    return c1;
}

template<class TContext, uint8_t depth>
void HierarchicalSystem::StateMachine<TContext, depth>::stageTransitionsTopDown(HierarchicalSystem::State<TContext>* state, HierarchicalSystem::State<TContext>* const ancestor, TransitionType type)
{
    static HierarchicalSystem::State<TContext>* stateStack[depth] = {};
    uint8_t stateStackTop = 0;

    while (state && state != ancestor && stateStackTop < depth)
    {
        stateStack[stateStackTop++] = state;
        state = state->parent;
    }

    while (stateStackTop > 0)
    {
        state = stateStack[--stateStackTop];
        coordinator.stageActiveTransition(status, state);
    }
}

template<class TContext, uint8_t depth>
void HierarchicalSystem::StateMachine<TContext, depth>::stageTransitionsBottomUp(HierarchicalSystem::State<TContext>* state, HierarchicalSystem::State<TContext>* const ancestor, TransitionType type)
{
    while (state && state != ancestor)
    {
        coordinator.stageActiveTransition(type, state);
        state = state->parent;
    }
}

template<class TContext, uint8_t depth>
void HierarchicalSystem::StateMachine<TContext, depth>::transitionTo(HierarchicalSystem::State<TContext>& target, bool immediate)
{
    targetState = &target;

    auto activeState = static_cast<HierarchicalSystem::State<TContext>*>(coordinator.getActiveState());
    auto leastCommonAncestor = findLeastCommonAncestorState(activeState, targetState);

    if (handleAncestorReentry && leastCommonAncestor == targetState)
        leastCommonAncestor = leastCommonAncestor->parent;

    if (activeState && activeState->is(StateStatus::Exiting))
        activeState = activeState->parent;

    coordinator.clearPendingTransitions();
    stageTransitionsTopDown(activeState, leastCommonAncestor, StateStatus::Exiting);

    coordinator.stageCurrentTransition(targetState);

    stageTransitionsBottomUp(targetState, leastCommonAncestor, StateStatus::Entering);

    if (immediate)
        coordinator.execute();
}

template<class TContext, uint8_t depth>
void HierarchicalSystem::StateMachine<TContext, depth>::update()
{
    stageTransitionsTopDown(targetState, nullptr, TransitionType::Update);
    
    coordinator.execute();
}