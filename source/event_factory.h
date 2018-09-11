#ifndef NETWORK_SHOOTER_PROJECT_EVENT_FACTORY_H
#define NETWORK_SHOOTER_PROJECT_EVENT_FACTORY_H

struct EventFactory
{
    static VariantEvent createHpEvent(float delta, int actorID);
};

#endif //NETWORK_SHOOTER_PROJECT_EVENT_FACTORY_H
