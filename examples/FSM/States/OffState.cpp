#include <Arduino.h>
#include "FooContext.hpp"

static void enter(FooContext* const ctx)
{
    ctx->count = 0;
}

static void update(FooContext* const ctx)
{
    if (digitalRead(4) == HIGH)
        ctx->fsm->transitionTo(FooContext::sRun);
}

FSM<FooContext>::State FooContext::sOff(enter, update, nullptr);