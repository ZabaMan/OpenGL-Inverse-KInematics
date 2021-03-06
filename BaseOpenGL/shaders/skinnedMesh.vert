#version 430 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 5) in ivec4 boneIDs; 
layout(location = 6) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBoneMatrices[MAX_BONES];

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIDs[i] == -1) 
            continue;
        if(boneIDs[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos,1.0f);
            break;
        }
        vec4 localPosition = finalBoneMatrices[boneIDs[i]] * vec4(aPos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBoneMatrices[boneIDs[i]]) * aNormal;
   }
	
    mat4 viewModel = view * model;
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; 
    gl_Position =  projection * viewModel * totalPosition;
    vs_out.TexCoords = aTexCoords;
}