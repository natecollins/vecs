#ifndef VECS_EVENTRECEIVER_H_
#define VECS_EVENTRECEIVER_H_

#include <memory>
#include "entdef.h"
#include "Entity.h"

namespace vecs
{
class EventBase;

class EventReceiver {
private:

public:
    EventReceiver() {};
    virtual ~EventReceiver() {};

    virtual void runEvent(EventBase* ev) = 0;

};

}

#endif // VECS_EVENTRECEIVER_H_
