#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
	vec2 texCoords;
	vec3 position;
} gs_in[];


out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;

uniform float time;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

void main() 
{
	vec3 normal = GetNormal();

	for (int i = 0; i < 3; ++i) 
	{
		gl_Position = explode(gl_in[i].gl_Position, normal);
		TexCoords = gs_in[i].texCoords;
		Position = gs_in[i].position;
		Normal = mat3(transpose(inverse(model))) * normal;
		EmitVertex();
	}

	EndPrimitive();

}