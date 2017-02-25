#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <libusb-1.0/libusb.h>
#include <map>
#include <vector>
#include <cstdint>

#define DEBUG_LIBUSB

enum KM_Command : unsigned char {
  commandSetCurrentProfile = 0x01,
  commandGetCurrentProfile = 0x81,
  commandSetProfileInformation = 0x02,
  commandGetProfileInformation = 0x82,
  commandSetLEDEffectDatabase = 0x08,
  commandGetLEDEffectDatabase = 0x88,
  commandSetLEDMatrixDatabase = 0x09,
  commandGetLEDMatrixDatabase = 0x89
};


typedef std::vector<unsigned char> data_vector;
typedef std::pair<data_vector, data_vector> command_response_t;

class Keyboard
{
public:
  Keyboard();
  virtual ~Keyboard();

  int connect();
  bool is_connected();
  int disconnnect();

  command_response_t send_command(KM_Command command, unsigned char* data, int data_length);

private:
  libusb_context *ctx;
  libusb_device_handle *dev;

  // CONSTANTS
  static const uint16_t G_SKILL_VENDOR_ID = 0x28da;
  static const uint16_t KM780_RGB_PRODUCT_ID = 0x1101;

  static const int ENDPOINT_IN = 0x83; //Endpoing 3 IN
  static const unsigned char OUT_INTERFACE = 1; //Interface for outgoing communications
  static const unsigned char IN_INTERFACE = 2; //Interface for incoming communications

  static const unsigned char KM_START_CHAR = 0x07; //This magic character is sent at the begging of each packet

  static const int CONNECTION_TIMEOUT = 500;

  enum Response_T : unsigned char {
    interrupt_transfer,
    control_transfer
  };

  static const std::map<KM_Command, Response_T> RESPONSE_TYPE_MAP;
};


#endif /* KEYBOARD_H */
