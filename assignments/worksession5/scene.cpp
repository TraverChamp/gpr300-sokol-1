#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

#include "ew/procGen.h"
Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    water = std::make_unique<ew::Shader>("assets/shaders/water/water.vs", "assets/shaders/water/water.fs");

    /*std::unique_ptr<ew::Texture> water128;
    std::unique_ptr<ew::Texture> water64;
    std::unique_ptr<ew::Texture> water32;
    std::unique_ptr<ew::Texture> water16;
    std::unique_ptr<ew::Texture> water8; */
    water128 = std::make_unique<ew::Texture>("assets/Textures/windwaker/water128.png");
    water64 = std::make_unique<ew::Texture>("assets/Textures/windwaker/water64.png");
    water32 = std::make_unique<ew::Texture>("assets/Textures/windwaker/water32.png");
    water16 = std::make_unique<ew::Texture>("assets/Textures/windwaker/water16.png");
    water8 = std::make_unique<ew::Texture>("assets/Textures/windwaker/water8.png");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 0.0f, 1.0f},
        .position = {2.0f, 2.0f, 2.0f},
    };
    material = {
        .ambient = {0.5f, 0.5f, 0.5f},
        .diffuse = {0.5f, 0.5f, 0.5f},
        .specular = {0.5f, 0.5f, 0.5f},
        .shininess = 1.0f,
    };
    plane.load(ew::createPlane(100.0f,100.0f, 10));
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

auto matrix = glm::mat4(1.0f);

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D,->getID());

    water->use();

    // scene matrices
    water->setMat4("model", matrix);
    water->setMat4("view_proj", view_proj);
    water->setVec3("camera_position", camera.position);

    water->setInt("zaToon", 0);

    water->setVec3("light.pos", light.position);
    water->setVec3("light.color", light.color);

    water->setVec3("material.diffuse", material.diffuse);
    water->setVec3("material.specular", material.specular);
    water->setVec3("material.ambient", material.ambient);
    water->setFloat("material.shininess", material.shininess);
    // draw suzanne
    
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);


    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());
    
    ImGuizmo::DrawGrid(view, proj, glm::value_ptr(glm::mat4(1.0f)), 100.0f);

    glm::mat4 light_matrix = glm::translate(glm::mat4(1.0f), light.position);
    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(light_matrix)
    );

    if (ImGuizmo::IsUsing()) {
        light.position =  glm::vec3(light_matrix[3]);
    }

    cameracontroller.Debug();

    ImGui::Begin("Controlls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    // light
    ImGui::ColorEdit3("Light.color", &light.color[0]);

    // material
     ImGui::SliderFloat3("Ambient", &material.ambient[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Diffuse", &material.diffuse[0], 0.0f, 1.0f);
    ImGui::SliderFloat3("Specular", &material.specular[0], 0.0f, 1.0f);
    ImGui::SliderFloat("Shininess", &material.shininess, 2.0f, 1024.0f);

    ImGui::DragFloat("Alpha", &debug.alpha);
    /* build debug ui here */

    ImGui::End();
}