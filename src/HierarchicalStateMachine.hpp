#pragma once
#include <stdint.h>
#include "StateMachineHandler.hpp"

#ifndef HSM_STACK_DEPTH
#define HSM_STACK_DEPTH 10
#endif

#ifndef HSM_TRANSITION_STACK_SIZE
#define HSM_TRANSITION_STACK_SIZE (STACK_DEPTH * 3)
#endif

template<class TContext>
class HierarchicalStateMachine
{
public:
    struct State : StateBase<TContext>
    {
        State* const parent;

        constexpr State(State* parent, StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : StateBase<TContext>(enter, update, exit), parent(parent)
        {

        }
    };

    explicit HierarchicalStateMachine(TContext* context) : handler(context)
    {

    }


    bool isInState(State& state) const
    {
        return handler.isInState(state);
    }

    void update();
    void transitionTo(State& state, bool immediate = false);

private:
    static State* findLeastCommonAncestorState(State* const s1, State* const s2);
    void stageTransitionTopDown(State* state, State* const ancestor, StateStatus status);
    void stageTransitionBottomUp(State* state, State* const ancestor, StateStatus status);

    StateMachineHandler<TContext, HSM_TRANSITION_STACK_SIZE> handler;
};

template<class TContext>
using HSM = HierarchicalStateMachine<TContext>;

template<class TContext>
typename HierarchicalStateMachine<TContext>::State* HierarchicalStateMachine<TContext>::findLeastCommonAncestorState(HierarchicalStateMachine<TContext>::State* const s1, HierarchicalStateMachine<TContext>::State* const s2)
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

template<class TContext>
void HierarchicalStateMachine<TContext>::stageTransitionTopDown(HierarchicalStateMachine<TContext>::State* state, HierarchicalStateMachine<TContext>::State* const ancestor, StateStatus status)
{
    State* stateStack[HSM_STACK_DEPTH];
    uint8_t stateStackTop = 0;

    while (state && state != ancestor && stateStackTop < HSM_STACK_DEPTH)
    {
        stateStack[stateStackTop++] = state;
        state = state->parent;
    }

    while (stateStackTop > 0)
    {
        handler.stageTransition(status, stateStack[--stateStackTop]);
    }
}

template<class TContext>
void HierarchicalStateMachine<TContext>::stageTransitionBottomUp(HierarchicalStateMachine<TContext>::State* state, HierarchicalStateMachine<TContext>::State* const ancestor, StateStatus status)
{
    while (state && state != ancestor)
    {
        handler.stageTransition(status, state);
        state = state->parent;
    }
}

template<class TContext>
void HierarchicalStateMachine<TContext>::transitionTo(HierarchicalStateMachine<TContext>::State& state, bool immediate)
{
    handler.clearStagedTransitions();
    
    handler.stageTransition(&state);

    auto act = static_cast<HierarchicalStateMachine<TContext>::State*>(handler.getActiveState());
    auto lca = findLeastCommonAncestorState(act, &state);

    stageTransitionBottomUp(&state, lca, StateStatus::Entering);
    stageTransitionTopDown(act && act->is(StateStatus::Exiting) ? act->parent : act, lca, StateStatus::Exiting);

    if (immediate)
        handler.execute();
}

template<class TContext>
void HierarchicalStateMachine<TContext>::update()
{
    handler.execute();

    stageTransitionBottomUp(static_cast<HierarchicalStateMachine<TContext>::State*>(handler.getCurrentState()), nullptr, StateStatus::Updating);
}