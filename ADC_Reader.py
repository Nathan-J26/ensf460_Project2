import serial
import time
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

PORT = 'COM3' # UART port Change if needed
BAUD = 9600 
DURATION = 60 # seconds to record data
SAMPLE_RATE = 0.001

def clean_entry(s):
    return "".join(c for c in s if c.isdigit() or c == ',')

print(f"Attempting to connect to {PORT}...")
try:
    ser = serial.Serial(PORT, BAUD, timeout=0)
except serial.serialutil.SerialException as e:
    print(f"Could not connect to {PORT}")
    print(f"REASON: {e}")
    print(f"Make sure that {PORT} is not in use by another program")
    exit(1)
print(f"Connected to {PORT} successfully.")

string = ""
ADCvalues = []
LEDvalues = []
timeValues = []

time.sleep(0.1)

print("Press PB3 to begin sampling\r", end='')
while(1): # waits until PB3 is pressed to begin sampling ADC
    raw = ser.readline().decode('utf-8', errors='ignore').strip()
    clean = "".join([c for c in raw if c.isdigit() or c == "," or c == "-"])

    if "," in clean: # if line contains a comma, PB3 was pressed and data transmission has started.
        break # break to begin sampling
        
    time.sleep(0.1)

print(f"\nCollecting {DURATION} seconds of data from {PORT}...")
for i in range(DURATION * int((1/SAMPLE_RATE))): # 1ms increments
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    string += line


    time.sleep(SAMPLE_RATE)


readings = string.split(";")
readings.pop()

for entry in readings:
    entry = clean_entry(entry)

    if not entry or "," not in entry:
        continue  # skip corrupted chunks


    parts = entry.split(",")

    if len(parts) != 2: # skip incomplete parts
        continue

    adc_str, led_str = parts

    try:
        adc_val = int(adc_str)
        led_val = int(led_str)

    except ValueError:
        continue  # skip totally corrupted items

    ADCvalues.append(adc_val)
    LEDvalues.append(led_val)


timeValues = np.linspace(0, DURATION, len(ADCvalues))
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

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8), sharex=True)

# PLOT 1: ADC VS Time
ax1.plot(timeValues, ADCvalues)
ax1.set_title("ADC Reading VS. Time")
ax1.set_xlabel("Time (s)")
ax1.set_ylabel("ADC Reading (%)")
ax1.grid(True)

# PLOT 2: LED Intensity VS Time
ax2.plot(timeValues, LEDvalues)
ax2.set_title("LED Intensity VS. Time")
ax2.set_xlabel("Time (s)")
ax2.set_ylabel("LED Intensity (%)")
ax2.grid(True)

plt.tight_layout()
plt.show()