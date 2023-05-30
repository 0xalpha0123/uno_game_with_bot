#pragma once

enum CardColor 
{
    Red    = 0,
    Green  = 1,
    Blue   = 2, 
    Yellow = 3,
};

enum CardValue
{
    Draw2     = 10, // Возьми 2
    Reverse   = 11, // Ход обратно
    Skip      = 12, // Пропусти ход
    Wild      = 13, // Закажи цвет
    WildDraw4 = 14, // Возьми 4
};

/// @brief Представление карты
struct Card
{
    /// @brief Цена карты действия (не дикой).
    const int ACTION_CARD_SCORE = 20;
    /// @brief Цена дикой карты.
    const int WILD_CARD_SCORE = 50;
    
    
    CardColor color;
    /**
     * Если карта является цифрой, то значение `value` — цифра на карте,
     * иначе используется значение enum CardValue.
    */
    int value;

    Card(CardColor color, int value): color(color), value(value) {}

    /// @return является ли карта корректной
    bool is_valid() const { return 0 <= value && value <= CardValue::WildDraw4; }
    
    /// @return является ли карта дикой
    bool is_wild() const { return value == CardValue::Wild || value == CardValue::WildDraw4; }
    
    /// @return является ли карта картой действия (не дикой)
    bool is_action() const;
    
    /// @brief Возвращает стоимость карты.
    /// @return стоимость карты.
    /// @details Дикая карта имеет цену 50 очков, другие карты действия имеют
    /// стоимость 20 очков, карты с цифрами имеют стоимость, соответствующую 
    /// их цифре.
    int getScore() const;
};
