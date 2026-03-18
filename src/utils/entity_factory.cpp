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

bool EntityFactory::readPath(const std::string& path, nlohmann::json* json_link) noexcept {
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
    *json_link = std::move(j);

    fmt::println("Successfully loaded level file: '{}'", fullPath);
    return true;
}

void EntityFactory::createFromFile(const std::string &path) noexcept {
    bool path_loaded = readPath(path, &m_levelJson);
    if (!path_loaded) {
        std::cout<< "Level <"<<path<<"> not found"<<std::endl;
    }

    float screen_width = config::CAMERA_VIEW_WIDTH;
    int file_scale_width = m_levelJson["scale"]["x"].get<float>();
    float scaling_factor = screen_width / file_scale_width;

    glm::vec2 player_position = {m_levelJson["player"]["position"]["x"].get<float>()*scaling_factor, m_levelJson["player"]["position"]["y"].get<float>()*scaling_factor};
    m_registry->ecs.emplace_or_replace<Position>(m_registry->player(), player_position);

    for (auto& platform : m_levelJson["platforms"]) {
        int id = platform["id"].get<int>();
        std::string texture = platform["texture"].get<std::string>();
        glm::vec2 pos = {platform["pos"]["x"].get<float>()*scaling_factor, platform["pos"]["y"].get<float>()*scaling_factor};
        createPlatform(id, texture, pos);
    }

    /*

    //float screen_height = config::CAMERA_VIEW_HEIGHT;

    //int level_id = m_levelJson["id"].get<int>();


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
    */
}

void EntityFactory::createPlatform(int id, std::string texture, glm::vec2 pos) noexcept {
    (void) id;

    entt::entity platform_entity = m_registry->ecs.create();

    std::string base_path = "/assets/textures/world_objects/platforms/" + texture;
    bool path_loaded = readPath(base_path + ".json", &m_objectJson);
    if (!path_loaded) {
        std::cout<< "Platform <"<<base_path<<"> not found"<<std::endl;
    }

    float screen_width = config::CAMERA_VIEW_WIDTH;
    int file_scale_width = m_objectJson["scale"]["x"].get<float>();
    float scaling_factor = screen_width / file_scale_width;

    glm::vec2 size = {m_objectJson["size"]["x"].get<float>()*scaling_factor, m_objectJson["size"]["y"].get<float>()*scaling_factor};
    m_registry->ecs.emplace<Position>(platform_entity, pos);
    m_registry->ecs.emplace<Dimension>(platform_entity, size);

    Platform pl;
    m_registry->ecs.emplace<Platform>(platform_entity, pl);

    auto& properties = m_objectJson["properties"];
    for (auto& hitbox : m_objectJson["hitboxes"]) {
        entt::entity hitbox_entity = m_registry->ecs.create();
        glm::vec2 hitbox_pos = pos + glm::vec2{hitbox["pos"]["x"].get<float>()*scaling_factor, hitbox["pos"]["y"].get<float>()*scaling_factor};
        glm::vec2 hitbox_size = {hitbox["size"]["x"].get<float>()*scaling_factor, hitbox["size"]["y"].get<float>()*scaling_factor};
        Hitbox h;
        h.can_climb = properties["can_climb"].get<bool>();
        h.can_pogo = properties["can_pogo"].get<bool>();
        h.touchable = properties["touchable"].get<bool>();
        m_registry->ecs.emplace<Hitbox>(hitbox_entity, h);
        m_registry->ecs.emplace<Position>(hitbox_entity, hitbox_pos);
        m_registry->ecs.emplace<Dimension>(hitbox_entity, hitbox_size);
    }

    Texture platform_text = Texture::init((getBasePath() + base_path + ".png").c_str());
    m_registry->ecs.emplace<Texture>(platform_entity, platform_text);
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

