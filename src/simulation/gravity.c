#include "gravity.h"

// Calculates gravitational force between two objects
// p1, p2: Position vectors
// m1, m2: Masses of objects
// G: Gravitational constant
// Returns: Force magnitude
double gravity_calulate_force(Vec2 p1, Vec2 p2, double m1, double m2, double G)
{
    // Calculate the vector from p2 to p1
    Vec2 r = vec2_sub(p1, p2);

    // Calculate the square of the distance between the two objects
    double distance_squared = vec2_dot(r, r);

    // Apply Newton's law of universal gravitation:
    // F = G * (m1 * m2) / r^2
    // where F is the force, G is the gravitational constant,
    // m1 and m2 are the masses, and r is the distance between the objects
    return G * m1 * m2 / distance_squared;
}