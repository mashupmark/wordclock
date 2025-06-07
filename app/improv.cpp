/**
 * This file was copied from https://github.com/improv-wifi/sdk-cpp to add the improv-wifi sdk to this project
 * It should not be modified manually
 */

#include <SmingCore.h>
#include "improv.h"
#include <cstring>

namespace improv
{

    ImprovCommand parse_improv_data(const std::vector<uint8_t> &data, bool check_checksum)
    {
        return parse_improv_data(data.data(), data.size(), check_checksum);
    }

    ImprovCommand parse_improv_data(const uint8_t *data, size_t length, bool check_checksum)
    {
        ImprovCommand improv_command;
        Command command = (Command)data[0];
        uint8_t data_length = data[1];

        if (data_length != length - 2 - check_checksum)
        {
            improv_command.command = UNKNOWN;
            return improv_command;
        }

        if (check_checksum)
        {
            uint8_t checksum = data[length - 1];

            uint32_t calculated_checksum = 0;
            for (uint8_t i = 0; i < length - 1; i++)
            {
                calculated_checksum += data[i];
            }

            if ((uint8_t)calculated_checksum != checksum)
            {
                improv_command.command = BAD_CHECKSUM;
                return improv_command;
            }
        }

        if (command == WIFI_SETTINGS)
        {
            uint8_t ssid_length = data[2];
            uint8_t ssid_start = 3;
            size_t ssid_end = ssid_start + ssid_length;
            if (ssid_end > length)
            {
                improv_command.command = UNKNOWN;
                return improv_command;
            }

            uint8_t pass_length = data[ssid_end];
            size_t pass_start = ssid_end + 1;
            size_t pass_end = pass_start + pass_length;
            if (pass_end > length)
            {
                improv_command.command = UNKNOWN;
                return improv_command;
            }

            std::string ssid(data + ssid_start, data + ssid_end);
            std::string password(data + pass_start, data + pass_end);
            return {.command = command, .ssid = ssid, .password = password};
        }

        improv_command.command = command;
        return improv_command;
    }

    bool parse_improv_serial_byte(size_t position, uint8_t byte, const uint8_t *buffer,
                                  std::function<bool(ImprovCommand)> &&callback, std::function<void(Error)> &&on_error)
    {
        if (position == 0)
            return byte == 'I';
        if (position == 1)
            return byte == 'M';
        if (position == 2)
            return byte == 'P';
        if (position == 3)
            return byte == 'R';
        if (position == 4)
            return byte == 'O';
        if (position == 5)
            return byte == 'V';

        if (position == 6)
            return byte == IMPROV_SERIAL_VERSION;

        if (position <= 8)
            return true;

        uint8_t type = buffer[7];
        uint8_t data_len = buffer[8];

        if (position <= 8 + data_len)
            return true;

        if (position == 8 + data_len + 1)
        {
            uint8_t checksum = 0x00;
            for (size_t i = 0; i < position; i++)
                checksum += buffer[i];

            if (checksum != byte)
            {
                on_error(ERROR_INVALID_RPC);
                return false;
            }

            if (type == TYPE_RPC)
            {
                auto command = parse_improv_data(&buffer[9], data_len, false);
                return callback(command);
            }
        }

        return false;
    }

    std::vector<uint8_t> build_rpc_response(Command command, const std::vector<std::string> &datum, bool add_checksum)
    {
        // Calculate the byte count to reserve memory to avoid reallocations
        // Frame length fixed: 3 = Command: 1 + frame length: 1 + checksum: 1
        size_t frame_length = 3;
        // Frame length variable: string lengths: n + length of data in datum
        frame_length += datum.size();
        for (int i = 0; i < datum.size(); i++)
        {
            frame_length += datum[i].length();
        }
        // Reserve frame_length bytes in vector
        std::vector<uint8_t> out(frame_length, 0);

        out[0] = command;

        // Copy data from datum input to out vector with lengths
        const size_t data_offset = 2;
        size_t pos = data_offset;
        for (const auto &str : datum)
        {
            out[pos] = static_cast<uint8_t>(str.length());
            pos++;
            std::memcpy(out.data() + pos, str.c_str(), str.length());
            pos += str.length();
        }

        out[1] = static_cast<uint8_t>(pos - data_offset);

        if (add_checksum)
        {
            uint32_t calculated_checksum = 0;

            for (uint8_t byte : out)
            {
                calculated_checksum += byte;
            }
            // Clear all bits, but the least significant byte
            calculated_checksum &= 0xFF;
            out[frame_length - 1] = static_cast<uint8_t>(calculated_checksum);
        }
        return out;
    }

#ifdef ARDUINO
    std::vector<uint8_t> build_rpc_response(Command command, const std::vector<String> &datum, bool add_checksum)
    {
        // Calculate the byte count to reserve memory to avoid reallocations
        // Frame length fixed: 3 = Command: 1 + frame length: 1 + checksum: 1
        size_t frame_length = 3;
        // Frame length variable: string lengths: n + length of data in datum
        frame_length += datum.size();
        for (int i = 0; i < datum.size(); i++)
        {
            frame_length += datum[i].length();
        }
        // Reserve frame_length bytes in vector
        std::vector<uint8_t> out(frame_length, 0);

        out[0] = command;

        // Copy data from datum input to out vector with lengths
        const size_t data_offset = 2;
        size_t pos = data_offset;
        for (const auto &str : datum)
        {
            out[pos] = static_cast<uint8_t>(str.length());
            pos++;
            std::memcpy(out.data() + pos, str.c_str(), str.length());
            pos += str.length();
        }

        out[1] = static_cast<uint8_t>(pos - data_offset);

        if (add_checksum)
        {
            uint32_t calculated_checksum = 0;

            for (uint8_t byte : out)
            {
                calculated_checksum += byte;
            }
            // Clear all bits, but the least significant byte
            calculated_checksum &= 0xFF;
            out[frame_length - 1] = static_cast<uint8_t>(calculated_checksum);
        }
        return out;
    }
#endif // ARDUINO

} // namespace improv

