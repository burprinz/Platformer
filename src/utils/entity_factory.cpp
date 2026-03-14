//
// Created by luisb on 10.03.2026.
//

#include <nlohmann/json.hpp>
#include <fstream>
#include "entity_factory.h"

#include <iostream>

#include "geometry.h"

EntityFactory EntityFactory::init(Window *window, Registry *registry) noexcept {
    EntityFactory self;
    self.m_window = window;
    self.m_registry = registry;
    return self;
}

bool EntityFactory::readPath(const std::string& path) noexcept {
    std::string basePath = getBasePath();
    std::string fullPath = basePath + path;

    // Allows for path to be just the filename
    if (!std::ifstream(fullPath).good())
    {
        fullPath = basePath + "/assets/roomSchemes/" + path;
    }

    //Tries Reading File
    std::ifstream file(fullPath);
    if (!file.is_open())
    {
        fmt::println("Could not open level file: '{}'", fullPath);
        return false;
    }

    //Tries Parsing JSON
    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (const std::exception& e)
    {
        fmt::println("Failed to parse JSON from '{}': {}", fullPath, e.what());
        return false;
    }

    //Saves Layout and Path
    //CHATGPT Suggested move as it is better in terms of memory. Also in this StackOverflow: https://stackoverflow.com/questions/3413470/what-is-stdmove-and-when-should-it-be-used
    m_levelJson = j;

    fmt::println("Successfully loaded level file: '{}'", fullPath);
    return true;
}

void EntityFactory::createFromFile(const std::string &path) noexcept {
    bool path_loaded = readPath(path);
    if (!path_loaded) {
        std::cout<< "Level <"<<path<<"> not found"<<std::endl;
    }

    float screen_width = config::CAMERA_VIEW_WIDTH;
    //float screen_height = config::CAMERA_VIEW_HEIGHT;

    //int level_id = m_levelJson["id"].get<int>();

    auto metadata = m_levelJson["meta"];
    int image_width = metadata["image_size"]["x"].get<int>();
    //int image_height = metadata["image_size"]["y"].get<int>();
    float scaling_factor = screen_width / image_width;
    //glm::vec2 scaling = {image_width * scaling_factor, image_height * scaling_factor};
    //(void) scaling;

    for (auto& object : metadata["images"]) {
        std::string image_path = m_texture_base_path + object["path"].get<std::string>();
        Texture text = Texture::init(image_path.c_str());

        RoomTexture room_text;
        room_text.pos = {0, 0};
        room_text.texture = text;

        m_registry->ecs.emplace<RoomTexture>(m_registry->ecs.create(), room_text);
    }

    for (auto& object : m_levelJson["objects"]) {
        //int id = object["id"].get<int>();
        std::string name = object["type"].get<std::string>();
        if (name == "Platform") {
            float x_pos = object["position"]["x"].get<float>();
            float y_pos = object["position"]["y"].get<float>();
            float width = object["size"]["w"].get<float>();
            float heigth = object["size"]["h"].get<float>();
            bool can_climb = object["can_climb"].get<bool>();
            bool touchable = object["touchable"].get<bool>();
            bool can_pogo = object["can_pogo"].get<bool>();
            entt::entity entity_id = m_registry->ecs.create();
            Platform pl;
            pl.can_climb = can_climb;
            pl.touchable = touchable;
            pl.can_pogo = can_pogo;
            m_registry->ecs.emplace<Platform>(entity_id, pl);
            m_registry->ecs.emplace<Position>(entity_id, glm::vec2(x_pos*scaling_factor, y_pos*scaling_factor));
            m_registry->ecs.emplace<Dimension>(entity_id, glm::vec2(width*scaling_factor, heigth*scaling_factor));
        } else if (name == "Polygon") {


            entt::entity entity_id = m_registry->ecs.create();

            float x_pos = object["position"]["x"].get<float>();
            float y_pos = object["position"]["y"].get<float>();

            std::vector<glm::vec2> vertices;
            for (auto& vertex : object["vertices"])
            {
                float x = vertex["x"].get<float>();
                float y = vertex["y"].get<float>();
                vertices.push_back({x*scaling_factor, y*scaling_factor});
            }

            PolygonShape poly = createPolygon(vertices);
            m_registry->ecs.emplace<PolygonShape>(entity_id, poly);
            m_registry->ecs.emplace<Position>(entity_id, glm::vec2(x_pos*scaling_factor, y_pos*scaling_factor));
        }
    }
}

/*
for (auto& object : m_levelJson["objects"])
{
    if (object["type"].get<std::string>() == "Platform")
    {
        int id = object["id"].get<int>();
        float x = object["position"]["x"].get<float>();
        float y = object["position"]["y"].get<float>();
        float w = object["dimension"]["w"].get<float>();
        float h = object["dimension"]["h"].get<float>();

        bool solid = true;
        if (object.contains("solid")) {
            solid = object["solid"].get<bool>();
        }
        bool teleportable = false;
        if (object.contains("teleportable")) {
            teleportable = object["teleportable"].get<bool>();
        }

        createPlatform(id, {x, y}, {w, h}, teleportable, solid);
    }
    */

