#include "registry.h"
#include "config.h"

Registry* Registry::init() noexcept {
	Registry* self = new Registry;

	self->m_player = self->ecs.create();

	return self;
}
