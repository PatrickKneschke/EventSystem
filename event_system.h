#pragma once


#include <cstdint>
#include <functional>
#include <queue>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>


using EventListenerID = int32_t;
using EventType = const char*;

using EventArg = std::variant<bool, int, float, double, std::string, void*>;
using EventCallback = std::function<void(struct Event)>;


struct Event {

    EventType type;
    std::vector<EventArg> args;
};


class EventSystem {

public:

    static void StartUp();
    static void ShutDown();

    static EventListenerID Subscribe(const EventType &type, const EventCallback &callback);
    static void Unsubscribe(const EventType &type, const EventListenerID listenerID);
    static void PublishQueued(const Event &event);
    static void PublishImmediate(const Event &event);

    static void Update();

    ~EventSystem() = default;


private:

    EventSystem();

    static EventSystem *sInstance;

    EventListenerID mTotalListeners;
    std::unordered_map<EventType, std::vector<std::pair<EventListenerID, EventCallback>>> mListeners;
    std::queue<Event> mEventQueue;
};