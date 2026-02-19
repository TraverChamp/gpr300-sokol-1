#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");

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
    //creating framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);  
    glGenTextures(1, &fbo_color_0);
    glBindTexture(GL_TEXTURE_2D,fbo_color_0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_color_0, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("were inclomplete\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}

Scene::~Scene()
{
    glDeleteFramebuffers(1, &fbo);  
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
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        // glDisable(GL_DEPTH_TEST);

        blinnphong->use();

        // scene matrices
        blinnphong->setMat4("model", matrix);
        blinnphong->setMat4("view_proj", view_proj);
        blinnphong->setVec3("camera_position", camera.position);

        blinnphong->setVec3("light.pos", light.position);
        blinnphong->setVec3("light.color", light.color);

        blinnphong->setVec3("material.diffuse", material.diffuse);
        blinnphong->setVec3("material.specular", material.specular);
        blinnphong->setVec3("material.ambient", material.ambient);
        blinnphong->setFloat("material.shininess", material.shininess);
        // draw suzanne
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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
    ImGui::Image(
        (void*)(intptr_t)fbo_color_0,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));
        //if(ImGui::BeginCombo("ShaderOpt", shaderOptions[index].c_str())) {}

    ImGui::End();
}