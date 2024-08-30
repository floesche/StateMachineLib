#include <Arduino.h>
#include "FooContext.hpp"

static void enter(FooContext* const ctx)
{
    ctx->elapsed = millis();
}

static void update(FooContext* const ctx)
{
    if (digitalRead(4, LOW))
        ctx->fsm->transitionTo(FooContext::sOff);
    else if (millis() - elapsed >= 5000)
        ctx->fsm->transitionTo(FooContext::sRun);
}

FSM<FooContext>::State const FooContext::sCooldown(enter, update, nullptr);