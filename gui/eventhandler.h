#include <SDL2/SDL.h>
#include <queue>
#include "../common.h"

struct g_pos {
	int x, y;
};

enum class event_type {
	start_drag,
	stop_drag,
	drag_motion,
};

enum class s_state {
	not_waiting,
	awaiting_move,
	dragging_piece,
	awaiting_pawn_upgrade,
};

struct s_event {
	event_type type;
	g_pos pos;
	SDL_Point motion;
};

class g_eventhandler {
private:
  int width, height;
  SDL_Event event;
  std::queue<s_event> eventQueue; 
  s_state st;

public:
  g_eventhandler(int, int);
  void setState(s_state);
  void poll();
  g_pos mouseToBoard(const SDL_Point &);
  void mouseDown();
  void mouseUp();
  void mouseMotion();
  void startDragging();
  void stopDragging();
  void dragPiece();
  bool getStartPos(start_pos &p);
};
