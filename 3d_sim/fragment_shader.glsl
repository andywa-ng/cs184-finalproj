#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 groundColor;

void main()
{
    // Water properties
    vec3 waterColor = vec3(0.2, 0.4, 0.8);
    float ambientStrength = 0.2;
    float specularStrength = 0.8;
    float shininess = 64.0;
    
    // Ambient
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);
    
    // Fresnel effect for water (simplified)
    float fresnelFactor = pow(1.0 - max(dot(norm, viewDir), 0.0), 4.0);
    vec3 fresnel = fresnelFactor * vec3(1.0, 1.0, 1.0);
    
    vec3 result;
    if (FragPos.y < -1.9) { // Check if rendering the ground plane
        result = groundColor; // Use the ground color
    } else {
        result = (ambient + diffuse) * waterColor + specular + fresnel * 0.5;
    }
    FragColor = vec4(result, 1.0); // Opaque ground
}