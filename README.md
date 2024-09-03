# StateMachine
A State Machine library for Arduino based on [rppelayo's finite state machine library](https://github.com/rppelayo/FiniteStateMachine?tab=readme-ov-file). Provides usage of both Finite State Machines (FSM) and Hierarchical State Machines (HSM).

see examples on how to use the library.

### Configuration
- `FSM_TRANSITION_STACK_SIZE`: controls how many transitions can be staged on the FSM. Default is 10.
- `HSM_TRANSITION_STACK_SIZE`: controls how many transitions can be staged on the HSM. Default is 30.
- `HSM_STACK_DEPTH`: sets the stack depth for the HSM (required for traversing states in top-down manner). Default is 10.
- Always call transitionTo at `begin` function to initialize the entry point of your state machine.

### Limitations
- no support for shallow/deep history states
  - workaround is to add your own history state in a user-defined context and manually assign it on each state's enter callbacks.
- unwanted callback execution if immediate transition is called in exit/enter callbacks
  - consider using immediate transition on update callbacks only (for now)

### TODO
- stop `handleTransition` if one of the callbacks on exit/enter calls an immediate transition
