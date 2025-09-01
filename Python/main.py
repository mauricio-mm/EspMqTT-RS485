import json
from PyQt5 import QtWidgets, uic, QtCore
from paho.mqtt import client as mqtt_client

app = QtWidgets.QApplication([])
janela = uic.loadUi("interface.ui")  # Certifique-se que o arquivo existe

client_id   = "py_iot"
broker      = 'broker.emqx.io'
port        = 1883
topic_sensor = "lab318/topic_sensor"

def timer():
    timer = QtCore.QTimer()
    timer.start(1000)

def sensor_message(message):
    try:
        payload = message.payload.decode()
        data = json.loads(payload)
        high = data.get("high", None)
        low = data.get("low", None)

        if high is not None and low is not None:
            print(f"High: {high}, Low: {low}")
            janela.lcdNumber.display(high)
            janela.lcdNumber_2.display(low)
        else:
            print("Dados incompletos no payload MQTT")

    except json.JSONDecodeError as e:
        print(f"Erro ao decodificar JSON: {e}")
    except Exception as e:
        print(f"Erro ao processar mensagem: {e}")

def on_message(client, userdata, message):
    print(f"Mensagem recebida de {message.topic}: {message.payload.decode()}")
    if message.topic == topic_sensor:
        sensor_message(message)

def connect_mqtt(client_id, broker, port):
    client = mqtt_client.Client(client_id=client_id)
    client.on_message = on_message
    client.connect(broker, port)
    print("Cliente conectado ao Broker MQTT")
    return client

def subscribe(client, topic):
    client.subscribe(topic)

def run():
    client = connect_mqtt(client_id, broker, port)
    subscribe(client, topic_sensor)
    client.loop_start()

    timer()
    janela.show()
    app.exec_()

    client.loop_stop()

if __name__ == '__main__':
    run()
