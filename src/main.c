#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "app/client_data.h"
#include "core/terminal.h"
#include "game/game_data.h"
#include "app/handlers.h"

static bool initialize()
{
	terminal_initialize();

	srand(time(NULL) + getpid());
	atexit(terminal_exit);

	if (!isatty(STDIN_FILENO))
		return false;

	terminal_enable_raw_mode();
	return true;
}

int main()
{
	// environment initialization:
	if (!initialize())
	{
		fprintf(stderr, "Failed to initialize terminal. Must be a TTY to work");
		return 1;
	}

	// data initialization:
	game_data game = { 0 };
	ui_data ui = { 0 };
	client_data clientData = { 0 };
	ui.isRunning = true;

	ui_print_menu(&game, &ui, &clientData);

	while (ui.isRunning)
	{
		switch (ui.currentView)
		{
			case UI_VIEW_MAIN_MENU:
				handle_main_menu(&game, &ui, &clientData);
				break;

			case UI_VIEW_PLAYING:
			{
				handle_playing(&game, &ui, &clientData);
				if (!handle_replay())
					return 0;
			}
			break;
		}
	}

	return 0;
}
