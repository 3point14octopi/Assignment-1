#include "Gameplay/Components/LightInputManager.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Application/Application.h"
#include "Application/ApplicationLayer.h"
#include "Application/Layers/RenderLayer.h"


LightInputManager::LightInputManager() :
	IComponent()
{ }
LightInputManager::~LightInputManager() = default;


void LightInputManager::RenderImGui() { }

nlohmann::json LightInputManager::ToJson() const {
	nlohmann::json result;
	return {
		{ "enter_material", EnterMaterial != nullptr ? EnterMaterial->GetGUID().str() : "null" },
		{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	};
}

LightInputManager::Sptr LightInputManager::FromJson(const nlohmann::json& blob) {
	LightInputManager::Sptr result = std::make_shared<LightInputManager>();
	result->EnterMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["enter_material"]));
	result->ExitMaterial  = ResourceManager::Get<Gameplay::Material>(Guid(blob["exit_material"]));
	return result;
}


void LightInputManager::Update(float deltaTime) {
	if (InputEngine::GetKeyState(GLFW_KEY_1) == ButtonState::Pressed) {
			GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(0.f);
			GetGameObject()->GetScene()->Lights[0].Range = 0.f;
			GetGameObject()->GetScene()->SetupShaderAndLights();
	}
	else if (!GetGameObject()->GetScene()->Lights[0].Color.x) {
		for (int i = 50; i < 56; i++) {
			if (InputEngine::GetKeyState(i) == ButtonState::Pressed) {
				GetGameObject()->GetScene()->Lights[0].Color = glm::vec3(1.f);
				GetGameObject()->GetScene()->Lights[0].Range = 100.f;
				GetGameObject()->GetScene()->SetupShaderAndLights();
			}
		}
	}

}
