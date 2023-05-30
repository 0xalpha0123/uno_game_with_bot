#include "stats.h"

void StatsObserver::assureIsAllocated() 
{
    if (!wins.empty()) return;
    wins.resize(game->numberOfPlayers());
    scores.resize(game->numberOfPlayers());
}

void StatsObserver::registerWin(int winnerIndex) 
{
    if (winnerIndex < 0) return;
    assureIsAllocated();
    for (int i = 0; i < wins.size(); i++)
        wins.at(i).push_back(i == winnerIndex);
    const auto playerScores = game->currentScores();
    for (int i = 0; i < scores.size(); i++)
        scores.at(i).push_back(playerScores.at(i));
}

StatsObserver::StatsObserver(const UnoGame * game): 
  game(game), wins(), scores()
{}

void StatsObserver::reserve(int numberOfPlayers, size_t numberOfGames)
{
    wins.resize(numberOfPlayers);
    scores.resize(numberOfPlayers);

    for (auto& vec: wins) vec.reserve(numberOfGames);
    for (auto& vec: scores) vec.reserve(numberOfGames);
}

StatsObserver runGames(UnoGame& game, int numberOfGames)
{
    StatsObserver observer(&game);
    observer.reserve(game.numberOfPlayers(), numberOfGames);
    game.addObserver(&observer);
    for(int i = 0; i < numberOfGames; ++i) 
    {
        game.runGame();
        game.shufflePlayers();
    }
    return observer;
}
