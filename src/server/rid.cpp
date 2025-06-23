
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "server/rid.hpp"

RIDServer::RIDServer() {
    constexpr size_t resource_types = 17; // Number of resource types
    next.resize(resource_types, 0); // Initialize next IDs for 5 resource types
    stack.resize(resource_types); // Initialize stacks for 5 resource types
    in_stack.resize(resource_types); // Initialize sets for tracking used RIDs
}

RID RIDServer::new_id(RID type) {
    if (!stack[type].empty()) {
        RID rid = stack[type].top();
        stack[type].pop(); // Remove the ID from the free list
        in_stack[type].erase(rid); // Remove the ID from the set of used RIDs
        std::cout << "Reusing RID: " << rid << " for type: " << type << std::endl; // Debug output
        return rid; // Return the top ID from the free list
    } else {
        RID id = next[type];
        next[type]++;
        std::cout << "New RID allocated: " << id << " for type: " << type << std::endl; // Debug output
        return id;
    }
}

void RIDServer::free(RID type, RID id) {
    // if (id < next[type]) {
    //     std::cout << "Freeing RID: " << id << " for type: " << type << std::endl; // Debug output
    //     if (in_stack[type].find(id) != in_stack[type].end()) {
    //         #ifdef ALCHEMIST_DEBUG
    //         std::cerr << "Attempted to free an already freed RID: " << id << " for type: " << type << std::endl;
    //         #endif
    //         return; // If the ID is already in the stack, do not free it again
    //     }
    //     stack[type].push(id); // Add the ID back to the free list
    //     in_stack[type].insert(id); // Mark the ID as used
    // } else {
    //     #ifdef ALCHEMIST_DEBUG
    //     std::cerr << "Attempted to free an invalid RID: " << id << " for type: " << type << std::endl;
    //     #endif
    // }
}

RIDServer &RIDServer::instance() {
    return *__instance; // Return the singleton instance
}

std::unique_ptr<RIDServer> RIDServer::__instance = nullptr; // Unique pointer to the RIDServer instance
