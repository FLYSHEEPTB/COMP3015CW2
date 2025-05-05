#version 410 core

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 vPosition[];
in vec3 vNormal[];
in vec3 vTangent[];
in vec2 vTexCoord[];

out vec3 fColor;

uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform float uNormalLength = 5.0;

void main() {
  int stepNum = 20;

  // if the vTangent is not available, we can calculate it
  vec3 tangent[3];
  vec3 p0 = vPosition[0] - vPosition[2];
  vec3 p1 = vPosition[1] - vPosition[2];
  vec3 p2 = vPosition[0] - vPosition[1];
  tangent[0] = normalize(cross(p0, p1));
  tangent[1] = normalize(cross(p1, p2));
  tangent[2] = normalize(cross(p2, p0));

  // for the three vertices of the triangle, we need to emit 3 lines as hair
  for (int i = 0; i < 3; i++) {
    vec3 pos = vPosition[i];

    fColor = vec3(1, 0, 0);
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();

    // separate the hair into multi segments
    for (int step = 0; step < stepNum; ++step) {
      float t = float(step + 1) / float(stepNum);

      // use sin to create a wave effect
      float waveFactor = sin(t * 3.14159 * 2.0);
      vec3 n = vNormal[i]; // + tangent[i] * waveFactor
      n.y *= abs(waveFactor * 10.0f);

      // gredient color
      fColor = vec3(t, t, t);
      pos = pos + normalize(n) * uNormalLength / float(stepNum);
      gl_Position = uProjectionMatrix * uViewMatrix * vec4(pos, 1.0);
      EmitVertex();
    }

    EndPrimitive();
  }
}