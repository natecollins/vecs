VECS ENTITY COMPONENT SYSTEM
==================================

This is an entity-component system I wrote to learn more about how an ECS is put together and operates.
VECS works, but is poorly documented and not optimized (as it makes use of inefficient STL containers).  

Notable features:  
- Each System may optionally operate in its own thread.
- Each System tracks its own timing and can have custom update timing.
- No arbitrary size limitations on entities, number of components, or number of systems.

Including `src/vecs.h` will give you access to all the classes, which reside in the namespace `vecs`.  


Classes Relevant To Use
-------------------------
**Domain**  
A container for Systems and Entities. Different domains can operate completely independent of one another.  

**System**  
A set of logic that operates periodically on some subset of entities.  

**Entity**  
A distict container instance for a set of components.  

**Component**  
Container for data to be put into an entity.  

**ComponentTypeSet**  
Define a set of component types.


Example
-------------------------
Taken from `test.cpp`:  
```
// Create a domain
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

// Update systems in domain
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
```


