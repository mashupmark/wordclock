#include <TimeKeeper.h>
#include <SmingCore.h>
#include <Network/NtpClient.h>
#include <Timezone.h>
#include <tzdata.h>
#include <config.h>

TimeKeeper::TimeKeeper(Config *config, String url, unsigned int requestInterval)
{
    this->config = config;
    client = new NtpClient(url, requestInterval, NtpTimeResultDelegate(&TimeKeeper::onNtpResponse, this));
    client->setAutoQuery(false);
}

TimeKeeper::~TimeKeeper()
{
    delete this->config;
    delete client;
}

void TimeKeeper::onNtpResponse(NtpClient &client, time_t ntpTime)
{
    Config::Settings generalSettings(*this->config);
    SystemClock.setTime(ntpTime, eTZ_UTC);
    this->setTimeZone(generalSettings.getTimezone());
}

boolean TimeKeeper::setTimeZone(String zoneName)
{
    auto zone = TZ::findZone(zoneName);
    if (zone == nullptr)
        return false;

    timeZone = Timezone::fromPosix(zone->tzstr);
    SystemClock.setTimeZone(timeZone.makeZoned(SystemClock.now(eTZ_UTC)).getZoneInfo());

    return true;
}

void TimeKeeper::enableAutoQuery(boolean enabled)
{
    client->setAutoQuery(enabled);
    client->requestTime();
}