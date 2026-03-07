#version 330 core
in vec3 fragNormal;
in float fragHeight;
in vec3 fragWorldPos;

out vec4 FragColor;

uniform vec3 lightDir;
uniform float minHeight;
uniform float maxHeight;

void main()
{
    vec3 normal = normalize(fragNormal);

    // Normalize height to 0-1 range
    float heightNorm = (fragHeight - minHeight) / max(maxHeight - minHeight, 0.001);

    // Terrain colors
    vec3 lowColor = vec3(0.2, 0.5, 0.2);   // Dark green (valleys)
    vec3 midColor = vec3(0.6, 0.5, 0.4);   // Brown (slopes)
    vec3 highColor = vec3(0.9, 0.9, 0.9);  // White (peaks)

    // Color blending based on height
    vec3 baseColor;
    if (heightNorm < 0.3) {
        float t = heightNorm / 0.3;
        baseColor = mix(lowColor, midColor, t);
    } else if (heightNorm < 0.7) {
        float t = (heightNorm - 0.3) / 0.4;
        baseColor = mix(midColor, highColor, t);
    } else {
        float t = (heightNorm - 0.7) / 0.3;
        baseColor = mix(highColor, vec3(1.0), t);
    }

    // Lighting calculation
    vec3 L = normalize(lightDir);
    float diffuse = max(dot(normal, L), 0.0);

    float ambient = 0.3;
    float lighting = ambient + (1.0 - ambient) * diffuse;

    vec3 finalColor = baseColor * lighting;

    FragColor = vec4(finalColor, 1.0);
}
