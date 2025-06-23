
#include "editor/server.hpp"

EditorServer &EditorServer::instance() {
    if (!__instance) {
        __instance = std::make_unique<EditorServer>();
    }
    return *__instance; // Return the singleton instance
}

std::unique_ptr<EditorServer> EditorServer::__instance = nullptr; // Singleton instance of EditorServer