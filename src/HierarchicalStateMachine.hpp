#pragma once
#include <stdint.h>
#include "StateMachineHandler.hpp"

#ifndef MAX_STACK_DEPTH
#define MAX_STACK_DEPTH 10
#endif

template<class TContext>
class HierarchicalStateMachine
{
public:
    struct State : StateBase<TContext>
    {
        State const* const parent;

        constexpr State(State const* parent, StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : StateBase<TContext>(enter, update, exit), parent(parent)
        {

        }
    };

    explicit HierarchicalStateMachine(TContext* context) : handler(context)
    {

    }


    bool isInState(State const& state) const
    {
        return handler.isInState(state);
    }

    void transitionTo(State const& state, bool force = false, bool immediate = false);
    void update();

private:
    static State const* findLeastCommonAncestorState(State const* const s1, State const* const s2);
    bool executeTopDown(State const* state, State const* ancestor, StateAction action, bool applyStateChange = false);
    bool executeBottomUp(State const* state, State const* ancestor, StateAction action, bool applyStateChange = false);
    void handleTransition();

    StateMachineHandler<TContext> handler;
};

template<class TContext>
using HSM = HierarchicalStateMachine<TContext>;

template<class TContext>
typename HierarchicalStateMachine<TContext>::State const* HierarchicalStateMachine<TContext>::findLeastCommonAncestorState(HierarchicalStateMachine<TContext>::State const* const s1, HierarchicalStateMachine<TContext>::State const* const s2)
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
bool HierarchicalStateMachine<TContext>::executeTopDown(HierarchicalStateMachine<TContext>::State const* state, HierarchicalStateMachine<TContext>::State const* ancestor, StateAction action, bool applyStateChange)
{
    if (state == nullptr)
        return false;

    HierarchicalStateMachine<TContext>::State const* stack[MAX_STACK_DEPTH];
    uint8_t stackTop = 0U;

    while (state && state != ancestor && stackTop < MAX_STACK_DEPTH)
    {
        stack[stackTop++] = state;
        state = state->parent;
    }

    while (stackTop > 0)
        if (handler.execute(stack[--stackTop], action, applyStateChange))
            break;

    return handler.isTransitioning();
}

template<class TContext>
bool HierarchicalStateMachine<TContext>::executeBottomUp(HierarchicalStateMachine<TContext>::State const* state, HierarchicalStateMachine<TContext>::State const* ancestor, StateAction action, bool applyStateChange)
{
    if (state == nullptr)
        return false;

    while (state && state != ancestor)
    {
        if (handler.execute(state, action, applyStateChange))
            break;

        state = state->parent;
    }

    return handler.isTransitioning();
}

template<class TContext>
void HierarchicalStateMachine<TContext>::handleTransition()
{
    while (handler.isTransitioning(true))
    {
        auto ccs = static_cast<HierarchicalStateMachine<TContext>::State const*>(handler.getCurrentState());
        auto ncs = static_cast<HierarchicalStateMachine<TContext>::State const*>(handler.getNextState());

        auto lca = findLeastCommonAncestorState(ccs, ncs);

        if (executeBottomUp(ccs, lca, StateAction::Exit, true))
            continue;

        if (executeTopDown(ncs, lca, StateAction::Enter, true))
            continue;

        handler.completeTransition();
    }
}

template<class TContext>
void HierarchicalStateMachine<TContext>::transitionTo(HierarchicalStateMachine<TContext>::State const& state, bool force, bool immediate)
{
    if (handler.trySetNextState(state, force) && immediate)
        handleTransition();
}

template<class TContext>
void HierarchicalStateMachine<TContext>::update()
{
    handleTransition();

    executeTopDown(static_cast<HierarchicalStateMachine<TContext>::State const*>(handler.getCurrentState()), nullptr, StateAction::Update);
}