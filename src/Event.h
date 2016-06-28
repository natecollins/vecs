#ifndef VECS_EVENT_H_
#define VECS_EVENT_H_

#include <memory>
#include "entdef.h"
#include "Domain.h"

namespace vecs
{

class EventBase {
public:
    EventBase() {};
    virtual ~EventBase() {};

    virtual EVNT_TYPE_ID getType() const = 0;
};

template<typename T>
class Event : public EventBase {
private:
    static EVNT_TYPE_ID etid;

public:
    Event();
    virtual ~Event();

    EVNT_TYPE_ID getType() const;
    static EVNT_TYPE_ID typeId();

};

template <typename T>
COMP_TYPE_ID Event<T>::etid = Domain::getNextEventTypeId();;

template <typename T>
Event<T>::Event() {
    static_assert(std::is_base_of<Event,T>(), "Events must be constructed with class derived from EventBase.");
}

template<typename T>
Event<T>::~Event() {
    //
}

template<typename T>
EVNT_TYPE_ID Event<T>::getType() const {
    return etid;
}

template<typename T>
EVNT_TYPE_ID Event<T>::typeId() {
    return etid;
}


}

#endif // VECS_EVENT_H_
