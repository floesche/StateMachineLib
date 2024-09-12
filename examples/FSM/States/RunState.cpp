#include <Arduino.h>
#include "FooContext.hpp"

static void enter(FooContext* const ctx)
{
    ctx->elapsed = millis();
}

static void update(FooContext* const ctx)
{
    auto now = millis();

    if (now - ctx->elapsed >= 500)
    {
        ctx->elapsed = now;
        ctx->count++;
    }

    if (digitalRead(4) == LOW)
        ctx->fsm->transitionTo(FooContext::sOff);
    else if (((ctx->count + 1) % 6) == 0)
        ctx->fsm->transitionTo(FooContext::sCooldown);
}

FState<FooContext> FooContext::sRun(enter, update, nullptr);