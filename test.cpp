#ifndef VECS_TEST_H_
#define VECS_TEST_H_

#include <cassert>
#include <cstdint>
#include <iostream>
#include "src/vecs.h"

using namespace vecs;

// Define some components
class Position : public Component<Position> {
public:
    int x,y;

    Position()
      : x(0), y(0) {}

    ComponentBase* clone() {
        Position* dup ( getParentEntity()->getDomain()->createComponent<Position>() );
        dup->x = x;
        dup->y = y;
        return dup;
    };
};

class Health : public Component<Health> {
public:
    int hitpoints, stamina;

    Health(int hitpoints=100, int stamina=100)
      : hitpoints(hitpoints), stamina(stamina) {}

    ComponentBase* clone() {
        Health* dup ( getParentEntity()->getDomain()->createComponent<Health>() );
        dup->hitpoints = hitpoints;
        dup->stamina = stamina;
        return dup;
    };
};

class Movement : public Component<Movement> {
public:
    int dx, dy;

    Movement()
      : dx(2), dy(3) {}

    ComponentBase* clone() {
        Movement* dup ( getParentEntity()->getDomain()->createComponent<Movement>() );
        dup->dx = dx;
        dup->dy = dy;
        return dup;
    };
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


void entity_test() {
    // Domain first
    Domain* d0 (new Domain());

    // Create systems
    MovementSystem* s0 (new MovementSystem());
    s0->requireType(Position::typeId());
    s0->requireType(Movement::typeId());

    // Add systems
    d0->addSystem(s0);

    // Create some components
    Health* h0 = d0->createComponent<Health>(101,102);
    Health* h1 = d0->createComponent<Health>(99);
    Position* p0 ( d0->createComponent<Position>() );
    Position* p1 ( d0->createComponent<Position>() );
    Movement* v0 ( d0->createComponent<Movement>() );
    Movement* v1 ( d0->createComponent<Movement>() );

    // Create some entities
    Entity* e0 = d0->createEntity();
    Entity* e1 = d0->createEntity();

    e0->addComponent<Health>(h0);
    e0->addComponent<Position>(p0);
    e0->addComponent<Movement>(v0);
    e1->addComponent<Health>(h1);
    e1->addComponent<Position>(p1);
    e1->addComponent<Movement>(v1);

    // ComponentType test
    ComponentTypeSet t0;
    t0.addType(Position::typeId());
    t0.addType(Movement::typeId());

    if (e0->hasComponents(t0)) {
        std::cout << "Entity_0 matches the set." << std::endl;
    }
    if (e1->hasComponents(t0)) {
        std::cout << "Entity_1 matches the set." << std::endl;
    }

    Entity* e2 = d0->cloneEntity(e0);

    std::cout << "First Entity ID           : " << e0->getId() << std::endl;
    std::cout << "Second (cloned) Entity ID : " << e2->getId() << std::endl;

    // Test Systems
    d0->update();
    d0->update();

    std::cout << "Creating entities: ";
    for (int mc=0; mc < 2; ++mc) {
        Entity* el = d0->createEntity();
        el->addComponent<Health>( d0->createComponent<Health>() );
        el->addComponent<Position>( d0->createComponent<Position>() );
        el->addComponent<Movement>( d0->createComponent<Movement>() );
        std::cout << el->getId() << " ";
    }
    std::cout << std::endl;

    std::vector<Entity*> vc1 = d0->getEntitiesWithComponent( Position::typeId() );
    std::cout << "Components with Position : " << vc1.size() << std::endl;
    std::vector<Entity*> vc2 = d0->getEntitiesWithComponents( t0 );
    std::cout << "Components with Position/Movement : " << vc2.size() << std::endl;
    std::cout << "Found components: ";
    for (Entity* le : vc2) {
        std::cout << le->getId() << " ";
    }
    std::cout << std::endl;

    std::cout << "Getting health/stamina of entities: " << std::endl;
    std::vector<Entity*> vc3 = d0->getEntitiesWithComponent( Health::typeId() );
    for (Entity* le : vc3) {
        Health* leh ( le->getComponent<Health>( Health::typeId() ) );
        std::cout << "\tEnt: " << le->getId() << " \t" << leh->hitpoints << "/" << leh->stamina << " @ " << leh << std::endl;
    }

}

void pool_test() {
    Pool* p = new Pool(sizeof(Health));

    for (int i=0; i < 10000; i++) {
        void* h1 = (void*) p->allocate<Health>();

    }

    //TODO
}

int main() {
    std::cout << "Starting Pool tests..." << std::endl;
    pool_test();
    std::cout << "Starting Entity tests..." << std::endl;
    entity_test();
    exit(0);
}

#endif // VECS_TEST_H_
