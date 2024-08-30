#pragma once
#include <HierarchicalStateMachine.hpp>
#include <stdint.h>

struct FooContext
{
    HSM<FooContext>::State const sOff;
    HSM<FooContext>::State const sOnGroup;
    HSM<FooContext>::State const sRun;
    HSM<FooContext>::State const sCooldown;

    uint8_t count;
    uint8_t current;
    uint8_t elapsed;

    HSM<FooContext>* hsm;
};