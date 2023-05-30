#include "logger.h"

void printCard(std::ostream& out, const Card * card)
{
    if (card == nullptr)
    {
        out << "null";
        return;
    }
    if (!card->is_valid())
    {
        out << "error";
        return;
    }
    if (card->is_wild()) 
    {
        if (card->value == CardValue::Wild) out << "wild";
        else out << "wild draw 4";
        return;
    }
    printColor(out, card->color);
    switch(card->value)
    {
    case CardValue::Draw2:
        out << " draw 2"; break;
    case CardValue::Skip:
        out << " skip"; break;
    case CardValue::Reverse:
        out << " reverse"; break;
    default:
        out << " " << card->value; break;
    }
}

void Logger::printPlayer(int playerIndex)
{
    out << "Player #" << playerIndex << " ";
}

void printColor(std::ostream& out, CardColor color)
{
    switch (color)
    {
    case CardColor::Green:
        out << "green"; break;
    case CardColor::Yellow:
        out << "yellow"; break;
    case CardColor::Red:
        out << "red"; break;
    case CardColor::Blue:
        out << "blue"; break;
    }
}

Logger::Logger(std::ostream &outputStream):
    out(outputStream)
{}

void Logger::handlePlayerEntered(
    int playerIndex, const std::string &name)
{
    out << "Player '" << name << "' has entered the game with index " 
        << playerIndex << "." << std::endl;
}

void Logger::handleSetStarted(int gameNumber)
{
    out << "Set number " << gameNumber << " has started." << std::endl;
}

void Logger::handleDeckShuffled()
{
    out << "Deck is shuffled." << std::endl; 
}

void Logger::handleFirstCardPlaced(const Card *card)
{
    out << "First card is placed: ";
    printCard(out, card);
    out << "." << std::endl;
}

void Logger::handlePlayerDealt(int playerIndex, int cardsNumber)
{
    printPlayer(playerIndex);
    out << "has received " << cardsNumber << " cards." << std::endl;
}

void Logger::handleCardPlayed(int playerIndex, const Card *card)
{
    printPlayer(playerIndex);
    out << "played ";
    printCard(out, card);
    out << "." << std::endl;
}

void Logger::handlePlayerDrewAnotherCard(int playerIndex)
{
    printPlayer(playerIndex);
    out << "drew another card." << std::endl;
}

void Logger::handlePlayerDrewAndSkip(int playerIndex, int numberOfCards)
{
    printPlayer(playerIndex);
    if (numberOfCards == 0) out << "skipped turn." << std::endl;
    else out << "drew " << numberOfCards << " cards and skipped turn." 
             << std::endl;
}

void Logger::handlePlayerChangedColor(int playerIndex, CardColor newColor)
{
    printPlayer(playerIndex);
    out << "changed color to be ";
    printColor(out, newColor);
    out << "." << std::endl;
}

void Logger::handlePlayerSaid(int playerIndex, const std::string &message)
{
    printPlayer(playerIndex);
    out << "said: " << message << std::endl;
}

void Logger::handlePlayerDisqualified(
    int playerIndex, int handScore, const Card * card)
{
    printPlayer(playerIndex);
    out << "was disqualified for playing ";
    printCard(out, card);
    out << ", his hand score was " << handScore << "." << std::endl;
}

void Logger::handlePlayerWonSet(int playerIndex, int score)
{
    printPlayer(playerIndex);
    out << "won set and received " << score << " score." << std::endl;
}

void Logger::handlePlayerWonGame(int playerIndex, int totalScore)
{
    printPlayer(playerIndex);
    out << "won the game with total score: " << totalScore << "." << std::endl;
}

void Logger::handleDirectionChanged(GameDirection newDirection)
{
    out << "Game direction was changed to '"
        << (newDirection == GameDirection::Direct ? "direct" : "inverse")
        << "'." << std::endl;
}

void Logger::handleMessageOverflow()
{
    out << "Message queue overflew." << std::endl;
}
