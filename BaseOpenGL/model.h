#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/assimp_glm_helpers.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "bone.h"
using namespace std;



class Model
{
public:
    Model() {}

    // model data 
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    string name;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        string prefix = "C:/Users/Aaron/source/repos/Inverse Kinematics/BaseOpenGL/resources/";
        loadModel(prefix + path);
    }

    // draws the model, and thus all its meshes
    void Draw(Shader& shader);

    void CalculateBoneTransform(glm::mat4 localTransform, string boneName);
    std::vector<glm::mat4> GetBoneTransforms();
    Bone* FindBone(const std::string& name);


private:

    std::map<string, BoneInfo> m_BoneInfoMap;
    std::vector<Bone> m_Bones;
    int m_BoneCounter = 0;

    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);

    void SetVertexBoneDataToDefault(Vertex& vertex);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);
    

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    void SetVertexBoneData(Vertex& vertex, int boneID, float weight);

    void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);


    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    
    unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
};



#endif