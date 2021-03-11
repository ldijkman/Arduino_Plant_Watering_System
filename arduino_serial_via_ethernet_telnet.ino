/*
   This sketch
   Serial-To-Ethernet
   arduino with ethernetshield
   arduino sends data via ethernet telnet
   ethernetcable arduino <-> router can be 100 meters? 
   makes longer distance monitoring

  write incomming data to screen
   pi@raspberrypi:~ $ telnet 192.168.178.13

  write incomming data to logfile
   pi@raspberrypi:~ $ telnet 192.168.178.13 | tee -a -i /home/pi/LogFile


*/

// used parts of code from https://github.com/stepansnigirev/ArduinoSerialToEthernet

/*
   Uncomment one of the following lines depending on
   what ethernet library works with your board.
   If it is based on W5500 chip you will need
   Ethernet2.h and EthernetUdp2.h
*/
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
//#include <Ethernet2.h>
//#include <EthernetUdp2.h>

#define CMD_PORT 23                         // Port to send commands

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0x5C, 0xEE };  // replace this with your mac address


/*
  For manual configuration of the network uncomment the following lines
  and change the Ethernet.begin arguments in the setup() function
*/
//IPAddress ip(192, 168, 178, 13);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(255, 255, 0, 0);

EthernetServer cmdServer(CMD_PORT);

EthernetUDP Udp;



void setup() {
  // For DHCP use only mac. For manual configuration use all parameters.
  Ethernet.begin(mac);  //  Ethernet.begin(mac, ip, gateway, subnet);

  Serial.begin(115200);
  Serial.print("IP-address: ");
  Serial.println(Ethernet.localIP());

  cmdServer.begin();

}



void loop() {

  EthernetClient client = cmdServer.available(); // wait for a new client:

  cmdServer.println(millis() / 1000);

  while (Serial.available()) {
    cmdServer.print(char(Serial.read()));
  }

}
