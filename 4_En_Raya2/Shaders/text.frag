#version 460 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform vec3 outlineColor;

void main()
{
    float sampled = texture(text, TexCoords).r;
    color = vec4(textColor, 1.0);
    vec4 outlineBase = vec4(outlineColor, 1.0);
    if (sampled > 0.3 && sampled <= 0.5)
    {
        if (sampled < 0.4) // La vora es difumina a mesura que ens allunyem de la lletra
        {
            color = outlineBase * smoothstep(0.3, 0.4, sampled);
        }
        else // La vora tendeix al color interior de la lletra
        {
            color = mix(outlineBase, color, smoothstep(0.45, 0.5, sampled));
        }
    }
    else
    {
        color.a = float(sampled > 0.5);
    }
}  