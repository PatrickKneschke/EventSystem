
#include "event_system.h"

#include <assert.h>
#include <stdexcept>


EventSystem* EventSystem::sInstance = nullptr;


EventSystem::EventSystem() : mTotalListeners {0} {};


void EventSystem::StartUp() {

    if(!sInstance)
    {
        sInstance = new EventSystem;
    }
}


void EventSystem::ShutDown() {

    if(sInstance)
    {
        delete sInstance;
        sInstance = nullptr;
    }
}


EventListenerID EventSystem::Subscribe(const EventCallback &callback, EventType type) {

    assert(sInstance != nullptr);

    auto id = sInstance->mTotalListeners++;
    sInstance->mCallbacks[id] = {callback, 1};
    sInstance->mListeners[type].push_back(id);

    return id;
}


EventListenerID EventSystem::Subscribe(const EventCallback &callback, std::vector<EventType> types) {

    assert(sInstance != nullptr);

    auto id = sInstance->mTotalListeners++;
    sInstance->mCallbacks[id] = {callback, types.size()};

    for (auto type : types)
    {
        sInstance->mListeners[type].push_back(id);
    }
    

    return id;
}


void EventSystem::Unsubscribe(const EventListenerID listener, EventType type){

    assert(sInstance != nullptr);

    if(sInstance->mListeners.find(type) == sInstance->mListeners.end()) {

        return;
    }

    auto& listeners = sInstance->mListeners[type];
    auto it = std::lower_bound(listeners.begin(), listeners.end(), listener);
    if(*it == listener)
    {
        listeners.erase(it);
        if ( --sInstance->mCallbacks[*it].second == 0)
        {
            sInstance->mCallbacks.erase(*it);
        }
        
    }
}


void EventSystem::Unsubscribe(const EventListenerID listener, std::vector<EventType> types){

    for (auto type : types)
    {
        Unsubscribe(listener, type);
    } 
}


void EventSystem::PublishQueued(const Event &event) {

    assert(sInstance != nullptr);

    sInstance->mEventQueue.push(event);
}


void EventSystem::PublishImmediate(const Event &event) {

    assert(sInstance != nullptr);

    if(sInstance->mListeners.find(event.type) == sInstance->mListeners.end())
    {
        return;
    }

    for(auto id : sInstance->mListeners[event.type])
    {
        sInstance->mCallbacks[id].first(event);
    }
}


void EventSystem::Update() {

    assert(sInstance != nullptr);

    while(!sInstance->mEventQueue.empty())
    {
        Event event = sInstance->mEventQueue.front();
        sInstance->mEventQueue.pop();
        sInstance->PublishImmediate(event);
    }
}