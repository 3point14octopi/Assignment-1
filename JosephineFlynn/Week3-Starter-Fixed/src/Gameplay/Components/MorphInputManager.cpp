#include "Gameplay/Components/MorphInputManager.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Application/Application.h"
#include "Application/ApplicationLayer.h"
#include "Application/Layers/RenderLayer.h"
#include "Gameplay/Components/MorphAnimationManager.h"


MorphInputManager::MorphInputManager() :
	IComponent()
{ }
MorphInputManager::~MorphInputManager() = default;


void MorphInputManager::RenderImGui() { }

nlohmann::json MorphInputManager::ToJson() const {
	nlohmann::json result;
	return {
		{ "enter_material", EnterMaterial != nullptr ? EnterMaterial->GetGUID().str() : "null" },
		{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	};
}

MorphInputManager::Sptr MorphInputManager::FromJson(const nlohmann::json& blob) {
	MorphInputManager::Sptr result = std::make_shared<MorphInputManager>();
	result->EnterMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["enter_material"]));
	result->ExitMaterial  = ResourceManager::Get<Gameplay::Material>(Guid(blob["exit_material"]));
	return result;
}


void MorphInputManager::Awake() {
	for (int i = 0; i < sceneItems.size(); i++) {
		sceneItems[i]->Get<MorphAnimationManager>()->currentlyAnimating = false;
	}
}


void MorphInputManager::Update(float deltaTime) {
	if (InputEngine::GetKeyState(GLFW_KEY_5) == ButtonState::Pressed) {
		toggledOn = true;
		for (int i = 0; i < sceneItems.size(); i++) {
			sceneItems[i]->Get<MorphAnimationManager>()->currentlyAnimating = true;
		}
	}
	else if (toggledOn) {
		for (int i = 49; i < 56; i++) {
			if (InputEngine::GetKeyState(i) == ButtonState::Pressed && i != 53) {
				toggledOn = false;
				for (int i = 0; i < sceneItems.size(); i++) {
					sceneItems[i]->Get<MorphAnimationManager>()->currentlyAnimating = false;
				}
			}
		}
	}//j

}


void MorphInputManager::AddItem(Gameplay::GameObject::Sptr gOb) {
	sceneItems.push_back(gOb);
}

