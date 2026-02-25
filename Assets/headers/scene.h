#pragma once

#include <vector>

#include "lm2.hpp"
#include "vertex.h"


namespace assets {

	struct SceneData {
		std::vector< std::pair< std::vector<renderer::vertex>, std::vector<uint16_t> > > data;

	};
}