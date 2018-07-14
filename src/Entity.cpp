#include <typeinfo>
#include <sstream>
#include <iostream>
#include "Entity.h"
#include "Domain.h"


namespace vecs
{

Entity::Entity(Domain* m)
  : dom ( m ) {
    this->eid = Domain::getNextEntityId();
}

Entity::Entity(Domain* m, ENT_ID eid)
  : eid (eid), dom ( m ) {
    //
}

std::string Entity::getComponentNames() const {
    std::string names = "";
    for (auto cop : this->components) {
        std::stringstream sstr;
        sstr << cop.first;
        std::string ctid_str = sstr.str();
        ComponentBase* co = cop.second;
        if (names.length() != 0) { names += ","; };
        names += typeid(*co).name();
        names += "(" + ctid_str + ")";
    }
    return names;
}

Entity* Entity::getSelf() {
    return this;
}

bool Entity::operator==(const Entity& en) const {
    return (getId() == en.getId());
}

bool Entity::operator!=(const Entity& en) const {
    return (getId() != en.getId());
}

void Entity::lockComponents() {
    for (auto pair : components) {
        pair.second->lock();
    }
}

void Entity::unlockComponents() {
    for (auto pair : components) {
        pair.second->unlock();
    }
}

bool Entity::isValid() const {
    return (dom && eid != 0);
}

ENT_ID Entity::getId() const {
    return eid;
}

Domain* Entity::getDomain() const {
    return dom;
}

void Entity::destroyAllComponents() {
    /**
     * Don't bother sending ComponentDestroyed event because
     * we should have already sent an EntityDestroyed event
     */
    for (auto p : components) {
        dom->destroyComponent( p.second );
    }
    components.clear();
}

ComponentBase* Entity::addComponent(ComponentBase* c) {
    // Add component
    components.emplace(c->getType(), c);
    // Give component reference to Entity
    c->setParentEntity( getSelf() );
    // Throw ComponentAddedEvent
    Entity* self_ptr = dom->getEntity(getId());
    dom->emitComponentAdded(self_ptr, c);

    return c;
}

void Entity::destroyComponent(COMP_TYPE_ID ctid) {
    ComponentBase* c = getComponentBase(ctid);
    // Remove component
    components.erase(ctid);
    // Throw ComponentDetroyedEvent
    dom->emitComponentDestroyed(getSelf(), c);
    // Destroy pointer
    dom->destroyComponent(c);
}

ComponentBase* Entity::getComponentBase(COMP_TYPE_ID ctid) const {
    ComponentBase* c = nullptr;
    if (hasComponent(ctid)) {
        c = components.at(ctid);
    }
    return c;
}

bool Entity::hasComponent(COMP_TYPE_ID ctid) const {
    return (components.count(ctid) == 1);
}

bool Entity::hasComponents(const ComponentTypeSet& cset) const {
    // If passed set is empty, then match is false
    bool match = (cset.size() != 0);
    auto set_types = cset.getSet();
    for (const COMP_TYPE_ID& set_type : set_types) {
        match = (match && hasComponent( set_type ));
    }
    return match;
}

std::vector<ComponentBase*> Entity::getAllComponents() const {
    std::vector<ComponentBase*> comps;
    for (auto p : components) {
        comps.push_back( p.second );
    }
    return comps;
}

void Entity::dump() const {
    std::cout << "==============================================" << std::endl;
    std::cout << "Entity Dump for ID " << getId() << std::endl;
    std::cout << "==============================================" << std::endl;
    std::cout << "Component count: " << components.size() << std::endl;
    std::cout << "Components: " << getComponentNames() << std::endl;
}

}
