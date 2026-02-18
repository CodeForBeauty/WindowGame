#pragma once

#include "vertex.h"

#include <vector>

namespace assets {

bool loadModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<unsigned int>& indices);

} // namespace assets
