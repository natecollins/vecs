#ifndef VECS_DOMAIN_H_
#define VECS_DOMAIN_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

#include "entdef.h"
#include "Pool.h"
#include "ComponentBase.h"
#include "ComponentTypeSet.h"

namespace vecs
{
class Entity;
class System;
class EventReceiver;
class ComponentBase;

class Domain {
private:
    // List of instantiated entities
    std::unordered_map<ENT_ID, Entity*> entities;
    // List of Systems
    std::vector<System*> systems;
    // List of registered EventReceivers
    std::unordered_map<EVNT_TYPE_ID, std::vector<EventReceiver*> > receivers;
    // The ComponentPool allocators
    std::unordered_map<COMP_TYPE_ID, Pool*> cpools;
    // The Entity allocator
    Pool* epool;

    // Real time in ms last time update() was called
    int64_t rtime_ms;
    // Game/domain time in ms last time update() was called
    int64_t dtime_ms;
    // How many game/domain ms pass during a real world second. Default: 1000 (domain time = real time)
    int64_t ms_per_realsec;
    // How many game/domain ms passed during the past game loop
    int64_t dtime_elapsed_ms;
    // How any real ms passed during the past game loop
    int64_t rtime_elapsed_ms;

    // Variables to hold next unique id
    static COMP_TYPE_ID assign_component_type_id;
    static ENT_ID assign_entity_id;
    static COMP_ID assign_component_id;
    static EVNT_TYPE_ID assign_event_type_id;

public:
    static COMP_TYPE_ID getNextComponentTypeId();
    static ENT_ID getNextEntityId();
    static COMP_ID getNextComponentId();
    static EVNT_TYPE_ID getNextEventTypeId();

    Domain();
    virtual ~Domain() {};
    Domain* getSelf();

    void setDomainTime(int64_t dtime_ms);
    int64_t getDomainLoopTime() const;
    int64_t getDomainTime() const;
    void setTimeSpeed(int64_t ms_per_realsec);
    int64_t getTimeSpeed() const;
    int64_t getDomainTimeElapsed() const;
    int64_t getRealTimeElapsed() const;
    int64_t getRealLoopTime() const;
    int64_t getRealTime() const;

    Entity* createEntity();
    Entity* cloneEntity(Entity* original);
    void addEntity(Entity* en);
    bool hasEntity(ENT_ID eid);
    Entity* getEntity(ENT_ID eid);
    void destroyEntity(ENT_ID eid);
    void destroyEntity(Entity* ent);
    std::vector<Entity*> getEntitiesWithComponent(COMP_TYPE_ID ctid);
    std::vector<Entity*> getEntitiesWithComponents(ComponentTypeSet cset);
    std::vector<Entity*> getEntities();

    void update();

    template<typename T, typename ...Args>
    T* createComponent(Args&&... params);
    //template<typename T, typename ...Args>
    //T* emplaceComponent(Entity* ent, Args&&... params);
    template<typename T>
    void destroyComponent(T* ptr);
    void destroyComponent(ComponentBase *comp);

    void emitComponentAdded(Entity* en, ComponentBase* comp);
    void emitComponentDestroyed(Entity* en, ComponentBase* comp);

    System* addSystem(System* sy);

};

template<typename T, typename ...Args>
T* Domain::createComponent(Args&&... params) {
    static_assert(std::is_base_of<ComponentBase,T>(), "Domain could not createComponent(); must be created with class derived from ComponentBase.");
    COMP_TYPE_ID ctid = T::typeId();
    // If appropriate ComponentPool doesn't exist, create it
    if (cpools.count(ctid) == 0) {
        cpools[ctid] = new Pool(sizeof(T));
    }

    // Get memory address
    T* pComp = cpools[ctid]->allocate<T>();
    // Construct object at address
    new (pComp) T(std::forward<Args>(params)...);

    return pComp;
}

//template<typename T, typename ...Args>
//T* Domain::emplaceComponent(Entity* ent, Args&&... params) {
//    return ent->addComponent( createComponent<T>(std::forward<Args>(params)...) );
//}

template<typename T>
void Domain::destroyComponent(T* ptr) {
    static_assert(std::is_base_of<ComponentBase,T>(), "Domain could not destroyComponent(); must be destroyed with class derived from ComponentBase.");
    COMP_TYPE_ID ctid = T::typeId();
    cpools.at(ctid)->release<T>(ptr);
}

}

#endif // VECS_DOMAIN_H_
