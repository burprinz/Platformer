//
// Created by luisb on 10.03.2026.
//


#include <nlohmann/adl_serializer.hpp>

#include "window.h"
#include "core/registry.h"
#include <nlohmann/json.hpp>

class EntityFactory {
public:
    static EntityFactory init(Window* window, Registry* registry) noexcept;

    bool readPath(const std::string& path) noexcept;
    void createFromFile(const std::string& path) noexcept;

private:
    Registry* m_registry;
    Window* m_window;

    nlohmann::json m_levelJson;

    std::string m_texture_base_path = getBasePath() + "/assets/textures/";
};