/**
 * Starting from here custom code to integrate the improv protocol was added
 */

std::vector<uint8_t> createImprovResponse(improv::State state)
{
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(11);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_CURRENT_STATE;
    data[8] = 1;
    data[9] = state;

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data[10] = checksum;

    return data;
}

std::vector<uint8_t> createImprovResponse(std::vector<uint8_t> &response)
{
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(9);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_RPC_RESPONSE;
    data[8] = response.size();
    data.insert(data.end(), response.begin(), response.end());

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data.push_back(checksum);

    return data;
}

std::vector<uint8_t> createImprovResponse(improv::Error error)
{
    std::vector<uint8_t> data = {'I', 'M', 'P', 'R', 'O', 'V'};
    data.resize(11);
    data[6] = improv::IMPROV_SERIAL_VERSION;
    data[7] = improv::TYPE_ERROR_STATE;
    data[8] = 1;
    data[9] = error;

    uint8_t checksum = 0x00;
    for (uint8_t d : data)
        checksum += d;
    data[10] = checksum;

    return data;
}

// Constructs a vector to the local mDNS address of the wordclock
std::vector<std::string> getLocalUrl()
{
    return {String("http://wordclock.local").c_str()};
}

bool handleImprovCommand(improv::ImprovCommand cmd)
{
    switch (cmd.command)
    {
    case improv::Command::GET_CURRENT_STATE:
    {
        if (WifiStation.isConnected())
        {
            auto state = createImprovResponse(improv::State::STATE_PROVISIONED);
            Serial.write(state.data(), state.size());

            auto data = improv::build_rpc_response(improv::GET_CURRENT_STATE, getLocalUrl(), false);
            auto res = createImprovResponse(data);
            Serial.write(res.data(), res.size());
        }
        else
        {
            auto state = createImprovResponse(improv::State::STATE_AUTHORIZED);
            Serial.write(state.data(), state.size());
        }

        break;
    }

    case improv::Command::WIFI_SETTINGS:
    {
        if (cmd.ssid.length() == 0)
        {
            auto res = createImprovResponse(improv::Error::ERROR_INVALID_RPC);
            Serial.write(res.data(), res.size());
            break;
        }

        auto state = createImprovResponse(improv::STATE_PROVISIONING);
        Serial.write(state.data(), state.size());

        WifiStation.config(cmd.ssid.c_str(), cmd.password.c_str());
        if (WifiStation.connect())
        {
            auto state = createImprovResponse(improv::STATE_PROVISIONED);
            Serial.write(state.data(), state.size());

            std::vector<uint8_t> data = improv::build_rpc_response(improv::WIFI_SETTINGS, getLocalUrl(), false);
            auto res = createImprovResponse(data);
            Serial.write(res.data(), res.size());
        }
        else
        {
            auto state = createImprovResponse(improv::STATE_STOPPED);
            Serial.write(state.data(), state.size());
            auto data = createImprovResponse(improv::Error::ERROR_UNABLE_TO_CONNECT);
            Serial.write(data.data(), data.size());
        }
        break;
    }

    case improv::Command::GET_DEVICE_INFO:
    {
        std::vector<std::string> deviceInfo = {
            "Wordclock", // Firmware name
            "1.0.0",     // Firmware version
            "ESP8266",   // Hardware chip / variant
            "Wordclock"  // Device name
        };
        auto data = improv::build_rpc_response(improv::GET_DEVICE_INFO, deviceInfo, false);
        auto res = createImprovResponse(data);
        Serial.write(res.data(), res.size());
        break;
    }

    case improv::Command::GET_WIFI_NETWORKS:
    {
        WifiStation.startScan([](bool success, BssList &list) -> void
                              {
                                  for (auto network : list)
                                  {
                                      std::vector<std::string> networkInfo = {network.ssid.c_str(), String(network.rssi).c_str(), String(network.authorization == WifiAuthMode::AUTH_OPEN ? "NO" : "YES").c_str()};
                                      auto data = improv::build_rpc_response(improv::GET_WIFI_NETWORKS, networkInfo, false);
                                      auto res = createImprovResponse(data);
                                      Serial.write(res.data(), res.size());
                                      delay(1);
                                  }

                                  auto data = improv::build_rpc_response(improv::GET_WIFI_NETWORKS, std::vector<std::string>{}, false);
                                  auto res = createImprovResponse(data);
                                  Serial.write(res.data(), res.size()); });

        break;
    }

    default:
    {
        auto res = createImprovResponse(improv::ERROR_UNKNOWN_RPC);
        Serial.write(res.data(), res.size());
        return false;
    }
    }

    return true;
}

void parseImprovCommand(Stream &stream, char arrivedChar, unsigned short availableCharsCount)
{
    uint8_t position = 0;
    uint8_t buffer[256]; // Make sure the buffer is big enough to handle even long passwords

    auto handleImprovError = [](improv::Error err) -> void
    {
        debugf("Unexpected improv error: %s", err);
    };

    while (stream.available())
    {
        uint8_t b = stream.read();
        if (improv::parse_improv_serial_byte(position, b, buffer, handleImprovCommand, handleImprovError))
        {
            buffer[position++] = b;
        }
        else
        {
            break;
        }
    }
}