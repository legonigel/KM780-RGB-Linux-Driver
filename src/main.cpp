#include <stdio.h>
#include <QApplication>
#include "mainwindow.hpp"
#include "main.hpp"
#include "keyboard.hpp"


#define VERSION "0.1.0"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  Keyboard key = Keyboard();
  key.connect();
  if (key.is_connected()) {
    unsigned char mydata[] = {2};

    command_response_t response = key.send_command(commandSetCurrentProfile, mydata, 1);

    if(!response.first.empty()) {
      printf("Recieved: ");
      for (unsigned char n : response.first) {
        printf("%02x ", n);
      }
      printf("\n");
    }
    if(!response.second.empty()) {
      printf("Recieved: ");
      for(int i = 0; i < response.second.size(); i++) {
        if( i % 8 == 0) {
          printf("\n%02i: ", (i / 8));
        }
        printf("%02x ", response.second[i]);
      }
      printf("\n");
    }
  }

  return a.exec();
}
