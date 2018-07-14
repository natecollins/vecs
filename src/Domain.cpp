#include "Domain.h"
#include "Entity.h"
#include "Component.h"
#include "System.h"
#include "Event.h"
#include "EventReceiver.h"
#include "EventEmitter.h"
#include "EventsStandard.h"


namespace vecs
{

COMP_TYPE_ID Domain::assign_component_type_id = 0;
ENT_ID Domain::assign_entity_id = 0;
COMP_ID Domain::assign_component_id = 0;
EVNT_TYPE_ID Domain::assign_event_type_id = 0;

COMP_TYPE_ID Domain::getNextComponentTypeId() {
    Domain::assign_component_type_id += 1;
    return Domain::assign_component_type_id;
}

ENT_ID Domain::getNextEntityId() {
    Domain::assign_entity_id += 1;
    return Domain::assign_entity_id;
}

COMP_ID Domain::getNextComponentId() {
    Domain::assign_component_id += 1;
    return Domain::assign_component_id;
}

EVNT_TYPE_ID Domain::getNextEventTypeId() {
    Domain::assign_event_type_id += 1;
    return Domain::assign_event_type_id;
}

Domain::Domain()
  : rtime_ms (0), dtime_ms (0), ms_per_realsec (1000), dtime_elapsed_ms (1), rtime_elapsed_ms (1) {
    rtime_ms = getRealTime();
    epool = new Pool(sizeof(Entity));
}

Domain* Domain::getSelf() {
    return this;
}

void Domain::setDomainTime(int64_t dtime_ms) {
    this->dtime_ms = dtime_ms;
}

int64_t Domain::getDomainLoopTime() const {
    return dtime_ms;
}

int64_t Domain::getDomainTime() const {
    int64_t time_speed (getTimeSpeed());
    if (time_speed < 1) { return dtime_ms; }
    int64_t partial_elapsed_rtime_ms (getRealTime() - rtime_ms);
    double fraction_dtime_ms (partial_elapsed_rtime_ms / (double)time_speed);
    return (dtime_ms + (int64_t)fraction_dtime_ms);
}

void Domain::setTimeSpeed(int64_t ms_per_realsec) {
    this->ms_per_realsec = ms_per_realsec;
}

int64_t Domain::getTimeSpeed() const {
    return this->ms_per_realsec;
}

int64_t Domain::getDomainTimeElapsed() const {
    return this->dtime_elapsed_ms;
}

int64_t Domain::getRealTimeElapsed() const {
    return this->rtime_elapsed_ms;
}

int64_t Domain::getRealLoopTime() const {
    return rtime_ms;
}

int64_t Domain::getRealTime() const {
    auto d = std::chrono::high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

Entity* Domain::createEntity() {
    // Get memory address
    Entity* pEnt ( epool->allocate<Entity>());
    // Initialize Entity
    new (pEnt) Entity(getSelf());
    // Add Entity into the Domain
    addEntity(pEnt);
    // Godspeed, Entity
    return pEnt;
}

/**
 * Create new entity with duplicate components of the original entity
 */
Entity* Domain::cloneEntity(Entity* original) {
    // Create a new entity
    Entity* clone ( createEntity() );
    // Loop over components
    std::vector<ComponentBase*> orig_comps = original->getAllComponents();
    // Clone all the components
    for (ComponentBase* cbase : orig_comps) {
        clone->addComponent(cbase->clone());
    }
    return clone;
}

void Domain::addEntity(Entity* en) {
    entities.emplace(en->getId(), en);
    // Check each System for sending EntityAddedEvent
    for (auto siter = systems.begin(); siter != systems.end(); ++siter) {
        System* sy = *siter;
        // Check if the System is an EventEmitter
        EventReceiver* evrc = dynamic_cast<EventReceiver*>(sy);
        if (evrc != nullptr) {
            // Create the event and pass it
            EntityAddedEvent* ev (new EntityAddedEvent(en));
            evrc->runEvent(ev);
        }
    }
}

bool Domain::hasEntity(ENT_ID eid) {
    if (entities.find(eid) != entities.end()) {
        return true;
    }
    return false;
}

Entity* Domain::getEntity(ENT_ID eid) {
    Entity* en = nullptr;     // default empty ptr
    if (hasEntity(eid)) {
        en = entities.at(eid);
    }
    return en;
}

void Domain::destroyEntity(ENT_ID eid) {
    destroyEntity( getEntity(eid) );
}

void Domain::destroyEntity(Entity* en) {
    // Check each System for sending EntityDestroyedEvent
    for (auto siter = systems.begin(); siter != systems.end(); ++siter) {
        System* sy = *siter;
        // Check if the System is an EventEmitter
        EventReceiver* evrc = dynamic_cast<EventReceiver*>(sy);
        if (evrc != nullptr) {
            // Create the event and pass it
            EntityDestroyedEvent* ev (new EntityDestroyedEvent(en));
            evrc->runEvent(ev);
        }
    }
    // Destroy components
    en->destroyAllComponents();
    // Get rid of the entity
    entities.erase(en->getId());
    // Release back into pool
    epool->release<Entity>( en );
}

std::vector<Entity*> Domain::getEntitiesWithComponent(COMP_TYPE_ID ctid) {
    std::vector<Entity*> ents;
    if (cpools.count(ctid) != 0) {
        std::size_t offset;
        Pool* cpool ( cpools[ctid] );
        char* top (cpool->top);
        std::size_t bpc ( cpool->getBlocksPerChunk() );
        std::size_t bsize ( cpool->getBlockSize() );
        ents.reserve(cpool->chunks.size() * bpc);
        cpool->lock();
        const std::size_t ci_max ( cpool->chunks.size() - 1 );
        for (std::size_t ci=0; ci <= ci_max; ++ci) {
            for (std::size_t bi=0; bi < bpc; ++bi) {
                offset = bi * bsize;
                // If we're past the top of pool, we're done
                if (ci == ci_max && cpool->chunks[ci] + offset >= top) {
                    goto finished;
                }
                ComponentBase* pblock( (ComponentBase*)(cpool->chunks[ci]+offset) );
                // If the component isn't active, we'll skip it
                if (!pblock->isAvailable()) {
                    continue;
                }

                ents.push_back( pblock->parent_ent );
            }
        }
        finished:
        cpool->unlock();
    }
    return ents;
}

std::vector<Entity*> Domain::getEntitiesWithComponents(ComponentTypeSet cset) {
    // Find ComponentPool that matches the given cset with the smallest number of contents
    auto set_types = cset.getSet();
    COMP_TYPE_ID base_ctid = 0;
    for (const COMP_TYPE_ID& set_type : set_types) {
        if (base_ctid == 0 || (cpools.count(set_type) > 0 && cpools[set_type]->getCurrentBlockMax() < cpools[base_ctid]->getCurrentBlockMax())) {
            base_ctid = set_type;
        }
    }
    // Search for matches in the smallest component pool we found
    std::vector<Entity*> matches;
    if (base_ctid != 0) {
        std::vector<Entity*> search_pool (getEntitiesWithComponent(base_ctid));
        for (auto en : search_pool) {
            if (en->hasComponents(cset)) {
                matches.push_back(en);
            }
        }
    }
    return matches;
}

std::vector<Entity*> Domain::getEntities() {
    std::vector<Entity*> all_ents;
    for (auto iter = entities.begin(); iter != entities.end(); ++iter) {
        all_ents.push_back( iter->second );
    }
    return all_ents;
}

void Domain::update() {
    // Update times
    int64_t new_rtime_ms (getRealTime());
    rtime_elapsed_ms = new_rtime_ms - rtime_ms;
    rtime_ms = new_rtime_ms;
    if (ms_per_realsec < 1) { ms_per_realsec = 0; }   // time scale cannot be negative
    dtime_elapsed_ms = (rtime_elapsed_ms * ms_per_realsec) / (int64_t)1000;
    dtime_ms += dtime_elapsed_ms;

    // Iterate over Systems
    for (auto siter = systems.begin(); siter != systems.end(); ++siter) {
        System* sy = *siter;
        // Ignore updating systems that have been started externally
        if (sy->getThreadState() == NON_THREAD) {
            sy->internalUpdate();
        }
    }
}

void Domain::destroyComponent(ComponentBase* comp) {
    COMP_TYPE_ID ctid = comp->getType();
    cpools.at(ctid)->release<ComponentBase>(comp);
}

void Domain::emitComponentAdded(Entity* en, ComponentBase* comp) {
    // Check each System for sending ComponentAddedEvent
    for (auto siter = systems.begin(); siter != systems.end(); ++siter) {
        System* sy = *siter;
        // Check if the System is an EventEmitter
        EventReceiver* evrc = dynamic_cast<EventReceiver*>(sy);
        if (evrc != nullptr) {
            // Create the event and pass it
            ComponentAddedEvent* ev (new ComponentAddedEvent(en, comp));
            evrc->runEvent(ev);
        }
    }
}

void Domain::emitComponentDestroyed(Entity* en, ComponentBase* comp) {
    // Check each System for sending ComponentDestroyedEvent
    for (auto siter = systems.begin(); siter != systems.end(); ++siter) {
        System* sy = *siter;
        // Check if the System is an EventEmitter
        EventReceiver* evrc = dynamic_cast<EventReceiver*>(sy);
        if (evrc != nullptr) {
            // Create the event and pass it
            ComponentDestroyedEvent* ev (new ComponentDestroyedEvent(en, comp));
            evrc->runEvent(ev);
        }
    }
}

System* Domain::addSystem(System* sy) {
    systems.push_back(sy);
    sy->setDomain(getSelf());
    return sy;
}

}

