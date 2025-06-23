
#ifndef ALCHEMIST_EDITOR_SERVER_HPP
#define ALCHEMIST_EDITOR_SERVER_HPP

#ifdef ALCHEMIST_DEBUG
#include <iostream> // Include for debug output
#endif // ALCHEMIST_DEBUG

#include <vector>
#include <memory>

struct EditorServer {
    // std::vector<void*> servers; // Vector to hold various server instances

    // template <typename T>
    // T &get_server() {
    //     for (auto &server : servers) {
    //         if (auto ptr = dynamic_cast<T*>(server)) {
    //             return *ptr; // Return the server instance if found
    //         }
    //     }
    //     #ifdef ALCHEMIST_DEBUG
    //     std::cerr << "Server of type " << typeid(T).name() << " not found!" << std::endl;
    //     #endif
    //     return *static_cast<T*>(nullptr); // Return a null reference if not found
    // }

    template <typename T, typename... Args>
    T &emplace_server(Args&&... args) {
        #ifdef ALCHEMIST_DEBUG
        std::cout << "Creating new server of type " << typeid(T).name() << std::endl;
        #endif
        T::__instance = std::make_unique<T>(std::forward<Args>(args)...);
        return T::instance(); // Return the reference to the created server
    }

    static EditorServer &instance();

    static std::unique_ptr<EditorServer> __instance; // Singleton instance of EditorServer
};

#endif // ALCHEMIST_EDITOR_SERVER_HPP
