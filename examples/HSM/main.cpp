#include <Arduino.h>
#include "FooContext.hpp"

FooContext ctx;
HSM<FooContext> hsm(&ctx);

void setup() 
{
    pinMode(4, INPUT);

    ctx.hsm = &hsm;
    hsm.transitionTo(FooContext::sOff);
}

void loop() 
{
    hsm.update();
}
