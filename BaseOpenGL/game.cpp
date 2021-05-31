#include "game.h"
#include "resource_manager.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>


#include "camera.h"
#include "game_object.h"
#include <queue>
#include <random>
#include "text_renderer.h"
#include <iomanip>

#include <stdlib.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "line.h"

#include <string>

Camera camera(glm::vec3(0.0f, 5.0f, 20.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90, 0);

GameObject* Man;
GameObject* Cylinder;
GameObject* CylinderTwo;
GameObject* UpperArmBone;
GameObject* LowerArmBone;
GameObject* HandBone;
GameObject* LowerFingerBone;
GameObject* MidFingerBone;
GameObject* UpperFingerBone;
GameObject* EndEffector;
GameObject* Target;
Line AtoB, BtoC, CtoD, DtoA;
glm::vec3 A = glm::vec3(-6.0f, 10.0f, 2.0f);
glm::vec3 B = glm::vec3(-6.0f, 17.0f, 2.0f);
glm::vec3 C = glm::vec3(-6.0f, 17.0f, -2.0f);
glm::vec3 D = glm::vec3(-6.0f, 10.0f, -2.0f);
int animationPosition = 0;

unsigned int manDiffuseMap;
unsigned int manNormalMap;



string Current_Text;
bool menu = false;

string prefix = "C:/Users/Aaron/source/repos/Inverse Kinematics/BaseOpenGL/resources/";

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), LightPos(1.2f, 20.0f, 2.0f)
{

}

Game::~Game()
{

}

void Game::Init()
{
    
    // build and compile shaders
    // -------------------------
    ResourceManager::LoadShader("blinn.vert", "blinn.frag", nullptr, "Blinn");
    ResourceManager::LoadShader("skinnedMesh.vert", "skinnedMesh.frag", nullptr, "SkinnedMesh");

    //Models
    Man = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f),
        Model("man/Ch12_nonPBR (2).fbx"), glm::vec3(1.0f, 1.0f, 1.0f));

    Cylinder = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 1.0f, 0.0f));
    Cylinder->SetPosition(glm::vec3(-1.2f, 15.0f, 0.0f));
    Cylinder->SetConstraint("Z", 0, 180);

    CylinderTwo = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 0.8f, 0.0f));
    CylinderTwo->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));

    //**** Arm Setup
    UpperArmBone = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 1.0f, 0.0f));
    UpperArmBone->SetPosition(glm::vec3(-1.2f, 15.0f, 0.0f));
    UpperArmBone->SetConstraint("Z", 0, 180);

    LowerArmBone = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 0.8f, 0.0f));
    LowerArmBone->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));

    HandBone = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 1.0f, 0.0f));
    HandBone->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));

    LowerFingerBone = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 0.8f, 0.0f));
    LowerFingerBone->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));

    MidFingerBone = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 1.0f, 0.0f));
    MidFingerBone->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));

    UpperFingerBone = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("cylinder.obj"), glm::vec3(0.0f, 0.8f, 0.0f));
    UpperFingerBone->SetPosition(glm::vec3(0.0f, 4.0f, 0.0f));

    UpperArmBone->AddChild(LowerArmBone);
    LowerArmBone->AddChild(HandBone);
    HandBone->AddChild(LowerFingerBone);
    LowerFingerBone->AddChild(MidFingerBone);
    MidFingerBone->AddChild(UpperFingerBone);

    //**Arm Setup Done

    EndEffector = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("sphere.obj"), glm::vec3(0.0f, 1.0f, 0.0f));

    Cylinder->AddChild(CylinderTwo);
    CylinderTwo->AddChild(EndEffector);

    EndEffector->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));

    Cylinder->SetRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)));
    UpperArmBone->SetRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)));

    Target = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),
        Model("sphere.obj"), glm::vec3(1.0f, 0.0f, 0.0f));

    
    
    AtoB.Setup(A, B, glm::vec3(1, 0, 0));
    BtoC.Setup(B, C, glm::vec3(1, 0, 0));
    CtoD.Setup(C, D, glm::vec3(1, 0, 0));
    DtoA.Setup(D, A, glm::vec3(1, 0, 0));

    manDiffuseMap = loadTexture((prefix + "man/Ch12_1001_Diffuse.png").c_str());

    glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, Position);
    Man->MoveBone(model_, "mixamorig4:RightArm");
    
}

