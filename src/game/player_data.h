#pragma once
#include "defines.h"

struct
{
	int hand[HAND_SIZE];
	int index;
	int score;
} typedef player_data;

void initialize_player_data(player_data* player);
