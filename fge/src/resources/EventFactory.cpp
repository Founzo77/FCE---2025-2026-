#include <fge/resources/EventFactory.hpp>
#include <fge/utility.hpp>

namespace fge
{
    HANDLE EventFactory::buildEvent()
    {
        HANDLE event;

        event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        throwIfFailed(event, "Failed to create event HANDLE");

        return event;
    }
}
