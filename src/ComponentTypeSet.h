#ifndef VECS_COMPONENTTYPESET_H_
#define VECS_COMPONENTTYPESET_H_

#include <vector>
#include "entdef.h"

namespace vecs
{

class ComponentTypeSet {
private:
    std::vector<COMP_TYPE_ID> type_ids;

public:
    ComponentTypeSet() {};
    virtual ~ComponentTypeSet() {};

    uint32_t size() const;

    void addType(COMP_TYPE_ID type_id);

    const std::vector<COMP_TYPE_ID>& getSet() const;

    void dump() const;

};

}

#endif // VECS_COMPONENTTYPESET_H_
