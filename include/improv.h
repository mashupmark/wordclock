/**
 * This file was copied from https://github.com/improv-wifi/sdk-cpp to add the improv-wifi sdk to this project
 * It should not be modified manually
 */

#pragma once

#ifdef ARDUINO
#include <Arduino.h>
#endif // ARDUINO

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace improv
{

    static const char *const SERVICE_UUID = "00467768-6228-2272-4663-277478268000";
    static const char *const STATUS_UUID = "00467768-6228-2272-4663-277478268001";
    static const char *const ERROR_UUID = "00467768-6228-2272-4663-277478268002";
    static const char *const RPC_COMMAND_UUID = "00467768-6228-2272-4663-277478268003";
    static const char *const RPC_RESULT_UUID = "00467768-6228-2272-4663-277478268004";
    static const char *const CAPABILITIES_UUID = "00467768-6228-2272-4663-277478268005";

    enum Error : uint8_t
    {
        ERROR_NONE = 0x00,
        ERROR_INVALID_RPC = 0x01,
        ERROR_UNKNOWN_RPC = 0x02,
        ERROR_UNABLE_TO_CONNECT = 0x03,
        ERROR_NOT_AUTHORIZED = 0x04,
        ERROR_UNKNOWN = 0xFF,
    };

    enum State : uint8_t
    {
        STATE_STOPPED = 0x00,
        STATE_AWAITING_AUTHORIZATION = 0x01,
        STATE_AUTHORIZED = 0x02,
        STATE_PROVISIONING = 0x03,
        STATE_PROVISIONED = 0x04,
    };

    enum Command : uint8_t
    {
        UNKNOWN = 0x00,
        WIFI_SETTINGS = 0x01,
        IDENTIFY = 0x02,
        GET_CURRENT_STATE = 0x02,
        GET_DEVICE_INFO = 0x03,
        GET_WIFI_NETWORKS = 0x04,
        BAD_CHECKSUM = 0xFF,
    };

    static const uint8_t CAPABILITY_IDENTIFY = 0x01;
    static const uint8_t IMPROV_SERIAL_VERSION = 1;

    enum ImprovSerialType : uint8_t
    {
        TYPE_CURRENT_STATE = 0x01,
        TYPE_ERROR_STATE = 0x02,
        TYPE_RPC = 0x03,
        TYPE_RPC_RESPONSE = 0x04
    };

    struct ImprovCommand
    {
        Command command;
        std::string ssid;
        std::string password;
    };

    ImprovCommand parse_improv_data(const std::vector<uint8_t> &data, bool check_checksum = true);
    ImprovCommand parse_improv_data(const uint8_t *data, size_t length, bool check_checksum = true);

    bool parse_improv_serial_byte(size_t position, uint8_t byte, const uint8_t *buffer,
                                  std::function<bool(ImprovCommand)> &&callback, std::function<void(Error)> &&on_error);

    std::vector<uint8_t> build_rpc_response(Command command, const std::vector<std::string> &datum,
                                            bool add_checksum = true);
#ifdef ARDUINO
    std::vector<uint8_t> build_rpc_response(Command command, const std::vector<String> &datum, bool add_checksum = true);
#endif // ARDUINO

} // namespace improv

/**
 * Starting from here custom code to integrate the improv protocol was added
 */

std::vector<uint8_t> createImprovResponse(improv::State state);
std::vector<uint8_t> createImprovResponse(std::vector<uint8_t> &response);
std::vector<uint8_t> createImprovResponse(improv::Error error);

bool handleImprovCommand(improv::ImprovCommand cmd);
void parseImprovCommand(Stream &stream, char arrivedChar, unsigned short availableCharsCount);