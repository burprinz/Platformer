#pragma once

#include <common.h>

namespace assets {

enum struct Sound {
	music,
	count
};

enum struct Mesh {
	player,
	light,
	count
};

enum struct Shader {
	player,
	sprite,
	wind,
	egg,
    line,
	platform,
	illumination,
	beam_illumination,
	animation,
	text,
	shadow,
	circle_particle,
	polygon,
	beam,
	count
};

enum struct Texture {
		test_room,
	idle,
walking_left,
walking_right,
falling_left,
falling_right,
climbing_left,
climbing_right,
striking_left,
striking_right,
striking_up,
striking_down,
		count
};


template<typename AssetType>
std::string getAssetPath(const AssetType asset_id) noexcept;
} // namespace assets