#pragma once
#include <FiniteStateMachine.hpp>
#include <stdint.h>

struct FooContext
{
    static FState<FooContext> sOff;
    static FState<FooContext> sRun;
    static FState<FooContext> sCooldown;

    uint8_t count;
    unsigned long elapsed;

    FSM<FooContext>* fsm;
};
