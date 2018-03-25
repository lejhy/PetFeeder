#include <Hardware/CS_Driver.h>
#include <Devices/MSPIO.h>
#include <Devices/ESP8266.h>
#include <JSMN/jsmn.h>

/* VCC = 3.3V
 * GND = GND
 * TX = P3.2
 * RX = P3.3
 * RST = P6.1
 * GPIO15 = GND
 * EN = 3.3V */

/*AP Data*/
char SSID[] = "Redmi";
char SSID_PASSWORD[] = "lejhanec";

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_Config UART0Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
eUSCI_UART_Config UART2Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     13,
     0,
     37,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

/*Pointer to ESP8266 global buffer*/
char *ESP8266_Data;
/*We are connecting to petfeeder.chorvatskonaplazi.cz, ESP8266 can resolve DNS, pretty cool huh!*/
char HTTP_WebPage[] = "petfeeder.chorvatskonaplazi.cz";
/*HTTP is TCP port 80*/
char Port[] = "80";
/*Total number of compartments */
int maximumCrickets = 6;
/*Number of compartments left */
int cricketsLeft = 6;
/*Timestamp of the last command received*/
long lastCommandTimestamp = 0;

char* getData();
char* getCommand(char* list, jsmntok_t* tokens, int tokensSize);
long getCommandTimestamp(char* list, jsmntok_t* tokens, int tokensSize);
void sendCricketStatus();

void main(void)
{
	MAP_WDT_A_holdTimer();

    /*MSP432 Running at 24 MHz*/
	CS_Init();

	/*Initialize required hardware peripherals for the ESP8266*/
	UART_Init(EUSCI_A0_BASE, UART0Config);
	UART_Init(EUSCI_A2_BASE, UART2Config);
    GPIO_Init(GPIO_PORT_P6, GPIO_PIN1);

    MAP_Interrupt_enableMaster();

    ESP8266_Data = ESP8266_GetBuffer();

    /*Hard Reset ESP8266*/
    ESP8266_HardReset();
    __delay_cycles(48000000);
    /*flush reset data, we do this because a lot of data received cannot be printed*/
    UART_Flush(EUSCI_A2_BASE);

    /*Check UART connection to MSP432*/
    if(!ESP8266_CheckConnection())
    {
        MSPrintf(EUSCI_A0_BASE, "Failed MSP432 UART connection\r\n");
        /*Trap MSP432 if failed connection*/
        while(1);
    }

    MSPrintf(EUSCI_A0_BASE, "Nice! We are connected to the MSP432\r\n\r\n");

    MSPrintf(EUSCI_A0_BASE, "Disable echo\r\n\r\n");
    if (!ESP8266_DisableEcho())
    {
        MSPrintf(EUSCI_A0_BASE, "failed\r\n\r\n");
    } else {
        MSPrintf(EUSCI_A0_BASE, "success\r\n\r\n");
    }

    /*Connect to Access Point here*/
    MSPrintf(EUSCI_A0_BASE, "Connecting to AP %s\r\n\r\n", SSID);
    if(!ESP8266_ConnectToAP(SSID, SSID_PASSWORD))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to connect to SSID '%s' with SSID_PASSWORD '%s': %s\r\n\r\n", SSID, SSID_PASSWORD, ESP8266_Data);
       /* Trap MSP432 we are not connected*/
       while(1);
    }
    MSPrintf(EUSCI_A0_BASE, "Hooray! We are connected to AP %s\r\n\r\n", SSID);


    /*Enable multiple connections, up to 4 according to the internet*/
    if(!ESP8266_EnableMultipleConnections(true))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to set multiple connections\r\n");
        while(1);
    }

    MSPrintf(EUSCI_A0_BASE, "Multiple connections enabled\r\n\r\n");


    /* Our main loop where all the magic happens */
    while(1)
    {
        char* data = getData();
        if (data != NULL) {
            /*Parser for JSON string*/
            jsmn_parser parser;
            jsmn_init(&parser);
            jsmntok_t tokens[10];
            jsmn_parse(&parser, data, strlen(data), tokens, 10);

            long timestamp = getCommandTimestamp(data, tokens, 10);
            if (lastCommandTimestamp == 0) {
                lastCommandTimestamp = timestamp;
            } else if (lastCommandTimestamp < timestamp) {
                lastCommandTimestamp = timestamp;
                char* command = getCommand(data, tokens, 10);
                if (command != NULL) {
                    if (strcmp(command, "feed") == 0) {
                        if (cricketsLeft > 0) {
                            MSPrintf(EUSCI_A0_BASE, "Feeding\r\n\r\n");
                            cricketsLeft--;
                        }
                    } else if (strcmp(command, "left") == 0) {
                        MSPrintf(EUSCI_A0_BASE, "Going left\r\n\r\n");
                    } else if (strcmp(command, "right") == 0) {
                        MSPrintf(EUSCI_A0_BASE, "Going right\r\n\r\n");
                    }
                    // Don't forget to free the heap
                    free(command);
                }

            }
            free(data);
        }

        sendCricketStatus();

        __delay_cycles(48000000);
    }
}

