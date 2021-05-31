#include "game_object.h"
#include <glm/common.hpp>
#include <glm/common.hpp>
#include <glm/common.hpp>
#include <glm/gtc/quaternion.hpp>


GameObject::GameObject()
    : Position(0.0f, 0.0f, 0.0f), Size(1.0f, 1.0f, 1.0f), Color(1.0f), Rotation(1.0f, 0.0f, 0.0f, 0.0f), Child(), model(), Parent(), HasParent(false), Destroyed(false), HasChild(false), XaxisMin(-90), XaxisMax(90), YaxisMin(-90), YaxisMax(90), ZaxisMin(-90), ZaxisMax(90) { }

GameObject::GameObject(glm::vec3 pos, glm::vec3 size, Model model, glm::vec3 color)
    : Position(pos), Size(size), Color(color), Rotation(1.0f, 0.0f, 0.0f, 0.0f), Child(), model(model), Parent(), HasParent(false), Destroyed(false), HasChild(false), XaxisMin(-90), XaxisMax(90), YaxisMin(-90), YaxisMax(90), ZaxisMin(-90), ZaxisMax(90) { }

void GameObject::Draw(Shader& shader)
{
    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, Position);
    model_ = model_ * glm::toMat4(Rotation);
	model_ = glm::scale(model_, Size);
    shader.SetMatrix4("model", model_);
	shader.SetVector3f("objectColor", Color.x, Color.y, Color.z);
    model.Draw(shader);

    if (HasChild)
    {
        glm::mat4 model_child = glm::mat4(1.0f);
        model_child = glm::translate(model_child, Child->Position);
        model_child = model_child * glm::toMat4(Child->Rotation);
        model_child = glm::scale(model_child, Child->Size);

        Child->DrawChild(model_ * model_child, shader);
    }
}

void GameObject::MoveBone(glm::mat4 localTransform, string boneName)
{
    model.CalculateBoneTransform(localTransform, boneName);
}

Bone* GameObject::GetBone(string boneName)
{
    return model.FindBone(boneName);
}

std::vector<glm::mat4> GameObject::GetBoneTransforms() { return model.GetBoneTransforms(); }

void GameObject::SetPosition(glm::vec3 pos)
{
    Position = pos;
}

void GameObject::SetRotation(glm::quat rot)
{
    Rotation = rot;
}

void GameObject::ApplyRotation(glm::quat rot)
{
	Rotation = rot * Rotation;

    if (!UseConstraints)
        return;

    glm::vec3 e = glm::degrees(glm::eulerAngles(Rotation));
    cout << std::to_string(e.x) + ", " + std::to_string(e.y) + ", " + std::to_string(e.z) << endl;
    if (e.x > 180.0f)
        e.x -= 360.0f;
    e.x = glm::clamp(e.x, XaxisMin, XaxisMax);


    if (e.y > 180.0f)
        e.y -= 360.0f;
    e.y = glm::clamp(e.y, YaxisMin, YaxisMax);

    if (e.z > 180.0f)
        e.z -= 360.0f;
    e.z = glm::clamp(e.z, ZaxisMin, ZaxisMax);
    cout << std::to_string(e.x) + ", " + std::to_string(e.y) + ", " + std::to_string(e.z) << endl;
    Rotation = glm::quat(glm::radians(e));

}

void GameObject::SetConstraint(string axis, float min, float max)
{
    if (axis == "X")
    {
        XaxisMin = min;
        XaxisMax = max;
    }
    else if (axis == "Y")
    {
        YaxisMin = min;
        YaxisMax = max;
    }
    else if (axis == "Z")
    {
        ZaxisMin = min;
        ZaxisMax = max;
    }
}

void GameObject::AddChild(GameObject* GO)
{
    Child = GO;
    HasChild = true;
    GO->HasParent = true;
    GO->Parent = this;
}

glm::vec3 GameObject::GetWorldPos()
{
    glm::mat4 model_ = GetModelMatrix();

    glm::vec3 WP = glm::vec3(model_[3].x, model_[3].y, model_[3].z);
    return WP;
}

glm::vec3 GameObject::GetPositionInLocalSpace(glm::vec3 pos)
{
    if (HasParent)
        pos = pos - Parent->Position;

    //model_ = model_ * GetModelMatrix();
    //glm::vec3 LP = glm::vec3(model_[3].x, model_[3].y, model_[3].z);
    return pos;
}

glm::mat4 GameObject::GetModelMatrix()
{
    glm::mat4 model_ = glm::mat4(1.0f);
    model_ = glm::translate(model_, Position);
    model_ = model_ * glm::toMat4(Rotation);
    model_ = glm::scale(model_, Size);

    if (HasParent)
        return Parent->GetModelMatrix() * model_;

    return model_;
}


void GameObject::DrawChild(glm::mat4 model_, Shader& shader)
{
    shader.SetMatrix4("model", model_);
    shader.SetVector3f("objectColor", Color.x, Color.y, Color.z);
    model.Draw(shader);

    if (HasChild)
    {
        glm::mat4 model_child = glm::mat4(1.0f);
        model_child = glm::translate(model_child, Child->Position);
        model_child = model_child * glm::toMat4(Child->Rotation);
        model_child = glm::scale(model_child, Child->Size);

        Child->DrawChild(model_ * model_child, shader);
    }
}
