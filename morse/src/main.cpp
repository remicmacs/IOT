#include <Arduino.h>
#include <WiFi.h>
#include <inttypes.h>

const char* ssid = "APESP32";
const char* password =  "rootroot";

const int LED = 2;

const uint ServerPort = 23;
WiFiServer Server(ServerPort);

WiFiClient RemoteClient;

#define MORSE_CHAR_SIZE 5
#define LUT_SIZE 36

typedef unsigned int morse_char[MORSE_CHAR_SIZE];

const char ascii[LUT_SIZE+1] = "abcdefghijklmnopqrstuvwxyz0123456789";

const unsigned int morse[LUT_SIZE][MORSE_CHAR_SIZE] = {
  {0, 1, 9, 9, 9},
  {1, 0, 0, 0, 9},
  {1, 0, 1, 0, 9},
  {1, 0, 0, 9, 9},
  {0, 9, 9, 9, 9},
  {0, 0, 1, 0, 9},
  {1, 1, 0, 9, 9},
  {0, 0, 0, 0, 9},
  {0, 0, 9, 9, 9},
  {0, 1, 1, 1, 9},
  {1, 0, 1, 9, 9},
  {0, 1, 0, 0, 9},
  {1, 1, 9, 9, 9},
  {1, 0, 9, 9, 9},
  {1, 1, 1, 9, 9},
  {0, 1, 1, 0, 9},
  {1, 1, 0, 1, 9},
  {0, 1, 0, 9, 9},
  {0, 0, 0, 9, 9},
  {1, 9, 9, 9, 9},
  {0, 0, 1, 9, 9},
  {0, 0, 0, 1, 9},
  {0, 1, 1, 9, 9},
  {1, 0, 0, 1, 9},
  {1, 0, 1, 1, 9},
  {1, 1, 0, 0, 9}, // Z
  {1, 1, 1, 1, 1}, // 0
  {0, 1, 1, 1, 1},
  {0, 0, 1, 1, 1},
  {0, 0, 0, 1, 1},
  {0, 0, 0, 0, 1},
  {0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0},
  {1, 1, 0, 0, 0},
  {1, 1, 1, 0, 0},
  {1, 1, 1, 1, 0}
};

void ti() {
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(100);
}

void ta() {
  digitalWrite(LED, HIGH);
  delay(300);
  digitalWrite(LED, LOW);
  delay(100);
}

void display_morse_char(morse_char to_display) {
  for (int i = 0 ; i < MORSE_CHAR_SIZE; i++) {
    int subchar = to_display[i];
    if (9 == subchar) {
      i = MORSE_CHAR_SIZE + 1;
      break;
    } else if (0 == subchar) {
      Serial.print(".");
      ti();
    } else {
      Serial.print("-");
      ta();
    }
  }

  delay(300);
  Serial.print(" ");
}

void get_morse(morse_char * dest, int index) {
  for (int i = 0 ; i < MORSE_CHAR_SIZE; i++) {
    int subchar = morse[index][i];
    (*dest)[i] = subchar;
  }
}

void display_morse_msg(morse_char * msg, int msg_size) {
  for (int i = 0 ; i < msg_size ; i++) {
    display_morse_char(msg[i]);
  }
}

void transmit_morse_message(char * msg, int msg_size) {
  morse_char morse_msg[msg_size];
  for (int i = 0; i < msg_size ; i++) {
    char a_char = msg[i];
    if (!isascii(a_char)) {
      Serial.println("Message is not ASCII, dropping it");
      return;
    }
    a_char = tolower(a_char);
    int index = -1;
    for (int j = 0; i < LUT_SIZE ; j++) {
      if (ascii[j] == a_char) {
        index = j;
        break;
      }
    }

    get_morse(&(morse_msg[i]), index);
  }
  Serial.print("_._._. "); // CT (Start message)
  ta();ti();ta();ti();ta();ti();

  display_morse_msg(&(morse_msg[0]), msg_size);

  ti();ta();ti();ta();ti();ta();
  Serial.println("._._."); // AR (End message)
  return;
}

/**
 * Connection routine for telnet clients
 */
void checkForConnections()
{
  if (Server.hasClient())
  {
    // If we are already connected to another computer,
    // then reject the new connection. Otherwise accept
    // the connection.
    if (RemoteClient.connected())
    {
      Serial.println("Connection rejected");
      Server.available().stop();
    } else {
      Serial.println("Connection accepted");
      RemoteClient = Server.available();
    }
  }
}

/**
 * Client handling routine
 */
void listen() {
  char * buffer;
  uint8_t inputBuffer[30];
  while (RemoteClient.connected() && RemoteClient.available())
  {
    // Number of bytes received
    int received = RemoteClient.read(inputBuffer, sizeof(inputBuffer));

    // Processing needed size of buffer
    int needed = snprintf(NULL, 0, "Received %d bytes", received);
    // Allocating buffer
    buffer = (char *) malloc(needed + 1);

    // Displaying buffer
    sprintf(buffer, "Received %d bytes", received);
    Serial.println(buffer);
    Serial.println((char *)inputBuffer);

    free(buffer);

      // Allocating buffer
      buffer = (char *) malloc(received-1);
      strncpy(buffer, (char *) inputBuffer, received-2);
      buffer[received-2] = '\0';

    // Find if a command is found in the message
    if (strcmp(buffer, "On") == 0) {
      Serial.println("Found On");
      digitalWrite(LED, HIGH);
    } else if (strcmp(buffer, "Off") == 0) {
      Serial.println("Found Off");
      digitalWrite(LED, LOW);
    } else {
      int state = digitalRead(LED);
      bool was_on = (HIGH == state);
      digitalWrite(LED, LOW);
      Serial.print("Message to broadcast : \"");
      Serial.print(buffer);
      Serial.println("\"");

      transmit_morse_message(buffer, received-2);

      if (was_on) digitalWrite(LED, HIGH);
    }
  }
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Establishing connection to WiFi..");
  }

  // Blink three times if connection to WiFi is successful
  int blink = 3;
  while (blink-- > 0) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  Server.begin();
  Serial.println("Connected to network");

}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  WiFi.scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
}

void loop() {
    checkForConnections();
    listen();
}