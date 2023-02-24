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


struct EventListener;

class EventSystem {

    friend EventListener;

public:

    static void StartUp();
    static void ShutDown();

    static void PublishQueued(const Event &event);
    static void PublishImmediate(const Event &event);

    static void Update();

    ~EventSystem() = default;


private:

    static EventListenerID GetNewListenerID();
    static void Subscribe(const EventListener *listener);
    static void Unsubscribe(const EventListener *listener);

    EventSystem();

    static EventSystem *sInstance;

    EventListenerID mNextListenerID;
    std::unordered_map<EventType, std::vector<const EventListener*>> mListeners;

    std::queue<Event> mEventQueue;
};


struct EventListener {

    EventListener(EventCallback &&callback_, std::vector<EventType> &&types_) 
        : callback {std::forward<EventCallback>(callback_)}, types {std::forward<std::vector<EventType>>(types_)} 
    {
        id = EventSystem::GetNewListenerID();
        EventSystem::Subscribe(this);
    }

    ~EventListener() {

        EventSystem::Unsubscribe(this);
    }

    EventListenerID id;
    EventCallback callback;
    std::vector<EventType> types;
};