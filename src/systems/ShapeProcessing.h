#pragma once

#include <registry.h>
#include <common.h>

class ShapeProcessingSystem{
	public:
		static ShapeProcessingSystem init() noexcept;
		void deinit() noexcept;

		std::vector<int> triangulatePrism(const std::vector<glm::vec2>& vertices) noexcept;
		BoundingBox calculateBoundingBox(const std::vector<glm::vec2>& vertices) noexcept;
		bool pointInTriangle(const glm::vec2& point, const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) noexcept;
		bool circle_rectangle_collision(glm::vec2 circle_center, float circle_radius, glm::vec2 rect_position, glm::vec2 rect_size) noexcept;
		glm::vec2 get_closest_point_on_line(glm::vec2 pos, glm::vec2 line_start, glm::vec2 line_end) noexcept;

};