# TODO

1. Make a README file with instructions for building, running, etc.
1. Add an open source license.
1. Debug action types instead of action numbers.
1. Do not do selection on click. Only click and drag. Implement some minimum
   drag distance in screen space. Want to reuse click for other actions.
1. Improve color clarity. The color difference between low mass particles and
   high mass particles is not clear enough. The background needs to be darker.
   More void like.
1. User interaction: Circle area select mode. UI icon to switch between Rect
   mode and circle mode.
1. Window resizing. Set Size of window to screen size on startup. Do not enter
   fullscreen by default.
1. Fullscreen support.
1. Ability to click and drag individual(or groups) of particles to move them
   around.
1. Integration tests. Make use of Raylibs event automation capabilities to write
   integration tests that simulate a user performing a specific action or
   actions. Use these tests to check for regressions.
1. Improve selection controls. Do not apply action to selected particles
   immediately. Hold shift: Add to selected particles. Hold ctrl: Remove from
   selected particles. press button to apply action(make static, delete, etc).
1. Extend action system to be useful for startup initialization. Setup commands
   for example. Init empty simulation, configure initial conditions through a
   sequence of actions, then start the simulation.
1. Add action types to spawn new particles. One action should spawn a single
   high mass particle where the user inputs the size by dragging out a particles
   radius and the mass is proportional to the radius. One action should spawn
   clouds of particles within a region. Set Size distribution, mass
   distribution, and spawn enough particles to "fill" the cursor region.
1. Simulation start/stop support. This only plays or pauses the simulation step
   function. The simulation can still be interacted with when it is stopped.
1. Decouple the simulation update speed from the Raylib app loop. This is not
   moving the simulation to a separate thread. This is constructing the fixed
   loop update of the sim such that it updates within the app loop at the
   configured rate(or as close as performance allows).
1. Live configuration of the sim. This is not hot reloading. This is adjusting
   parameters of the sim during exection time. These are values like the
   gravitational constant, physics steps, collision steps, etc.
1. UI code hot reloading. The sim will not be hot reloaded. Hot reload the UI
   and user interactions code. This will improve development experience
   tremendously and unlock the future creation of an editor.
1. Element system. Correlate density and size of different "materials". This
   will correlate size and mass through material density. Draw different
   materials different colors. This will unlock classifiable behavior based on
   different combinations of material types.
