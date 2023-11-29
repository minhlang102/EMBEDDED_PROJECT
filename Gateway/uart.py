"""Implementation of UART communication"""

import serial.tools.list_ports
import extern
import time

BUTTON_IDs          = ["button1", "button2"]

def getPort():
    ports = serial.tools.list_ports.comports()
    N = len(ports)
    commPort = "None"
    for i in range(0, N):
        port = ports[i]
        strPort = str(port)
        print(strPort)
        if "CP210" in strPort:
            splitPort = strPort.split(" ")
            commPort = (splitPort[0])
            return commPort
    return "COM9"

def connectSerial():
    try:
        global ser
        ser = serial.Serial( port=getPort(), baudrate=9600)
        print("Serial: ",ser)
        extern.serial_flag = 0
    except:
        extern.serial_flag = 1
        print("Can not get port")

def processData(data):
    data = data.replace("!", "")
    data = data.replace("#", "")
    splitData = data.split(":")
    try:
        if splitData[1] == "TEMP":
            extern.temp = splitData[2]
        elif splitData[1] == "HUMI":
            extern.humi = splitData[2]
        elif splitData[1] == "LIGHT":
            extern.light = splitData[2]
        elif splitData[1] in BUTTON_IDs:
            extern.buttonResponse = (splitData[0], splitData[1], splitData[2])
    except:
        pass

mess = ""
def readSerial():
    if getPort() == "None":
        return
    try:
        bytesToRead = ser.inWaiting()
    except:
        print("Read Serial fail")
        connectSerial()
        

    if (bytesToRead > 0):
        global mess
        try:
            mess = mess + ser.read(bytesToRead).decode("UTF-8")
        except:
            print("Read Serial Fail")
            pass
        # if mess: print(mess)
        while ("#" in mess) and ("!" in mess):
            start = mess.find("!")
            end = mess.find("#")
            processData(mess[start:end + 1])
            if (end == len(mess)):
                mess = ""
            else:
                mess = mess[end+1:]

def writeSerial(commandToSend):
    for ch in commandToSend:
        try:
            ser.write(str(ch).encode("UTF-8"))
        except:
            print("Write Serial fail")
            connectSerial()
            pass
        time.sleep(0.2)

