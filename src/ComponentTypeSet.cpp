#include "ComponentTypeSet.h"
#include <iostream>

namespace vecs
{

uint32_t ComponentTypeSet::size() const {
    return this->type_ids.size();
}

void ComponentTypeSet::addType(COMP_TYPE_ID type_id) {
    this->type_ids.push_back(type_id);
}

const std::vector<COMP_TYPE_ID> &ComponentTypeSet::getSet() const {
    return this->type_ids;
}

void ComponentTypeSet::dump() const {
    std::cout << "Component Type Set ( ";
    for( auto t_id : type_ids ) {
        std::cout << t_id << " ";
    }
    std::cout << ")" << std::endl;
}

}

