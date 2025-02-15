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
    SystemClock.setTime(ntpTime, eTZ_UTC);
}

boolean TimeKeeper::setTimeZone(String zoneName)
{
    auto zone = TZ::findZone(zoneName);
    if (zone == nullptr)
        return false;

    timeZone = Timezone::fromPosix(zone->tzstr);
    SystemClock.setTimeZone(timeZone.makeZoned(SystemClock.now()).getZoneInfo());

    return true;
}

void TimeKeeper::enableAutoQuery(boolean enabled)
{
    client->setAutoQuery(enabled);
    client->requestTime();
}