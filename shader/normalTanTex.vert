#version 410 core

// input data
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

// uniform input data
uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 lightSpaceMatrix;

// output data
out vec3 vPosition;
out vec3 vNormal;
out vec3 vTangent;
out vec2 vTexCoord;
out vec4 vFragPosLightSpace;

void main() {
  // set vertex position
  gl_Position =
      uProjectionMatrix * uViewMatrix * uModelMatrix * vec4(aPosition, 1.0f);

  // set vertex shader output
  // will be interpolated for each fragment
  vPosition = (uModelMatrix * vec4(aPosition, 1.0f)).xyz;
  vNormal = uNormalMatrix * aNormal;
  vTangent = uNormalMatrix * aTangent;
  vTexCoord = aTexCoord;
  vFragPosLightSpace = lightSpaceMatrix * uModelMatrix * vec4(aPosition, 1.0f);
}
