#include "render.h"
#include "config.h"
#include "core/registry.h"

#include <iostream>


constexpr int LIGHT_EFFECT_FRAMECOUNT = 32;          // number of frames in light effect texture
constexpr float LIGHT_EFFECT_ANIMATION_SPEED = 0.1f;        // time (in seconds) per frame
constexpr float SHADOW_EXTEND_DISTANCE = 100.0f;  // distance of shadow cast



RenderSystem RenderSystem::init(Window *window, Registry *registry, ParticleSystem* particles) noexcept {
    RenderSystem self;

    self.m_window = window;
    self.m_registry = registry;
	self.m_particles = particles;
    self.m_registry->m_viewport.m_origin = glm::uvec2(0);
	self.m_registry->m_viewport.m_size = self.m_window->windowExtent();

    // load resources for rendering
    self.m_player_mesh = Mesh::init(assets::Mesh::player).value();

    self.m_player_shader = Shader::init(assets::Shader::player).value();
    self.m_line_shader = Shader::init(assets::Shader::line).value();
	self.m_polygon_shader = Shader::init(assets::Shader::polygon).value();
	self.m_platform_shader=Shader::init(assets::Shader::platform).value();
    self.m_illumination_shader = Shader::init(assets::Shader::illumination).value();
    self.m_beam_illumination_shader = Shader::init(assets::Shader::beam_illumination).value();
	self.m_text_shader = Shader::init(assets::Shader::text).value();
	self.m_sprite_shader = Shader::init(assets::Shader::sprite).value();
	self.m_circle_particle_shader = Shader::init(assets::Shader::circle_particle).value();

	self.m_door_texture=Texture::init(assets::Texture::door);
	self.m_animation_shader = Shader::init(assets::Shader::animation).value();
	self.m_beam_shader = Shader::init(assets::Shader::beam).value();
	self.m_shadow_shader = Shader::init(assets::Shader::shadow).value();


    self.m_player_animation_texture = Texture::init(assets::Texture::player_animation);

	glm::uvec2 extent = self.m_window->windowExtent();
	self.m_intermediate_framebuffer = Framebuffer::init({ static_cast<GLsizei>(extent.x), static_cast<GLsizei>(extent.y), 1, true });
    self.m_shadow_framebuffer = Framebuffer::init({ static_cast<GLsizei>(extent.x), static_cast<GLsizei>(extent.y), 1, true });
    self.m_beam_shadow_framebuffer = Framebuffer::init({ static_cast<GLsizei>(extent.x), static_cast<GLsizei>(extent.y), 1, true });

    glGenVertexArrays(1, &self.m_quad_vao);
    glBindVertexArray(self.m_quad_vao);
    glGenBuffers(1, &self.m_quad_ebo);

    constexpr std::array<GLuint, 6> indices = {0, 1, 2, 2, 3, 0};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.m_quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// line VAO/VBO
    glGenVertexArrays(1, &self.m_line_vao);
    glBindVertexArray(self.m_line_vao);
    glGenBuffers(1, &self.m_line_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self.m_line_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 2, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	// triangle VAO/VBO
    glGenVertexArrays(1, &self.m_triangle_vao);
    glBindVertexArray(self.m_triangle_vao);
	glGenBuffers(1, &self.m_triangle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self.m_triangle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 3, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	glGenBuffers(1, &self.m_triangle_ebo);

	// platform VAO/VBO
    glGenVertexArrays(1, &self.m_platform_vao);
    glBindVertexArray(self.m_platform_vao);

    glGenBuffers(1, &self.m_platform_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self.m_platform_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

    GLuint platform_indices[6] = { 0,2,1,1,2,3 };
    glGenBuffers(1, &self.m_platform_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.m_platform_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platform_indices), platform_indices, GL_STATIC_DRAW);

    // Shadow VAO/VBO
    glGenVertexArrays(1, &self.m_shadow_vao);
    glBindVertexArray(self.m_shadow_vao);
    glGenBuffers(1, &self.m_shadow_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, self.m_shadow_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAX_SHADOW_VERTICES, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

	// Load Font
    if (FT_Init_FreeType(&self.ft_library)) {
        std::cout<<"Error initializing FreeType Library"<<std::endl;
    }
    if (FT_New_Face(self.ft_library, (getBasePath() + "/assets/fonts/arial.ttf").c_str(), 0, &self.ft_face)) {
        std::cout<<"Error loading Font"<<std::endl;
    }
    FT_Set_Pixel_Sizes(self.ft_face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(self.ft_face, c, FT_LOAD_RENDER)) {
            std::cout<<"Error loading Glyph for "<<c<<std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            self.ft_face->glyph->bitmap.width, self.ft_face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, self.ft_face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character new_char = {
            texture,
            glm::ivec2(self.ft_face->glyph->bitmap.width, self.ft_face->glyph->bitmap.rows),
            glm::ivec2(self.ft_face->glyph->bitmap_left, self.ft_face->glyph->bitmap_top),
            static_cast<unsigned int>(self.ft_face->glyph->advance.x)
        };

        self.Characters.insert(std::pair<char, Character>(c, new_char));

    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Done_Face(self.ft_face);
    FT_Done_FreeType(self.ft_library);

    glGenVertexArrays(1, &self.m_text_vao);
    glGenBuffers(1, &self.m_text_vbo);
    glBindVertexArray(self.m_text_vao);
    glBindBuffer(GL_ARRAY_BUFFER, self.m_text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,4,GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	glGenVertexArrays(1, &self.m_circle_vao);
	glBindVertexArray(self.m_circle_vao);
	glGenBuffers(1, &self.m_circle_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, self.m_circle_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*6, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(glm::vec2),nullptr);


    return self;
}

void RenderSystem::deinit() noexcept {
    m_player_mesh.deinit();
    m_player_shader.deinit();
    m_line_shader.deinit();
	m_polygon_shader.deinit();
    m_platform_shader.deinit();
	m_circle_particle_shader.deinit();
    m_illumination_shader.deinit();
    m_beam_illumination_shader.deinit();
	m_text_shader.deinit();
	m_sprite_shader.deinit();
	m_door_texture.deinit();
	m_animation_shader.deinit();
    m_shadow_shader.deinit();
    m_shadow_framebuffer.deinit();
    m_beam_shadow_framebuffer.deinit();
	m_beam_shader.deinit();

}

// Returns UV coordinates for a given frame index in a sprite sheet with specified columns and rows, implemented by ChatGPT
glm::vec4 RenderSystem::getFrameUV(int index, int columns, int rows) noexcept {
    float frameWidth = 1.0f / columns;
    float frameHeight = 1.0f / rows;

    int x = index % columns;
    int y = index / columns;

    y = index;

    glm::vec4 uv;
    uv.x = x * frameWidth;
    uv.y = y * frameHeight;
    uv.z = uv.x + frameWidth;
    uv.w = uv.y + frameHeight;

    return uv;
}

glm::mat4 RenderSystem::getViewMatrix() noexcept {
	return glm::lookAt(glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}


// TODO (same for both functions): only works if spritesheet is completely filled out, i.e. no empty space at the end
void RenderSystem::spritesheetNormalAnimation(int framecount, float animationSpeed, glm::vec2 spritesheetSize, Shader& shader, entt::entity entity, const char* textureString, Texture texture, const glm::mat4& projection) noexcept {
    const float time = static_cast<float>(glfwGetTime());
    const float frame_float = fmod(time, animationSpeed * framecount) / animationSpeed;    // current frame as float
    const int frameA = static_cast<int>(frame_float);               // current frame
    const int frameB = (frameA + 1) % framecount;      // next frame (with wrap-around)

    const float t = frame_float - frameA;       // interpolation factor between frameA and frameB (0.0 to 1.0)

    glm::vec4 uvA = getFrameUV(frameA, spritesheetSize.x, spritesheetSize.y);
    glm::vec4 uvB = getFrameUV(frameB, spritesheetSize.x, spritesheetSize.y);

    glBindVertexArray(m_quad_vao);

    Shader& light_effect_shader = shader;

    glm::vec2 position = m_registry->ecs.get<Position>(entity).pos;        // Position of the texture
    glm::vec2 scale = m_registry->ecs.get<Scale>(entity).scale;             // Scaling of the texture
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::scale(model, glm::vec3(scale, 1));

    light_effect_shader.use()
        .setMat4("view", getViewMatrix())
        .setMat4("projection", projection)
        .setMat4("model", model)
        .setSampler(textureString, 0)
        .setFloat("t", t)
        .setVec4("uvA", uvA)
        .setVec4("uvB", uvB);
    texture.bind(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void RenderSystem::spritesheetPingpongAnimation(int frameCount, float animationSpeed, glm::vec2 spritesheetSize, Shader& shader, entt::entity entity, const char* textureString, Texture texture, const glm::mat4& projection) noexcept {
    // I think there's probably a smarter way to do all this, but that's all I could think of
    int frameindex = frameCount - 1;      // index of corresponding frame
	float time = static_cast<float>(glfwGetTime());

    float frame_float = fmod(time, animationSpeed * frameindex * 2.f) / animationSpeed;    // current frame as float

    int pingpong_sign = 1;
    bool reverse = floor(frame_float) >= (frameCount);
    if (reverse) {
        // reverse animation for ping-pong effect
        frame_float = (frameindex * 2.f) + 1 - frame_float;
        pingpong_sign = -1;
    }

    int frameA = static_cast<int>(frame_float);               // current frame
    int frameB = frameA + pingpong_sign;                            // next frame (with pingpong)

    if (frameB < 0) {
        frameB = 1;
    }
    else if (frameB >= frameCount) {
        frameB = frameindex - 1;
    }

    // interpolation factor for frameA and frameB, with adjustment for reverse direction
    float t = reverse ? 1 - (frame_float - frameA) : frame_float - frameA;

    glm::vec4 uvA = getFrameUV(frameA, spritesheetSize.x, spritesheetSize.y);
    glm::vec4 uvB = getFrameUV(frameB, spritesheetSize.x, spritesheetSize.y);

    glBindVertexArray(m_quad_vao);

    Shader& light_effect_shader = shader;

	glm::vec2 position = m_registry->ecs.get<Position>(entity).pos;        // Position of the texture
	glm::vec2 scale = m_registry->ecs.get<Scale>(entity).scale;             // Scaling of the texture
    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(position, 0));
    model = glm::scale(model, glm::vec3(scale, 1));

    light_effect_shader.use()
        .setMat4("view", getViewMatrix())
        .setMat4("projection", projection)
        .setMat4("model", model)
        .setSampler(textureString, 0)
        .setFloat("t", t)
        .setVec4("uvA", uvA)
        .setVec4("uvB", uvB);
    texture.bind(0);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void RenderSystem::step(const float /*delta*/) noexcept {
    Viewport viewport = m_registry->m_viewport;


	// Camera-derived projection (world scrolling)
	const auto& cam = m_registry->m_camera;
	glm::vec2 half = 0.5f * cam.viewSize;
	glm::mat4 view = getViewMatrix();
	glm::mat4 projection = glm::ortho(
		cam.center.x - half.x, cam.center.x + half.x,
		cam.center.y - half.y, cam.center.y + half.y,
		-1.f, 1.f
	);

    glm::uvec2 extent = m_intermediate_framebuffer.extent();
    glViewport(0, 0, extent.x, extent.y);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // --- Draw player (FIXED: actually draws and closes scope)
    // render scene


	// Draw Player
	glm::vec2 player_pos = m_registry->ecs.get<Position>(m_registry->player()).pos;        // Position of the texture
	glm::vec2 player_size = m_registry->ecs.get<Dimension>(m_registry->player()).dim;
	//float player_radius = m_registry->ecs.get<Radius>(m_registry->player()).r;        // Position of the texture
	//drawQuadBySize(player_pos, player_size, glm::vec3(0,1,0), 1, view, projection);
	drawQuad(player_pos+glm::vec2(0,player_size.y), player_pos+player_size,player_pos, player_pos+glm::vec2(player_size.x,0),glm::vec3(0,1,0), 1, view, projection);
	//drawCircleParticle(player_pos, player_radius, glm::vec3(0, 1, 0), 1, view, projection);


    // Draw Plattform
    {
        for (entt::entity platform_entity: m_registry->ecs.view<Platform>()) {
            glm::vec2 pos1 = m_registry->ecs.get<Position>(platform_entity).pos;
            glm::vec2 size = m_registry->ecs.get<Dimension>(platform_entity).dim;

            glm::vec2 pos2 = pos1 + glm::vec2{0, size.y};
            glm::vec2 pos3 = pos1 + glm::vec2{size.x, 0};
            glm::vec2 pos4 = pos3 + glm::vec2{0, size.y};

        	float opacity = 1.f;
        	glm::vec3 color = glm::vec3{ 1.f, 0.f, 0.f };

/*
        	if (m_app->debug_mode) {
        		bool b = false;
        		for (Entity platform_collision_entity : m_registry->hitting) {
        			if (platform_entity == platform_collision_entity) {
        				b = true;
        				break;
        			}
        		}
        		if (b) {
        			color = {1.f, 0.f, 0.f};
        		}
        	}
*/
        	drawQuad(pos1, pos2, pos3, pos4, color, opacity, view, projection);
		}
    	//m_registry->hitting.clear();
	}

    glViewport(viewport.m_origin.x, viewport.m_origin.y,
               viewport.m_size.x, viewport.m_size.y);
}

void RenderSystem::drawString(std::string text, glm::vec2 pos, float scale, glm::vec3 color, glm::mat4 view,
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

void RenderSystem::drawRainbowString(std::string text, glm::vec2 pos, float scale, std::vector<glm::vec3> colors, std::vector<int> color_ids, glm::mat4 view, glm::mat4 projection) noexcept {

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

void RenderSystem::drawCharacter(Character ch, glm::vec2 position, float scale) noexcept {
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

float RenderSystem::getStringWidth(std::string text, float scale) noexcept {
	std::string::const_iterator c;
	float whole_width = 0.0f;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];
		whole_width += (ch.advance >> 6) * scale;
	}
	return whole_width;
}

void RenderSystem::drawBeam(glm::vec2 start, glm::vec2 end, glm::vec3 color,
							float half_width, float core_ratio,
							glm::mat4 view, glm::mat4 projection) noexcept
{
	glm::vec2 d = end - start;
	float len = glm::length(d);
	if (len < 1e-6f) return;

	glm::vec2 dir = d / len;
	glm::vec2 n   = glm::vec2{-dir.y, dir.x};

	glm::vec2 tl = start + n * half_width;
	glm::vec2 tr = end   + n * half_width;
	glm::vec2 bl = start - n * half_width;
	glm::vec2 br = end   - n * half_width;

	glm::vec2 points[4] = { tl, tr, bl, br };

	glBindVertexArray(m_platform_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_platform_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

	m_beam_shader.use()
		.setMat4("view", view)
		.setMat4("projection", projection)
		.setVec3("color", color)
		.setVec2("beam_start", start)
		.setVec2("beam_end", end)
		.setFloat("half_width", half_width)
		.setFloat("core_ratio", core_ratio);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderSystem::drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, glm::mat4 view,
                            glm::mat4 projection) noexcept {
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

void RenderSystem::drawArrow(glm::vec2 start_point, glm::vec2 end_point, glm::vec3 color, float line_width, glm::mat4 view, glm::mat4 projection) noexcept {
	glm::vec2 direction = glm::normalize(end_point - start_point);
	glm::vec2 perp = glm::vec2(-direction.y, direction.x);

	glm::vec2 arrow_1_direction = glm::normalize(-direction + perp);
	glm::vec2 arrow_1_perp = glm::vec2(-arrow_1_direction.y, arrow_1_direction.x);
	glm::vec2 arrow_2_direction = glm::normalize(-direction - perp);
	glm::vec2 arrow_2_perp = glm::vec2(-arrow_2_direction.y, arrow_2_direction.x);

	drawQuad(end_point - direction * 0.003f + perp * line_width,
		end_point - direction * 0.003f - perp * line_width,
		start_point + perp * line_width,
		start_point - perp * line_width,
		color, 1.f, view, projection);
	drawQuad(end_point + arrow_1_direction * 0.1f + arrow_1_perp * line_width,
		end_point - line_width * perp + arrow_1_perp * line_width,
		end_point + arrow_1_direction * 0.1f - arrow_1_perp * line_width,
		end_point - line_width * perp - arrow_1_perp * line_width,
		color, 1.f, view, projection);
	drawQuad(end_point + arrow_2_direction * 0.1f + arrow_2_perp * line_width,
		end_point + arrow_2_perp * line_width,
		end_point + arrow_2_direction * 0.1f - arrow_2_perp * line_width,
		end_point - arrow_2_perp * line_width,
		color, 1.f, view, projection);
}


void RenderSystem::drawQuad(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
	//draws a black quad
	glm::vec2 points[4] = { topleft, topright, bottomleft, bottomright };

	glBindVertexArray(m_platform_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_platform_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);

	m_platform_shader.use()
	.setMat4("view", view)
	.setMat4("projection", projection)
	.setVec3("fcolor", color)
	.setFloat("opacity", opacity);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderSystem::drawQuadBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
	float w = size.x/2;
	float h = size.y/2;

	glm::vec2 tl = center + glm::vec2{-w, h};
	glm::vec2 tr = center + glm::vec2{w, h};
	glm::vec2 bl = center + glm::vec2{-w, -h};
	glm::vec2 br = center + glm::vec2{w, -h};

	drawQuad(tl, tr, bl, br, color, opacity, view, projection);
}

void RenderSystem::drawCircleParticle(glm::vec2 position, float radius, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
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

	m_circle_particle_shader.use()
		.setMat4("view", view)
		.setMat4("projection", projection)
		.setVec3("fcolor", color)
		.setFloat("opacity", opacity)
		.setVec2("center", position)
		.setFloat("radius", radius);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RenderSystem::drawQuadraticMirror(glm::vec2 topleft, glm::vec2 topright, glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept {
	drawQuad(topleft, topright, bottomleft, bottomright, color, opacity, view, projection);
}

void RenderSystem::reset() noexcept {
}

static std::pair<glm::uvec2, glm::uvec2> computeViewportConfig(const glm::uvec2 resolution) noexcept {
    float window_aspect_ratio = (float) resolution.x / resolution.y;
    if (window_aspect_ratio > config::GAME_ASPECT_RATIO) {
        glm::uvec2 viewport_size = {resolution.y * config::GAME_ASPECT_RATIO, resolution.y};
        uint32_t offset = (resolution.x - viewport_size.x) / 2;
        glm::uvec2 viewport_offset = {offset, 0};

        return {viewport_offset, viewport_size};
    }

    glm::uvec2 viewport_size = {resolution.x, resolution.x / config::GAME_ASPECT_RATIO};
    uint32_t offset = (resolution.y - viewport_size.y) / 2;
    glm::uvec2 viewport_offset = {0, offset};

    return {viewport_offset, viewport_size};
}

void RenderSystem::drawPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color, glm::mat4 projection) noexcept {
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

void RenderSystem::onResizeCallback(GLFWwindow *, int width, int height) noexcept {
    auto [origin, size] = computeViewportConfig({width, height});
	m_registry->m_viewport.m_origin = origin;
    m_registry->m_viewport.m_size = size;

    // reinitialize off-screen framebuffer
    m_intermediate_framebuffer.deinit();
    m_intermediate_framebuffer = Framebuffer::init({(GLsizei) size.x, (GLsizei) size.y, 1, true});
    m_shadow_framebuffer.deinit();
    m_shadow_framebuffer = Framebuffer::init({(GLsizei) size.x, (GLsizei) size.y, 1, true});
	m_beam_shadow_framebuffer.deinit();
	m_beam_shadow_framebuffer = Framebuffer::init({(GLsizei) size.x, (GLsizei) size.y, 1, true});
}
