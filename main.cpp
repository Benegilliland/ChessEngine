#include "engine.h"

int main(int argc, char *argv[])
{
	bool gui_enabled = true;
	control white = control::human;
	control black = control::computer;
	int width = 800;
	int height = 800;

	engine chess(white, black, gui_enabled, width, height);
	chess.run();
	return 0;
}