char* getData() {
    /*Data that will be sent to the HTTP server. */
    char HTTP_Request[] = "GET /get.php?last_timestamp=true&device_id=1&from_device=false HTTP/1.0\r\n"
                          "Host: petfeeder.chorvatskonaplazi.cz\r\n"
                          "\r\n";
    /*Subtract one to account for the null character*/
    uint32_t HTTP_Request_Size = sizeof(HTTP_Request) - 1;

    /*Try to establish TCP connection to a HTTP server*/
    if(!ESP8266_EstablishConnection('0', TCP, HTTP_WebPage, Port))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to connect to: %s\r\nERROR: %s\r\n", HTTP_WebPage, ESP8266_Data);
        return NULL;
    }

    MSPrintf(EUSCI_A0_BASE, "Connected to: %s\r\n\r\n", HTTP_WebPage);

    /*Query data to connected HTTP server, in order to do this look for an API and request a key*/
    if(!ESP8266_SendData('0', HTTP_Request, HTTP_Request_Size))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
        return NULL;
    }

    /*We succesfully sent the data to the HTTP server, we are now going to receive a lot of data from the ESP8266 (or depending
     * on what information you are querying, CHANGE ESP8266 BUFFER SIZE AND UARTA2 BUFFER SIZE, OTHERWISE YOU WILL LOSS DATA!!!!!).*/
    MSPrintf(EUSCI_A0_BASE, "Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);

    char* listStart = strchr(ESP8266_Data, '[');
    char* listEnd = strchr(ESP8266_Data, ']') + 1;
    while (strchr(listEnd, ']') != NULL) {
        listEnd = strchr(listEnd, ']');
    }
    int listLength = listEnd - listStart;
    char* list = malloc(listLength + 1);
    int i;
    for (i = 0; i < listLength; i++) {
        list[i] = listStart[i];
    }
    list[i] = '\0';
    MSPrintf(EUSCI_A0_BASE, "%s\r\n\r\n", list);

    return list;
}

char* getCommand(char* list, jsmntok_t* tokens, int tokensSize) {

    int i;
    for (i = 0; i < tokensSize; i++) {
        if (tokens[i].type == JSMN_STRING) {
            char name[32];
            int size = tokens[i].end - tokens[i].start;
            int j;
            for (j = 0; j < size; j++) {
                name[j] = list[tokens[i].start+j];
            }
            name[j] = '\0';
            MSPrintf(EUSCI_A0_BASE, "Assessing string: %s\r\n", name);

            if (strcmp(name, "command") == 0) {
                i++;
                size = tokens[i].end - tokens[i].start;
                char* command = malloc(size+1); //extra one for the null at the end
                for (j = 0; j < size; j++) {
                    command[j] = list[tokens[i].start+j];
                }
                command[j] = '\0';
                MSPrintf(EUSCI_A0_BASE, "Command value: %s\r\n", command);
                return command;
            }
        }
    }
    // Nothing was found, so return null
    return NULL;
}

long getCommandTimestamp(char* list, jsmntok_t* tokens, int tokensSize) {

    int i;
    for (i = 0; i < tokensSize; i++) {
        if (tokens[i].type == JSMN_STRING) {
            char name[32];
            int size = tokens[i].end - tokens[i].start;
            int j;
            for (j = 0; j < size; j++) {
                name[j] = list[tokens[i].start+j];
            }
            name[j] = '\0';
            MSPrintf(EUSCI_A0_BASE, "Assessing string: %s\r\n", name);

            if (strcmp(name, "command_timestamp") == 0) {
                i++;
                size = tokens[i].end - tokens[i].start;
                long timestamp = 0;
                for (j = 0; j < size; j++) {
                    timestamp = timestamp * 10 + (list[tokens[i].start+j] - '0');
                }
                MSPrintf(EUSCI_A0_BASE, "Timestamp value: %i\r\n", timestamp);
                return timestamp;
            }
        }
    }
    // Nothing was found, so return zero
    return 0;
}

void sendCricketStatus() {
    /*Data that will be sent to the HTTP server. */
    char HTTP_Request[256];
    char firstNumber[10];
    char secondNumber[10];
    int i = 0;
    int num = cricketsLeft;
    while (num > 0 || i == 0) {
        int digit = num%10;
        firstNumber[i] = '0' + digit;
        num = num/10;
        i++;
    }
    firstNumber[i] = '\0';
    i = 0;
    num = maximumCrickets;
    while (num > 0 || i == 0) {
        int digit = num%10;
        secondNumber[i] = '0' + digit;
        num = num/10;
        i++;
    }
    secondNumber[i] = '\0';
    HTTP_Request[0] = '\0';
    strcat(HTTP_Request, "GET /post.php?device_id=1&command={\"current\":");
    strcat(HTTP_Request, firstNumber);
    strcat(HTTP_Request, ",\"maximum\":");
    strcat(HTTP_Request, secondNumber);
    strcat(HTTP_Request, "}&from_device=true HTTP/1.0\r\n"
                          "Host: petfeeder.chorvatskonaplazi.cz\r\n"
                          "\r\n");

    /*Subtract one to account for the null character*/
    uint32_t HTTP_Request_Size = strlen(HTTP_Request);

    /*Try to establish TCP connection to a HTTP server*/
    if(!ESP8266_EstablishConnection('1', TCP, HTTP_WebPage, Port))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to connect to: %s\r\nERROR: %s\r\n", HTTP_WebPage, ESP8266_Data);
        return;
    }

    MSPrintf(EUSCI_A0_BASE, "Connected to: %s\r\n\r\n", HTTP_WebPage);


    /*Query data to connected HTTP server, in order to do this look for an API and request a key*/
    if(!ESP8266_SendData('1', HTTP_Request, HTTP_Request_Size))
    {
        MSPrintf(EUSCI_A0_BASE, "Failed to send: %s to %s \r\nError: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
        return;
    }

    /*We succesfully sent the data to the HTTP server, we are now going to receive a lot of data from the ESP8266 (or depending
     * on what information you are querying, CHANGE ESP8266 BUFFER SIZE AND UARTA2 BUFFER SIZE, OTHERWISE YOU WILL LOSS DATA!!!!!).*/
    MSPrintf(EUSCI_A0_BASE, "Data sent: %s to %s\r\n\r\nESP8266 Data Received: %s\r\n", HTTP_Request, HTTP_WebPage, ESP8266_Data);
}


