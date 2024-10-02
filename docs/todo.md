# TODO

## Documentation

- **Essential**
  - Make a README file with instructions for building, running, etc.
  - Add an open source license.

## User Interaction

- **Core**
  - Do not do selection on click. Only click and drag. Implement some minimum drag distance in screen space. Want to reuse click for other actions.
  - Ability to click and drag individual (or groups) of particles to move them around.
  - Simulation start/stop support. This only plays or pauses the simulation step function. The simulation can still be interacted with when it is stopped.
- **Enhancements**
  - User interaction: Circle area select mode. UI icon to switch between Rect mode and circle mode.
  - Add action types to spawn new particles. One action should spawn a single high mass particle where the user inputs the size by dragging out a particle's radius and the mass is proportional to the radius. One action should spawn clouds of particles within a region. Set size distribution, mass distribution, and spawn enough particles to "fill" the cursor region.
- **Optimizations**
  - UI code hot reloading. The sim will not be hot reloaded. Hot reload the UI and user interactions code. This will improve development experience tremendously and unlock the future creation of an editor.

## Visuals

- **Core**
  - Improve color clarity. The color difference between low mass particles and high mass particles is not clear enough. The background needs to be darker. More void-like.
- **General**
  - Element system. Correlate density and size of different "materials". This will correlate size and mass through material density. Draw different materials different colors. This will unlock classifiable behavior based on different combinations of material types.

## Window Management

- **Core**
  - Window resizing. Set size of window to screen size on startup. Do not enter fullscreen by default.
- **General**
  - Fullscreen support.

## Simulation

- **Core**
  - Decouple the simulation update speed from the Raylib app loop. This is not moving the simulation to a separate thread. This is constructing the fixed loop update of the sim such that it updates within the app loop at the configured rate (or as close as performance allows).
- **General**
  - Live configuration of the sim. This is not hot reloading. This is adjusting parameters of the sim during execution time. These are values like the gravitational constant, physics steps, collision steps, etc.
- **Polish**
  - Extend action system to be useful for startup initialization. Setup commands for example. Init empty simulation, configure initial conditions through a sequence of actions, then start the simulation.

## Testing

- **Core**
  - Integration tests. Make use of Raylib's event automation capabilities to write integration tests that simulate a user performing a specific action or actions. Use these tests to check for regressions.

## Documentation

- **Core**
  - Create a comprehensive guide for setting up the development environment, including dependencies and build instructions.
- **Enhancements**
  - Document the `src/verlet.c` module, focusing on the `compute_velocity` function, to explain its role in the simulation.

## User Interaction

- **Enhancements**
  - Implement a "gravity well" tool that allows users to create temporary gravitational fields by clicking and holding on the simulation space.
  - Implement keyboard shortcuts for common actions like starting/stopping the simulation and switching selection modes.

## Visuals

- **Core**
  - Develop a particle collision visualization feature that highlights particles upon collision.
- **Optimizations**
  - Add a color gradient legend to the UI that explains the color coding of particles based on their mass or velocity.
  - Add particle trails to visually represent the path of particles over time.

## Window Management

- **Core**
  - Ensure cross-platform compatibility by testing and fixing any window management issues on different operating systems.
- **Enhancements**
  - Develop a responsive UI layout that adapts to different screen sizes and orientations.
  - Implement a settings menu to allow users to configure window size and fullscreen mode preferences.

## Simulation

- **Core**
  - Refactor the simulation loop to improve efficiency and scalability.
- **Enhancements**
  - Introduce a "time warp" feature that allows users to speed up or slow down the simulation.
  - Introduce a feature to save and load simulation states, allowing users to pause and resume their work.

## Testing

- **Enhancements**
  - Create a test suite for the `src/logging.c` module to verify that log levels are set correctly.
  - Develop unit tests for the particle interaction logic to ensure accuracy and prevent regressions.
