"""Implementation of communication protocols"""

NO_OF_RESEND    = 3     #maximum number of resending data
ACK_TIMEOUT     = 3000  #time that we wait for ack

#Cases in FSM
SEND            = 100
WAIT            = 200
EXIT            = 400

class stop_and_wait_ARQ(object):
    """Interface for stop and wait ARQ protocol.
    """

    def __init__(self, client, setTimer, timer_flag):
        """Create instance of stop and wait ARQ.

        :param client: MQTT client
        :param setTimer: timer function use for ack timeout
        :param timer_flag: timer_flag of setTimer function

        """
        self.client     = client
        self.buffer     = []
        self.resend     = 0
        self.case       = EXIT
        self.setTimer   = setTimer
        self.timer_flag = timer_flag
        self.receiver   = None

    def addData(self, data):
        """Adding data that need to send to buffer"""
        self.buffer.append(data)

    def setReceiver(self, receiver):
        """Set receiver"""
        self.receiver = receiver

    def sendData(self):
        """
        Whatever the data that sender wants to send, 
        he sends the data to the receiver. 
        After sending the data, he stops and waits 
        until he receives the acknowledgment from the receiver.
        In this case, sender is our MQTT client and receiver is the feed on Adafruit server
        """

        if self.case == EXIT:
            self.buffer.clear()
            return
        
        elif self.case == SEND:
            if len(self.buffer) == 0:
                print("Nothing to send")
                return

            print("Sending data...")
            self.client.publish(self.receiver, self.buffer[0])
            self.case = WAIT
            self.setTimer(ACK_TIMEOUT)
        
        elif self.case == WAIT:
            if self.timer_flag:
                if self.resend == NO_OF_RESEND:
                    print("Failed to send data")
                    self.case = EXIT
                    return
                
                #Try to resend
                self.resend += 1
                self.case = SEND

    def sendSuccess(self, receiver, ack_data):
        """
        This function is used to detect if our data is published to server
        """
        if self.case == WAIT:
            if self.receiver == receiver and ack_data == str(self.buffer[0]):
                print("Data has published: ", receiver, ack_data)
                self.case = SEND
                self.resend = 0
                self.buffer.pop(0)

    def startSending(self):
        """Start sending data to receiver"""
        self.case = SEND

    def stopSending(self):
        """Stop sending data to receiver"""
        self.case = EXIT