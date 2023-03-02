#include "Program.h"

#pragma execution_character_set( "utf-8" )

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

///////////////////////////////////
//
//
//////////////////////////////////

int main()
{
	SetConsoleOutputCP(65001);

	///////////load icon////////////////////

	GLFWimage images[1];

	images[0].pixels = stbi_load("icon.png", &images[0].width, &images[0].height, 0, 4);

	Program program("Perlin Noise 2D", true, images);

	stbi_image_free(images[0].pixels);

	///////////main loop////////////////////

	while (!program.getWindowShouldClose())
	{
		///////////draw////////////////////

		program.render();

		///////////update////////////////////

		program.update();
	}

	///////////free memory////////////////////

	return 0;
}

//////////////////////////////////
