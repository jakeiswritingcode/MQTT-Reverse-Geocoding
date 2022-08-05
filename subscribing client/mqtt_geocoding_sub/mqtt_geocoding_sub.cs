 // mqtt_geocoding_sub.cs
// by Jake Charles Osborne III



using System;
using System.Text;
using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Client.Options;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;



namespace mqtt_geocoding_sub
{
    class Subscriber
    {
        // HttpClient is instantiated once per application, rather than per-use.
        static readonly HttpClient httpClient = new HttpClient();

        static async Task Main(string[] args)
        {
            var factory = new MqttFactory();
            IMqttClient client = factory.CreateMqttClient();
            var options = new MqttClientOptionsBuilder()
                    .WithClientId(Guid.NewGuid().ToString())
                    .WithTcpServer("localhost", 1883)
                    .WithCleanSession()
                    .Build();

            client.UseConnectedHandler(async e =>
            {
                Console.WriteLine("Conected to the broker successfully.\n");

                var topicFilter = new MqttTopicFilterBuilder()
                    .WithTopic("globe-topic")
                    .Build();
                await client.SubscribeAsync(topicFilter);
            });

            client.UseDisconnectedHandler(e =>
            {
                Console.WriteLine("Disconnected from the broker successfully.\n");
            });

            client.UseApplicationMessageReceivedHandler(async e =>
            {
                // parse MQTT message

                JObject mqttJson = JObject.Parse(Encoding.UTF8.GetString(e.ApplicationMessage.Payload));
                string lat = mqttJson["latitude"].ToString();
                string lon = mqttJson["longitude"].ToString();

                // construct API request string

                string bingMapsKey = System.IO.File.ReadAllText("bing-maps-key.txt");
                string apiRequest = "http://dev.virtualearth.net/REST/v1/Locations/" + lat + "," + lon + "?o=json&key=" + bingMapsKey;

                // get API response from Bing Maps

                string responseBody = "";
                try
                {
                    HttpResponseMessage response = await httpClient.GetAsync(apiRequest);
                    response.EnsureSuccessStatusCode();
                    responseBody = await response.Content.ReadAsStringAsync();
                }
                catch (HttpRequestException exc)
                {
                    Console.WriteLine("\nException Caught!");
                    Console.WriteLine("Message :{0} ", exc.Message);
                }

                // parse API response

                JObject apiJson = JObject.Parse(responseBody);
                string responseCount = apiJson["resourceSets"][0]["estimatedTotal"].ToString();
                string locationData = "\"No Geolocation Data Available\"";
                if (responseCount == "1")
                {
                    locationData = apiJson["resourceSets"][0]["resources"][0]["name"].ToString();
                }

                // print data to console window
                Console.WriteLine("Latitude: " + lat);
                Console.WriteLine("Longitude: " + lon);
                Console.WriteLine(locationData + "\n");
            });

            await client.ConnectAsync(options);

            Console.ReadLine();

            await client.DisconnectAsync();
        }
    }
}