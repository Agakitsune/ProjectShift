
#ifdef ALCHEMIST_DEBUG
#include <iostream>
#endif // ALCHEMIST_DEBUG

#include "memory/rid.hpp"

RID RIDServer::get_id(RID type) {
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

RIDServer &new_rid_server() {
    if (rid_server) {
        delete rid_server; // Clean up existing instance
    }
    rid_server = new RIDServer();
    rid_server->next = std::vector<RID>();
    rid_server->stack = std::vector<std::stack<RID>>();
    return *rid_server;
}

RIDServer &get_rid_server() {
    return *rid_server;
}

RIDAtlas &new_rid_atlas() {
    if (rid_server == nullptr) {
        new_rid_server();
    }
    if (atlas) {
        delete atlas; // Clean up existing instance
    }
    atlas = new RIDAtlas();
    atlas->MEMORY = new_resource();
    atlas->IMAGE = new_resource();
    atlas->BIND = new_resource();
    return *atlas;
}

RIDAtlas &get_rid_atlas() {
    if (atlas == nullptr) {
        new_rid_atlas();
    }
    return *atlas;
}

RID new_id(RID type) {
    return rid_server->get_id(type);
}

RID new_resource() {
    RID type = rid_server->next_id++;
    rid_server->next.push_back(0); // Initialize the next ID for this type
    rid_server->stack.push_back(std::stack<RID>()); // Initialize the free list for this type
    return type;
}

RIDServer *rid_server = nullptr;
RIDAtlas *atlas = nullptr;
