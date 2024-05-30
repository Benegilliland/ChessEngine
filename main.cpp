#include "engine.h"

int main(int argc, char *argv[])
{
	bool gui_enabled = true;
	int width = 1000;
	int height = 1000;

	engine chess(gui_enabled, width, height);
	chess.run();
	return 0;
}
