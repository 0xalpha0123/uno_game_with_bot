#pragma once
#include <string>
#include <vector>
#include <tuple>
#include <list>
#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <random>
#include <set>

#include "card.h"
#include "events.h"
#include "game_components.h"

class UnoGame;

/**
 * @brief Абстрактный игрок. Является наблюдателем игры и имеет интерфейс 
 * взаимодействия с игрой.
 * 
 * @details Игрок имеет чисто виртуальные функции, которые необходимо 
 * реализовать, чтобы настроить взаимодействие игрока с игрой.
 * 
 * Чтобы игрок мог наблюдать и реагировать на игровые события, нужно перегрузить 
 * методы класса Observer, однако это не является обязательным.
 * 
 * Также игрок имеет интерфейс для отправки сообщений другим игрокам и интерфейс
 * получения публичной информации о текущем состоянии игры. Они реализованы 
 * защищенными методами.
*/
class UnoPlayer: public Observer 
{
    /// @brief Номер игрока за столом. Выдается один раз в начале игры.
    int playerIndex_;
    /// @brief Указатель на текущую игру. Необходим для получения информации об игре.
    const UnoGame * currentGame;
    /// @brief Очередь сообщений, куда игрок отправляет сообщения.
    MessageQueue * messageQueue;
    

    /// @brief Метод, который вызывается классом Игры при начале игры. 
    /// @param playerIndex номер игрока.
    /// @param currentGame указатель на текущую игру.
    /// @param queue очередь сообщений текущей игры.
    void addToGame(
        int playerIndex, 
        const UnoGame* currentGame, 
        MessageQueue * queue);
    
    // Для того, чтобы метод выше можно было вызвать только из класса игры
    friend class UnoGame;
protected:
    /// @brief Отправляет сообщение от игрока другим игрокам.
    /// @param message сообщение.
    /// @return true, если отправка произошла успешно, иначе false.
    bool say(const std::string& message);

    /// @brief Получение константного указателя на игру для получения публичной
    /// информации о текущем состоянии игры.
    /// @return текущая игра.
    const UnoGame* game() const { return currentGame; }

public:
    /// @return номер этого игрока за столом.
    int playerIndex() const { return playerIndex_; }

    /// @brief Игрок возвращает свое имя.
    /// @return имя игрока.
    virtual std::string name() const = 0;
    
    /// @brief Игрок получает на руки карты.
    /// @param cards массив карт.
    virtual void receiveCards(const std::vector<const Card*>& cards) = 0;

    /// @brief Игрок возвращает карту, которую он сыграет (положит в сброс).
    /// @return карта, которую игрок положит в сброс.
    virtual const Card * playCard() = 0;

    /// @brief Если у игрока нет подходящих карт, он тянет дополнительную 
    /// карту и говорит, хочет ли он ее сразу положить или нет.
    /// @param additionalCard дополнительная карта.
    /// @return true, если игрок сразу же кладет эту карту, иначе false.
    virtual bool drawAdditionalCard(const Card * additionalCard) = 0;

    /// @brief Если игрок положил "Закажи цвет" или "Возьми четыре", то 
    /// игра запросит у него новый цвет.
    /// @return новый цвет.
    virtual CardColor changeColor() = 0;
};


/**
 * @brief "Игра"; класс, реализующий алгоритм проведения партии и игры (серии 
 * партий до 500 очков).
 * 
 * @details Игра хранит игроков, наблюдателей за игрой, очередь сообщений 
 * игроков, текущее состояние партии. 
 * 
 * Игра имеет интерфейс, позволяющий получить текущее состояние игры.
*/
class UnoGame 
{
public:
    // Константы для игры

    /// @brief Количество очков, необходимое для победы.
    const int WINNING_SCORE = 500;
    /// @brief Минимальное количество игроков.
    const int MIN_NUMBER_OF_PLAYERS = 2;
    /// @brief Максимальное количество игроков.
    const int MAX_NUMBER_OF_PLAYERS = 10;

    /// @brief Размер колоды.
    const int DECK_SIZE = 108;

    /// @brief Максимальное количество сообщений в очереди по умолчанию
    const int DEFAULT_MESSAGE_QUEUE_LIMIT = 50;

    /// @brief Сколько карт изначально раздается игрокам.
    const int INITIAL_CARDS_NUMBER = 7;

