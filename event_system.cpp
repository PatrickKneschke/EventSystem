
#include "event_system.h"

#include <assert.h>
#include <stdexcept>


EventSystem* EventSystem::sInstance = nullptr;


EventSystem::EventSystem() : mNextListenerID {0} {};


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


EventListenerID EventSystem::GetNewListenerID() {

    assert(sInstance != nullptr);

    return sInstance->mNextListenerID++;
}


void EventSystem::Subscribe(const EventListener *listener) {

    assert(sInstance != nullptr);

    for(auto type : listener->types) {

        sInstance->mListeners[type].push_back(listener);
    }
}


void EventSystem::Unsubscribe(const EventListener *listener){

    assert(sInstance != nullptr);

    for(auto type : listener->types)
    {
        auto it = std::lower_bound(sInstance->mListeners[type].begin(), sInstance->mListeners[type].end(), listener, [](const EventListener *a, const EventListener *b){

             return a->id < b->id;
        });

        if((*it)->id ==listener->id)
        {
            sInstance->mListeners[type].erase(it);
        }
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

    for (auto el : sInstance->mListeners[event.type])
    {
        el->callback(event);
    }
}


void EventSystem::Update() {

    assert(sInstance != nullptr);

    while (!sInstance->mEventQueue.empty())
    {
        Event event = sInstance->mEventQueue.front();
        sInstance->mEventQueue.pop();
        sInstance->PublishImmediate(event);
    } 
}