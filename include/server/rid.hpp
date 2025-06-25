
#ifndef ALCHEMIST_MEMORY_RID_HPP
#define ALCHEMIST_MEMORY_RID_HPP

#include <vector>
#include <stack>
#include <set>

#include <cstdint>
#include <memory>

typedef uint64_t RID; // Resource ID type, typically used for identifying resources in a system

constexpr RID RID_INVALID = UINT64_MAX; // Invalid RID value, used to indicate an uninitialized or invalid resource ID

struct RIDServer {
    std::vector<RID> next;
    std::vector<std::stack<RID>> stack;
    std::vector<std::set<RID>> in_stack; // Set to track used RIDs for each type

    RIDServer();
    ~RIDServer() = default;

    RID new_id(RID type);
    void free(RID type, RID id);

    static const RID MEMORY = 0; // Resource ID for memory management
    static const RID BIND = 1; // Resource ID for binding resources
    static const RID IMAGE = 2; // Resource ID for images
    static const RID BUFFER = 3; // Resource ID for buffers
    static const RID MESH = 4; // Resource ID for meshes
    static const RID RENDER_PASS = 5; // Resource ID for render passes
    static const RID COMMAND_POOL = 6; // Resource ID for command pools
    static const RID QUEUE = 7; // Resource ID for queues
    static const RID PIPELINE = 8; // Resource ID for pipelines
    static const RID FRAMEBUFFER = 9; // Resource ID for framebuffers
    static const RID DESCRIPTOR_LAYOUT = 11; // Resource ID for descriptor layouts
    static const RID DESCRIPTOR_SET = 11; // Resource ID for descriptor sets
    static const RID DESCRIPTOR_POOL = 12; // Resource ID for descriptor pools
    static const RID PIPELINE_LAYOUT = 13; // Resource ID for pipeline layouts
    static const RID SHADER = 14; // Resource ID for shaders
    static const RID IMAGE_VIEW = 15; // Resource ID for image views
    static const RID SAMPLER = 16; // Resource ID for samplers


    static RIDServer &instance();
    static std::unique_ptr<RIDServer> __instance; // Unique pointer to the RIDServer instance
};

// struct RIDAtlas {
//     RID MEMORY;
//     RID BIND;
    
//     RID IMAGE;
//     RID BUFFER;
    
//     RID MESH;
// };

// RIDServer &new_rid_server();
// RIDServer &RIDServer::instance();

// RIDAtlas &new_rid_atlas();
// RIDAtlas &get_rid_atlas();

// RID RIDServer::instance().new_id(RID type);
// RID new_resource();

// extern RIDServer *rid_server;
// extern RIDAtlas *atlas;

#endif // ALCHEMIST_MEMORY_RID_HPP
