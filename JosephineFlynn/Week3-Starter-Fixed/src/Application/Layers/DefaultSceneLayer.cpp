#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"

//new components
#include "Gameplay/Components/ColCorInputManager.h"
#include "Gameplay/Components/LightInputManager.h"
#include "Gameplay/Components/MorphAnimationManager.h"
#include "Gameplay/Components/MorphRenderComponent.h"
#include "Gameplay/Components/MorphInputManager.h"


// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad;
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}

void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	Application& app = Application::Get();

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		
		// Shader
		ShaderProgram::Sptr customShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ShaderPartType::Vertex, "shaders/vertex_shaders/vert_morph.glsl"},
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/sad_toggled_phong.glsl" }
		});
		customShader->SetDebugName("Custom_Shader_Program");


		// Load in the meshes
		MeshResource::Sptr handFrame0 = ResourceManager::CreateAsset<MeshResource>("Meshes/hand/handIdleMesh-0.obj");
		MeshResource::Sptr handFrame1 = ResourceManager::CreateAsset<MeshResource>("Meshes/hand/handIdleMesh-3.obj");
		MeshResource::Sptr handFrame2 = ResourceManager::CreateAsset<MeshResource>("Meshes/hand/handIdleMesh-4.obj");
		MeshResource::Sptr handFrame3 = ResourceManager::CreateAsset<MeshResource>("Meshes/hand/handIdleMesh-5.obj");
		MeshResource::Sptr boxFrame1 = ResourceManager::CreateAsset<MeshResource>("Meshes/cubes/1.obj");
		MeshResource::Sptr boxFrame2 = ResourceManager::CreateAsset<MeshResource>("Meshes/cubes/2.obj");
		MeshResource::Sptr boxFrame3 = ResourceManager::CreateAsset<MeshResource>("Meshes/cubes/3.obj");

		// Load in some textures
		Texture2D::Sptr    boxTexture   = ResourceManager::CreateAsset<Texture2D>("textures/forggyBonds.png");
		Texture2D::Sptr    handTex   = ResourceManager::CreateAsset<Texture2D>("textures/Hand.png");


		// Loading in a 1D LUT
		Texture1D::Sptr toonLut = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png"); 
		toonLut->SetWrap(WrapMode::ClampToEdge);

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" }
		});

		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>(); 

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap); 
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up 
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Loading in colour lookup tables
		Texture3D::Sptr warmLut   = ResourceManager::CreateAsset<Texture3D>("luts/warm_custom.CUBE");  
		Texture3D::Sptr coolLut   = ResourceManager::CreateAsset<Texture3D>("luts/cool_custom.CUBE");
		Texture3D::Sptr spookyLut = ResourceManager::CreateAsset<Texture3D>("luts/spooky_custom.CUBE");

		// Configure the color correction LUT
		scene->SetColorLUT(warmLut);




		// Create our materials
		Material::Sptr boxMaterial = ResourceManager::CreateAsset<Material>(customShader);
		{
			boxMaterial->Name = "Box";
			boxMaterial->Set("u_Material.Diffuse", boxTexture);
			boxMaterial->Set("u_Material.Shininess", 0.1f);
		}

		Material::Sptr handMat = ResourceManager::CreateAsset<Material>(customShader);
		{
			handMat->Name = "Hand_Mat";
			handMat->Set("u_Material.Diffuse", handTex);
			handMat->Set("u_Material.Shininess", 0.5f);
		}






		


		// Create some lights for our scene
		scene->Lights.resize(1);
		scene->Lights[0].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 100.0f;

		

		// Set up the scene's camera
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion({ -10, -6, 7.6 });
			camera->SetRotation(glm::vec3(60.f, 0.f, -56.f));
			camera->LookAt(glm::vec3(0.0f));
		}

		// Set up all our sample objects

		GameObject::Sptr magicBox = scene->CreateGameObject("Magic Box 1");
		{
			// Set and rotation position in the scene
			magicBox->SetPostion(glm::vec3(0, -0.0f, 1.0f));
			magicBox->SetRotation(glm::vec3(0.f, 90.f, 0.f));

			// Add a render component
			RenderComponent::Sptr renderer = magicBox->Add<RenderComponent>();
			renderer->SetMesh(boxFrame2);
			renderer->SetMaterial(boxMaterial);

			//morph
			MorphRenderComponent::Sptr morphRenderer = magicBox->Add<MorphRenderComponent>(boxFrame2);

			MorphAnimationManager::Sptr animator = magicBox->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{boxFrame1, boxFrame3}, 0.5);
			animator->SetContinuity(true);
		}


		GameObject::Sptr hand = scene->CreateGameObject("Hand");
		{
			// Set position in the scene 
			hand->SetPostion(glm::vec3(0.f, 0.0f, 3.7f));
			hand->SetRotation(glm::vec3(-3, 37.f, 69.f));

			//renderer
			RenderComponent::Sptr renderer = hand->Add<RenderComponent>();
			renderer->SetMesh(handFrame0);
			renderer->SetMaterial(handMat);


			//morph
			MorphRenderComponent::Sptr morphRenderer = hand->Add<MorphRenderComponent>(handFrame0);

			MorphAnimationManager::Sptr animator = hand->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{handFrame1, handFrame2, handFrame3}, 0.5);
			animator->SetContinuity(true);
		}
		



		/// <summary>
		///	This object manages input to showcase the CG effects for this assignment
		/// </summary>
		GameObject::Sptr graphicsManager = scene->CreateGameObject("Graphics_Manager");
		{
			graphicsManager->SetPostion(glm::vec3(100.f, 100.f, 0));

			//renderer
			RenderComponent::Sptr renderer = graphicsManager->Add<RenderComponent>();
			renderer->SetMesh(handFrame0);
			renderer->SetMaterial(handMat);

			//morph
			MorphRenderComponent::Sptr morphRenderer = graphicsManager->Add<MorphRenderComponent>(handFrame0);

			MorphAnimationManager::Sptr animator = graphicsManager->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{handFrame1, handFrame2, handFrame3}, 0.5);
			animator->SetContinuity(true);


			//turns scene light off if 1 is pressed, and back on if any of keys 2-6 are pressed
			LightInputManager::Sptr lightSwitch = graphicsManager->Add<LightInputManager>();
			
			MorphInputManager::Sptr morphToggle = graphicsManager->Add<MorphInputManager>();
			morphToggle->AddItem(hand);
			morphToggle->AddItem(graphicsManager);
			morphToggle->AddItem(magicBox);

			//toggles on/off warm(8), cool(9), and spooky(0) colour correction
			ColCorInputManager::Sptr cc_manager = graphicsManager->Add<ColCorInputManager>();
			cc_manager->PushLUT(warmLut);
			cc_manager->PushLUT(coolLut);
			cc_manager->PushLUT(spookyLut);
		}




		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}
