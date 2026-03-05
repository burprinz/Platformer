#pragma once

#include <core/registry.h>
#include <window.h>

#include <utils/mesh.h>
#include <utils/shader.h>
#include <utils/framebuffer.h>

#include "particle.h"
#include "freetype/freetype.h"

struct Character {
	unsigned int id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class RenderSystem {
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		static RenderSystem init(Window* window, Registry* registry, ParticleSystem* particles) noexcept;
		void deinit() noexcept;
		void step(const float delta) noexcept;
		void reset() noexcept;
		void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;

		void spritesheetNormalAnimation(int framecount, float animationSpeed, glm::vec2 spritesheetSize, Shader& shader, entt::entity entity, const char* textureString, Texture texture, const glm::mat4& projection) noexcept;
		void spritesheetPingpongAnimation(int frameCount, float animationSpeed, glm::vec2 spritesheetSize, Shader& shader, entt::entity entity, const char* textureString, Texture texture, const glm::mat4& projection) noexcept;

		glm::mat4 getViewMatrix() noexcept;
		glm::vec4 getFrameUV(int index, int columns, int rows) noexcept;

		void drawLine(glm::vec2 start, glm::vec2 end, glm::vec3 color, glm::mat4 view, glm::mat4 projection) noexcept;
		void drawBeam(glm::vec2 start, glm::vec2 end, glm::vec3 color, float half_width, float core_ratio, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws Arrows with start_point, end_point and direction
		 * @param start_point the start point
		 * @param end_point the end point
		 * @param color the color
		 * @param line_width the line width (divided by two)
		 * @param view view of the step algorithm
		 * @param projection projection of the step algorithm
		 */
		void drawArrow(glm::vec2 start_point, glm::vec2 end_point, glm::vec3 color, float line_width, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws a quad with pos and size
		 * @param center the center pos
		 * @param size the quad size
		 * @param color the color of the quad
		 * @param opacity the opacity of the quad
		 * @param view view of the step algorithm
		 * @param projection projection of the step algorithm
		 */
		void drawQuadBySize(glm::vec2 center, glm::vec2 size, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws a quad on the screen
		 * @param topleft topleft corner
		 * @param topright topright corner
		 * @param bottomleft botleft corner
		 * @param bottomright botright corner
		 * @param color the color of the quad
		 * @param opacity the opacity of the quad
		 * @param view view of the step algorithm
		 * @param projection projection of the step algorithm
		 */
		void drawQuad(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws a qadratic mirror on the screen
		 * @param topleft topleft corner
		 * @param topright topright corner
		 * @param bottomleft botleft corner
		 * @param bottomright botright corner
		 * @param color color
		 * @param view view of the step algorithm
		 * @param projection projection of the step algorithm
		 */
		void drawQuadraticMirror(glm::vec2 topleft, glm::vec2 topright,glm::vec2 bottomleft, glm::vec2 bottomright, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws a text on the screen
		 * @param text the string
		 * @param pos the center position
		 * @param scale the size
		 * @param color the text color
		 * @param view view of the step algorithm
		 * @param projection projection of the step algorithm
		 */
		void drawString(std::string text, glm::vec2 pos, float scale, glm::vec3 color, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws a colored string on the screen
		 * @param text the text
		 * @param pos the center pos
		 * @param scale the size
		 * @param colors the color list
		 * @param color_ids the color-id list for every char
		 * @param view view of the step algorithm
		 * @param projection projection of the step algorithm
		 */
		void drawRainbowString(std::string text, glm::vec2 pos, float scale, std::vector<glm::vec3> colors, std::vector<int> color_ids, glm::mat4 view, glm::mat4 projection) noexcept;

		/**
		 * Draws a single character on the screen
		 * @param ch the char
		 * @param position the position
		 * @param scale the size
		 */
		void drawCharacter(Character ch, glm::vec2 position, float scale) noexcept;

		/**
		 * Returns the whole width of a string
		 * @param text the string
		 * @param scale the size
		 * @return the width
		 */
		float getStringWidth(std::string text, float scale) noexcept;

		void drawSlider(glm::vec2 center, float width, float slider_position, glm::vec3 line_color, glm::vec3 dot_color, glm::mat4 view, glm::mat4 projection) noexcept;
		void drawCircleParticle(glm::vec2 position, float radius, glm::vec3 color, float opacity, glm::mat4 view, glm::mat4 projection) noexcept;
		void drawPolygon(glm::vec2 position, glm::vec2 scale, PolygonShape polygon, glm::vec3 color, glm::mat4 projection) noexcept;

	private:
		Registry* m_registry;
		Window* m_window;
		ParticleSystem* m_particles;

		// UI transitions
		float m_level_end_fade = 0.f;
		float m_level_end_fade_speed = 2.0f;

		Framebuffer m_intermediate_framebuffer;
		Framebuffer m_shadow_framebuffer;
		Framebuffer m_beam_shadow_framebuffer;

		Mesh m_player_mesh;
		Shader m_player_shader;
        Shader m_line_shader;
		Shader m_platform_shader;
		Shader m_illumination_shader;
		Shader m_beam_illumination_shader;
		Shader m_text_shader;
		Shader m_animation_shader;
		Shader m_sprite_shader;
		Shader m_shadow_shader;

		Shader m_circle_particle_shader;

		Shader m_polygon_shader;
		Shader m_beam_shader;

		Texture m_door_texture;
		Texture m_player_animation_texture;

		GLuint m_light_effect_vao;
		GLuint m_light_effect_vbo;

		GLuint m_quad_vao;
		GLuint m_quad_ebo;

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
		GLuint m_platform_vao;
		GLuint m_platform_vbo;
		GLuint m_platform_ebo;

		struct mirrir {
			glm::vec2 m_position;
			glm::vec2 m_size;
			float angle;
		};
		GLuint m_mirror_vao;
		GLuint m_mirror_vbo;
		GLuint m_mirror_ebo;

		FT_Library ft_library;
		FT_Face ft_face;

		std::map<char, Character> Characters;

		GLuint m_text_vao;
		GLuint m_text_vbo;

		GLuint m_shadow_vao;
		GLuint m_shadow_vbo;
		static constexpr size_t MAX_SHADOW_VERTICES = 1024;

		GLuint m_circle_vao;
		GLuint m_circle_vbo;

		// Layered rendering helpers (world vs UI)
		static bool shouldRenderWorld(const int ui_mode) noexcept;
		void renderWorldPass(const float delta, const int ui_mode, const CameraState& cam, const glm::mat4& view, const glm::mat4& projection) noexcept;
		void renderUIPass(const int ui_mode, const CameraState& cam, const glm::mat4& view, const glm::mat4& projection) noexcept;

		void renderShadowMask(glm::vec2 light_pos, glm::mat4 view, glm::mat4 projection) noexcept;
		void renderBeamShadowMask(glm::vec2 light_pos, glm::mat4 view, glm::mat4 projection) noexcept;
		std::vector<glm::vec2> computeShadowGeometry(glm::vec2 light_pos, entt::entity platform) noexcept;
};
