#include "include/creature.h"
#include "include/utils.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

const float PI = 3.14159f;

Creature createCreature(float radius, float initialSpeed) {
    float angle = randomFloat(0, 2 * PI);
    float x = radius * cos(angle);
    float z = radius * sin(angle);
    float y = randomFloat(-5.0f, 5.0f);

    float vx = randomFloat(-1.0f, 1.0f);
    float vy = randomFloat(-0.5f, 0.5f);
    float vz = randomFloat(-1.0f, 1.0f);

    float length = sqrt(vx * vx + vy * vy + vz * vz);
    vx /= length;
    vy /= length;
    vz /= length;

    Creature c = { x, y, z, vx, vy, vz, initialSpeed };
    return c;
}

void renderCreature(const Creature& c) {
    glPushMatrix();
    glTranslatef(c.x, c.y, c.z);
    glutSolidSphere(1.0f, 16, 16);  // render as a simple sphere
    glPopMatrix();
}