#ifndef VECS_BENCH_H_
#define VECS_BENCH_H_

#include <iostream>
#include <chrono>
#include "src/vecs.h"

using namespace vecs;
using namespace std::chrono;

// Define some components
class Position : public Component<Position> {
public:
    int x,y;

    Position()
      : x(0), y(0) {}

    ComponentBase* clone() {
        Position* dup (new Position());
        dup->x = x;
        dup->y = y;
        return dup;
    }
};

class Health : public Component<Health> {
public:
    int hitpoints, stamina;

    Health()
      : hitpoints(100), stamina(100) {}

    ComponentBase* clone() {
        Health* dup (new Health());
        dup->hitpoints = hitpoints;
        dup->stamina = stamina;
        return dup;
    }
};

class Movement : public Component<Movement> {
public:
    int dx, dy;

    Movement()
      : dx(2), dy(3) {}

    ComponentBase* clone() {
        Movement* dup ( new Movement() );
        dup->dx = dx;
        dup->dy = dy;
        return dup;
    }
};

// Define some systems
class MovementSystem : public System {
public:
    MovementSystem() {
        //
    }

    virtual ~MovementSystem() {
        //
    }

    void update() {
        std::cout << "MovementSystem::update call..." << std::endl;
        for (auto eiter = entities.begin(); eiter != entities.end(); ++eiter) {
            Entity* en = eiter->second;
            //
            Movement* m = en->getComponent<Movement>(Movement::typeId());
            Position* p = en->getComponent<Position>(Position::typeId());
            int ox = p->x;
            int oy = p->y;
            p->x += m->dx;
            p->y += m->dy;
            std::cout << "MovementSystem::update (Ent #" << en->getId() << ")>> " << ox << "," << oy << " to " << p->x << "," << p->y << std::endl;
        }
    }
};

void entity_bench() {
    // Manager first
    Domain* d0 (new Domain());

    // Create systems
    MovementSystem* s0 (new MovementSystem());
    s0->requireType(Position::typeId());
    s0->requireType(Movement::typeId());

    // Add systems
    d0->addSystem(s0);

    const int bench_size = 10000000;
    Entity* e;
    Position* p;
    Health* h;

    // Create entities
    std::cout << "Creating " << bench_size << " entities." << std::endl;

    high_resolution_clock::time_point b1 = high_resolution_clock::now();

    for (int i=0; i < bench_size; ++i) {
        d0->createEntity();
    }

    high_resolution_clock::time_point b2 = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(b2 - b1);
    std::cout << "Done: " << time_span.count() << " seconds." << std::endl;


    // Destroy entities
    std::vector<Entity*> ents = d0->getEntities();
    std::cout << "Destroying " << ents.size() << " entities." << std::endl;
    b1 = high_resolution_clock::now();

    for (auto ent : ents) {
        d0->destroyEntity(ent);
    }

    b2 = high_resolution_clock::now();

    time_span = duration_cast<duration<double>>(b2 - b1);
    std::cout << "Done: " << time_span.count() << " seconds." << std::endl;

    // Create entities with two components each
    std::cout << "Re-creating " << bench_size << " entities and adding two components to each." << std::endl;

    b1 = high_resolution_clock::now();

    for (int i=0; i < bench_size; ++i) {
        e = d0->createEntity();
        p = d0->createComponent<Position>();
        e->addComponent(p);
        h = d0->createComponent<Health>();
        e->addComponent(h);
    }

    b2 = high_resolution_clock::now();

    time_span = duration_cast<duration<double>>(b2 - b1);
    std::cout << "Done: " << time_span.count() << " seconds." << std::endl;

    // Loop over entities, getting one component
    Position* pos = nullptr;
    Health* hth = nullptr;
    ents = d0->getEntities();
    std::cout << "Looping over " << bench_size << " entities, getting 1 component." << std::endl;
    b1 = high_resolution_clock::now();

    for (auto ent : ents) {
        pos = ent->getComponent<Position>( Position::typeId() );
    }

    b2 = high_resolution_clock::now();

    time_span = duration_cast<duration<double>>(b2 - b1);
    std::cout << "Done: " << time_span.count() << " seconds." << std::endl;

    // Loop over enetities, getting two components
    std::cout << "Looping over " << bench_size << " entities, getting 2 component." << std::endl;
    b1 = high_resolution_clock::now();

    for (auto ent : ents) {
        pos = ent->getComponent<Position>( Position::typeId() );
        hth = ent->getComponent<Health>( Health::typeId() );
    }

    b2 = high_resolution_clock::now();

    time_span = duration_cast<duration<double>>(b2 - b1);
    std::cout << "Done: " << time_span.count() << " seconds." << std::endl;

}

int main() {
    entity_bench();
    exit(0);
}

#endif // VECS_BENCH_H_
