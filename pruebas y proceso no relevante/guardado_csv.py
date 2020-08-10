import serial   # import Serial Library
import os
from datetime import datetime   #in order to get the date and the time

now = datetime.now()    # datetime object containing current date and time

# Month abbreviation, day and year
#E.g. dec-12-02,22:12:08
dt = now.strftime("%b-%d-%Y,%H:%M:%S")

f = open('data.csv', 'a')     #opens the file just for making sure it exists
f.close()

if os.stat("data.csv").st_size == 0:    #if the file is empty it writes the headers
    f = open('data.csv', 'a')
    f.write("date,time,BPM,SpO2,R,Temp\n")
    print("date,time,BPM,SpO2,R,Temp")
    f.close()


arduinoData = serial.Serial('COM3', '9600')

while True: # While loop that loops forever
    while (arduinoData.inWaiting()==0): #Wait here until there is data
        pass #do nothing

    arduinoString = arduinoData.readline() #read the line of text from the serial port
    dataArray = arduinoString.decode().split(',')

    vitalSigns =    {
        'heartBeatData' : str(int(float(dataArray[0]))),
        'oxygenData' : str(int(float(dataArray[1]))),
        'RData' : str(float(dataArray[2])),
        'tempData' : str(float(dataArray[3]))   }
        
    now = datetime.now()    # datetime object containing current date and time

    # Month abbreviation, day and year
    #E.g. dec-12-02,22:12:08
    dt = now.strftime("%b-%d-%Y,%H:%M:%S")

    f = open('data.csv', 'a')
    f.write(dt + ',' + vitalSigns['heartBeatData'] + ','+ vitalSigns['oxygenData'] + ','+ vitalSigns['RData'] + ','+ vitalSigns['tempData'] + '\n')
    f.close()

    print(dt, end=',')
    for i in vitalSigns.values():
        if i == str(float(dataArray[3])):
            print(i)
        else:
            print(i, end=',')