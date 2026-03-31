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
    blinnshadow = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnshadow.fs");
    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");
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
    CreateFrameBuffer();
    CreateDepthBuffer();
    plane.load(ew::createPlane(100.0f,100.0f, 10));
    
}
void Scene::CreateFrameBuffer() {
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
        printf("were incomplete\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}
void Scene::CreateDepthBuffer() {
    glGenFramebuffers(1, &shadow_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);  
    glGenTextures(1, &shadow_depth);
    glBindTexture(GL_TEXTURE_2D,shadow_depth);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("were inclomplete\n");
    }
    glDrawBuffers(0, nullptr);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}
Scene::~Scene()
{
    glDeleteFramebuffers(1, &fbo);
    glDeleteFramebuffers(1, &shadow_fbo);
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
    const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    const auto light_view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    const auto light_view_proj = light_proj * light_view;
    
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
    {
        // render depth scene only
        // depth shader
        // susazzne
        // fromt the light
        glEnable(GL_CULL_FACE);
        if(debug.cull_front == true) {
            glCullFace(GL_FRONT);
        }
        else {
            glCullFace(GL_BACK);
        }
        glEnable(GL_DEPTH_TEST);

        depth->use();

        depth->setMat4("model", matrix);
        depth->setMat4("light_view_proj", light_view_proj);

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        // glDisable(GL_DEPTH_TEST);

        blinnshadow->use();

        // scene matrices
        blinnshadow->setMat4("model", matrix);
        blinnshadow->setMat4("view_proj", view_proj);
        blinnshadow->setVec3("camera_position", camera.position);

        blinnshadow->setVec3("light.pos", light.position);
        blinnshadow->setVec3("light.color", light.color);

        blinnshadow->setMat4("light_view_proj", light_view_proj);

        blinnshadow->setVec3("material.diffuse", material.diffuse);
        blinnshadow->setVec3("material.specular", material.specular);
        blinnshadow->setVec3("material.ambient", material.ambient);
        blinnshadow->setFloat("material.shininess", material.shininess);
        // draw suzanne
        suzanne->draw();
        
        // render lane
    }
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
    ImGui::Image(
    (void*)(intptr_t)shadow_depth,
    ImVec2(400, 300),
    ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}