float p_ambient = 0.3f;
float p_specCoef = 0.0f;
float p_phongExponent = 128.0f;

float Y = 0.0f;
float X = 0.0f;
float Z = 0.0f;
bool animateTarget;
float d = 0.0f;
float speed = 0.5f;

int a = 0;
int arm = 0;
const char* arms[]{"2 Bone", "6 Bone"};
bool syncSkinnedMesh;

void Game::Update(float dt)
{
	
    {
        ImGui::Begin("Settings");
        
        float lightPos[3] = {LightPos.x, LightPos.y, LightPos.z};

        if(ImGui::Button("Debug CCD"))
            DebugCCD();

        if (ImGui::CollapsingHeader("Target Position"))
        {
            
            ImGui::SliderFloat("X", &X, -50.0f, 50.0f);
            ImGui::SliderFloat("Y", &Y, -50.0f, 50.0f);
            ImGui::SliderFloat("Z", &Z, -50.0f, 50.0f);

            ImGui::NewLine();
            ImGui::Checkbox("Animate Target", &animateTarget);
            if (animateTarget)
                ImGui::SliderFloat("Speed", &speed, 0.0f, 1.0f);
            if(ImGui::Button("Reset Animation"))
            {
                animationPosition = 0;
                d = 0;
            }
            
        }
        if (ImGui::CollapsingHeader("Bones"))
        {
            ImGui::ListBox("Arm", &a, arms, IM_ARRAYSIZE(arms));
            if(ImGui::Button("Set Arm"))
            {
                arm = a;
	            if(arm == 0)
	            {
                    CylinderTwo->AddChild(EndEffector);
	            }
                else if(arm == 1)
                {
                    UpperFingerBone->AddChild(EndEffector);
                }
            }
            if (ImGui::Button(a == 0 ? Cylinder->UseConstraints ? "Disable Constraints" : "Enable Constraints" : UpperArmBone->UseConstraints ? "Disable Constraints" : "Enable Constraints"))
            {
                arm = a;
                if (arm == 0)
                {
                    Cylinder->UseConstraints = !Cylinder->UseConstraints;
                    CylinderTwo->UseConstraints = !CylinderTwo->UseConstraints;
                }
                else if (arm == 1)
                {
                    bool constraint = !UpperArmBone->UseConstraints;
                    LowerArmBone->UseConstraints = constraint;
                    HandBone->UseConstraints = constraint;
                    LowerFingerBone->UseConstraints = constraint;
                    MidFingerBone->UseConstraints = constraint;
                    UpperFingerBone->UseConstraints = constraint;
                    UpperArmBone->UseConstraints = constraint;
                }
            }
            if (arm == 0) {
                ImGui::NewLine();
                ImGui::NewLine();
                ImGui::Checkbox("Skinned Mesh", &syncSkinnedMesh);
            }
        }
        if (ImGui::CollapsingHeader("Light"))
        {
            ImGui::SliderFloat3("Position", lightPos , -20.0f, 20.0f);
            LightPos.x = lightPos[0];
            LightPos.y = lightPos[1];
            LightPos.z = lightPos[2];
        }
        

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
        ImGui::End();
    }

    if(animateTarget)
    {
        glm::vec3 pos;
        d += dt * speed;

        glm::vec3 start, end;
        if (animationPosition == 0) {
            start = A; end = B;
        }
        else if (animationPosition == 1) {
            start = B; end = C;
        }
        else if (animationPosition == 2) {
            start = C; end = D;
        }
        else if (animationPosition == 3) {
            start = D; end = A;
        }

        pos = glm::lerp(start, end, d);

        Target->Position.x = pos.x;
        Target->Position.y = pos.y;
        Target->Position.z = pos.z;

        if(glm::distance2(Target->Position, end) < 0.05f)
        {
            animationPosition += 1;

            if (animationPosition == 4)
                animationPosition = 0;

            d = 0.0f;
        }
    }
    else {
        Target->Position.x = X;
        Target->Position.y = Y;
        Target->Position.z = Z;
    }
    CCD();

    if(syncSkinnedMesh && arm == 0)
    {
        SyncSkinnedMesh();
    }

	//Man->Rotation.z = Man->Rotation.z + (30.0f * dt);
    //Cylinder->Rotation.z = Cylinder->Rotation.z + (30.0f * dt);
}

