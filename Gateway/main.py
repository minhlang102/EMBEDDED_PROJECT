from    setup               import *
from    threading           import Thread, Event
#sys.path.append( '.' )
#from    simpleAl.simpleAl   import *

sensor_type         = "INIT"
def sensor_fsm():
    """FSM for sending sensor data to server"""
    global sensor_type

    if sensor_type == "INIT":
        setTimer0(PUBLISH_PERIOD)
        sensor_type = HUMI_SENSOR

    elif extern.timer0_flag:
        if sensor_type == TEMP_SENSOR:
            sensor_type = LIGHT_SENSOR
            topic       = "Light_Sensor"
            sensor_data = extern.light

        elif sensor_type == LIGHT_SENSOR:
            sensor_type = HUMI_SENSOR
            topic       = "Humi_Sensor"
            sensor_data = extern.humi

        elif sensor_type == HUMI_SENSOR:
            sensor_type = TEMP_SENSOR
            topic       = "Temp_Sensor"
            sensor_data = extern.temp
        
        sensor_ARQ.setReceiver(topic)
        sensor_ARQ.addData(sensor_data)
        sensor_ARQ.startSending()
        setTimer0(PUBLISH_PERIOD)

event = Event()
def readSerialBackground(event):
    """Loop background for reading serial data"""
    while not event.is_set():
        readSerial()
        if (extern.serial_flag):
            client.publish("error", "Can not connect to MCU")
            event.wait(3)
        pass

def nonBlockingInput(event):
    """Reading input from console without blocking"""
    while not event.is_set():
        try:
            ch = input()
        except:
            return
        writeSerial(ch)

def publishData(event):
    """Loop background for publishing data to Adafruit"""
    while not event.is_set():
        sensor_fsm()
        sensor_ARQ.sendData()

def button_processing(event):
    """Loop background for button processing"""
    while not event.is_set():
        for button in BUTTON_IDs:
            BUTTONs[button].readSerial(extern.buttonResponse)
            BUTTONs[button].send_ack()
            event.wait(0.5)


connectSerial()
Thread(target=readSerialBackground, args=(event,)).start()
Thread(target=publishData, args=(event,)).start()
Thread(target=nonBlockingInput, args=(event,)).start()
Thread(target=button_processing, args=(event,)).start()

try:
    while True:
        timer_run()
        event.wait(1)
        pass
        #image_detector()
        
except KeyboardInterrupt:
    event.set()
    sys.exit()
