from paho.mqtt import client as mqtt_client
import mqtt
import utils
import interface

app = QtWidgets.QApplication([])
janela = uic.loadUi("interface.ui")

client_id   = "py_iot"
broker      = 'broker.emqx.io'
port        = 1883

topic_sensor   = "lab318/topic_sensor"

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
