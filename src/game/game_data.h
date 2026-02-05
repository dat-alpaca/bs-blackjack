#pragma once
#include "deck.h"
#include "player_data.h"
#include "game_result.h"


typedef struct game_data
{
	deck deck;
	game_result result;

	player_data userData;
	player_data dealerData;
} game_data;

void initialize_game(game_data* game);

int game_get_card_value(int card);

void game_draw_card(deck* deck, player_data* player);