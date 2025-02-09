#include <TimeKeeper.h>
#include <SmingCore.h>
#include <Network/NtpClient.h>
#include <Timezone.h>
#include <tzdata.h>

TimeKeeper::TimeKeeper(String url, unsigned int requestInterval)
{
    client = new NtpClient(url, requestInterval, NtpTimeResultDelegate(&TimeKeeper::onNtpResponse, this));
    client->setAutoQuery(false);
}

TimeKeeper::~TimeKeeper()
{
    delete client;
}

void TimeKeeper::onNtpResponse(NtpClient &client, time_t ntpTime)
{
    updateSystemTimeZone(ntpTime);
}

void TimeKeeper::updateSystemTimeZone(time_t systemTime)
{
    static ZonedTime nextChange{TZ::invalidTime};

    if (nextChange == TZ::invalidTime)
        nextChange = timeZone.makeZoned(systemTime);
    else if (systemTime < nextChange)
        return;

    SystemClock.setTimeZone(nextChange.getZoneInfo());
    nextChange = timeZone.getNextChange(systemTime);
}

boolean TimeKeeper::setTimeZone(String zoneName)
{
    auto zone = TZ::findZone(zoneName);
    if (zone == nullptr)
        return false;

    timeZone = Timezone::fromPosix(zone->tzstr);
    return true;
}

void TimeKeeper::enableAutoQuery(boolean enabled)
{
    client->setAutoQuery(enabled);
}