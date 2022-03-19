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
/// A class that manages the colour correction used in the scene, using input keys 8-0
/// </summary>
class ColCorInputManager : public Gameplay::IComponent {

public:
	enum EffectIndex {

	};

	typedef std::shared_ptr<ColCorInputManager> Sptr;
	ColCorInputManager();
	virtual ~ColCorInputManager();

	Gameplay::Material::Sptr        EnterMaterial;
	Gameplay::Material::Sptr        ExitMaterial;


	// Inherited from IComponent
	virtual void Awake()override;
	virtual void Update(float)override;
	virtual void RenderImGui() override;
	virtual nlohmann::json ToJson() const override;
	static ColCorInputManager::Sptr FromJson(const nlohmann::json& blob);
	MAKE_TYPENAME(ColCorInputManager);


	void HandleLUTCommand(int lutIndex);
	void ToggleCurrentLUT();
	void SetLUT(Texture3D::Sptr);
	void PushLUT(Texture3D::Sptr);

protected:
	std::vector<Gameplay::GameObject>sceneObjects;
	std::vector<std::vector<Gameplay::Material::Sptr>>sceneMaterials;
	std::vector<Texture3D::Sptr>sceneLUTs;

	Texture3D::Sptr currentLUT = nullptr;
	bool LUTactive = false;

	int cooldown = 0;
};