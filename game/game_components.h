#pragma once

#include <list>
#include <tuple>
#include <string>

/**
 * @brief Очередь сообщений для игроков. Сюда игроки отправляют свои сообщения,
 * которые потом отображаются наблюдателям.
*/
class MessageQueue
{
public:
    /// @brief Элемент очереди сообщения, хранит номер игрока, отправившего
    /// сообщение, и само сообщение.
    using Entry = std::tuple<int, std::string>;
    
    /// @brief Итератор для просмотра очереди сообщений.
    using const_iterator = std::list<Entry>::const_iterator;
private:
    /// @brief Массив, хранящий сообщения.
    std::list<Entry> queue;
    /// @brief Максимальный размер очереди сообщений. Если < 0, то размер не
    /// ограничивается, но это может привести к бесконечным циклам.
    int maximumCapacity;
    bool overflow;
public:
    MessageQueue(int maximumCapacity);

    /// @brief Добавляет сообщение в очередь сообщений.
    /// @param playerIndex номер игрока, отправившего сообщение.
    /// @param message сообщение игрока.
    /// @return false, если достигнут максимальный размер очереди и сообщение 
    /// добавить не удалось, иначе true.
    bool addMessage(int playerIndex, const std::string& message);

    void setLimit(int newMessageQueueLimit);
    
    const_iterator cbegin() const { return queue.cbegin(); }
    const_iterator cend() const { return queue.cend(); }

    bool hasOverflow() const { return overflow; }

    void clear() { queue.clear(); overflow = false; }
};
