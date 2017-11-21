#ifndef VECS_SYSTEM_H_
#define VECS_SYSTEM_H_

#include <memory>
#include "ComponentBase.h"
#include "Component.h"
#include "ComponentTypeSet.h"
#include "EventReceiver.h"

namespace vecs
{
class Domain;
class Entity;

enum SystemThreadState : int32_t {
    NON_THREAD = 0,
    THREAD_QUEUED = 1,
    THREAD_RUNNING = 2,
    THREAD_TERM_REQUESTED = 4,
    THREAD_TERMINATED = 8,
};

class System : public EventReceiver {
private:
    // Link to manager that holds this system
    Domain* dom;
    // This System requires this type set
    ComponentTypeSet type_set;
    // Thread State
    SystemThreadState thread_state;
    // minumum time required between running update code (ms)
    int64_t min_update_period;
    // the last time update code ran
    int64_t last_update;
    // the last time delta in ms
    int64_t last_delta;

    // rescan for matching entities
    void rescan();

protected:
    // This is our current list of valid (matching the type set) entities
    std::unordered_map<ENT_ID, Entity*> entities;

    const Domain* getDomain();

public:
    System(SystemThreadState thread_state=NON_THREAD, Domain* dom=nullptr);
    virtual ~System();
    void setDomain(Domain* dom);

    void setThreadState(SystemThreadState thread_state);
    SystemThreadState getThreadState() const;

    void internalUpdate();

    void setMinimumUpdateTime(int64_t time_ms);
    int64_t getTimeDelta() const;
    int64_t getLastUpdateTime() const;

    void requireType(COMP_TYPE_ID ctid);
    bool validEntity(Entity* en) const;
    // Check if this system is already tracking the entity
    bool isTracking(Entity* en) const;
    // Attempt to track the given entity (assuming type_set match)
    void trackEntity(Entity* en);
    // Stop tracking the given entity
    void forgetEntity(Entity* en);

    virtual void runEvent(EventBase* ev);
    virtual void update() = 0;

    virtual void dump() const;
};

}

#endif // VECS_SYSTEM_H_
