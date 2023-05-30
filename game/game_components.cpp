#include "game_components.h"

MessageQueue::MessageQueue(int maximumCapacity): 
    maximumCapacity(maximumCapacity), 
    queue(),
    overflow(false)
{
}

bool MessageQueue::addMessage(int playerIndex, const std::string &message)
{
    if (maximumCapacity >= 0 && queue.size() >= maximumCapacity) 
    {
        overflow = true;
        return false;
    }
    queue.push_back(std::make_tuple(playerIndex, message));
    return true;
}

void MessageQueue::setLimit(int newMessageQueueLimit)
{
    maximumCapacity = newMessageQueueLimit;
    if (maximumCapacity >= 0 && queue.size() > maximumCapacity) 
        overflow = true;
}
