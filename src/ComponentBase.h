#ifndef VECS_COMPONENTBASE_H_
#define VECS_COMPONENTBASE_H_

#include <atomic>
#include <memory>
#include <cstdint>
#include "entdef.h"

namespace vecs
{
class Entity;

/**
 * The base of Component classes. Holder of unique component type ids.
 * Create Components using the Domain instance function, createComponent<>()
 * Contains a built-in spinlock.
 */
class ComponentBase {
    friend class Domain;
private:
    // optional reference to entity holding this component
    Entity* parent_ent;
    // Spinlock flag
    std::atomic_flag spinlock;
    // Is this component currently in use (or destroyed/abandoned)
    uint8_t state;

public:
    enum ComponentState {
        STATE_DESTROYED = 0x00,
        STATE_INITIALIZING = 0x01,
        STATE_AVAILABLE = 0x0F,
    };
    bool isDestroyed() {
        return (state == STATE_DESTROYED);
    }
    bool isAvailable() {
        return (state == STATE_AVAILABLE);
    }

    ComponentBase();
    virtual ~ComponentBase();

    // Spin Lock
    void lock();
    void unlock();

    ComponentBase* getSelf();

    virtual ComponentBase* clone() = 0;

    void setParentEntity(Entity* parent);
    const Entity* getParentEntity() const;

    virtual COMP_ID getId() const = 0;
    virtual COMP_TYPE_ID getType() const;

};

}

#endif // VECS_COMPONENTBASE_H_
