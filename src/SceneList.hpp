#pragma once
#include "Utilities/Glm.hpp"
#include <functional>
#include <string>
#include <tuple>
#include <vector>

namespace Assets
{
	class Model;
	class Texture;
}

typedef std::tuple<std::vector<Assets::Model>, std::vector<Assets::Texture>> SceneAssets;

class SceneList final
{
public:

	struct CameraInitialSate
	{
		glm::mat4 ModelView;
		float FieldOfView;
		float Aperture;
		float FocusDistance;
		float ControlSpeed;
		bool GammaCorrection;
		bool HasSky;
	};

	static SceneAssets CubeAndSpheres(CameraInitialSate& camera);
	static SceneAssets RayTracingInOneWeekend(CameraInitialSate& camera);
	static SceneAssets PlanetsInOneWeekend(CameraInitialSate& camera);
	static SceneAssets LucyInOneWeekend(CameraInitialSate& camera);
	static SceneAssets CornellBox(CameraInitialSate& camera);
	static SceneAssets CornellBoxLucy(CameraInitialSate& camera);
	static SceneAssets CubesAndCommonScene(CameraInitialSate& camera);
	static SceneAssets CylinderCubesCommonScene(CameraInitialSate& camera);
	// static SceneAssets TestScene(CameraInitialSate& camera);
	static SceneAssets TreesAndGrass(CameraInitialSate& camera);
	static SceneAssets blender_2_78(CameraInitialSate& camera);
	static SceneAssets blender_3_2(CameraInitialSate& camera);
	// static SceneAssets blender_3_3(CameraInitialSate& camera);

	static const std::vector<std::pair<std::string, std::function<SceneAssets (CameraInitialSate&)>>> AllScenes;
};