glm::vec3 newTargetPosition;
bool tried;
bool triedReset;

void Game::CCD(bool reset)
{
    if (reset)
        tried = false;

    if (tried && newTargetPosition == Target->Position)
        return;

    newTargetPosition = Target->Position;

    GameObject* Bone2Arm[3]{ EndEffector, CylinderTwo, Cylinder };
    GameObject* Bone6Arm[7]{ EndEffector, UpperFingerBone, MidFingerBone, LowerFingerBone, HandBone, LowerArmBone, UpperArmBone };
    int boneCount = 0;
    GameObject** bones = Bone2Arm;

    if (arm == 0) 
    {
        boneCount = 3;
    }
    else if(arm == 1)
    {
        boneCount = 7;
        bones = Bone6Arm;
    }

    float sqrDistance;
    int iterationCount = 0;
    
    do
    {
	    for(int i = 1; i < boneCount; i++)
	    {
	    	sqrDistance = glm::distance2(bones[0]->GetWorldPos(), Target->Position);
            if (sqrDistance < 0.2f)
                break;

            RotateBone(bones[0], bones[i], Target->Position);

            
	    }

        sqrDistance = glm::distance2(bones[0]->GetWorldPos(), Target->Position);
        iterationCount++;
    } 
    while (sqrDistance > 0.2f && iterationCount <= 100);

    tried = true;
    

    if(sqrDistance > 0.2f && !triedReset && arm == 0)
    {
        triedReset = true;
        DebugReset();
        return;
    }

    triedReset = false;
    
    
}

void Game::RotateBone(GameObject* effector, GameObject* bone, glm::vec3 target)
{
    glm::vec3 effectorPos = effector->GetWorldPos();
    glm::vec3 bonePos = bone->GetWorldPos();


    glm::vec3 boneToEffector = glm::normalize(effectorPos - bonePos);
    glm::vec3 boneToGoal =  glm::normalize(target - bonePos);



    float dp = glm::dot(boneToEffector, boneToGoal);
    if (dp >= 1)
        return;

    glm::vec3 crossResult = glm::cross(boneToEffector, boneToGoal);
    float angle = glm::acos(dp);
    glm::quat rot = glm::normalize(angleAxis(angle, crossResult));
    bone->ApplyRotation(rot);
}

int debugBone = 1;

void Game::DebugCCD()
{

    newTargetPosition = Target->Position;

    GameObject* bones[3]{ EndEffector, CylinderTwo, Cylinder };

    RotateBone(bones[0], bones[debugBone], Target->Position);

    debugBone = debugBone == 1 ? 2 : 1;
}

