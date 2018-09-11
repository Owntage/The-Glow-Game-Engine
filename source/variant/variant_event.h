#ifndef VARIANT_EVENT_H
#define VARIANT_EVENT_H

#include "variant_map.h"
#include "event.h"

struct VariantEvent : VariantMap, Event
{
    VariantEvent(std::string name, bool global, int actorID) : Event(name, global, actorID) {}
    VariantEvent(std::string name) : Event(name) {}
    VariantEvent() {}

    template<typename STREAM_T>
    friend STREAM_T& operator<<(STREAM_T& s, VariantEvent& e)
    {
        s << (Event&) e << (VariantMap&) e;
		return s;
    }

    template<typename STREAM_T>
    friend STREAM_T& operator>>(STREAM_T& s, VariantEvent& e)
    {
        s >> (Event&) e >> (VariantMap&) e;
		return s;
    }
};

#endif
