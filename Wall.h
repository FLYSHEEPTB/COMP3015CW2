#pragma once

#include <memory>

#include "VertexArray.h"
#include "helper/Texture.h"

struct Wall {
  glm::mat4 modelMatrix;
  std::shared_ptr<VertexArray> va;
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Texture> texNormal;
};

std::shared_ptr<VertexArray> CreateWallVA();