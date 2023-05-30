#pragma once 
#include <string>
#include <iterator>
#include "card.h"

/**
 * @brief Направление игры. Если направление прямое (Direct), то следующим
 * будет игрок с бОльшим номером в списке, иначе — с меньшим.
*/
enum GameDirection
{
    Direct  = 0,
    Inverse = 1,
};

/**
 * @brief Наблюдатель — сущность, которая может реагировать на игровые события.
 * С каждым игровым событием связан метод класса, который будет вызван для
 * наблюдателя, когда событие произойдет.
 * 
 * @details Игрок является наблюдателем, так что он может необходимым ему образом
 * запоминать информацию о том, что происходит в игре, для того, чтобы потом 
 * использовать это в своей тактике.
 * 
 * Также наблюдатель может выполнять служебные функции, например, осуществлять 
 * вывод партии в консоль или в другой интерфейс или собирать статистику.
*/
class Observer 
{
public:
    /// @brief Событие 1. Игрок с именем `name` вошел в игру под номером `k`.
    /// @param playerIndex номер игрока в списке.
    /// @param name имя игрока.
    virtual void handlePlayerEntered(int playerIndex, const std::string& name) {}

    /// @brief Событие 2. Начинается партия с номером `gameNumber`.
    /// @param gameNumber номер партии.
    virtual void handleSetStarted(int gameNumber) {}

    /// @brief Событие 3. Колода обновлена и перемешана.
    virtual void handleDeckShuffled() {}

    /// @brief Событие 4. Выложена первая карта сброса.
    /// @param card первая карта сброса.
    virtual void handleFirstCardPlaced(const Card * card) {}

    /// @brief Событие 5. Игрок получил карты из колоды.
    /// @param playerIndex номер игрока в списке.
    /// @param cardsNumber количество карт.
    virtual void handlePlayerDealt(int playerIndex, int cardsNumber) {}

    /// @brief Событие 6. Игрок положил карту в сброс.
    /// @param playerIndex номер игрока в списке.
    /// @param card карта, которую положил игрок.
    virtual void handleCardPlayed(int playerIndex, const Card * card) {}

    /// @brief Событие 7. Игрок получил дополнительную карту.
    /// @param playerIndex номер игрока в списке.
    virtual void handlePlayerDrewAnotherCard(int playerIndex) {}

    /// @brief Событие 8. Игрок получил карты и пропустил ход.
    /// @param playerIndex номер игрока в списке.
    /// @param numberOfCards количество карт.
    virtual void handlePlayerDrewAndSkip(int playerIndex, int numberOfCards) {}

    /// @brief Событие 9. Игрок заказал цвет.
    /// @param playerIndex номер игрока в списке.
    /// @param newColor новый цвет.
    virtual void handlePlayerChangedColor(int playerIndex, CardColor newColor) {}

    /// @brief Событие 10. Игрок отправил сообщение.
    /// @param playerIndex номер игрока в списке.
    /// @param message сообщение игрока.
    virtual void handlePlayerSaid(int playerIndex, const std::string& message) {}

    /// @brief Событие 11. Игрок дисквалифицирован.
    /// @param playerIndex номер игрока в списке.
    /// @param handScore суммарная стоимость его карт.
    /// @param card карта, которую он положил
    virtual void handlePlayerDisqualified(
        int playerIndex, 
        int handScore,
        const Card * card) {}

    /// @brief Событие 12. Игрок выигрывает партию.
    /// @param playerIndex номер игрока в списке.
    /// @param score количество выигранных очков. 
    virtual void handlePlayerWonSet(int playerIndex, int score) {}

    /// @brief Событие 13. Игрок выигрывает игру.
    /// @param playerIndex номер игрока в списке.
    /// @param totalScore суммарное количество очков.
    virtual void handlePlayerWonGame(int playerIndex, int totalScore) {}

    /// @brief Событие 14. Изменилось направление игры.
    /// @param newDirection новое направление игры.
    virtual void handleDirectionChanged(GameDirection newDirection) {}

    /// @brief Событие 15. Достигнут лимит количества сообщений.
    virtual void handleMessageOverflow() {}

    /// @brief Событие 16. Достигнут лимит ходов в партии. Партия заканчивается
    /// в ничью.
    virtual void handleTurnsLimitReached() {}

    /// @brief Событие 17. Достигнут лимит партий в игре.
    /// @param winnerIndex индекс игрока с наибольшим количеством очков, если 
    /// его можно определить однозначно, иначе -1.
    /// @param winnerScore наибольшее количество очков среди игроков.
    virtual void handleSetsLimitReached(int winnerIndex, int winnerScore) {}
};

/// @brief Игровые события, соответствуют методам класса Observer
enum GameEvent
{
    PlayerEntered = 1,
    SetStarted,
    DeckShuffled,
    FirstCardPlaced,
    PlayerDealt,
    CardPlayed,
    PlayerDrewAnotherCard,
    PlayerDrewAndSkipped,
    PlayerChangedColor,
    PlayerSaid,
    PlayerDisqualified,
    PlayerWonSet,
    PlayerWonGame,
    DirectionChanged,
    MessageOverflow,
    TurnsLimitReached,
    SetsLimitReached,
};

/**/ 

