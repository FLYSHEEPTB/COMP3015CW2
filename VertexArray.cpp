#include "VertexArray.h"

#include "helper/utilities.h"

VertexArray::VertexArray(const std::vector<GLfloat> &vertices) {
	// create VBO
	glGenBuffers(1, &gVBO);  // generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0],
		GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);          // generate unused VAO identifier
	glBindVertexArray(gVAO);              // create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);  // bind the VBO

	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTexTan),
		reinterpret_cast<void *>(offsetof(
			VertexNormTexTan, position)));  // specify format of position data
	glVertexAttribPointer(
		1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTexTan),
		reinterpret_cast<void *>(offsetof(
			VertexNormTexTan, normal)));  // specify format of colour data
	glVertexAttribPointer(
		2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNormTexTan),
		reinterpret_cast<void *>(offsetof(
			VertexNormTexTan, texCoord)));  // specify format of tangent data
	glVertexAttribPointer(
		3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNormTexTan),
		reinterpret_cast<void *>(
			offsetof(VertexNormTexTan,
				tangent)));  // specify format of texture coordinate data

	glEnableVertexAttribArray(0);  // enable vertex attributes
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}
