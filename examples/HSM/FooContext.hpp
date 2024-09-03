#pragma once
#include <HierarchicalStateMachine.hpp>
#include <stdint.h>

struct FooContext
{
    static HSM<FooContext>::State sOff;
    static HSM<FooContext>::State sOnGroup;
    static HSM<FooContext>::State sRun;
    static HSM<FooContext>::State sCooldown;

    uint8_t count;
    uint8_t current;
    uint8_t elapsed;

    HSM<FooContext>* hsm;
};