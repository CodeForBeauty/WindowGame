#pragma once

#include "vertex.h"

#include <vector>
#include <istream>

namespace assets {

bool loadObjModel(const char* filepath, std::vector<renderer::vertex>& vertices, std::vector<uint16_t>& indices);

void readModelFromMemory(std::vector<renderer::vertex>& vertices, std::vector<uint16_t>& indices, std::istream& stream);

} // namespace assets
