#include <Arduino.h>
#include "FooContext.hpp"

FooContext ctx;
FSM<FooContext> fsm(&ctx);

void setup() 
{
    pinMode(4, INPUT);
    
    ctx.fsm = &fsm;
    fsm.transitionTo(FooContext::sOff);

}

void loop() 
{
    fsm.update();
}
