#ifndef VECS_EVENTSSTANDARD_H_
#define VECS_EVENTSSTANDARD_H_

#include <memory>
#include "Event.h"
#include "Entity.h"

namespace vecs {

// Default Events
class EntityAddedEvent : public Event<EntityAddedEvent> {
private:
    Entity* en;

public:
    EntityAddedEvent(Entity* en);
    virtual ~EntityAddedEvent();

    Entity* getEntity();

};

class EntityDestroyedEvent : public Event<EntityDestroyedEvent> {
private:
    Entity* en;

public:
    EntityDestroyedEvent(Entity* en);
    virtual ~EntityDestroyedEvent();

    Entity* getEntity();

};

class ComponentAddedEvent : public Event<ComponentAddedEvent> {
private:
    Entity* en;
    ComponentBase* comp;

public:
    ComponentAddedEvent(Entity* en, ComponentBase* comp);
    virtual ~ComponentAddedEvent();

    Entity* getEntity();
    ComponentBase* getComponent();

};

class ComponentDestroyedEvent : public Event<ComponentDestroyedEvent> {
private:
    Entity* en;
    ComponentBase* comp;

public:
    ComponentDestroyedEvent(Entity* en, ComponentBase* comp);
    virtual ~ComponentDestroyedEvent();

    Entity* getEntity();
    ComponentBase* getComponent();

};

}

#endif // VECS_EVENTSSTANDARD_H_
