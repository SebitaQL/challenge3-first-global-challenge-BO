#-----specifing libraries
import serial   # import Serial Library
import time     # import time library
import numpy    # Import numpy
import math     # importa math library
import matplotlib.pyplot as plt #import matplotlib library
from drawnow import *
import os       #in order to verify if a file is empty
from datetime import datetime   #in order to get the date and the time

# Month abbreviation, day and year
#E.g. dec-12-02,22:12:08
dt = now.strftime("%b-%d-%Y,%H:%M:%S")

#--------crating a file
f = open('data.csv', 'a')     #opens the file just for making sure it exists
f.close()

if os.stat("data.csv").st_size == 0:    #if the file is empty it writes the headers
    f = open('data.csv', 'a')
    f.write("date,time,BPM,SpO2,R,Temp\n")
    #print("date,time,BPM,SpO2,R,Temp")
    f.close()

#--- variables (Serial data format  = heartBeat,oxigen,R,Temp)
#--- plot variables
oxygenPlot = []
heartBeatPlot = []
RPlot = []
TempPlot = []

arduinoData = serial.Serial('COM3', '9600') #Creating our serial object named arduinoData 9600 baudios
#arduinoData.flushInput()
plt.ion() #Tell matplotlib you want interactive mode to plot live data
cnt=0 

#--- functions
def makeFig(): #Create a function that makes our desired plot
    plt.ylim(40,200)                                #Set y min and max values
    plt.title('Pulsioxímetro, temperatura')                           #Plot the title
    plt.grid(True)                                  #Turn the grid on
    plt.ylabel('data') 
    plt.xlabel('beats')                             #Set x and y labels
    plt.plot(heartBeatPlot, 'ro-', label='BPM')     #plot data ('xo-')
    plt.plot(oxygenPlot, 'bo-', label='SpO2')
    plt.legend(loc='upper left')                    #plot the legend

    plt2=plt.twinx()                                #Create a second y axis
    plt2.plot(oxygenPlot, 'go-', label='SpO2')
    plt2.ylim(30,40)                         #Set limits of second y axis- adjust to readings you are getting
    plt2.set_ylabel('Temp (°C)')                #label second y axis
    plt2.ticklabel_format(useOffset=False)        #Force matplotlib to NOT autoscale y axis
    plt2.legend(loc='upper right')                #plot the legend

    # #plt3=plt.twinx()                                #Create a second y axis
    # plt3.plot(RPlot, 'go-', label='R')
    # #plt.ylim(0,3)                         #Set limits of second y axis- adjust to readings you are getting
    # #plt3.set_ylabel('R')                #label second y axis
    # plt3.ticklabel_format(useOffset=False)        #Force matplotlib to NOT autoscale y axis
    # plt3.legend(loc='lower right')                #plot the legend

# main 
while True: # While loop that loops forever
    while (arduinoData.inWaiting()==0): #Wait here until there is data
        pass #do nothing
    
    # format data
    arduinoString = arduinoData.readline() #read the line of text from the serial port
    dataArray = arduinoString.decode().split(',')   #Split it into an array called dataArray oxig 

    #--- save data
    vitalSigns =    {
        'heartBeatData' : str(int(float(dataArray[0]))),
        'oxygenData' : str(int(float(dataArray[1]))),
        'RData' : str(float(dataArray[2])),
        'tempData' : str(float(dataArray[3]))   }

    #--- build independent data
    #auxHeartBeatData.append(auxArduinoFloat)
    heartBeatPlot.append(vitalSigns['heartBeatData'])
    oxygenPlot.append(vitalSigns['oxygenData'])        #Build our aux array by appending data readings
    RPlot.append(vitalSigns['RData'])
    TempPlot.append(vitalSigns['tempData'])
    
    #--- call plot
    drawnow(makeFig)                       #Call drawnow to update our live graph
    plt.pause(.000001)                     #Pause Briefly. Important to keep drawnow from crashing (sec)
    cnt=cnt+1
    if(cnt>100000):                        #If you have 50 or more points, delete the first one from the array
        heartBeatPlot.pop(0)                #This allows us to just see the last 50 data points
        oxygenPlot.pop(0)
        RPlot.pop(0)
        TempPlot.pop(0)

    #print(heartBeatData)
    #print oxigenData
    #print RData
    
    #---save in a csv file in the follow format:
        #date,time,BPM,SpO2,R,Temp
    now = datetime.now()    # datetime object containing current date and time

    # Month abbreviation, day and year
        #E.g. dec-12-02,22:12:08
    dt = now.strftime("%b-%d-%Y,%H:%M:%S")

    f = open('data.csv', 'a')
    f.write(dt + ',' + vitalSigns['heartBeatData'] + ','+ vitalSigns['oxygenData'] + ','+ vitalSigns['RData'] + ','+ vitalSigns['tempData'] + '\n')
    f.close()       #opens, writes and closes the file

    # print(dt, end=',')
    # for i in vitalSigns.values():
    #     if i == str(float(dataArray[3])):
    #         print(i)
    #     else:
    #         print(i, end=',')

 