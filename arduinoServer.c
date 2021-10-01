/*
 A simple Arduino Ethernet web server. 
 */

#include <SPI.h>
#include <Ethernet.h>

// You can change the MAC and IP addresses to suit your network:

byte mac[] = { 0X52, 0X64, 0X75, 0X69, 0X6E, 0X6F };
IPAddress ip( 192,168,0,97 );

EthernetServer server(80); // Port 80 is HTTP port
char new_state[1024];

void setup()
{
  Serial.begin(9600);
  // Start the Ethernet server:
  Ethernet.begin(mac, ip);

  server.begin();

  // Set the digital pins ready to write to
  for (int pin = 2; pin <= 9; pin++) {
    pinMode(pin, OUTPUT);
  }

  Serial.print("Serving on http://");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();

  if (client) {

    // Serial.println("Client connected");

    while (client.connected()) {

      int i = 0;
      int head = 1;
      int body = 0;

      while(client.available()) {
        char c = client.read();
        if (c == '\n') {

          if ( i <= 2 ) {

            // an http request ends with a blank line

            sendPage(client);
            if ( head == 1 ) {
              body = 1;
              head = 0;
            }

          }

          i = -1;

        }
        if ( body == 1 ) {
          new_state[i] = c;
        }
        i++;
        new_state[i] = '\0';
      }
      i = 0;
    }

    // Serial.println("Disconnected");
    /*
    if ( strlen(new_state) > 0 ){
      Serial.print ("[");
      Serial.print(new_state);
      Serial.println ("]");
    }
    */
    // Post data looks like pinD2=On
    if ( strncmp( new_state, "pinD", 4) == 0 ) {
      int pin = new_state[4] - 48; // Convert ascii to int
      // Serial.println(pin);
      if ( strncmp( new_state+5, "=On", 3) == 0 ) {
        digitalWrite(pin, 1);
      } 
      else if ( strncmp( new_state+5, "=Off", 4) == 0 ) {
        digitalWrite(pin, 0);
      }
    }

  }

}

void sendPage(EthernetClient client)
{

  // Serial.println("Sending response");

  // send a standard http response header
  client.println("HTTP/1.0 200 OK\Content-Type: text/html\n\n<html>\n<head>");
  client.println("<link rel='icon' href='data:;base64,iVBORw0KGgo='>");
  client.println("<title>POST Pin controller</title>\n</head>\n<body>\n");
  client.println("<h2>Buttons turn pins on or off</h2>");
  client.println("<form method='post' action='/' name='pins'>");

  char line[1024];
  int pin;

  for ( pin=2; pin<=9; pin++ ) {
    sprintf(line, "<input name='pinD%d' type='submit' value='On' />\n", pin);
    client.print(line);
    sprintf(line, "<input name='pinD%d' type='submit' value='Off' /> %d<br />\n", pin, pin);
    client.print(line);
  }

  client.println("</form>\n</body>\n</html>");
  client.stop();

}