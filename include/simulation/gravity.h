#ifndef GRAVITY_H
#define GRAVITY_H

#include "math/vector.h"

// Gravitational constant (G) in N⋅m^2/kg^2
#define GRAVITATIONAL_CONSTANT 6.67430e-11

// Calculates gravitational force between two objects
// p1, p2: Position vectors
// m1, m2: Masses of objects
// G: Gravitational constant
// Returns: Force magnitude
double calculate_force(Vec2 p1, Vec2 p2, double m1, double m2, double G);

#endif // GRAVITY_H