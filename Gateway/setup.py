"""Initialize parameter and object for our system"""

import  sys
from    uart                import *
from    protocol            import *
from    software_timer      import *
from    button              import *
from    Adafruit_IO         import MQTTClient


AIO_FEED_IDs        = ["button1", "button2", "Temp_Sensor", "Light_Sensor", "Humi_Sensor", "ack"]
AIO_USERNAME        = "triethoang"
AIO_KEY             = "aio_umlo071AYkloQUQLosj60P5Cahbx"
BUTTON_IDs          = ["button1", "button2"]

#Sensor ID
TEMP_SENSOR         = 1
LIGHT_SENSOR        = 2
HUMI_SENSOR         = 3

#Timing Period
PUBLISH_PERIOD      = 10000
READ_SERIAL_PERIOD  = 500

def connected(client):
    print("Ket noi thanh cong ...")
    for topic in AIO_FEED_IDs:
        client.subscribe(topic)

def subscribe(client , userdata , mid , granted_qos):
    print("Subscribe thanh cong ...")

def disconnected(client):
    print("Ngat ket noi ...")
    sys.exit (1)

def message(client , feed_id , payload):
    #print("Nhan du lieu: " + payload + " , feed id: " + feed_id)

    if feed_id in BUTTON_IDs:
        print("button event")
        BUTTONs[feed_id].writeSerial(payload)

    elif feed_id == "ack":
        splitACK = payload.split(":")
        BUTTONs[splitACK[0]].stop_ack()
        
    else:
        sensor_ARQ.sendSuccess(feed_id, payload)
        sensor_ARQ.stopSending()

client = MQTTClient(AIO_USERNAME , AIO_KEY)
client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message
client.on_subscribe = subscribe
client.connect()
client.loop_background()

#Initialize ARQ object
sensor_ARQ          = stop_and_wait_ARQ(client, setTimer1, extern.timer1_flag)
BUTTONs             = {}
BUTTONs["button1"]  = Button(client, "button1", setTimer2, extern.timer2_flag)
BUTTONs["button2"]  = Button(client, "button2", setTimer3, extern.timer3_flag)