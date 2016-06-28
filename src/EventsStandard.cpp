#include "EventsStandard.h"
#include "Domain.h"
#include "Entity.h"


namespace vecs {

EntityAddedEvent::EntityAddedEvent(Entity* en) {
    this->en = en;
}

EntityAddedEvent::~EntityAddedEvent() {
    //
}

Entity* EntityAddedEvent::getEntity() {
    return en;
}

EntityDestroyedEvent::EntityDestroyedEvent(Entity* en) {
    this->en = en;
}

EntityDestroyedEvent::~EntityDestroyedEvent() {
    //
}

Entity* EntityDestroyedEvent::getEntity() {
    return en;
}

ComponentAddedEvent::ComponentAddedEvent(Entity* en, ComponentBase* comp) {
    this->en = en;
    this->comp = comp;
}

ComponentAddedEvent::~ComponentAddedEvent() {
    //
}

Entity* ComponentAddedEvent::getEntity() {
    return en;
}

ComponentBase* ComponentAddedEvent::getComponent() {
    return comp;
}

ComponentDestroyedEvent::ComponentDestroyedEvent(Entity* en, ComponentBase* comp) {
    this->en = en;
    this->comp = comp;
}

ComponentDestroyedEvent::~ComponentDestroyedEvent() {
    //
}

Entity* ComponentDestroyedEvent::getEntity() {
    return en;
}

ComponentBase* ComponentDestroyedEvent::getComponent() {
    return comp;
}

}
