#pragma once

#include "vertex.h"

#include <vector>

namespace assets {

bool loadModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<uint16_t>& indices);

} // namespace assets
