#version 430 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

const float PI = 3.14159265359;

in vec4 Color;
in flat uint ParticleType;

void main()
{
	vec2 centeredPos = 2.0 * (TexCoords - 0.5);
	vec4 color  = Color;

	if(ParticleType == 1)
	{
		//if(dot(centeredPos, centeredPos) > 1.0)
			//discard;
	}
	else if(ParticleType == 0)
	{
		color.rgb *= vec3(0.5, 0.5, 1.0);
		color.a *= max(1.0 - length(centeredPos), 0.0);
	}
	else
	{
		float dist = max(1.0 - length(centeredPos), 0.0);

		color.rgb = mix(vec3(1.0, 0.0, 0.0), vec3(1.0), dist * dist * dist);
		color.a = dist * dist;
	}

	FragColor = color;
}