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
	count
};

template<typename AssetType>
std::string getAssetPath(const AssetType asset_id) noexcept;

} // namespace assets
