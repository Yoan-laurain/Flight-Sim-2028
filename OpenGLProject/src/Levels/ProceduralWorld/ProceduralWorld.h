#pragma once

#include "Levels/Level.h"

# define PLANE_FILE_PATH "res/models/airplane/scene.gltf"

class ProceduralWorld : public Level
{
	public:
		ProceduralWorld();
		~ProceduralWorld() override;

	private:

		void CreateAirPlanes(unsigned int count);
};