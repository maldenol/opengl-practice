#version 460 core

uniform vec3  ambLightCol;
uniform float ambLightInt;
uniform float ambCoef;

uniform vec3  lightCol;
uniform float lightInt;
uniform float diffCoef;
uniform float specCoef;

uniform vec3  lightPos;
uniform vec3 cameraPos;

uniform mat4 model;

uniform sampler2D texture0;

in vec3 fPos;
in vec3 fNormal;
in vec3 fColor;
in vec2 fTexCoords;

out vec4 FragColor;

void main() {
  vec3 ambient = ambLightCol * ambLightInt * ambCoef;

  vec3 normal = mat3(transpose(inverse(model))) * fNormal;
  vec3 lightDir = normalize(lightPos - fPos);
  vec3 diffuse = lightCol * lightInt * max(dot(lightDir, normal), 0.0f) * diffCoef;

  vec3 reflDir = 2.0f * dot(lightDir, normal) * normal - lightDir;
  vec3 viewDir = normalize(cameraPos - fPos);
  vec3 specular = lightCol * lightInt * max(dot(viewDir, reflDir), 0.0f) * specCoef;

  vec3 phong = fColor * (ambient + diffuse + specular);

  FragColor = texture(texture0, fTexCoords) * vec4(phong, 1.0f);
}
