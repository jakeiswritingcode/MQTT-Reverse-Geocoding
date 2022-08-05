// mqtt_geocoding_pub.c
// by Jake Charles Osborne III



#include <stdio.h>
#include <mosquitto.h>
#include <windows.h>



int getRandomInt(int lowerBound, int upperBound) // [LB, UB)
{
    return lowerBound + rand() % (upperBound - lowerBound);
}

void setToRandomNumbers(char* array)
{
    for (int i = 0; array[i] != '\0'; ++i) {
        array[i] = '0' + rand() % 10;
    }
}

void itos(int i, char* s) // positive only
{
    int digits = 0;

    do {
        s[digits] = '0' + (i % 10);
        i /= 10;
        ++digits;
    } while (i != 0);
    s[digits] = '\0';

    for (int i = 0; i < digits / 2; ++i) {
        char temp = s[i];
        s[i] = s[digits - 1 - i];
        s[digits - 1 - i] = temp;
    }
}

void getPost(char* str)
{
    str[0] = '\0';

    // initialize variables

    char latitude[8];
    char latitudeDecimals[13] = "123456789012\0";

    char longitude[8];
    char longitudeDecimals[13] = "123456789012\0";

    // generate data

    itos(getRandomInt(0, 90), latitude);
    setToRandomNumbers(latitudeDecimals);

    itos(getRandomInt(0, 180), longitude);
    setToRandomNumbers(longitudeDecimals);

    // construct results

    strcat_s(str, 64, "{");

    strcat_s(str, 64, "\"latitude\":\"");
    if (rand() % 2 == 0) strcat_s(str, 64, "-");
    strcat_s(str, 64, latitude);
    strcat_s(str, 64, ".");
    strcat_s(str, 64, latitudeDecimals);
    strcat_s(str, 64, "\"");

    strcat_s(str, 64, ",");

    strcat_s(str, 64, "\"longitude\":\"");
    if (rand() % 2 == 0) strcat_s(str, 64, "-");
    strcat_s(str, 64, longitude);
    strcat_s(str, 64, ".");
    strcat_s(str, 64, longitudeDecimals);
    strcat_s(str, 64, "\"");

    strcat_s(str, 64, "}");

    strcat_s(str, 64, "\0");
}

int main()
{
    int rc;
    struct mosquitto* mosq;
    mosquitto_lib_init();
    mosq = mosquitto_new("publisher-test", true, NULL);
    rc = mosquitto_connect(mosq, "localhost", 1883, 60);
    if (rc != 0) {
        printf("Clinet could not connect to broker.\n");
        mosquitto_destroy(mosq);
        return -1;
    }
    printf("We are now connected to the broker!\n");

    srand(time(NULL));
    char msg[64];
    int delay = 3 /*seconds*/;
    int timeout = 5 * 60 /*minutes*/ / delay;
    for (int count = 0; count < timeout; ++count) {
        getPost(msg);
        mosquitto_publish(mosq, NULL, "globe-topic", strlen(msg) + 1, msg, 0, false);
        printf("msg #%d sent\n", count + 1);
        Sleep(delay * 1000);
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}