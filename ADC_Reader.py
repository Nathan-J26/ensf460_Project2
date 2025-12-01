import serial
import time
import matplotlib.pyplot as plt
import pandas as pd

PORT = 'COM3' # UART port Change if needed
BAUD = 4800 
DURATION = 10 # seconds to record data

print(f"Attempting to connect to {PORT}...")
try:
    ser = serial.Serial(PORT, BAUD, timeout=0)
except serial.serialutil.SerialException as e:
    print(f"Could not connect to {PORT}")
    print(f"REASON: {e}")
    print(f"Make sure that {PORT} is not in use by another program")
    exit(1)
print(f"Connected to {PORT} successfully.")

ADCvalues = []
voltageValues = []
timeValues = []

time.sleep(0.1)
while(1): # waits until PB2 is pressed to begin sampling ADC
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    line = "".join([char for char in line if char.isalpha()])
    print("Mode 1: Press PB2 to begin sampling\r", end='')
    if(len(line) == 0): # if no alpha characters are read, that means PB2 was pressed
        break # break to begin sampling
        
    time.sleep(0.1)

print(f"Collecting {DURATION} seconds of data from {PORT}...")
for i in range(DURATION * 10): # 10 seconds in 100ms increments
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    line = "".join([char for char in line if char.isdecimal()]) # convert read string to proper format
    print(f"ADC Reading: {line}")
    try:
        adcVal = int(line)
        ADCvalues.append(adcVal)
        voltageValues.append(3.3 * (adcVal / 1023))
        timeValues.append(i/10)
    except ValueError as e:
        pass
    time.sleep(0.1)
    
print(f"Collected {len(ADCvalues)} ADC readings")

print("Closing serial connection...")
ser.close()
print(f"{PORT} closed")

data = {
    "Time (s)": timeValues,
    "ADC buffer": ADCvalues,
    "ADC voltage": voltageValues
}

df = pd.DataFrame(data)
df.to_excel("output.xlsx", index=False)

# PLOT 1: ADC Buffer VS Time
plt.plot(timeValues, ADCvalues)
plt.title("ADC Buffer Readings VS. Time")
plt.xlabel("Time (s)")
plt.xticks([i for i in range(0, 11, 1)])
plt.ylabel("ADC Reading")
plt.yticks([i for i in range(0, 1100, 50)])
plt.grid(True)
plt.show()

# PLOT 2: ADC Voltage VS Time
plt.plot(timeValues, voltageValues)
plt.title("ADC Voltage Reading VS. Time")
plt.xlabel("Time (s)")
plt.xticks([i for i in range(0, 11, 1)])
plt.ylabel("ADC Voltage (V)")
plt.yticks([i/10 for i in range(0, 40, 5)])
plt.grid(True)
plt.show()