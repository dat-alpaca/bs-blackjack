#include "player_data.h"
#include "platform.h"

void initialize_player_data(player_data* player)
{
	BS_ASSERT_NULL(player);

	player->index = 0;
	player->score = 0;
}