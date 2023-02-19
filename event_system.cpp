
#include "event_system.h"

#include <assert.h>
#include <stdexcept>


EventSystem* EventSystem::sInstance = nullptr;


EventSystem::EventSystem() : mTotalListeners {0} {};


void EventSystem::StartUp() {

    if(!sInstance) {

        sInstance = new EventSystem;
    }
}


void EventSystem::ShutDown() {

    if(sInstance) {

        delete sInstance;
    }
}


EventListenerID EventSystem::Subscribe(const EventType &type, const EventCallback &callback) {

    assert(sInstance != nullptr);

    auto id = sInstance->mTotalListeners++;
    sInstance->mListeners[type].emplace_back(id, callback);

    return id;
}


void EventSystem::Unsubscribe(const EventType &type, const EventListenerID listenerID){

    assert(sInstance != nullptr);

    if(sInstance->mListeners.find(type) == sInstance->mListeners.end()) {

        throw std::runtime_error("Tried to unsubscribe to unknown event.");
    }

    auto& listeners = sInstance->mListeners[type];
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), 
        [listenerID](const std::pair<EventListenerID, EventCallback> &pair) {
            return pair.first == listenerID;
        }), 
        listeners.end()
    );
}


void EventSystem::PublishQueued(const Event &event) {

    assert(sInstance != nullptr);

    sInstance->mEventQueue.push(event);
}


void EventSystem::PublishImmediate(const Event &event) {

    assert(sInstance != nullptr);

    if(sInstance->mListeners.find(event.type) == sInstance->mListeners.end()) {

        return;
    }

    for(auto &[id, callback] : sInstance->mListeners[event.type]) {

        callback(event);
    }
}


void EventSystem::Update() {

    assert(sInstance != nullptr);

    while(!sInstance->mEventQueue.empty()) {

        Event event = sInstance->mEventQueue.front();
        sInstance->mEventQueue.pop();
        sInstance->PublishImmediate(event);
    }
}