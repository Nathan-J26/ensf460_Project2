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
LEDvalues = []
timeValues = []

time.sleep(0.1)
while(1): # waits until PB3 is pressed to begin sampling ADC
    line = ser.readline().decode('utf-8', errors='ignore').strip()

    print("Press PB3 to begin sampling\r", end='')
    if "," in line: # if line contains a comma, PB3 was pressed and data transmission has started.
        break # break to begin sampling
        
    time.sleep(0.1)

print(f"\nCollecting {DURATION} seconds of data from {PORT}...")
for i in range(DURATION * 10): # 100ms increments
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    if "," in line:
        try:
            adc_str, led_str = line.split(",")
            adcVal = int(adc_str)
            ledVal = int(led_str)

            ADCvalues.append(adcVal)
            LEDvalues.append(ledVal)
            timeValues.append(i/10)

            print(f"ADC: {adcVal}, LED: {ledVal}")

        except ValueError:
            pass # corrupted UART line, ignore

    time.sleep(0.1)
    
print(f"Collected {len(ADCvalues)} ADC readings")

print("Closing serial connection...")
ser.close()
print(f"{PORT} closed")

data = {
    "Time (s)": timeValues,
    "ADC Readings (%)": ADCvalues,
    "LED Intensity (%)": LEDvalues
}

df = pd.DataFrame(data)
df.to_excel("output.xlsx", index=False)

# PLOT 1: ADC VS Time
plt.plot(timeValues, ADCvalues)
plt.title("ADC Reading VS. Time")
plt.xlabel("Time (s)")
plt.ylabel("ADC Reading (%)")
plt.xlim(-5, 15)
plt.ylim(-5, 105)
plt.grid(True)
plt.show()

# PLOT 2: LED Intensity VS Time
plt.plot(timeValues, LEDvalues)
plt.title("LED Intensity VS. Time")
plt.xlabel("Time (s)")
plt.ylabel("LED Intensity (%)")
plt.xlim(-5, 15)
plt.ylim(-5, 105)
plt.grid(True)
plt.show()