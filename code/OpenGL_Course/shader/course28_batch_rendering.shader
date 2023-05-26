#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in float texId;

out vec2 v_TexCoord;
out float v_texId;

uniform mat4 u_modelViewProjection;

void main()
{
	gl_Position = u_modelViewProjection * position;
	v_TexCoord = texCoord;
	v_texId = texId;
};


#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_texId;
uniform sampler2D u_Texture[2];

void main()
{
	int id = int(v_texId);
	vec4 texColor = texture(u_Texture[id], v_TexCoord);
	color = texColor; // set the output color
};