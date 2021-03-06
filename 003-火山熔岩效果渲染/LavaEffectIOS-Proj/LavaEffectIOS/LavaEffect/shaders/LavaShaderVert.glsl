#version 300 es

uniform vec2 uvScale;

uniform mat4    uModelViewProjectionMatrix;
uniform mat4    uModelViewMatrix;
uniform mat4    uProjectionMatrix;

layout(location = 0) in vec3  position;
layout(location = 1) in vec3  normal;
layout(location = 2) in vec2  uv;

out vec2 vUv;

void main(){
    
    vUv =  uv * uvScale;
//    vec4 mvPosition = uModelViewMatrix * vec4(position,1.0);
//    gl_Position = uProjectionMatrix * mvPosition;
    gl_Position = uModelViewProjectionMatrix * vec4(position,1.0);
    
}

