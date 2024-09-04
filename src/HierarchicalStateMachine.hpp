#pragma once
#include <stdint.h>
#include "StateMachineHandler.hpp"

#ifndef HSM_STACK_DEPTH
#define HSM_STACK_DEPTH 10
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
    static constexpr uint8_t QUEUE_SIZE = (HSM_STACK_DEPTH * 3) + 1;

    static State* findLeastCommonAncestorState(State* const s1, State* const s2);
    void queueTransitionTopDown(State* state, State* ancestor, StateStatus status);
    void queueTransitionBottomUp(State* state, State* ancestor, StateStatus status);

    StateMachineHandler<TContext, QUEUE_SIZE + 1> handler;
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
void HierarchicalStateMachine<TContext>::queueTransitionTopDown(HierarchicalStateMachine<TContext>::State* state, HierarchicalStateMachine<TContext>::State* ancestor, StateStatus status)
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
        handler.queueTransition(status, stateStack[--stateStackTop]);
    }
}

template<class TContext>
void HierarchicalStateMachine<TContext>::queueTransitionBottomUp(HierarchicalStateMachine<TContext>::State* state, HierarchicalStateMachine<TContext>::State* ancestor, StateStatus status)
{
    while (state && state != ancestor)
    {
        handler.queueTransition(status, state);
        state = state->parent;
    }
}

template<class TContext>
void HierarchicalStateMachine<TContext>::transitionTo(HierarchicalStateMachine<TContext>::State& state, bool immediate)
{
    auto act = static_cast<HierarchicalStateMachine<TContext>::State*>(handler.getActiveState());
    auto lca = findLeastCommonAncestorState(act, &state);

    handler.setNextState(&state);
    handler.beginTransitionQueue();
    
    queueTransitionTopDown(act && act->is(StateStatus::Exiting) ? act->parent : act, lca, StateStatus::Exiting);
    queueTransitionBottomUp(&state, lca, StateStatus::Entering);

    handler.endTransitionQueue(&state);

    if (immediate)
        handler.execute();
}

template<class TContext>
void HierarchicalStateMachine<TContext>::update()
{
    queueTransitionTopDown(static_cast<HierarchicalStateMachine<TContext>::State*>(handler.getNextState()), nullptr, StateStatus::Updating);
    
    handler.execute();
}