    /// @brief Ограничение на количество ходов по умолчанию
    const unsigned DEFAULT_TURNS_LIMIT = 100000U;

    /// @brief Ограничение на количество партий в одной игре по умолчанию
    const unsigned DEFAULT_SETS_LIMIT = 1000;

private:

    /// @brief Очередь сообщений
    MessageQueue messageQueue;

    /// @brief Список игроков
    std::vector<UnoPlayer *> players;

    /// @brief Генератор псевдослучайных чисел.
    /// @details Используется для перемешивания колоды и рассадки игроков в 
    /// случайном порядке.
    std::minstd_rand randomEngine;

    // Игровое состояние
    
    /// @brief Текущее направление игры.
    GameDirection currentDirection_;
    /// @brief Текущий цвет.
    CardColor currentColor_;
    /// @brief Оставшаяся колода карт.
    std::vector<const Card *> deck;
    /// @brief Стопка сброса.
    std::vector<const Card *> discardPile;
    /// @brief Номер активного игрока.
    int activePlayerIndex_;
    /// @brief Игрок, который будет ходить в этой партии первым
    int currentDiller_;
    /// @brief Текущий выигрыш в партии. Не 0 только если какой-то игрок был
    /// дисквалифицирован.
    int currentSetScore_;

    /// @brief Номер текущей партии.
    int currentSetNumber_;

    /// @brief Номер текущего хода
    unsigned currentTurnNumber_;

    /// @brief Ограничение на число ходов
    unsigned turnsLimit;

    /// @brief Ограничение на число партий
    unsigned setsLimit;

    struct PlayerInfo
    {
        /// @brief Карты на руках у игрока.
        std::list<const Card *> hand;
        /// @brief Количество его очков.
        int currentScore;

        PlayerInfo(): hand(), currentScore(0) {}
    };

    /// @brief Информация об игроках.
    std::vector<PlayerInfo> playerInfo;

public:
    UnoGame();
    ~UnoGame();

    // Интерфейс для получения текущего состояния партии.

    /// @return текущее направление игры.
    GameDirection currentDirection() const { return currentDirection_; }
    /// @return текущий цвет.
    CardColor currentColor() const { return currentColor_; }
    /// @return верхняя карта стопки сброса или nullptr, если стопка сброса пуста.
    const Card* topCard() const;
    /// @return количество карт у игроков.
    std::vector<int> numberOfCards() const;
    /// @return возвращает текущий выигрыш в партии; не 0, только если какой-то
    /// игрок был дисквалифицирован.
    int currentSetScore() const { return currentSetScore_; }
    /// @return возвращает номер в списке активного игрока.
    int activePlayerIndex() const { return activePlayerIndex_; }
    /// @return количество очков на момент начала партии у всех игроков.
    std::vector<int> currentScores() const;
    /// @return количество карт в колоде.
    int cardsLeft() const;
    /// @return количество игроков.
    int numberOfPlayers() const { return players.size(); }
    /// @return номер текущей партии.
    int currentSetNumber() const { return currentSetNumber_; }
    /// @return номер текущего хода.
    int currentTurnNumber() const { return currentTurnNumber_; }
    /// @return номер игрока, который будет ходить первым в эту игру
    int currentDiller() const { return currentDiller_; }

    // Интерфейс для подготовки игры

    /// @brief Добавление игрока в игру.
    /// @param player игрок.
    /// @throws std::overflow_error, если игроков уже 10.
    void addPlayer(UnoPlayer* player);

    /// @brief Добавление наблюдателя в игру.
    /// @param observer наблюдатель.
    void addObserver(Observer* observer);
    
    /// @brief Расположить игроков в случайном порядке.
    void shufflePlayers();

    /// @brief Расположить игроков в заданном порядке.
    /// @param permutation перестановка, i-тый игрок меняется местами с 
    /// permutation[i]-тым игроком, нумерация с нуля
    /// @throws std::out_of_range если размер permutation больше числа игроков
    /// или значение permutation вне диапазона [0; число игроков)
    void shufflePlayers(const std::vector<int>& permutation);

    /// @brief Установить игрока, который будет ходить первым в следующей партии
    /// @param diller номер игрока
    void setCurrentDiller(int diller);

