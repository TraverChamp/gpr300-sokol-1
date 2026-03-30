#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"
enum fx 
{
    FX_NONE = 0,
    FX_INVERT = 1,
    FX_BLUR = 2,
    FX_ABBERATION = 3,
    FX_GRAY = 4,
    FX_KERNEL = 5,
    FX_EDGE_DET = 6,
    FX_RIDGE = 7,
    FX_FISHEYE = 8,
};
static std::vector<std::string> post_processing_effects = {
    "None",
    "Inverse",
    "Blur",
    "Chromatic Aberration",
    "Grayscale",
    "Kernel Base",
    "Sharpening",
    "Edge Detection",
    "Ridge",
    "Fisheye"
};
struct  {
    float strength = 15.0f;
    float alpha = 64.0f;
    int width = 3;
    int spacing = 2;
} debug;
struct
{
    int index = 0;

    struct
    {
        float strength = 16.0f;
    } blur;

    struct
    {
        glm::vec3 offset = glm::vec3(0.009f, 0.006f, -0.006f);
        glm::vec2 direction = glm::vec2(1.0f);
    } chromatic;
} effect;
struct fullscreen_quad {
    GLuint vao;
    GLuint vbo;
    void Initialize()
    {
        float quad_verts[] = {
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };
        
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        //bind
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), &quad_verts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float)*2));

        glBindVertexArray(0);
    }
}fullscreen_quad;
struct framebuffer {
    GLuint fbo;
    GLuint color0;
    GLuint color1;
    GLuint color2;
    GLuint depth;
    void Initialize() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);  
    glGenTextures(1, &color0);
    glBindTexture(GL_TEXTURE_2D,color0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0, 0);
    
    // Create depth texture
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("were incomplete\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    };
}framebuffer;
    void post_process(ew::Shader* shader)
{
    shader->use();
    shader->setInt("texture0", 0);

    // what other uniforms should we send ?
    switch (effect.index)
    {
    case FX_GRAY:
        break;
    case FX_BLUR:
        shader->setFloat("strength", effect.blur.strength);
        break;
    case FX_INVERT:
        break;
    case FX_ABBERATION:
        shader->setVec3("offset", effect.chromatic.offset);
        shader->setVec2("direction", effect.chromatic.direction);
        break;
    case FX_EDGE_DET:
        break;
    case FX_KERNEL:
        break;
    case FX_RIDGE:
        break;
    case FX_FISHEYE:
        break;
    default:
        break;
    }
    // fullscreen quad pipeline:
    glDisable(GL_DEPTH_TEST);

    // clear default buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw fullscreen quad
    glBindVertexArray(fullscreen_quad.vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer.color0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
};
Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/fullscreen.fs")); // default
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/inverse.fs")); // Inverse
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/Blur1.fs")); // Blur
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/chromatic.fs")); // Chroma
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/grayscale.fs")); // Grays
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/Kernel.fs")); // Kernel
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/Sharpen.fs")); // Sharpening
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/edge.fs")); // Edge
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/Ridge.fs")); // Ridge
    fxShaders.push_back(std::make_unique<ew::Shader>("assets/shaders/PostProcess/fullscreen.vs", "assets/shaders/PostProcess/fisheye.fs")); // Fisheye



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
    fullscreen_quad.Initialize();

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
        for(auto x = -debug.width; x<=debug.width; x++) {
            for(auto y = -debug.width; y<=debug.width;y++) {
                auto position = glm::vec3(x*debug.spacing , 0, y * debug.spacing);
                auto mat = glm::translate(glm::mat4(1.0f), position);

                blinnphong->setMat4("model", mat);
            }
        }
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    Scene::post_process(fxShaders[effect.index].get());
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
        if(ImGui::BeginCombo("ShaderOpt", post_processing_effects[effect.index].c_str())) {
            for (auto n = 0; n < post_processing_effects.size(); ++n)
        {
            auto is_selected = (post_processing_effects[effect.index] == post_processing_effects[n]);
            if (ImGui::Selectable(post_processing_effects[n].c_str(), is_selected))
            {
                effect.index = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }

    ImGui::End();
}
}