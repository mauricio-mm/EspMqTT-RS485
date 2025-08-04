

def connect_mqtt(client_id, broker, port):
    client = mqtt_client.Client(client_id)
    client.on_message = on_message
    client.connect(broker, port)
    print("Cliente conectado ao Broker MQTT")
    return client

def subscribe(client, topic):
    client.subscribe(topic)   
    
def on_message(client, userdata, message):
    print(f"Mensagem recebida de {message.topic}: {message.payload.decode()}")    
    if message.topic == topic_sensor:
        sensor_message(message)
