#pragma once
#include <stdint.h>

enum struct StateStatus : uint8_t { Standby, Entering, Updating, Exiting };

template<class TContext>
using StateCallback = void (*)(TContext* const);

template<class TContext>
struct StateBase
{
public:
    bool is(StateStatus status) const
    {
        return this->status == status;
    }

    void enter(TContext* const context) const
    {
        status = StateStatus::Entering;

        if (enterCallback)
            enterCallback(context);

        status = StateStatus::Updating;
    }

    void update(TContext* const context) const
    {
        if (updateCallback)
            updateCallback(context);
    }

    void exit(TContext* const context) const
    {
        status = StateStatus::Exiting;

        if (exitCallback)
            exitCallback(context);

        status = StateStatus::Standby;
    }

protected:
    StateBase(StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : status(static_cast<StateStatus>(0)), enterCallback(enter), updateCallback(update), exitCallback(exit)
    {

    }

private:
    mutable StateStatus status;

    StateCallback<TContext> const enterCallback;
    StateCallback<TContext> const updateCallback;
    StateCallback<TContext> const exitCallback;
};