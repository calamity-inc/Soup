#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	struct elo
	{
		static constexpr int BASE_RATING = 1500;

		[[nodiscard]] static float getProbabiltyPlayerAWins(int rating_player_a, int rating_player_b)
		{
			return 1.0 / (1.0 + pow(10.0, (rating_player_b - rating_player_a) / 400.0));
		}

		[[nodiscard]] static int getRatingAdjustment(int rating_player_a, int rating_player_b, float outcome)
		{
			return getRatingAdjustment(rating_player_a, rating_player_b, outcome, getProbabiltyPlayerAWins(rating_player_a, rating_player_b));
		}

		[[nodiscard]] static int getRatingAdjustment(int rating_player_a, int rating_player_b, float outcome, float probability_player_a_wins)
		{
			constexpr float k_factor = 32.0f;
			return round(k_factor * (outcome - probability_player_a_wins));
		}

		static void update(int& rating_player_a, int& rating_player_b, float outcome)
		{
			update(rating_player_a, rating_player_b, outcome, getProbabiltyPlayerAWins(rating_player_a, rating_player_b));
		}

		static void update(int& rating_player_a, int& rating_player_b, float outcome, float probability_player_a_wins)
		{
			auto adjustment = getRatingAdjustment(rating_player_a, rating_player_b, outcome, probability_player_a_wins);
			rating_player_a += adjustment;
			rating_player_b -= adjustment;
		}
	};
}
