#pragma once
#include <stdbool.h>

#include "ui.h"
#include "app/client_data.h"
#include "game/game_data.h"

void handle_main_menu(game_data* game, ui_data* ui, client_data* clientData);
void handle_playing(game_data* game, ui_data* ui, client_data* clientData);

bool handle_replay();