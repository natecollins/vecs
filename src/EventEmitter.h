#ifndef VECS_EVENTEMITTER_H_
#define VECS_EVENTEMITTER_H_

#include <memory>
#include "entdef.h"

namespace vecs
{
class Domain;

class EventEmitter {
private:
    Domain* dom;

public:
    EventEmitter(Domain* dom);
    virtual ~EventEmitter() {};

    Domain* getEmitDomain();

};

}

#endif // VECS_EVENTEMITTER_H_
