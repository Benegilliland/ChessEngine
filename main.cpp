#include "engine.h"

int main(int argc, char *argv[])
{
	bool gui_enabled = true;
	int width = 800;
	int height = 800;

	engine chess(gui_enabled, width, height);
	chess.run();
	return 0;
}
