#!/usr/bin/env python3
from serial import Serial
import serial
import time

portn = "/dev/ttyUSB1"
baudr = 19200

debug = False

""" Establish serial connection """
try:
    ##Serial connection to the Arduino
    ser = serial.Serial(
        port=portn ,\
        baudrate=baudr,\
        parity=serial.PARITY_NONE,\
        stopbits=serial.STOPBITS_ONE,\
        bytesize=serial.EIGHTBITS,\
        timeout=0)
    print("Connected to: " + ser.portstr)
except IOError:
    print ("Serial error" + "\n Exiting Serial Connection \n")
    quit()
except OSError:
    print ("Serial error" + "\n Exiting Serial Connection \n")
    quit()

# wait for initialisation to be done
time.sleep(2)

""" Wrapper function for transmitting data to the Screen """
def send_display_data(name, value):
    cmd_string = name + " " + str(value) + " \n"
    ret = ser.write(cmd_string.encode('utf-8'))
    if (ret <= 0): print("Serial Write Unsuccessful")
    if debug: print(cmd_string)
    time.sleep(2.5)

import json
import zmq

""" Establish ZeroMQ Connection """
context = zmq.Context()

subscriber = context.socket(zmq.SUB)
subscriber.setsockopt( zmq.LINGER, 0 )
subscriber.setsockopt_string(zmq.SUBSCRIBE, 'G')
subscriber.connect("tcp://127.0.0.1:5678")

""" Send data of static system info """
while True:
    _, plugin, data_raw = subscriber.recv_multipart()
    data = json.loads(data_raw)
    if plugin == b'load':
        # num of cpu cores
        send_display_data("Num_Cores", data['cpucore'])
        break

while True:
    _, plugin, data_raw = subscriber.recv_multipart()
    data = json.loads(data_raw)
    if plugin == b'mem':
        # ram total
        send_display_data("Ram_Amount", int(round(data['total']*1.25/1000000000, 0)))
        break

while True:
    _, plugin, data_raw = subscriber.recv_multipart()
    data = json.loads(data_raw)
    if plugin == b'system':
        # system name
        send_display_data("Hostname", data['hostname'])
        send_display_data("OS_Version", data['os_version'])
        break


""" Poll changing data and send it to the display """
try:
    while True:
        # load new dataset
        _, plugin, data_raw = subscriber.recv_multipart()
        data = json.loads(data_raw)
        # get desired data from relevant keys
        if plugin == b'sensors':
            # cpu package temp
            send_display_data("Package_Temp", data['Package id 0.value'])
            # cpu edge temp
            send_display_data("Edge_Temp", data['edge.value'])
            # gpu package temp, reported wrong in glances for amdgpu atm
            #try:
            #    send_display_data("GPU_Temp", data['amdgpu 1.value'])
            #except KeyError:
            #    pass
            # gpu package temp nvidia, need nvidia gpu to test
            #try:
            #    send_display_data("GPU_Temp", data['nvidia'])
            #except KeyError:
            #    pass
        elif plugin == b'cpu':
            # cpu usage
            send_display_data("Cpu_Usage", int(round(data['total'], 0)))
        elif plugin == b'mem':
            # ram usage
            send_display_data("Ram_Usage", int(round(data['used']/data['total']*100, 0)))
            # gpu usage TODO
except KeyboardInterrupt:
    pass

# Destructor
subscriber.close()
context.term()
ser.close()


"""
# load new set of data
    _, plugin, data_raw = subscriber.recv_multipart()
    data = json.loads(data_raw)
    plugin
    print('{} => {}'.format(plugin, data))



# print possible sensors:
for k in data.keys():
    print(k)



# install
pip install glances, zmq, py3nvml, pyserial
"""