/// 
/**
 * @brief Служебный класс для рассылки событий множеству участников.
 * @tparam iterator  input_iterator, указывающий на тип Observer *.
 * @details Каждое событие в этом классе обрабатывается по следующему алгоритму:
 * 1. Вызывается функция beforeEach, куда передается тип события.
 * 2. Для всех наблюдателей от begin() до end() вызывается обработчик этого 
 * события с этими параметрами.
 * 3. Вызывается функция afterEach, куда передается тип события.
 * 
 * Для использования класса необходимо определить методы begin и end, 
 * возвращающие начало и конец множества наблюдателя. 
 * 
 * Для задания дополнительного поведения можно переопределить функции 
 * beforeEach и afterEach.
 * 
 * Пример использования класса можно посмотреть в классе UnoGame. 
*/
template<class iterator>
class Broadcaster: public Observer
{
protected:
    virtual iterator begin() = 0;
    virtual iterator end() = 0;
    virtual void beforeEach(GameEvent event) {}
    virtual void afterEach(GameEvent event) {}

public:

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
        int playerIndex, 
        int handScore,
        const Card * card);
    virtual void handlePlayerWonSet(int playerIndex, int score);
    virtual void handlePlayerWonGame(int playerIndex, int totalScore);
    virtual void handleDirectionChanged(GameDirection newDirection);
    virtual void handleMessageOverflow();
    virtual void handleTurnsLimitReached();
    virtual void handleSetsLimitReached(int winnerIndex, int winnerScore);
};

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerEntered(
    int playerIndex, const std::string &name)
{
    beforeEach(GameEvent::PlayerEntered);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerEntered(playerIndex, name);
    afterEach(GameEvent::PlayerEntered);
}

template <class iterator>
inline void Broadcaster<iterator>::handleSetStarted(int gameNumber)
{
    beforeEach(GameEvent::SetStarted);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleSetStarted(gameNumber);
    afterEach(GameEvent::SetStarted);
}

template <class iterator>
inline void Broadcaster<iterator>::handleDeckShuffled()
{
    beforeEach(GameEvent::DeckShuffled);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleDeckShuffled();
    afterEach(GameEvent::DeckShuffled);
}

template <class iterator>
inline void Broadcaster<iterator>::handleFirstCardPlaced(const Card *card)
{
    beforeEach(GameEvent::FirstCardPlaced);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleFirstCardPlaced(card);
    afterEach(GameEvent::FirstCardPlaced);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerDealt(int playerIndex, int cardsNumber)
{
    beforeEach(GameEvent::PlayerDealt);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerDealt(playerIndex, cardsNumber);
    afterEach(GameEvent::PlayerDealt);
}

template <class iterator>
inline void Broadcaster<iterator>::handleCardPlayed(int playerIndex, const Card *card)
{
    beforeEach(GameEvent::CardPlayed);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleCardPlayed(playerIndex, card);
    afterEach(GameEvent::CardPlayed);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerDrewAnotherCard(int playerIndex)
{
    beforeEach(GameEvent::PlayerDrewAnotherCard);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerDrewAnotherCard(playerIndex);
    afterEach(GameEvent::PlayerDrewAnotherCard);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerDrewAndSkip(int playerIndex, int numberOfCards)
{
    beforeEach(GameEvent::PlayerDrewAndSkipped);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerDrewAndSkip(playerIndex, numberOfCards);
    afterEach(GameEvent::PlayerDrewAndSkipped);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerChangedColor(int playerIndex, CardColor newColor)
{
    beforeEach(GameEvent::PlayerChangedColor);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerChangedColor(playerIndex, newColor);
    afterEach(GameEvent::PlayerChangedColor);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerSaid(int playerIndex, const std::string &message)
{
    beforeEach(GameEvent::PlayerSaid);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerSaid(playerIndex, message);
    afterEach(GameEvent::PlayerSaid);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerDisqualified(int playerIndex, int handScore, const Card *card)
{
    beforeEach(GameEvent::PlayerDisqualified);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerDisqualified(playerIndex, handScore, card);
    afterEach(GameEvent::PlayerDisqualified);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerWonSet(int playerIndex, int score)
{
    beforeEach(GameEvent::PlayerWonSet);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerWonSet(playerIndex, score);
    afterEach(GameEvent::PlayerWonSet);
}

template <class iterator>
inline void Broadcaster<iterator>::handlePlayerWonGame(int playerIndex, int totalScore)
{
    beforeEach(GameEvent::PlayerWonGame);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handlePlayerWonGame(playerIndex, totalScore);
    afterEach(GameEvent::PlayerWonGame);
}

template <class iterator>
inline void Broadcaster<iterator>::handleDirectionChanged(GameDirection newDirection)
{
    beforeEach(GameEvent::DirectionChanged);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleDirectionChanged(newDirection);
    afterEach(GameEvent::DirectionChanged);
}

template <class iterator>
inline void Broadcaster<iterator>::handleMessageOverflow()
{
    beforeEach(GameEvent::MessageOverflow);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleMessageOverflow();
    afterEach(GameEvent::MessageOverflow);
}

template <class iterator>
inline void Broadcaster<iterator>::handleTurnsLimitReached()
{
    beforeEach(GameEvent::TurnsLimitReached);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleTurnsLimitReached();
    afterEach(GameEvent::TurnsLimitReached);
}

template <class iterator>
inline void Broadcaster<iterator>::handleSetsLimitReached(int winnerIndex, int winnerScore)
{
    beforeEach(GameEvent::SetsLimitReached);
    for (auto it = begin(); it != end(); ++it)
        (*it)->handleSetsLimitReached(winnerIndex, winnerScore);
    afterEach(GameEvent::SetsLimitReached);
}
