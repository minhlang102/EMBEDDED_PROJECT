"""Implementation of button class"""

from protocol       import *
from uart           import *


class Button(object):
    """Interface for a feed button on Adafruit IO 
    """
    def __init__(self, client, feed_id, setTimer, timer_flag, id = 1):
        """Create instance of button.

        :param client: MQTT client
        :param feed_id: Adafruit.IO feed id for your button.
        :param setTimer: timer function use for ack timeout
        :param timer_flag: timer_flag of setTimer function

        """
        self.client     = client
        self.feed_id    = feed_id
        self.status     = 0
        self.ARQ        = stop_and_wait_ARQ(self.client, setTimer, timer_flag)
        self.serial     = 0
        self.id         = str(id)
        self.ARQ.setReceiver("ack")
        
    def send_ack(self):
        """Send ack back to server"""
        self.ARQ.sendData()

    def writeSerial(self, data):
        """Button sends serial data to MCU when receive signal from server"""
        self.status = data
        msg = "!1:" + self.feed_id + ":" + data + "#"
        print(msg)
        writeSerial(msg)
        self.serial = 1

    def readSerial(self, response):
        """Button reads serial data from MCU"""
        if self.serial == 0:
            return
        if response == (self.id ,self.feed_id, self.status):
            self.start_ack()
            self.serial = 0
        else:
            self.writeSerial(self.status)

    def start_ack(self):
        """Start sending button ack"""
        ack = self.feed_id + ":" + str(self.status)
        self.ARQ.addData(ack)
        self.ARQ.startSending()
    
    def stop_ack(self):
        """Stop sending button ack"""
        self.ARQ.stopSending()
