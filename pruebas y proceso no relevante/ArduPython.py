import serial # import Serial Library
import time
import numpy  # Import numpy
import math
import matplotlib.pyplot as plt #import matplotlib library
from drawnow import *


tempThermistor= []
tempDS=[]
arduinoData = serial.Serial('COM7', '9600') #Creating our serial object named arduinoData
plt.ion() #Tell matplotlib you want interactive mode to plot live data
cnt=0


 
def makeFig(): #Create a function that makes our desired plot
    plt.ylim(12,350)                                #Set y min and max values
    plt.title('My Live Streaming Sensor Data')      #Plot the title
    plt.grid(True)                                  #Turn the grid on
    plt.ylabel('Temp C')                            #Set ylabels
    plt.plot(tempThermistor, 'ro-', label='Degrees C')       #plot the temperature
    plt.legend(loc='upper left')                    #plot the legend
    # plt2=plt.twinx()                                #Create a second y axis
    # plt.ylim(93450,93525)                           #Set limits of second y axis- adjust to readings you are getting
    # plt2.plot(tempDS, 'b^-', label='Degrees (C)') #plot pressure data
    # plt2.set_ylabel('tempDS (Pa)')                    #label second y axis
    # plt2.ticklabel_format(useOffset=False)           #Force matplotlib to NOT autoscale y axis
    # plt2.legend(loc='upper right')                  #plot the legend
    
 
while True: # While loop that loops forever
    while (arduinoData.inWaiting()==0): #Wait here until there is data
        pass #do nothing
    arduinoString = arduinoData.readline() #read the line of text from the serial port    
    # dataArray = arduinoString.split(',')   #Split it into an array called dataArray
    tempAux = float(arduinoString)            #Convert first element to floating number and put in temp
    
    # tempAux = float(dataArray[0])
    Vo = tempAux * 0.00488
    tempT = (1.0/((1.0/4267.0)*math.log(((5.0-Vo)/Vo)*0.047)+(1.0/298.0))) - 273.0
    # tempAuxDS = float(dataArray[1])            #Convert second element to floating number and put in P
    
    tempThermistor.append(tempT)                     #Build our tempF array by appending temp readings
    # tempDS.append(tempAuxDS)                     #Building our pressure array by appending P readings
    
    drawnow(makeFig)                       #Call drawnow to update our live graph
    plt.pause(.000001)                     #Pause Briefly. Important to keep drawnow from crashing
    cnt=cnt+1
    if(cnt>100000):                            #If you have 50 or more points, delete the first one from the array
        #  tempT.pop(0)                       #This allows us to just see the last 50 data points
        tempThermistor.pop(0)                       #This allows us to just see the last 50 data points
        # tempDS.pop(0)
    
    print tempT
    # print tempDS
    # print arduinoString
    # print arduinoString_v
    #print dataArray[0]
    #print dataArray[1]