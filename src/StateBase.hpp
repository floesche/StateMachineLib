#pragma once

enum struct StateAction { Enter, Update, Exit };

template<class TContext>
using StateCallback = void (*)(TContext* const);

template<class TContext>
struct StateBase
{
public:
    void execute(StateAction action, TContext* const context) const
    {
        switch (action)
        {
            case StateAction::Enter: 
            {
                if (enterCallback)
                    enterCallback(context);

                break;
            }

            case StateAction::Update:
            {
                if (updateCallback)
                    updateCallback(context);

                break;
            }

            case StateAction::Exit:
            {
                if (exitCallback)
                    exitCallback(context);

                break;
            }
        }
    }

protected:
    constexpr StateBase(StateCallback<TContext> enter, StateCallback<TContext> update, StateCallback<TContext> exit) : enterCallback(enter), updateCallback(update), exitCallback(exit)
    {

    }

private:
    StateCallback<TContext> const enterCallback;
    StateCallback<TContext> const updateCallback;
    StateCallback<TContext> const exitCallback;
};