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


/* @brief Singleton class that maps EventTypes to a list of EventListeners.
 *        Events can be submitted for immediate publication or stored in a queue to be published by calling Update().
 *        EventSystem instance is initialized by calling StartUp(), before events can be submitted. 
 *        Eventsystem instance is destroyed by calling ShutDown(), after all EventListeners have been destroyed. 
 * @class
 */
struct EventListener;

class EventSystem {

    friend EventListener;

public:

    /* @brief Create new EventSystem instance if not already initialized.
    */
    static void StartUp();

    /* @brief Destroys EventSystem instance if already initialized.
    */
    static void ShutDown();

    /* @brief Submits an Event for future publication when Update() is called.
    */
    static void PublishQueued(const Event &event);

    /* @brief Submits an Event to be published now.
    */
    static void PublishImmediate(const Event &event);

    /* @brief Publishes all Events currently stored in the event queue.
    */
    static void Update();

    ~EventSystem() = default;


private:

    /* @brief Returns a new unique id for an EventListener.
    */
    static EventListenerID GetNewListenerID();

    /* @brief Adds an EventListener to the Eventsystem by mapping it to the EventTypes it responds to.
     *
     * @param listener    The EventListener to add to the EventSystem,
    */
    static void Subscribe(const EventListener *listener);

    /*@brief Removes the EventListener from the EventSystem.
     *
     * @param listener    The EventListener to remove from the EventSystem,
    */
    static void Unsubscribe(const EventListener *listener);

    EventSystem();

    static EventSystem *sInstance;

    EventListenerID mNextListenerID;
    std::unordered_map<EventType, std::vector<const EventListener*>> mListeners;

    std::queue<Event> mEventQueue;
};


/* @brief Wrapper for an EventCallback that also stores a list of EventTypes the callback responds to. 
 *        Recieves a unique id from the EventSystem upon initialization.
 *        Subscribes to the EventSystem on initialization and unsubscribes on destruction.     
 *
 * @struct
 */
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