#pragma once

#include "vertex.h"

#include <vector>

namespace assets {

std::vector<renderer::vertex> loadModel(const char* filepath);

} // namespace assets
