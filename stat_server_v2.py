#!/usr/bin/env python3
from serial import Serial
import serial
import time
import psutil

portn = "/dev/ttyUSB1"
baudr = 19200

debug = False

sendDelay = 2.5

gpuname = 'amdgpu'

gpu = 'amd'

class serialMonitor():
    def __init__(self):
        """ Class Constructor, initialises serial connection and periodically polls sensors. """
        # Establish serial connection
        try:
            self.ser = serial.Serial(
                port=portn ,\
                baudrate=baudr,\
                parity=serial.PARITY_NONE,\
                stopbits=serial.STOPBITS_ONE,\
                bytesize=serial.EIGHTBITS,\
                timeout=0)
            print("Connected to: " + self.ser.portstr)
        except IOError:
            print ("Serial error" + "\n Exiting Serial Connection \n")
            quit()
        except OSError:
            print ("Serial error" + "\n Exiting Serial Connection \n")
            quit()

    def __del__(self):
        """ Class Destructor, closes serial connection """
        self.ser.close()

    def send_display_data(self, name, value):
        """ Wrapper function for transmitting data to the Screen """
        cmd_string = name + " " + str(value) + " \n"
        ret = self.ser.write(cmd_string.encode('utf-8'))
        if (ret <= 0): print("Serial Write Unsuccessful")
        if debug: print(cmd_string)
        try:
            time.sleep(sendDelay)
        except KeyboardInterrupt:
            quit()


if __name__ == '__main__':
    monitor = serialMonitor()
    time.sleep(3)
    # Send static data
    monitor.send_display_data("Num_Cores", psutil.cpu_count())
    monitor.send_display_data("Ram_Amount", int(psutil.virtual_memory()[0]/1024000000))

    # Send changing data
    try:
        while True:
            monitor.send_display_data("GPU_Temp", psutil.sensors_temperatures()[gpuname][0][1])
            monitor.send_display_data("Package_Temp", psutil.sensors_temperatures()['coretemp'][0][1])
            monitor.send_display_data("Cpu_Usage", psutil.cpu_percent())
            monitor.send_display_data("Ram_Usage", psutil.virtual_memory()[2])
    except KeyboardInterrupt:
        quit()




"""
TODO How to poll gpu data:

LINUX:
    amdgpu -> psutil.sensors_temperatures()['amdgpu'][0][1]
    nvidia -> ????
    intel -> prob psutil


WINDOWS:
    https://stackoverflow.com/questions/62617789/get-cpu-and-gpu-temp-using-python-windows

"""