void Game::DebugRotateBone(GameObject* effector, GameObject* bone, glm::vec3 target)
{
    glm::vec3 effectorPos = bone->GetPositionInLocalSpace(effector->GetWorldPos());
    glm::vec3 bonePos = bone->GetPositionInLocalSpace(bone->GetWorldPos());
    target = bone->GetPositionInLocalSpace(target);
    cout << "Effector Position: " + toString(effectorPos) << endl;
    cout << "Bone Position: " + toString(bonePos) << endl;

    glm::vec3 boneToEffector = glm::normalize(effectorPos - bonePos);
    glm::vec3 boneToGoal = glm::normalize(target - bonePos);
    cout << "Bone To Effector: " + toString(boneToEffector) << endl;
    cout << "Bone To Target: " + toString(boneToGoal) << endl;


    float dp = glm::dot(boneToEffector, boneToGoal);
    if (dp >= 1)
        return;

    glm::vec3 crossResult = glm::cross(boneToEffector, boneToGoal);
    cout << "Cross Between ToEffector and ToTarget: " + toString(crossResult) << endl;
    float angle = glm::angle(boneToGoal, boneToEffector);
    cout << "Angle To Take: " + std::to_string(glm::degrees(angle)) << endl;
    glm::quat rot = glm::normalize(angleAxis(angle, crossResult));
    bone->ApplyRotation(rot);// = bone->Rotation * rot;
}

void Game::DebugReset()
{
	if(arm == 0)
	{
        Cylinder->SetRotation(glm::angleAxis(glm::radians(90.0f), glm::vec3(0, 0, 1)));
        CylinderTwo->SetRotation(glm::quat(1, 0, 0, 0));
        CCD(true);
	}
}

void Game::SyncSkinnedMesh()
{
    //glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, CylinderTwo->Position);
    Man->MoveBone(model_, "mixamorig4:RightArm");

    model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, EndEffector->Position);
    Man->MoveBone(model_, "mixamorig4:RightForeArm");
}

bool keyDown = false;

void Game::ProcessInput(float dt, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dt);


    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && !keyDown)
    {
        if(!menu)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        keyDown = true;
        menu = !menu;
    }
    else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE && keyDown)
    {
        keyDown = false;
    }

    //if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !keyDown)
    //{
    //    DebugReset();

    //    keyDown = true;
    //}
    //else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE && keyDown)
    //{
    //    keyDown = false;
    //}
}


void Game::Render()
{
    

    ResourceManager::GetShader("SkinnedMesh").Use();
    Shader shader = ResourceManager::GetShader("SkinnedMesh");
    // view/projection transformations
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)this->Width / this->Height, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("lightPos", this->LightPos);
    shader.SetVector3f("viewPos", camera.Position);
    shader.SetFloat("ambient", p_ambient);
    shader.SetFloat("specSize", p_specCoef);
    shader.SetFloat("specStrength", p_phongExponent);
    shader.SetFloat("textureScale", 1.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, manDiffuseMap);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, manNormalMap);

    auto transforms = Man->GetBoneTransforms();
    for (int i = 0; i < transforms.size(); ++i)
    {
        shader.SetMatrix4(("finalBoneMatrices[" + std::to_string(i) + "]").c_str(), transforms[i]);
    }


    Man->Draw(shader);


    ResourceManager::GetShader("Blinn").Use();
    shader = ResourceManager::GetShader("Blinn");
    shader.SetMatrix4("projection", projection);
    shader.SetMatrix4("view", view);
    shader.SetVector3f("light.position", this->LightPos);
    shader.SetVector3f("viewPos", camera.Position);
    shader.SetFloat("ambient", p_ambient);
    shader.SetFloat("specSize", p_specCoef);
    shader.SetFloat("specStrength", p_phongExponent);

    if (arm == 0 && !syncSkinnedMesh)
        Cylinder->Draw(shader);
    else if (arm == 1)
        UpperArmBone->Draw(shader);

    Target->Draw(shader);

    if (animateTarget) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 MVP = projection * view * model;

        AtoB.setMVP(MVP);
        AtoB.draw();
        BtoC.setMVP(MVP);
        BtoC.draw();
        CtoD.setMVP(MVP);
        CtoD.draw();
        DtoA.setMVP(MVP);
        DtoA.draw();
    }

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::ProcessMouseMovement(float xoffset, float yoffset)
{
    if(!menu)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

void Game::ProcessMouseScroll(float yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int Game::loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

string Game::toString(glm::vec3 toString)
{
    return std::to_string(toString.x) + ", " + std::to_string(toString.y) + ", " + std::to_string(toString.z);
}