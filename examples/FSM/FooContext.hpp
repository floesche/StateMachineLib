#pragma once
#include <FiniteStateMachine.hpp>
#include <stdint.h>

struct FooContext
{
    static FSM<FooContext>::State const sOff;
    static FSM<FooContext>::State const sRun;
    static FSM<FooContext>::State const sCooldown;

    uint8_t count;
    uint8_t elapsed;

    FSM<FooContext>* fsm;
};