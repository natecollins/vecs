#include "System.h"
#include "Entity.h"
#include "Event.h"
#include "EventsStandard.h"
#include <iostream>

namespace vecs
{

System::System(SystemThreadState thread_state, Domain* dom)
  : min_update_period (0), last_update (0) {
    setDomain(dom);
    this->thread_state = thread_state;
}

System::~System() {
    //
}

Domain* const System::getDomain() {
    return this->dom;
}

void System::setDomain(Domain* dom) {
    this->dom = dom;
    rescan();
}

void System::setThreadState(SystemThreadState thread_state) {
    this->thread_state = thread_state;
}

SystemThreadState System::getThreadState() const {
    return thread_state;
}

void System::internalUpdate() {
    // Update time
    int64_t now = 0;
    if (getThreadState() == NON_THREAD) {
        now = dom->getDomainLoopTime();
    }
    else {
        now = dom->getDomainTime();
    }
    // Don't run update code more frequently than the minimum
    last_delta = now - last_update;
    if (last_delta >= min_update_period) {
        last_update = now;

        // Call update
        update();
    }
}

void System::setMinimumUpdateTime(int64_t time_ms) {
    min_update_period = time_ms;
}

int64_t System::getTimeDelta() const {
    return last_delta;
}

int64_t System::getLastUpdateTime() const {
    return last_update;
}

/**
 * Rescan manager for matching entities to track.
 * If no manager has been set, no action occurs.
 */
void System::rescan() {
    if (dom != nullptr) {
        // Forget all our existing entities
        for (auto spair : this->entities) {
            auto en = spair.second;
            forgetEntity(en);
        }
        // Get a new list of matching entities
        std::vector<Entity*> matches = dom->getEntitiesWithComponents(type_set);
        // Track new entities
        for (auto mat : matches) {
            trackEntity(mat);
        }
    }
}

/**
 * Add a Component Type this system needs to require.
 * Forces a entity rescan, which is very slow.
 */
void System::requireType(COMP_TYPE_ID ctid) {
    this->type_set.addType(ctid);
    rescan();
}

/**
 * Check if the given Entity matches the required COMP_TYPE_IDs
 */
bool System::validEntity(Entity* en) const {
    return en->hasComponents(type_set);
}

bool System::isTracking(Entity* en) const {
    return entities.count(en->getId()) != 0;
}

void System::trackEntity(Entity* en) {
    if (!isTracking(en) && validEntity(en)) {
        entities.emplace(en->getId(), en);
    }
}

void System::forgetEntity(Entity* en) {
    if (isTracking(en)) {
        entities.erase(en->getId());
    }
}

void System::runEvent(EventBase* ev) {
	auto etype = ev->getType();
    if (etype == EntityAddedEvent::typeId()) {
        // Convert event
        auto eaev = static_cast<EntityAddedEvent*>(ev);
        // Begin tracking entity (auto checks for validation and duplicates)
        trackEntity(eaev->getEntity());
    }
    else if (etype == EntityDestroyedEvent::typeId()) {
        auto edev = static_cast<EntityDestroyedEvent*>(ev);
        forgetEntity(edev->getEntity());
    }
    else if (etype == ComponentAddedEvent::typeId()) {
        // Convert event
        auto caev = static_cast<ComponentAddedEvent*>(ev);
        // Begin tracking entity (auto checks for validation and duplicates)
        trackEntity(caev->getEntity());
    }
    else if (etype == ComponentDestroyedEvent::typeId()) {
        auto cdev = static_cast<ComponentDestroyedEvent*>(ev);
        Entity* cd_ent = cdev->getEntity();
        if (!validEntity(cd_ent)) {
            forgetEntity(cd_ent);
        }
    }
}

void System::dump() const {
    std::cout << "===================================" << std::endl;
    std::cout << "System Dump" << std::endl;
    std::cout << "===================================" << std::endl;
    type_set.dump();
    std::cout << "Tracking entity count: " << entities.size() << std::endl;
}

}

