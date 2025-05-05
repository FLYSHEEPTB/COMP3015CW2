#include "Spirit.h"

#include <vector>


static std::vector<GLfloat> vertices = {
	0,0,0,0,//0
	1,0,1,0,//2
	0,1,0,1,//1
	0,1,0,1,//1
	1,0,1,0,//2
	1,1,1,1//3
};

Spirit::Spirit()
{
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
		0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
		reinterpret_cast<void *>(sizeof(float) * 0));  // specify format of position data
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
		reinterpret_cast<void *>(sizeof(float) * 2));  // specify format of colour data

	glEnableVertexAttribArray(0);  // enable vertex attributes
	glEnableVertexAttribArray(1);
}
