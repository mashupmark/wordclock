#include <Network/NtpClient.h>
#include <Timezone.h>
#include <config.h>

class TimeKeeper
{
public:
    TimeKeeper(Config *config, String url, unsigned int requestInterval);
    ~TimeKeeper();

    boolean setTimeZone(String timeZone);
    void enableAutoQuery(boolean enabled);

private:
    Config *config;
    Timezone timeZone;
    NtpClient *client;

    void onNtpResponse(NtpClient &client, time_t ntpTime);
};