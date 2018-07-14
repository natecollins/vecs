#include "EventEmitter.h"

#include "Domain.h"

namespace vecs
{

EventEmitter::EventEmitter(Domain* dom) {
    this->dom = dom;
}

Domain* EventEmitter::getEmitDomain() {
    return this->dom;
}

}

