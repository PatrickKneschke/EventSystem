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
    std::unordered_map<const char*, EventArg> args;
};


class EventSystem {

public:

    static void StartUp();
    static void ShutDown();

    static EventListenerID Subscribe(const EventCallback &callback, EventType type);
    static EventListenerID Subscribe(const EventCallback &callback, std::vector<EventType> types);
    static void Unsubscribe(const EventListenerID listener, EventType type);
    static void Unsubscribe(const EventListenerID listener, std::vector<EventType> types);

    static void PublishQueued(const Event &event);
    static void PublishImmediate(const Event &event);

    static void Update();

    ~EventSystem() = default;


private:

    EventSystem();

    static EventSystem *sInstance;

    EventListenerID mTotalListeners;
    std::unordered_map<EventListenerID, std::pair<EventCallback, uint32_t>> mCallbacks;
    std::unordered_map<EventType, std::vector<EventListenerID>> mListeners;

    std::queue<Event> mEventQueue;
};