#include "assets.h"

static constexpr std::array<const char*, static_cast<size_t>(assets::Sound::count)> SOUND_FILES = {
	"music.wav"
};

template<> std::string assets::getAssetPath<assets::Sound>(const assets::Sound asset_id) noexcept {
	return getBasePath() + "/assets/sounds/" + SOUND_FILES[static_cast<size_t>(asset_id)];
}

static constexpr std::array<const char*, static_cast<size_t>(assets::Mesh::count)> MESH_FILES = {
	"light.obj",
};

template<> std::string assets::getAssetPath<assets::Mesh>(const assets::Mesh asset_id) noexcept {
	return getBasePath() + "/assets/mesh/" + MESH_FILES[static_cast<size_t>(asset_id)];
}

static constexpr std::array<const char*, static_cast<size_t>(assets::Shader::count)> SHADER_FILES = {
	"player",
	"sprite",
	"wind",
	"egg",
    "line",
	"platform",
	"illumination",
	"beam_illumination",
	"animation",
	"text",
	"shadow",
	"circle_particle",
	"polygon",
	"beam"
};

template<> std::string assets::getAssetPath<assets::Shader>(const assets::Shader asset_id) noexcept {
	return getBasePath() + "/assets/shaders/" + SHADER_FILES[static_cast<size_t>(asset_id)];
}

static constexpr std::array<const char*, static_cast<size_t>(assets::Texture::count)> TEXTURE_FILES = {
	"rooms/TestRoom.png",
	"backgrounds/test/1.png",
	"backgrounds/test/2.png",
	"backgrounds/test/3fx.png",
	"backgrounds/test/4.png",
	"backgrounds/test/5.png",
	"backgrounds/test/6fx.png",
	"backgrounds/test/7.png",
	"backgrounds/test/8fx.png",
	"backgrounds/test/9.png",
	"character/player_test/idle.png",
"character/player_test/walking_left.png",
"character/player_test/walking_right.png",
"character/player_test/falling_left.png",
"character/player_test/falling_right.png",
"character/player_test/climbing_left.png",
"character/player_test/climbing_right.png",
"character/player_test/striking_left.png",
"character/player_test/striking_right.png",
"character/player_test/striking_up.png",
"character/player_test/striking_down.png"
};


template<> std::string assets::getAssetPath<assets::Texture>(const assets::Texture asset_id) noexcept {
	return getBasePath() + "/assets/textures/" + TEXTURE_FILES[static_cast<size_t>(asset_id)];
}
