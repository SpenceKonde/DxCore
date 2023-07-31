"""
Serial driver for UPDI stack
"""
import time
from logging import getLogger
import serial
from serial.serialutil import SerialException

from . import constants


class UpdiPhysical:
    """
    PDI physical driver using a given serial port at a given baud
    """

    def __init__(self, port, baud=115200):
        """
        Initialise the serial port
        """
        self.logger = getLogger(__name__)

        # Inter-byte delay
        self.ibdly = 0.0001
        self.port = port
        self.baud = baud
        self.ser = None

        self.initialise_serial(self.port, self.baud)

        # send an initial break as handshake
        self.send([constants.UPDI_BREAK])

    def change_baud(self, newbaud):
        self.logger.info("Switching to '%d' baud", newbaud)
        self.ser.baudrate = newbaud

    def initialise_serial(self, port, baud):
        """
        Standard serial port initialisation
        :param port: serial port to use
        :param baud: baud rate
        """
        self.logger.info("Opening port '%s' at '%d' baud", port, baud)
        try:
            self.ser = serial.Serial(None, baud, parity=serial.PARITY_EVEN, timeout=1, stopbits=serial.STOPBITS_TWO)
            self.ser.port = port
            self.ser.dtr = False
            self.ser.rts = False
            self.ser.open()
        except SerialException:
            self.logger.error("Unable to open serial port '%s'", port)
            raise

    def _loginfo(self, msg, data):
        if data and isinstance(data[0], str):
            i_data = [ord(x) for x in data]
        else:
            i_data = data
        data_str = "[" + ", ".join([hex(x) for x in i_data]) + "]"
        self.logger.debug("%s : %s", msg, data_str)

    def send_double_break(self):
        """
        Sends a double break to reset the UPDI port

        BREAK is actually a condition which involves a serial line being held low for longer than it's ordinary character (including framing and parity bits).
        In this case that's 12 bits. But we want twice that because the chip and programmer could be maximally out of sync, and the chip may be running at a way-wrong baud rate, etc
        Documentation says it could potentially take as long as 25 ms to guarantee a break - corresponding to twice the length of a normal break (hence the official, yet rather unclear and strangly
        unprofessional "double break" name), To me "double break" implies that two breaks are generated in succcession. That is not what is described in the datasheet, the datasheet describes a
        double break as simply a break condition lasting double the length of a normal break (idea here is that even if the chip has somehow gotten the idea that you're talking to it at )
        """

        self.logger.info("extra-long break requested. Close serial port, reopen @ 300, send 0x00, receive the 0x00, and then proceed. ")

        # Re-init at a lower baud
        # At 300 baud, the break character will pull the line low for 10 bit periods. This is 33ms
        # Which is slightly above the recommended 24.6ms
        self.ser.close()
        temporary_serial = serial.Serial(None, 300, parity=serial.PARITY_EVEN, timeout=1, stopbits=serial.STOPBITS_ONE)
        temporary_serial.port = self.port
        temporary_serial.dtr = False
        temporary_serial.rts = False
        temporary_serial.open()

        # Old comment: Send two break characters, with 1 stop bit in between
        # 7/31: No! This is not what we want to do! We want to send ONE very long break character which must not have a stop bit in the middle:
        # See `36.3.1.2.1 BREAK in One-Wire Mode` it must be at keast 24.6 ms long.
        temporary_serial.write([constants.UPDI_BREAK])

        time.sleep(0.1)
        # Wait for the double-length break end
        temporary_serial.read(1)
        # now read and discard the 0x00 that we sent
        # close this temp serial port and reopen at the normal baud rate.
        temporary_serial.close()
        self.logger.info("Double-break sent. Re-initializeing USART to retry.")
        self.initialise_serial(self.port, self.baud)
        # time.sleep(0.1)

    def send(self, command):
        """
            Sends a char array to UPDI with NO inter-byte delay
            Note that the byte will echo back
        """
        self.logger.debug("send %d bytes", len(command))
        self._loginfo("data: ", command)

        self.ser.write(command)
        # it will echo back.
        echo = self.ser.read(len(command))

    def receive(self, size):
        """
        Receives a frame of a known number of chars from UPDI
        :param size: bytes to receive
        """
        response = bytearray()
        retry = 3

        # For each byte
        while size and retry:
            character = self.ser.read()

            # Anything in?
            if character:
                response.append(ord(character))
                size -= 1
            else:
                retry -= 1
        if len(response) == size:
            self._loginfo("Received expected number of bytes", response)
        elif len(response) == 0:
            self.logger.debug("We were supposed to receive {:n} bytes - we got nothing! Check connections.".format(size))
        else:
            self.logger.debug("We were supposed to receive {:n} bytes - we got only {:n}. This is not a good thing.".format(size, len(response)))
        return response

    def sib(self):
        """
        System information block is just a string coming back from a SIB command
        """
        self.send([
            constants.UPDI_PHY_SYNC, constants.UPDI_KEY | constants.UPDI_KEY_SIB | constants.UPDI_SIB_32BYTES])
        return self.ser.readline()

    def __del__(self):
        if self.ser:
            self.logger.info("Closing port '%s'", self.port)
            self.ser.close()
