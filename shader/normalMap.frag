#version 410 core

// interpolated values from the vertex shaders
in vec3 vPosition;
in vec3 vNormal;
in vec3 vTangent;
in vec2 vTexCoord;
in vec4 vFragPosLightSpace;

// light properties
struct Light {
  vec3 pos;
  vec3 dir;
  vec3 La;
  vec3 Ld;
  vec3 Ls;
  vec3 att; // constant, linear, quadratic
};

// material properties
struct Material {
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
  float shininess;
};

// uniform input data
uniform vec3 uViewpoint;
uniform Light uLight;
uniform Material uMaterial;
uniform sampler2D uTextureSampler;

uniform bool useNormalSampler;
uniform sampler2D uNormalSampler;

// shadow
uniform sampler2D uShadowMap;

uniform float alpha = 1.0;

// output data
out vec4 fColor;

struct PBRMaterial {
  vec3 albedo;
  float metallic;
  float roughness;
  float ao;
};

const float PI = 3.14159265359;

// shadow calculation
float ShadowCalculation(sampler2D shadowmap, vec3 lightPos,
                        vec4 fragPosLightSpace) {
  // perform perspective divide
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

  // transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;

  // get the closest depth value from the light's perspective
  float closestDepth = texture(shadowmap, projCoords.xy).r;

  float currentDepth = projCoords.z;

  // check whether current frag pos is in shadow or not
  float bias = 0.005;

  // float shadow = (currentDepth - bias) > closestDepth  ? 1.0 : 0.0;

  // PCF
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(shadowmap, 0);
  for (int x = -1; x <= 1; ++x) {
    for (int y = -1; y <= 1; ++y) {
      float pcfDepth =
          texture(shadowmap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;

  // keep the shadow at 0.0 when outside the far_plane region of the light's
  // frustum.
  if (projCoords.z > 1.0)
    shadow = 0.0;

  return shadow;
}

// convert from Blinn-Phong to PBR material
PBRMaterial ConvertToPBR(vec3 Kd, vec3 Ks, float shininess) {
  PBRMaterial pbrMat;

  // Base color (albedo) comes from diffuse component
  pbrMat.albedo = Kd;

  // Metallic workflow uses specular intensity relative to diffuse
  float specularIntensity = length(Ks);
  float diffuseIntensity = length(Kd);
  pbrMat.metallic =
      clamp(specularIntensity / max(diffuseIntensity, 1e-3), 0.0, 1.0);

  // Higher shininess = smoother surface = lower roughness
  pbrMat.roughness = 1.0 - sqrt(shininess / 100.0);
  pbrMat.roughness = clamp(pbrMat.roughness, 0.05, 1.0);

  pbrMat.ao = 1.0;

  return pbrMat;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
  return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 useDummy() {
  vec3 dummy = uLight.dir + uLight.La + uLight.Ls + uMaterial.Ka;
  return dummy;
}

void main() {
  vec3 dummy = useDummy();

  // shadow
  float shadow = ShadowCalculation(uShadowMap, uLight.pos, vFragPosLightSpace);
  shadow = clamp(shadow, 0.0f, 1.0f);

  PBRMaterial pbrMat =
      ConvertToPBR(uMaterial.Kd, uMaterial.Ks, uMaterial.shininess);

  // fragment normal
  // tangent, bitangent and normalMap
  vec3 n = normalize(vNormal);

  if (useNormalSampler) {
    vec3 tangent = normalize(vTangent);
    vec3 biTangent = normalize(cross(tangent, n));
    vec3 normalMap = 2.0f * texture(uNormalSampler, vTexCoord).xyz - 1.0f;

    n = normalize(mat3(tangent, biTangent, n) * normalMap);
  }

  // vector toward the viewer
  vec3 v = normalize(uViewpoint - vPosition);

  vec3 F0 = vec3(0.04);
  F0 = mix(F0, pbrMat.albedo, pbrMat.metallic);

  // vector towards the light
  vec3 l = normalize(uLight.pos - vPosition);

  // halfway vector
  vec3 h = normalize(l + v);

  float dotLN = max(dot(l, n), 0.0f);

  float attenuation = 0.0;

  // calculate the light contribution
  vec3 totalLo = vec3(0.0);
  if (dotLN > 0.0f) {
    // attenuation
    float dist = length(uLight.pos - vPosition);
    attenuation = 1.0f / (uLight.att.x + dist * uLight.att.y +
                          dist * dist * uLight.att.z);

    // the original light intensity is too weak, so we make it brilliant
    const float lightIntensity = 10.0f;
    vec3 radiance = uLight.Ld * attenuation * lightIntensity;

    // use physical based rendering model
    float NDF = DistributionGGX(n, h, pbrMat.roughness);
    float G = GeometrySmith(n, v, l, pbrMat.roughness);
    vec3 F = fresnelSchlick(max(dot(h, v), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - pbrMat.metallic;

    vec3 nominator = NDF * G * F;
    float denominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    totalLo += (kD * pbrMat.albedo / PI + specular) * radiance * dotLN;
  }

  vec3 ambient = vec3(0.1) * pbrMat.albedo * pbrMat.ao;
  vec3 color = ambient + totalLo;

  // gamma correction
  color = color / (color + vec3(1.0));
  color = pow(color, vec3(1.0 / 2.2));

  fColor.rgb = color;

  // modulate with texture
  fColor.rgb *= texture(uTextureSampler, vTexCoord).rgb;

  // apply shadow
  fColor.rgb *= vec3(1.0f - shadow);

  fColor.a = alpha + dummy.x * 0.000001f;
}