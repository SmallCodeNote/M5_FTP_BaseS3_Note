#include <Arduino.h>
#include <M5Unified.h>
#include <SPI.h>
#include <time.h>
#include <M5_Ethernet.h>
#include "M5_Ethernet_FtpClient.hpp"

// == M5Basic_Bus ==
/*#define SCK  18
#define MISO 19
#define MOSI 23
#define CS   26
*/

// == M5CORES2_Bus ==
/*#define SCK  18
#define MISO 38
#define MOSI 23
#define CS   26
*/

// == M5PoECAM_Bus ==
/*#define SCK 23
#define MISO 38
#define MOSI 13
#define CS 4
*/

// == M5CORES3_Bus/M5CORES3_SE_Bus ==
#define SCK 36
#define MISO 35
#define MOSI 37
#define CS 9

//  01 05 00 01 02 00 9d 6a
char uart_buffer[8] = {0x01, 0x05, 0x00, 0x01, 0x02, 0x00, 0x9d, 0x6a};
char uart_rx_buffer[8] = {0};

char Num = 0;
char stringnum = 0;
unsigned long W5500DataNum = 0;
unsigned long Send_Num_Ok = 0;
unsigned long Rec_Num = 0;
unsigned long Rec_Num_Ok = 0;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 25, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

EthernetClient NtpClient(123);
EthernetClient FtpClient(21);

char ftp_server[] = "192.168.25.77";
char ftp_user[] = "ftpusr";
char ftp_pass[] = "ftpword";
char ftp_dirName[] = "/dataDir";
char ftp_newDirName[] = "/dataDir";
M5_Ethernet_FtpClient ftp(ftp_server, ftp_user, ftp_pass, 60000);

char textArray[] = "textArray";

void M5Begin()
{
  auto cfg = M5.config();
  cfg.serial_baudrate = 19200;
  M5.begin(cfg);
}
void EthernetBegin()
{
  SPI.begin(SCK, MISO, MOSI, -1);
  Ethernet.init(CS);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
}
void setup()
{
  // Open serial communications and wait for port to open:
  M5Begin();
  M5.Power.begin();
  EthernetBegin();

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  M5.Lcd.println("M5Stack W5500 Test");

  String answerArry[256];
  ftp.OpenConnection();
 
  // ftp.InitAsciiPassiveMode();
  // ftp.ContentListWithListCommand("", answerArry);

  ftp.AppendTextLine("testFileESP32.txt", "Test data sample");

  /*
    ftp.MakeDir("TestDir");
    ftp.ChangeWorkDir("TestDir"); // 必要に応じてディレクトリを変更

    ftp.NewFile("test.txt");

    ftp.WriteData((unsigned char *)textArray, sizeof(textArray) - 1);
    // delay(1000);
    ftp.CloseFile();

  */
  ftp.CloseConnection();
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank)
        {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println(
              "Connection: close");     // the connection will be closed
                                        // after completion of the
                                        // response
          client.println("Refresh: 5"); // refresh the page
                                        // automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          client.println("<body>");
          client.println("<h1>M5Stack W5500 Test</h1>");
          client.println("<br />");
          client.println("<p>Please click here</p>");
          client.println(
              "<a href=\"http://www.M5Stack.com\">M5Stack</a>");
          client.println("<br />");
          client.println("<br />");
          client.println("<br />");

          client.print("W5500 Counter Num :");
          client.print(W5500DataNum);
          client.println("<br />");
          client.println("<br />");
          W5500DataNum++;

          client.print("Rec_Num_Ok Counter :");
          client.print(Rec_Num_Ok);
          client.println("<br />");
          client.println("<br />");

          client.print("Rec_Num Counter :");
          client.print(Rec_Num);
          client.println("<br />");
          client.println("<br />");

          client.println("</body>");

          client.println("</html>");
          break;
        }
        if (c == '\n')
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r')
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}