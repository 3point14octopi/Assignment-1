#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Scene.h"
#include "Graphics/Textures/Texture3D.h"
#include "Gameplay/InputEngine.h"
#include "GLFW/glfw3.h"
#include "Application/Layers/RenderLayer.h"

/// <summary>
/// A glorified light switch that "turns off" a light in the scene if the 1 key is pressed, 
/// and turns it back on if any of keys 2 through 7 are pressed
/// </summary>
class LightInputManager : public Gameplay::IComponent {

public:

	typedef std::shared_ptr<LightInputManager> Sptr;
	LightInputManager();
	virtual ~LightInputManager();

	Gameplay::Material::Sptr        EnterMaterial;
	Gameplay::Material::Sptr        ExitMaterial;


	// Inherited from IComponent
	virtual void Update(float)override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static LightInputManager::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(LightInputManager);


protected:
	int cooldown = 0;
};