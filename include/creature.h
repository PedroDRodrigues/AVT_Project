#pragma once

struct Creature {
    float x, y, z;
    float vx, vy, vz;
    float speed;
};

Creature createCreature(float radius, float initialSpeed);
void renderCreature(const Creature& c);