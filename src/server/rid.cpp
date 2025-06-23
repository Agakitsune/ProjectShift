
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "server/rid.hpp"

RIDServer::RIDServer() {
    constexpr size_t resource_types = 17; // Number of resource types
    next.resize(resource_types, 0); // Initialize next IDs for 5 resource types
    stack.resize(resource_types); // Initialize stacks for 5 resource types
}

RID RIDServer::new_id(RID type) {
    if (!stack[type].empty()) {
        RID rid = stack[type].top();
        stack[type].pop(); // Remove the ID from the free list
        return rid; // Return the top ID from the free list
    } else {
        RID id = next[type];
        next[type]++;
        return id;
    }
}

void RIDServer::free(RID type, RID id) {
    if (id < next[type]) {
        stack[type].push(id); // Add the ID back to the free list
    } else {
        #ifdef ALCHEMIST_DEBUG
        std::cerr << "Attempted to free an invalid RID: " << id << " for type: " << type << std::endl;
        #endif
    }
}

RIDServer &RIDServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<RIDServer> RIDServer::__instance = nullptr; // Unique pointer to the RIDServer instance
