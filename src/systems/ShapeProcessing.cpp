#include "ShapeProcessing.h"

ShapeProcessingSystem ShapeProcessingSystem::init() noexcept {
	ShapeProcessingSystem self;
	return self;
}

void ShapeProcessingSystem::deinit() noexcept {
	// Nothing to deinitialize for now
}

/**
* WARNING: No error handling for invalid input shapes!
* Triangulates a prism with ear clipping method
* This will only work if the vertices are defined in CCW order (gegen den Uhrzeigersinn).
* Doesn't work for specific complex shapes, f.e. self intersecting shapes
* @param vertices The 2D vertices of the prism
* @return A vector of triangle indices of the triangulated polygon
*/
std::vector<int> ShapeProcessingSystem::triangulatePrism(const std::vector<glm::vec2>& vertices) noexcept {
    const int n = vertices.size();
    std::vector<int> active_vertices;
    for (int i = 0; i < n; i++) {       // this vector acts as a copy of the vertices, the active vertices will shrink
        active_vertices.push_back(i);
    }
    std::vector<int> triangle_indices;

    if (n < 3) return triangle_indices; // Not enough vertices to form a triangle

    int i = 1;
    while (active_vertices.size() > 3) {
        // loop over all vertices to find ears
        int active_vertices_size = active_vertices.size();
        glm::vec2 prev = vertices[active_vertices[(i - 1 + active_vertices_size) % active_vertices_size]];
        glm::vec2 curr = vertices[active_vertices[i]];
        glm::vec2 next = vertices[active_vertices[(i + 1) % active_vertices_size]];

        float cross = (curr.x - prev.x) * (next.y - curr.y) - (curr.y - prev.y) * (next.x - curr.x);

        if (cross <= 0) {
            i++;
            continue;   // Not a convex corner, skip
        }
        int m = active_vertices.size();
        for (int j = 0; j < m; j++) {
            if (j == i - 1 || j == i || j == i + 1) continue; // Skip triangle vertices themselves
            // Cross product to check if point is inside triangle
            glm::vec2 other_point = vertices[active_vertices[j]];
            float cross1 = (curr.x - prev.x) * (other_point.y - prev.y) - (curr.y - prev.y) * (other_point.x - prev.x);
            float cross2 = (next.x - curr.x) * (other_point.y - curr.y) - (next.y - curr.y) * (other_point.x - curr.x);
            float cross3 = (prev.x - next.x) * (other_point.y - next.y) - (prev.y - next.y) * (other_point.x - next.x);
            if (!((cross1 >= 0 && cross2 >= 0 && cross3 >= 0) || (cross1 <= 0 && cross2 <= 0 && cross3 <= 0))) {
                if (j == m - 1) {
                    // No other vertex is inside the triangle, it's an ear
                    triangle_indices.push_back(active_vertices[i - 1]);
                    triangle_indices.push_back(active_vertices[i]);
                    triangle_indices.push_back(active_vertices[i + 1]);
                    active_vertices.erase(active_vertices.begin() + i);
                    i = 0;
                    break;
                }
            }
        }
        i++;
    }
    triangle_indices.push_back(active_vertices[0]);
    triangle_indices.push_back(active_vertices[1]);
    triangle_indices.push_back(active_vertices[2]);

    return triangle_indices;
}

/**
* Calculates the axis-aligned bounding box of a set of 2D vertices
* @param vertices The 2D vertices
* @return The bounding box enclosing the vertices
*/
BoundingBox ShapeProcessingSystem::calculateBoundingBox(const std::vector<glm::vec2>& vertices) noexcept {
    BoundingBox bbox;
    if (vertices.empty()) {
        bbox.min = { 0.f, 0.f };
        bbox.max = { 0.f, 0.f };
        return bbox;
    }
    glm::vec2 min = vertices[0];
    glm::vec2 max = vertices[0];
    for (const auto& vertex : vertices) {
        if (vertex.x < min.x) min.x = vertex.x;
        if (vertex.y < min.y) min.y = vertex.y;
        if (vertex.x > max.x) max.x = vertex.x;
        if (vertex.y > max.y) max.y = vertex.y;
    }
    bbox.min = min;
    bbox.max = max;
    return bbox;
}

// Helper function to compute the sign of the area formed by three points
float sign(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) noexcept {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

/**
* Checks if a point is inside a triangle
* @param point The point to check
* @param vertices The vertices of the triangle
* @return true if the point is inside the triangle, false otherwise
*/
bool ShapeProcessingSystem::pointInTriangle(const glm::vec2& point, const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) noexcept {
	float sign1 = sign(point, v0, v1);
    float sign2 = sign(point, v1, v2);
    float sign3 = sign(point, v2, v0);

	if (sign1 == 0 || sign2 == 0 || sign3 == 0) {
        return true; // Point is on the edge of the triangle
    }

    bool has_neg = (sign1 < 0) || (sign2 < 0) || (sign3 < 0);
    bool has_pos = (sign1 > 0) || (sign2 > 0) || (sign3 > 0);
	return !(has_neg && has_pos);

}

bool ShapeProcessingSystem::circle_rectangle_collision(glm::vec2 circle_center, float circle_radius, glm::vec2 rect_position, glm::vec2 rect_size) noexcept {
    glm::vec2 check_pos = {circle_center.x, circle_center.y};
    if (circle_center.x < rect_position.x) {
        // Links pruefen
        check_pos.x = rect_position.x;
    } else if (circle_center.x > rect_position.x + rect_size.x) {
        // Rechts pruefen
        check_pos.x = rect_position.x + rect_size.x;
    }
    if (circle_center.y < rect_position.y) {
        // Unten pruefen
        check_pos.y = rect_position.y;
    } else if (circle_center.y > rect_position.y + rect_size.y) {
        // Oben pruefen
        check_pos.y = rect_position.y + rect_size.y;
    }
    float dist = glm::length(circle_center - check_pos);
    return dist < circle_radius;
}

glm::vec2 ShapeProcessingSystem::get_closest_point_on_line(glm::vec2 pos, glm::vec2 line_start, glm::vec2 line_end) noexcept {
    glm::vec2 line_dir = line_end - line_start;
    glm::vec2 pos_start_dist = pos - line_start;
    float pos_dir = glm::dot(pos_start_dist, line_dir);
    float x = pos_dir / glm::dot(line_dir, line_dir);
    x = (x<0.0f)?0.0f:x;
    x = (x>1.0f)?1.0f:x;
    return line_start + x * line_dir;
}