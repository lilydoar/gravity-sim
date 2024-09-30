# Testing Plan

This document outlines the testing strategy for the project, focusing on both unit tests and integration tests. The goal is to ensure the correctness and robustness of the core components, as well as the overall system behavior.

## Unit Tests

Unit tests will focus on individual functions and components to ensure they work correctly in isolation. The following areas will be covered:

### Particle Selection
- **Rectangle Selection**:
  - Test selecting particles within a defined rectangle and ensure the correct particles are identified.
  - Test edge cases where the rectangle is very small or very large.
  - Test with no particles in the rectangle to ensure no false positives.

- **Circle Selection**:
  - Test selecting particles within a defined circle and ensure the correct particles are identified.
  - Test edge cases where the circle is very small or very large.
  - Test with no particles in the circle to ensure no false positives.

### Particle Modification
- **Mode Change**:
  - Test changing particle modes from dynamic to static and vice versa.
  - Test that only the intended particles are modified.
  - Test behavior when attempting to change the mode of already static particles.

### Simulation Step
- **Force Calculation**:
  - Test force calculation between two particles with known positions and masses.
  - Test with particles at the same position to ensure no division by zero errors.
  - Test with particles at maximum and minimum distances.

- **Integration**:
  - Test particle position and velocity updates over a single simulation step.
  - Test with static particles to ensure they do not move.

- **Collision Resolution**:
  - Test collision resolution between two dynamic particles.
  - Test collision resolution between a dynamic and a static particle.
  - Test with overlapping particles to ensure they are separated correctly.

## Integration Tests

Integration tests will focus on the interaction between components, especially those that involve user input and system state changes. These tests will ensure that the system behaves correctly as a whole. The integration tests will be completed in a later task using raylib automation events. The following areas will be covered:

### User Input and Interaction
- **Rectangle Selection and Action Application**:
  - Simulate user input to select a rectangle and apply an action to make particles static.
  - Verify that the correct particles are modified and the logging output is as expected.

- **Camera Movement and Zoom**:
  - Simulate user input for camera movement and zoom.
  - Verify that the camera updates correctly and the simulation view changes accordingly.

- **Simulation Initialization and Step Execution**:
  - Initialize the simulation with a set of options and verify the initial state of particles.
  - Execute several simulation steps and verify the consistency of particle states and positions.

### Logging and Debugging
- **Logging Output**:
  - Verify that logging output is correct for various actions, including truncation of long particle lists.
  - Test logging output for different levels of particle modifications (e.g., few vs. many).

This testing plan will help ensure the core components of the project are functioning correctly and that the system behaves as expected under various conditions. It will also facilitate future extensions by providing a solid foundation of tests to catch regressions and ensure new features integrate smoothly.
