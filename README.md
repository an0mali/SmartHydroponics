# SmartHydroponics
Arduino project utilizing barometric pressure sensors to create a "smart" hydroponic airstone pump that easily adds smart features to other dumb containers. This includes plant health monitoring and detailed data logging. Please offer me a job if you use this code

## Current Features include:

OLED display showing runtime, fluid level, temperature, and days left until refill.
Fluid level monitoring
Quick and easy one-button calibration to adapt to [any?] container
Days remaining until refill calculations based on real-time ensor data and actual plant consumption rate
Daily fluid consumption rate calculations
Plant growth/health data inferred via liquid uptake measurement
Detailed Hourly/Daily data logging to SD card

## Components Used/Required:

2x Arduino Nano
2x BMP180 Barometric Pressure/Temperature sensors
OLED JMDO.96C Display
Airstone Pump and airstone
Arduino microSD card module

## Future Releases//TODO Note:
    Better balance of code between both arduino units (second arduino was afterthought addition once I ran out of memory)
    Memory Opt: Maybe convert floats to ints by x100m then /100 to use when accuracy isnt needed and float is stored in global scope?
    Implement software based air pump control
