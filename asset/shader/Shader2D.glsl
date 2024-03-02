#type vertex

#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

out vec2 TexCoord;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(aPos, 1.0);
    TexCoord = aTex;
}

#type fragment

#version 450 core

in vec2 TexCoord;

out vec4 color;
uniform sampler2D u_Texture;
uniform vec4 u_Color;

void main() {
    color = texture(u_Texture, TexCoord) * u_Color;
}
