#include "Wall.h"

// vertex positions, normals, tangents and texture coordinates
std::vector<GLfloat> vertices = {
	-1.0f, -1.0f, 0.0f,  // vertex 0: position
	0.0f,  0.0f,  1.0f,  // vertex 0: normal
	0.0f,  0.0f,         // vertex 0: texture coordinate
	1.0f,  0.0f,  0.0f,  // vertex 0: tangent
	1.0f,  -1.0f, 0.0f,  // vertex 1: position
	0.0f,  0.0f,  1.0f,  // vertex 1: normal
	1.0f,  0.0f,         // vertex 1: texture coordinate
	1.0f,  0.0f,  0.0f,  // vertex 1: tangent
	-1.0f, 1.0f,  0.0f,  // vertex 2: position
	0.0f,  0.0f,  1.0f,  // vertex 2: normal
	0.0f,  1.0f,         // vertex 2: texture coordinate
	1.0f,  0.0f,  0.0f,  // vertex 2: tangent
	1.0f,  1.0f,  0.0f,  // vertex 3: position
	0.0f,  0.0f,  1.0f,  // vertex 3: normal
	1.0f,  1.0f,         // vertex 3: texture coordinate
	1.0f,  0.0f,  0.0f,  // vertex 3: tangent
};

std::shared_ptr<VertexArray> CreateWallVA() {
	return std::make_shared<VertexArray>(vertices);
}
