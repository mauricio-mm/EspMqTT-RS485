import time
from PyQt5.QtCore import *
from PyQt5.QtGui import QPixmap
from PyQt5 import QtWidgets, QtCore
from PyQt5 import uic

def sensor_message(message):    
    try:
        payload = message.payload.decode()
        temp_str, hum_str = payload.split(':')

        temp = float(temp_str)
        hum = float(hum_str)            

        janela.lcdNumber.display(temp)
        janela.lcdNumber_2.display(hum)           

    except Exception as e:
        print(f"Erro ao processar mensagem: {e}")

def publish_led(client):
    global enable
    enable = not enable    
    client.publish(topic_led, b'1' if enable else b'0')    
    
    if not enable:
        # 
    else:
        # 

def timer():
    timer = QtCore.QTimer()    
    timer.start(1000)