    /// @brief Установить ограничение числа сообщений.
    /// @param m новое число сообщений, если `m < 0`, то ограничения не будет.
    /// Имейте в виду, что отсутствие ограничения может привести к бесконечному
    /// циклу.
    void setMessageQueueSizeLimit(int m);

    /// @brief Установить ограничение на число ходов в партии.
    /// @param limit максимальное число ходов, если передан 0, то ограничение не 
    /// ставится.
    void setTurnsLimit(unsigned limit) { turnsLimit = limit; }

    /// @brief Установить ограничение на число партий в игре.
    /// @param limit максимальное число партий, если передан 0, то ограничение 
    /// не ставится.
    void setSetsLimit(unsigned limit) { setsLimit = limit; }

    /// @brief Устанавливает новый сид для генератора.
    /// @param seed значение сида.
    void setRandomGeneratorSeed(unsigned seed);


    // Интерфейс для проведения игры

    /// @brief Очищает прогресс игроков за партию.
    void initPlayerInfo();

    /// @brief Проводит одну партию.
    /// @return номер победителя в списке и количество выигранных очков, если 
    /// победитель есть, иначе (-1, 0).
    /// @throws std::underflow_error, если игроков меньше 2. 
    /// @details Проводит партию по алгоритму проведения партии, после чего 
    /// обновляет количество очков у победителя.
    /// Победителя не будет, если будет превышен лимит ходов.
    std::tuple<int, int> runSet();
    
    /// @brief Проводит серию партий, пока один из игроков не наберет 500 очков.
    /// @return номер победителя в списке и количество его очков.
    /// @throws std::underflow_error, если игроков меньше 2.
    std::tuple<int, int> runGame();

    /// @brief Устанавливается игроком, который ходит первым в следующей партии,
    /// следующего игрока
    void stepCurrentDiller();

protected:

    // Эти методы можно использовать для тестирования
    // Если унаследовать класс от класса игры, можно переопределить 
    // методы `chooseCards` и `chooseFirstCard` для выдачи игроку карт 
    // в специальном порядке и выбора первой карты.
    // "подмешивать" игроку карты. Метод getDeck() позволяет наследнику 
    // получить доступ к колоде, чтобы выбрать нужные карты.

    /// @brief Метод возвращает карты, которые нужно выдать игроку.
    /// @param player игрок, которому предназначаются карты.
    /// @param numberOfCards количество карт.
    /// @return массив карт, предназначающихся игроку; размер массива должен 
    /// быть `numberOfCards`, все карты должны быть из колоды; если возвращается
    /// пустой вектор, то берутся случайные карты из колоды.
    /// @details Перегрузите этот метод, чтобы задать специальное поведение для
    /// тестирования игры. Учтите, что если метод будет возвращать вектор,
    /// размер которого не будет равен `numberOfCards` или который будет 
    /// содержать повторяющиеся элементы или элементы не из колоды, 
    /// то далее будет выброшено исключение ( @see getCardsFromDeck ).
    virtual std::vector<const Card*> chooseCards(
        const UnoPlayer* player, 
        int numberOfCards);

    /// @brief Выбор первой карты в партии. Это должна быть карта из колоды.
    /// @return первая карта или `nullptr` для случайного выбора карты.
    virtual const Card * chooseFirstCard();
    
    /// @brief Доступ к колоде карт, для перегрузок метода chooseCards.
    /// @return текущая колода.
    const std::vector<const Card*>& getDeck() const { return deck; }

private:

    // Служебные классы и методы

