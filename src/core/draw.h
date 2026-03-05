//
// Created by luisb on 05.03.2026.
//
#pragma once

#include <utils/mesh.h>
#include <utils/shader.h>
#include <utils/framebuffer.h>
#include <core/registry.h>

#include "window.h"
#include "freetype/freetype.h"


struct Character {
    unsigned int id;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};



class Draw {
public:
    Draw() = default;
    ~Draw() = default;

    static Draw *init(Window* window) noexcept;
    void deinit() noexcept;

    /*
     * Geometry
     */
    void drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, glm::mat4 view, glm::mat4 projection) noexcept;

    //TODO void drawRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;
    //TODO void drawRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;
    void fillRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;
    void fillRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;

    //TODO void drawCircle(glm::vec2 position, float radius, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;
    void fillCircle(glm::vec2 position, float radius, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;

    void drawPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color, glm::mat4 projection) noexcept;
    void fillPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color, glm::mat4 projection) noexcept;


    /*
     * Forms
     */
    void drawArrow(glm::vec2 start_point, glm::vec2 end_point, glm::vec3 color, float line_width, glm::mat4 view, glm::mat4 projection) noexcept;

    /*
     * Text
     */
    void drawString(std::string text, glm::vec2 pos, float scale, glm::vec3 color, glm::mat4 view, glm::mat4 projection) noexcept;
    void drawRainbowString(std::string text, glm::vec2 pos, float scale, std::vector<glm::vec3> colors, std::vector<int> color_ids, glm::mat4 view, glm::mat4 projection) noexcept;
    void drawCharacter(Character ch, glm::vec2 position, float scale) noexcept;
    float getStringWidth(std::string text, float scale) noexcept;


    /*
     * Helpers
     */
    glm::mat4 getViewMatrix() noexcept;

private:
    Window* m_window;

    Mesh m_player_mesh;
    Shader m_player_shader;
    Shader m_line_shader;
    Shader m_platform_shader;
    Shader m_text_shader;
    //Shader m_animation_shader;
    //Shader m_sprite_shader;

    Shader m_circle_shader;

    Shader m_polygon_shader;

    //Texture m_player_animation_texture;

    //GLuint m_quad_vao;
    //GLuint m_quad_ebo;

    struct Line { glm::vec2 position; };
    GLuint m_line_vao;
    GLuint m_line_vbo;

    GLuint m_triangle_vao;
    GLuint m_triangle_vbo;
    GLuint m_triangle_ebo;

    struct platform {
        glm::vec2 m_position;
        glm::vec2 m_size;
    };
    GLuint m_rect_vao;
    GLuint m_rect_vbo;
    GLuint m_rect_ebo;

    FT_Library ft_library;
    FT_Face ft_face;
    std::map<char, Character> Characters;
    GLuint m_text_vao;
    GLuint m_text_vbo;


    GLuint m_circle_vao;
    GLuint m_circle_vbo;

};


