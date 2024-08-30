#include "FooContext.hpp"

static void update(FooContext* const ctx)
{
    if (ctx->current - ctx->elapsed >= 5000)
        ctx->hsm->transitionTo(FooContext::sRun);
}

HSM<FooContext>::State const FooContext::sCooldown(&FooContext::sOnGroup, nullptr, update, nullptr);