    /** 
     * @brief Интерфейс для рассылки событий игрокам и всем наблюдателем.
     * @details Вызов обработчика события в этом классе повлечет вызов 
     * этого обработчика у всех добавленных наблюдателей. 
     * 
     * Также для всех обработчиков, кроме `handlePlayerSaid` и 
     * `handleMessageOverflow` вызывает обработку очереди сообщений 
     * (см. flushMessages()).
    */
    class EventBroadcaster: 
        public Broadcaster<std::list<Observer*>::iterator> 
    {
        std::list<Observer *> listeners;
        MessageQueue * queue;

        /**
         * @brief Обработка всех накопившихся сообщений.
         * @details Очередь сообщений проходится по порядку появления сообщений, 
         * для каждого сообщения вызывается обработчик `handlePlayerSaid`. 
         * 
         * Обратите внимание, что вызов этих обработчиков может повлечь добавление 
         * новых сообщений, но по умолчанию очередь имеет ограничение на количество
         * возможных сообщений.
         * 
         * Если во время обработки очереди произошло переполнение очереди, то 
         * вызывается обработчик `handleMessageOverflow`.
         * 
         * После обработки очереди сообщений в любом случае очередь очищается.
        */ 
        void flushMessages();
    protected:
        void afterEach(GameEvent event) override;
        std::list<Observer*>::iterator begin() override { return listeners.begin(); }
        std::list<Observer*>::iterator end() override { return listeners.end(); }

    public:
        EventBroadcaster(MessageQueue * messageQueue);

        void addListener(Observer * listener);
        void setQueue(MessageQueue * queue) { this->queue = queue; }
    };

    EventBroadcaster broadcaster;

    /// @brief Перестановка игроков без проверок.
    /// @see void shufflePlayers(const std::vector<int>&);
    void shufflePlayers_(const std::vector<int>& permutation);   

    /// @brief Наполняет колоду по правилам Уно.
    void prepareDeck();
    /// @brief Перемещает все карты игры в колоду
    void moveToDeck();
    /// @brief Мешает колоду.
    void shuffleDeck();
    /// @brief Удаляет динамическую память в колоде и стопке сброса.
    void clearDeck();
    /// @brief Очищает информацию об картах игроков, переносит карты из рук в 
    /// колоду.
    void clearHands();

    /// @brief Выбирает карты из колоды с помощью метода `chooseCards`, 
    /// добавляет их в руку игроку в `playerInfo` и удаляет из колоды.
    /// @param player игрок, для которого выбираются карты.
    /// @param numberOfCards число карт.
    /// @return карты, которые были выбраны.
    /// @throws std::length_error, если `chooseCards` возвращает неправильное 
    /// количество карт;
    /// @throws std::domain_error, если `chooseCards` возвращает значения, 
    /// которые либо повторяются, либо отсутствуют в колоде.
    std::vector<const Card*> getCardsFromDeck(
        const UnoPlayer* player, int numberOfCards);

    /// @brief Выдает игроку карты из колоды. Может вызвать событие 
    /// "Колода обновлена и перемешана"
    /// @param player игрок, которому выдаются карты.
    /// @param numberOfCards количество карт.
    /// @return true, если карт хватает, false, если карты выдать нельзя.
    bool dealCards(UnoPlayer * player, int numberOfCards);

    /// @brief Выбор первой карты, которая помещается в стопку сброса.
    void placeFirstCard();

    /// @brief Проводит одну партию без инициализации и очистки колоды.
    /// @see runSet()
    std::tuple<int, int> runSet_();

    /// @brief Переместить все карты кроме верхней в колоду и перемешать.
    void flushDiscardPile();

    UnoPlayer * activePlayer();
    std::list<UnoPlayer *>::iterator activePlayerIterator(
        std::list<UnoPlayer *>& playersList);
    
    /// @brief Реализует логику перехода к следующему игроку.
    /// @param playersList список игроков в партии.
    /// @return номер следующего игрока.
    int nextPlayerIndex(std::list<UnoPlayer *>& playersList);

    /// @return true, если у игрока есть карта чтобы положить ее на `topCard_`
    bool canPlaceCard(UnoPlayer * player, const Card* topCard_);

    /// @return true, если у игрока на руках есть карта с таким цветом.
    bool haveMatchingColor(UnoPlayer * player, CardColor color);
    
    /// @brief Подсчитывает количество очков (сумма стоимостей карты на руке игрока).
    /// @param playerIndex игрок, чьи очки подсчитываются.
    /// @return сумма стоимостей карт игрока.
    int countHandScore(int playerIndex);

    /// @brief Находит игрока с наибольшим количеством очков
    /// @return возвращает номер игрока в списке и количество его очков; в случае
    /// если игрока нельзя определить однозначно, вместо номера игрока возвращает -1
    std::tuple<int, int> findWinner();

    /// @brief Проверяет, нет ли в колоде повторяющихся карт. Для отладки.
    /// @return true, если в колоде нет повторяющихся карт.
    bool deckIsConsistent();
};
