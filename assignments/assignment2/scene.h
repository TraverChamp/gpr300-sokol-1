#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/materials.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"
struct {
float alpha = 64.0f;
}debug;
class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);
    void CreateFrameBuffer();
    void CreateDepthBuffer();

  private:
  unsigned int fbo;
  unsigned int fbo_color_0;
  unsigned int fbo_depth;

  unsigned int shadow_fbo;
  unsigned int shadow_depth;
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> blinnshadow;
    std::unique_ptr<ew::Shader> depth;
    
    ew::Mesh plane;
    batteries::light_t light;
    batteries::material_t material;
};
