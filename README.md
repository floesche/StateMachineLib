# StateMachine
A State Machine library for Arduino based on [rppelayo's finite state machine library](https://github.com/rppelayo/FiniteStateMachine?tab=readme-ov-file). Provides usage of both Finite State Machines (FSM) and Hierarchical State Machines (HSM).

### Configuration
- `MAX_STACK_DEPTH`: sets the maximum stack depth for the HSM (required for traversing states from top-down manner). Default is 10.
- Always call transitionTo at `begin` function to initialize the entry point of your state machine.

### Limitations
- no support for shallow/deep history states
  - workaround is to add your own history state in a user-defined context and manually assign it on each state's enter callbacks.
