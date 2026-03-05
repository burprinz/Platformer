//
// Created by luisb on 05.03.2026.
//

#include "draw.h"

#include <iostream>

Draw *Draw::init(Window* window) noexcept {
    Draw *self = new Draw;
    self->m_window = window;

     // load resources for rendering
    self->m_player_mesh = Mesh::init(assets::Mesh::player).value();

    self->m_player_shader = Shader::init(assets::Shader::player).value();
    self->m_line_shader = Shader::init(assets::Shader::line).value();
	self->m_polygon_shader = Shader::init(assets::Shader::polygon).value();
	self->m_platform_shader=Shader::init(assets::Shader::platform).value();
	self->m_text_shader = Shader::init(assets::Shader::text).value();

	// line VAO/VBO
    glGenVertexArrays(1, &self->m_line_vao);
    glBindVertexArray(self->m_line_vao);
    glGenBuffers(1, &self->m_line_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_line_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	// triangle VAO/VBO
    glGenVertexArrays(1, &self->m_triangle_vao);
    glBindVertexArray(self->m_triangle_vao);
	glGenBuffers(1, &self->m_triangle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self->m_triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 3, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	glGenBuffers(1, &self->m_triangle_ebo);

	// platform VAO/VBO
    glGenVertexArrays(1, &self->m_rect_vao);
    glBindVertexArray(self->m_rect_vao);

    glGenBuffers(1, &self->m_rect_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_rect_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    GLuint platform_indices[6] = { 0,2,1,1,2,3 };
    glGenBuffers(1, &self->m_rect_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->m_rect_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platform_indices), platform_indices, GL_STATIC_DRAW);

	// Load Font
    if (FT_Init_FreeType(&self->ft_library)) {
        std::cout<<"Error initializing FreeType Library"<<std::endl;
    }
    if (FT_New_Face(self->ft_library, (getBasePath() + "/assets/fonts/arial.ttf").c_str(), 0, &self->ft_face)) {
        std::cout<<"Error loading Font"<<std::endl;
    }
    FT_Set_Pixel_Sizes(self->ft_face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(self->ft_face, c, FT_LOAD_RENDER)) {
            std::cout<<"Error loading Glyph for "<<c<<std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            self->ft_face->glyph->bitmap.width, self->ft_face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, self->ft_face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character new_char = {
            texture,
            glm::ivec2(self->ft_face->glyph->bitmap.width, self->ft_face->glyph->bitmap.rows),
            glm::ivec2(self->ft_face->glyph->bitmap_left, self->ft_face->glyph->bitmap_top),
            static_cast<unsigned int>(self->ft_face->glyph->advance.x)
        };

        self->Characters.insert(std::pair<char, Character>(c, new_char));

    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(self->ft_face);
    FT_Done_FreeType(self->ft_library);

    glGenVertexArrays(1, &self->m_text_vao);
    glGenBuffers(1, &self->m_text_vbo);
    glBindVertexArray(self->m_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, self->m_text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	glGenVertexArrays(1, &self->m_circle_vao);
	glBindVertexArray(self->m_circle_vao);
	glGenBuffers(1, &self->m_circle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self->m_circle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*6, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(glm::vec2),nullptr);


    return self;
}

void Draw::deinit() noexcept {
    m_player_mesh.deinit();
    m_player_shader.deinit();
    m_line_shader.deinit();
    m_polygon_shader.deinit();
    m_platform_shader.deinit();
    m_circle_shader.deinit();
    m_text_shader.deinit();
}

void Draw::drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, glm::mat4 view, glm::mat4 projection) noexcept {
    glBindVertexArray(m_line_vao);
    glm::vec2 points[2] = {start, end};

    glBindBuffer(GL_ARRAY_BUFFER, m_line_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

    m_line_shader.use()
            .setMat4("view", view)
            .setMat4("projection", projection)
            .setVec3("color", color);

    glLineWidth(1);
    glDrawArrays(GL_LINES, 0, 2);
}

void Draw::fillRect(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
    //draws a black quad
    glm::vec2 points[4] = { topleft, topright, bottomleft, bottomright };

    glBindVertexArray(m_rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

    m_platform_shader.use()
    .setMat4("view", view)
    .setMat4("projection", projection)
    .setVec3("fcolor", color)
    .setFloat("opacity", opacity);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Draw::fillRectBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
    float w = size.x/2;
    float h = size.y/2;

    glm::vec2 tl = center + glm::vec2{-w, h};
    glm::vec2 tr = center + glm::vec2{w, h};
    glm::vec2 bl = center + glm::vec2{-w, -h};
    glm::vec2 br = center + glm::vec2{w, -h};

    fillRect(tl, tr, bl, br, color, opacity, view, projection);
}

void Draw::fillCircle(glm::vec2 position, float radius, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
    glm::vec2 tl = {-radius, radius};
    glm::vec2 tr = {radius, radius};
    glm::vec2 bl = {-radius, -radius};
    glm::vec2 br = {radius, -radius};
    glm::vec2 triangles[6] = {
        tl, bl, tr,
        tr, bl, br
    };

    glBindVertexArray(m_circle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_circle_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangles), triangles);

    m_circle_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec3("fcolor", color)
        .setFloat("opacity", opacity)
        .setVec2("center", position)
        .setFloat("radius", radius);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Draw::drawPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color, glm::mat4 projection) noexcept {
    std::vector<glm::vec2> vertices = polygon.vertices;
    std::vector<int> indices = polygon.triangle_indices;
    if (vertices.size() < 3) {
        return; // Not enough vertices to form a polygon
    }

    glBindVertexArray(m_triangle_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_triangle_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_DYNAMIC_DRAW);

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(scale, 1.0f));

    m_polygon_shader.use()
        .setMat4("view", getViewMatrix())
        .setMat4("projection", projection)
        .setMat4("model", model)
        .setVec3("color", color);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}



void Draw::drawString(std::string text, glm::vec2 pos, float scale, glm::vec3 color, glm::mat4 view,
                            glm::mat4 projection) noexcept {

    m_text_shader.use()
        .setMat4("view", view)
        .setMat4("projection", projection)
        .setVec3_f("textColor", color)
        .setSampler("text" , 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_text_vao);

    float text_width = getStringWidth(text, scale);
    pos.x -= text_width/2.0f;
    pos.y -= Characters[0].size.y*scale/2.0f;

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        glm::vec2 position = {pos.x + ch.bearing.x*scale, pos.y - (ch.size.y - ch.bearing.y) * scale};

        drawCharacter(ch, position, scale);

        pos.x += (ch.advance >> 6) * scale;
    }
}

void Draw::drawRainbowString(std::string text, glm::vec2 pos, float scale, std::vector<glm::vec3> colors, std::vector<int> color_ids, glm::mat4 view, glm::mat4 projection) noexcept {

    m_text_shader.use()
    .setMat4("view", view)
    .setMat4("projection", projection)
    .setSampler("text" , 0);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_text_vao);

    float text_width = getStringWidth(text, scale);
    pos.x -= text_width/2.0f;
    pos.y -= Characters[0].size.y*scale/2.0f;

    std::string::const_iterator c;
    int i = 0;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        m_text_shader.setVec3_f("textColor", colors[color_ids[i]]);

        glm::vec2 position = {pos.x + ch.bearing.x*scale, pos.y - (ch.size.y - ch.bearing.y) * scale};

        drawCharacter(ch, position, scale);

        pos.x += (ch.advance >> 6) * scale;
        i += 1;
    }
}

