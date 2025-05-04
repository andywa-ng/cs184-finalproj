#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 groundColor;
uniform vec3 dropletColor;
uniform vec3 lightColor;
uniform samplerCube skybox;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    float ratio = 1.00 / 1.33; // IOR ratio air/water

    if (FragPos.y < -1.9) { // Ground plane - Use previous Phong lighting
        vec3 objectColor = groundColor;
        float ambientStrength = 0.1;
        float specularStrength = 0.7;
        float shininess = 32.0;
        vec3 ambient = ambientStrength * lightColor;
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 reflectDirLight = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDirLight), 0.0), shininess);
        vec3 specular = specularStrength * spec * lightColor; 
        vec3 lighting = (ambient + diffuse) * objectColor + specular;
        FragColor = vec4(lighting, 1.0); 

    } else { // Droplet/Particle - Apply reflection/refraction
        // Refraction
        vec3 refractDir = refract(-viewDir, norm, ratio);
        vec3 refractionColor = texture(skybox, refractDir).rgb;

        // Reflection
        vec3 reflectDir = reflect(-viewDir, norm);
        vec3 reflectionColor = texture(skybox, reflectDir).rgb;

        // Fresnel (Schlick's approximation)
        float R0 = pow((1.0 - 1.33) / (1.0 + 1.33), 2.0); // Reflectance at normal incidence
        float cosTheta = max(dot(-viewDir, norm), 0.0);
        float fresnel = R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0);

        // Mix reflection and refraction based on Fresnel
        vec3 finalColor = mix(refractionColor, reflectionColor, fresnel);

        // Optional: Add subtle tint or simple ambient light
        // finalColor = mix(finalColor, dropletColor * 0.5, 0.1); // Example tint
         finalColor += vec3(0.05) * lightColor; // Add a little ambient

        FragColor = vec4(finalColor, 1.0); // Still opaque for now
    }
}