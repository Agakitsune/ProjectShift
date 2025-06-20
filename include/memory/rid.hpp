
#ifndef ALCHEMIST_MEMORY_RID_HPP
#define ALCHEMIST_MEMORY_RID_HPP

#include <vector>
#include <stack>

#include <cstdint>

typedef uint64_t RID; // Resource ID type, typically used for identifying resources in a system

constexpr RID RID_INVALID = UINT64_MAX; // Invalid RID value, used to indicate an uninitialized or invalid resource ID

struct RIDServer {
    std::vector<RID> next;
    std::vector<std::stack<RID>> stack;

    RID next_id = 0; // Next available ID to be assigned to a resource type

    RID get_id(RID type);
    void free(RID type, RID id);
};

struct RIDAtlas {
    RID MEMORY;
    RID IMAGE;
    RID BIND;
};

RIDServer &new_rid_server();
RIDServer &get_rid_server();

RIDAtlas &new_rid_atlas();
RIDAtlas &get_rid_atlas();

RID new_id(RID type);
RID new_resource();

extern RIDServer *rid_server;
extern RIDAtlas *atlas;

#endif // ALCHEMIST_MEMORY_RID_HPP
