#include "EventEmitter.h"

#include "Domain.h"

namespace vecs
{

EventEmitter::EventEmitter(Domain* dom) {
    this->dom = dom;
}

EventEmitter::~EventEmitter() {
    //
}

Domain* EventEmitter::getEmitDomain() {
    return this->dom;
}

}

