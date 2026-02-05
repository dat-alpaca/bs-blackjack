#pragma once
#include <stdbool.h>
#include "app/client_data.h"
#include "game/game_data.h"

enum
{
	UI_VIEW_MAIN_MENU = 0,
	UI_VIEW_PLAYING
} typedef ui_view;

enum
{
	UI_OPTION_PLAY = 0,
	UI_OPTION_QUIT = 1,
	UI_OPTION_AMOUNT = 2
} typedef ui_option;

struct
{
	ui_view currentView;
	ui_option option;
	bool isRunning;
} typedef ui_data;

void ui_cycle_option(ui_data* ui, bool reverse);

void ui_print_header(client_data* clientData);
void ui_print_options(game_data* game, client_data* clientData);
void ui_print_menu(game_data* game, ui_data* ui, client_data* clientData);

void ui_print_card(int card);
void ui_print_cards(player_data* player);
void ui_reveal_cards(game_data* game);

void ui_print_win(game_data* game, client_data* clientData);
void ui_print_lose(game_data* game, client_data* clientData);
void ui_print_draw(game_data* game, client_data* clientData);