void Draw::drawCharacter(Character ch, glm::vec2 position, float scale) noexcept {
    float w = ch.size.x * scale;
    float h = ch.size.y * scale;
    float verts[6][4] = {
        {position.x, position.y+h, 0.0f, 0.0f},
        {position.x, position.y, 0.0f, 1.0f},
        {position.x + w, position.y, 1.0f, 1.0f},
        {position.x, position.y+h, 0.0f, 0.0f},
        {position.x + w, position.y, 1.0f, 1.0f},
        {position.x + w, position.y + h, 1.0f, 0.0f}
    };
    glBindTexture(GL_TEXTURE_2D, ch.id);
    glBindBuffer(GL_ARRAY_BUFFER, m_text_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

float Draw::getStringWidth(std::string text, float scale) noexcept {
    std::string::const_iterator c;
    float whole_width = 0.0f;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];
        whole_width += (ch.advance >> 6) * scale;
    }
    return whole_width;
}


void Draw::drawArrow(glm::vec2 start_point, glm::vec2 end_point, glm::vec3 color, float line_width, glm::mat4 view, glm::mat4 projection) noexcept {
    glm::vec2 direction = glm::normalize(end_point - start_point);
    glm::vec2 perp = glm::vec2(-direction.y, direction.x);

    glm::vec2 arrow_1_direction = glm::normalize(-direction + perp);
    glm::vec2 arrow_1_perp = glm::vec2(-arrow_1_direction.y, arrow_1_direction.x);
    glm::vec2 arrow_2_direction = glm::normalize(-direction - perp);
    glm::vec2 arrow_2_perp = glm::vec2(-arrow_2_direction.y, arrow_2_direction.x);

    fillRect(end_point - direction * 0.003f + perp * line_width,
        end_point - direction * 0.003f - perp * line_width,
        start_point + perp * line_width,
        start_point - perp * line_width,
        color, 1.f, view, projection);
    fillRect(end_point + arrow_1_direction * 0.1f + arrow_1_perp * line_width,
        end_point - line_width * perp + arrow_1_perp * line_width,
        end_point + arrow_1_direction * 0.1f - arrow_1_perp * line_width,
        end_point - line_width * perp - arrow_1_perp * line_width,
        color, 1.f, view, projection);
    fillRect(end_point + arrow_2_direction * 0.1f + arrow_2_perp * line_width,
        end_point + arrow_2_perp * line_width,
        end_point + arrow_2_direction * 0.1f - arrow_2_perp * line_width,
        end_point - arrow_2_perp * line_width,
        color, 1.f, view, projection);
}



glm::mat4 Draw::getViewMatrix() noexcept {
    return glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}