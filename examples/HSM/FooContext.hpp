#pragma once
#include <HierarchicalStateMachine.hpp>
#include <stdint.h>

struct FooContext
{
    static HState<FooContext> sOff;
    static HState<FooContext> sOnGroup;
    static HState<FooContext> sRun;
    static HState<FooContext> sCooldown;

    uint8_t count;
    unsigned long current;
    unsigned long elapsed;

    HSM<FooContext, 5>* hsm;
};
