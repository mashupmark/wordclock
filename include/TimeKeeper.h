#include <Network/NtpClient.h>
#include <Timezone.h>

class TimeKeeper
{
public:
    TimeKeeper(String url, unsigned int requestInterval);
    ~TimeKeeper();

    boolean setTimeZone(String timeZone);
    void updateSystemTimeZone(time_t systemTime);
    void enableAutoQuery(boolean enabled);

private:
    Timezone timeZone;
    NtpClient *client;

    void onNtpResponse(NtpClient &client, time_t ntpTime);
};