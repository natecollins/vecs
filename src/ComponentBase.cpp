#include <chrono>
#include <thread>
#include "ComponentBase.h"
#include "Domain.h"
#include "Entity.h"

namespace vecs
{

ComponentBase::ComponentBase()
  : parent_ent (nullptr), spinlock (ATOMIC_FLAG_INIT), state (STATE_INITIALIZING) {
    state = STATE_AVAILABLE;
}

ComponentBase::~ComponentBase() {
    state = STATE_DESTROYED;
}

void ComponentBase::lock() {
    while (spinlock.test_and_set()) {
        std::this_thread::sleep_for( std::chrono::microseconds(1) );
    };
}

void ComponentBase::unlock() {
    spinlock.clear();
}

ComponentBase* ComponentBase::getSelf() {
    return this;
}

void ComponentBase::setParentEntity(Entity* parent) {
    this->parent_ent = parent;
}

Entity* const ComponentBase::getParentEntity() const {
    return parent_ent;
}

COMP_TYPE_ID ComponentBase::getType() const {
    return 0;
}


}

