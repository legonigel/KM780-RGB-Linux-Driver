#include "keyboard.hpp"


/*
 * Default Keyboard Constructor
 */
Keyboard::Keyboard()
{
  ctx = NULL;
  dev = NULL;
  int r;
  r = libusb_init(&ctx); //Open libusb session
  if(r != 0) {
    ctx = NULL;
    return;
  }
#ifdef DEBUG_LIBUSB
  libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);
#endif
}

/*
 * Destructor for keyboard,
 * Cleanup libusb
 */
Keyboard::~Keyboard()
{
  if(dev) {
    disconnnect();
  }
  if(ctx) {
    libusb_exit(ctx);
  }
}

/*
 * Connects to the keyboard before any communication can be sent
 * Returns 0 on success
 */
int Keyboard::connect()
{
  if(dev) {
    //Already connected, do nothing
    return -1;
  }
  int r;
  dev = libusb_open_device_with_vid_pid(ctx, G_SKILL_VENDOR_ID, KM780_RGB_PRODUCT_ID);

  if(dev == NULL) {
    return -1;
  }

  //Get the kernel to let go
  r = libusb_set_auto_detach_kernel_driver(dev, OUT_INTERFACE);
  if(r != LIBUSB_SUCCESS) {
    return -1;
  }

  r = libusb_set_auto_detach_kernel_driver(dev, IN_INTERFACE);
  if(r != LIBUSB_SUCCESS) {
    return -1;
  }

  //This interface is ours
  r = libusb_claim_interface(dev, OUT_INTERFACE);
  if(r != LIBUSB_SUCCESS) {
    return -1;
  }

  r = libusb_claim_interface(dev, IN_INTERFACE);
  if(r != LIBUSB_SUCCESS) {
    return -1;
  }

  return 0;
}


/*
 * Returns true if the keyboard is connected
 */
bool Keyboard::is_connected()
{
  return dev != NULL;
}

/*
 * Disconnects from the keyboard
 * Returns 0 on success
 */
int Keyboard::disconnnect()
{
  if(dev == NULL) {
    return 0; //Already disconnected
  }
  libusb_release_interface(dev, OUT_INTERFACE);
  libusb_release_interface(dev, IN_INTERFACE);
  libusb_close(dev);
  dev = NULL;
  return 0;
}

command_response_t Keyboard::send_command(KM_Command command, unsigned char* data, int data_length)
{
  const static uint8_t request_type = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT;
  const static uint8_t request = LIBUSB_REQUEST_SET_CONFIGURATION;
  const static uint16_t value = 0x0307; //Magic number from keyboard manufacturer.

  const static uint16_t index = 0x0001; //Another magic number from manufacturer.
  const static uint16_t length = 0x0108; //264 bytes long

  int r;

  unsigned char send_data[length] = {KM_START_CHAR, command};

  if (data && data_length && data_length <= 262) {
    for(int i = 0; i < data_length; i++) {
      send_data[i + 2] = data[i];
    }
  }

  printf("Doing first request \n");
  printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", request_type, request, value & 0xFF, value >> 8, index & 0xFF, index >> 8, length & 0xFF, length >> 8);

  r = libusb_control_transfer(dev, request_type, request, value, index, send_data, length, CONNECTION_TIMEOUT);

  if (r == LIBUSB_ERROR_TIMEOUT) {
    printf("Request timed out");
    return command_response_t();
  } else if (r == LIBUSB_ERROR_PIPE) {
    printf("Control request not supported by device");
    return command_response_t();
  } else if (r == LIBUSB_ERROR_NO_DEVICE) {
    printf("Device disconnected");
    return command_response_t();
  } else if( r < 0) {
    printf("LIBUSB Error %d\n", r);
    printf("%s", libusb_strerror((libusb_error)r));
    return command_response_t();
  }

  //Get response from EP3(IN)
  const static int response_length = 8;

  data_vector response(response_length);
  int transferred;
  bool got_correct_response = false;

  printf("Doing second request \n");

  r = 0;
  while(!got_correct_response) {
    r = libusb_interrupt_transfer(dev, ENDPOINT_IN, &response[0], response_length, &transferred, CONNECTION_TIMEOUT);

    if(r != 0) {
      printf("Interrupt transfer failed: %d\n", r);
      if(r == -7) {
        break;
      }
    }

    if (response[0] == 0 && response[1] == 0 && response[2] == command) {
      got_correct_response = true;
    }
  }

  printf("Got resposne %d\n", got_correct_response);

  data_vector response_data(length);

  printf("Doing third request \n");

  if (RESPONSE_TYPE_MAP.at(command) == control_transfer) {
    //Additional work to do for a control transfer type response
    const static uint8_t response_request_type = LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN;
    const static uint8_t response_request =  LIBUSB_REQUEST_CLEAR_FEATURE;
    r = libusb_control_transfer(dev, response_request_type, response_request, value, index, &response_data[0], length, CONNECTION_TIMEOUT);
    if (r == LIBUSB_ERROR_TIMEOUT) {
      printf("Request timed out");
      return command_response_t();
    } else if (r == LIBUSB_ERROR_PIPE) {
      printf("Control request not supported by device");
      return command_response_t();
    } else if (r == LIBUSB_ERROR_NO_DEVICE) {
      printf("Device disconnected");
      return command_response_t();
    } else if( r < 0) {
      printf("LIBUSB Error %d\n", r);
      printf("%s", libusb_strerror((libusb_error)r));
      return command_response_t();
    }

  }


  return command_response_t(response, response_data);
}

/*
 * Constant response type map
 */
const std::map<KM_Command, Keyboard::Response_T> Keyboard::RESPONSE_TYPE_MAP = {
  {commandSetCurrentProfile,     interrupt_transfer},
  {commandGetCurrentProfile,     interrupt_transfer},
  {commandSetProfileInformation, interrupt_transfer},
  {commandGetProfileInformation, control_transfer},
  {commandSetLEDEffectDatabase,  interrupt_transfer},
  {commandGetLEDEffectDatabase,  control_transfer},
  {commandSetLEDMatrixDatabase,  interrupt_transfer},
  {commandGetLEDMatrixDatabase,  control_transfer}
};
