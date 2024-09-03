#pragma once
#include <FiniteStateMachine.hpp>
#include <stdint.h>

struct FooContext
{
    static FSM<FooContext>::State sOff;
    static FSM<FooContext>::State sRun;
    static FSM<FooContext>::State sCooldown;

    uint8_t count;
    uint8_t elapsed;

    FSM<FooContext>* fsm;
};