#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 Ke;
uniform float Ns;

uniform vec3 viewPos;

uniform int lightingModel; // 0 = Phong, 1 = Blinn, 2 = Cook-Torrance
uniform int numLights;

uniform vec3 lightPos[25];
uniform vec3 lightColor[25];

out vec4 FragColor;

void main() {

    vec3 ambient = Ka * 0.2;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 totalDiffuse = vec3(0);
    vec3 totalSpecular = vec3(0);

    for(int i=0;i<numLights;i++){
        vec3 lightDir = normalize(lightPos[i] - FragPos);

        float diff = max(dot(norm, lightDir), 0.0);
        totalDiffuse += diff * Kd * lightColor[i];

        if(lightingModel == 0){
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), Ns);
            totalSpecular += Ks * spec * lightColor[i];
        }
        else if(lightingModel == 1){
            vec3 H = normalize(lightDir + viewDir);
            float spec = pow(max(dot(norm, H), 0.0), Ns);
            totalSpecular += Ks * spec * lightColor[i];
        }
        else {
            float roughness = 0.5;
            vec3 H = normalize(lightDir + viewDir);

            float NdotH = max(dot(norm,H), 0.0);
            float NdotV = max(dot(norm,viewDir),0.0);
            float NdotL = max(dot(norm,lightDir),0.0);
            float VdotH = max(dot(viewDir,H),0.0);

            float alpha = roughness * roughness;
            float alpha2 = alpha * alpha;

            float D = alpha2 / (3.14159 * pow(NdotH*NdotH*(alpha2-1.0)+1.0, 2.0));
            float F = 0.04 + (1.0 - 0.04) * pow(1.0 - VdotH, 5.0);
            float k = alpha / 2.0;
            float G = NdotL / (NdotL*(1.0-k)+k) * NdotV/(NdotV*(1.0-k)+k);

            totalSpecular += Ks * (D*F*G) / (4.0*NdotL*NdotV + 0.001) * lightColor[i];
        }
    }

    FragColor = vec4(ambient + totalDiffuse + totalSpecular + Ke, 1.0);
}
