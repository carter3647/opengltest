#version 330 core
    out vec4 FragColor;
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoords;
    uniform vec3 lightPos;
    uniform vec3 lightColor;
    uniform sampler2D ourTexture; // Handled automatically by OpenGL texture bindings
    void main() {
       // Ambient light
       float ambientStrength = 0.2;
       vec3 ambient = ambientStrength * lightColor;
       
       // Diffuse light
       vec3 norm = normalize(Normal);
       vec3 lightDir = normalize(lightPos - FragPos);
       float diff = max(dot(norm, lightDir), 0.0);
       vec3 diffuse = diff * lightColor;
       
       // Sample the texture image color at the current UV coordinates
       vec4 texColor = texture(ourTexture, TexCoords);
       
       vec3 result = (ambient + diffuse) * texColor.rgb;
       FragColor = vec4(result, 1.0);
    }