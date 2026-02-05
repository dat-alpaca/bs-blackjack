#include "game_data.h"
#include "platform.h"

#include <stdlib.h>

void initialize_game(game_data* game)
{
	BS_ASSERT_NULL(game);

	initialize_deck(&game->deck);
	shuffle_deck(&game->deck);

	game->result = GAME_RESULT_NONE;

	initialize_player_data(&game->dealerData);
	initialize_player_data(&game->userData);
}

int game_get_card_value(int card)
{
	if (card >= 10)
		return 10;

	return card;
}

void game_draw_card(deck* deck, player_data* player)
{
	BS_ASSERT_NULL(deck);
	BS_ASSERT_NULL(player);

	if (player->index >= 13 || deck->index >= 52)
	{
		fprintf(stderr, "Invalid deck index position");
		exit(1);
	}

	player->hand[player->index] = deck->deck[deck->index];
    
    int card = player->hand[player->index];
	player->score += game_get_card_value(player->hand[player->index]);
    
    if(card == 1 && player->score <= 11)
        player->score += 10;

	++deck->index;
	++player->index;
}