#pragma once
#include <ostream>
#include <iostream>

#include "../game/events.h"

void printCard(std::ostream& out, const Card * card);
void printColor(std::ostream& out, CardColor color);

class Logger: public Observer
{
    std::ostream& out;

    void printPlayer(int playerIndex);

public:
    Logger(std::ostream& outputStream = std::cout);

    virtual void handlePlayerEntered(int playerIndex, const std::string& name);
    virtual void handleSetStarted(int gameNumber);
    virtual void handleDeckShuffled();
    virtual void handleFirstCardPlaced(const Card * card);
    virtual void handlePlayerDealt(int playerIndex, int cardsNumber);
    virtual void handleCardPlayed(int playerIndex, const Card * card);
    virtual void handlePlayerDrewAnotherCard(int playerIndex);
    virtual void handlePlayerDrewAndSkip(int playerIndex, int numberOfCards);
    virtual void handlePlayerChangedColor(int playerIndex, CardColor newColor);
    virtual void handlePlayerSaid(int playerIndex, const std::string& message);
    virtual void handlePlayerDisqualified(
        int playerIndex, int handScore, const Card * card);
    virtual void handlePlayerWonSet(int playerIndex, int score);
    virtual void handlePlayerWonGame(int playerIndex, int totalScore);
    virtual void handleDirectionChanged(GameDirection newDirection);
    virtual void handleMessageOverflow();
};