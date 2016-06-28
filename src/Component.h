#ifndef VECS_COMPONENT_H_
#define VECS_COMPONENT_H_

#include "entdef.h"
#include "ComponentBase.h"
#include "Domain.h"

namespace vecs
{

/**
 * Component class containing RTTI.
 * Create Components using the Domain instance function, createComponent<>()
 */
template<typename T>
class Component : public ComponentBase {
private:
    // Type Id for given component
    static COMP_TYPE_ID ctid;
    // Id for the given component
    COMP_ID cid;

public:
    Component();
    virtual ~Component();

    virtual ComponentBase* clone() = 0;
    T* getDerivedComponent() const;

    COMP_ID getId() const;
    virtual COMP_TYPE_ID getType() const;
    static COMP_TYPE_ID typeId();

};

template <typename T>
COMP_TYPE_ID Component<T>::ctid = Domain::getNextComponentTypeId();

template <typename T>
Component<T>::Component() {
    static_assert(std::is_base_of<Component,T>(), "Components must be constructed with class derived from ComponentBase.");
    this->cid = Domain::getNextComponentId();
}

template<typename T>
Component<T>::~Component() {
    //
}

template <typename T>
T* Component<T>::getDerivedComponent() const {
    T* derived = static_cast<T*>( this );
    return derived;
}

template<typename T>
COMP_ID Component<T>::getId() const {
    return this->cid;
}

template<typename T>
COMP_TYPE_ID Component<T>::getType() const {
    return ctid;
}

template<typename T>
COMP_TYPE_ID Component<T>::typeId() {
    return ctid;
}

}

#endif // VECS_COMPONENT_H_
