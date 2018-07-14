#ifndef VECS_ENTITY_H_
#define VECS_ENTITY_H_

#include <memory>
#include <vector>
#include <string>

#include "entdef.h"
#include "ComponentBase.h"
#include "Component.h"
#include "ComponentTypeSet.h"


namespace vecs
{
class Domain;

class Entity {
    friend class Domain;
private:
    ENT_ID eid;
    Domain* dom;
    std::unordered_map<COMP_TYPE_ID, ComponentBase*> components;

    void destroyAllComponents();

public:
    // ctors/dtor
    Entity(Domain* m);
    Entity(Domain* m, ENT_ID eid);
    virtual ~Entity() {};

    // Get all the components class names as a comma concatinated string
    std::string getComponentNames() const;
    Entity* getSelf();

    // Copy ctor
    Entity(Entity&&) = default;
    // Assignment
    Entity& operator=(Entity&&) = default;
    // Compare
    bool operator==(const Entity& en) const;
    bool operator!=(const Entity& en) const;

    // Spin Locks
    void lockComponents();
    void unlockComponents();

    // Check if entity has id, manager, and has not been destroyed
    bool isValid() const;
    ENT_ID getId() const;
    Domain* getDomain() const;
    void destroy();

    // Components
    template <typename T>
    T* addComponent(T* c);
    ComponentBase* addComponent(ComponentBase* c);
    void destroyComponent(COMP_TYPE_ID ctid);
    ComponentBase* getComponentBase(COMP_TYPE_ID ctid) const;
    template <typename T>
    T* getComponent(COMP_TYPE_ID ctid);
    bool hasComponent(COMP_TYPE_ID ctid) const;
    bool hasComponents(const ComponentTypeSet& cset) const;
    std::vector<ComponentBase*> getAllComponents() const;

    void dump() const;

};

/* TEMPLATE FUNCTION DEFINITIONS */
template <typename T>
T* Entity::addComponent(T* c) {
    static_assert(std::is_base_of<ComponentBase,T>(), "Cannot add T as Component. Base class of T is not of type ComponentBase.");
    // Add component
    components.emplace(c->getType(), c);
    // Give component reference to Entity
    c->setParentEntity( getSelf() );
    // Throw ComponentAddedEvent
    Entity* self_ptr = dom->getEntity(getId());
    dom->emitComponentAdded(self_ptr, c);

    return c;
}

template <typename T>
T* Entity::getComponent(COMP_TYPE_ID ctid) {
    T* cmp = static_cast<T*>( getComponentBase(ctid) );
    return cmp;
}

}

#endif // VECS_ENTITY_H_

