#pragma once

#include <common.h>

namespace assets {

enum struct Sound {
	music,
	music_endboss,
	level_complete,
	light_switch_activated,
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
		mirror,
		door,
		player_animation,
		test_room,
		background_1,
		background_2,
		background_3,
		background_4,
		background_5,
		background_6,
		background_7,
		background_8,
		background_9,
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
