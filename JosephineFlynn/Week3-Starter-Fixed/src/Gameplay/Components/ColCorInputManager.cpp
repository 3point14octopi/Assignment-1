#include "Gameplay/Components/ColCorInputManager.h"
#include "Gameplay/Components/ComponentManager.h"
#include "Gameplay/GameObject.h"
#include "Application/Application.h"
#include "Application/ApplicationLayer.h"
#include "Application/Layers/RenderLayer.h"
#include "Gameplay/Material.h"


ColCorInputManager::ColCorInputManager() :
	IComponent()
{ }
ColCorInputManager::~ColCorInputManager() = default;


void ColCorInputManager::RenderImGui() { }

nlohmann::json ColCorInputManager::ToJson() const {
	nlohmann::json result;
	return {
		{ "enter_material", EnterMaterial != nullptr ? EnterMaterial->GetGUID().str() : "null" },
		{ "exit_material", ExitMaterial != nullptr ? ExitMaterial->GetGUID().str() : "null" }
	};
}

ColCorInputManager::Sptr ColCorInputManager::FromJson(const nlohmann::json& blob) {
	ColCorInputManager::Sptr result = std::make_shared<ColCorInputManager>();
	result->EnterMaterial = ResourceManager::Get<Gameplay::Material>(Guid(blob["enter_material"]));
	result->ExitMaterial  = ResourceManager::Get<Gameplay::Material>(Guid(blob["exit_material"]));
	return result;
}

void ColCorInputManager::Awake() {
	Application::Get().GetLayer<RenderLayer>()->SetRenderFlags(RenderFlags::None);
}


void ColCorInputManager::Update(float deltaTime) {
	cooldown = (cooldown) ? cooldown - 1 : cooldown;

	if (!cooldown) {
		if (InputEngine::GetKeyState(GLFW_KEY_8) == ButtonState::Pressed) {
			HandleLUTCommand(0);
		}
		else if (InputEngine::GetKeyState(GLFW_KEY_9) == ButtonState::Pressed) {
			HandleLUTCommand(1);
		}
		else if (InputEngine::GetKeyState(GLFW_KEY_0) == ButtonState::Pressed) {
			HandleLUTCommand(2);
		}
	}
}


void ColCorInputManager::HandleLUTCommand(int lutIndex) {
	if (sceneLUTs.size() > lutIndex) {
		if (sceneLUTs[lutIndex] != nullptr) {
			if (!LUTactive) {
				SetLUT(sceneLUTs[lutIndex]);
				ToggleCurrentLUT();
			}
			else if (currentLUT != sceneLUTs[lutIndex]) {
				SetLUT(sceneLUTs[lutIndex]);
			}
			else {
				ToggleCurrentLUT();
			}

			cooldown = 20;
		}
	}
}

void ColCorInputManager::PushLUT(Texture3D::Sptr lut) {
	sceneLUTs.push_back(lut);
}

void ColCorInputManager::SetLUT(Texture3D::Sptr lut) {
	GetGameObject()->GetScene()->SetColorLUT(lut);
	currentLUT = lut;
}

void ColCorInputManager::ToggleCurrentLUT() {
	RenderLayer::Sptr rLayer = Application::Get().GetLayer<RenderLayer>();
	RenderFlags toggle = rLayer->GetRenderFlags();
	toggle = (toggle == RenderFlags::None) ? RenderFlags::EnableColorCorrection : RenderFlags::None;
	rLayer->SetRenderFlags(toggle);
	LUTactive = !LUTactive;
}