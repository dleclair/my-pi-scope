#include <stdio.h>
#include <pigpio.h>

#define ADS1015_ADDRESS 0x48  // Default I2C address for ADS1015
#define CONFIG_REG 0x01       // Configuration register
#define CONVERSION_REG 0x00   // Conversion register

int read_ads1015_sample(int i2c_bus, int device_address) {
    // Initialize the pigpio library
    if (gpioInitialise() < 0) {
        fprintf(stderr, "pigpio initialization failed\n");
        return -1;
    }

    // Open I2C connection
    int handle = i2cOpen(i2c_bus, device_address, 0);
    if (handle < 0) {
        fprintf(stderr, "Failed to open I2C connection\n");
        gpioTerminate();
        return -1;
    }

    // Configure ADS1015 for single-ended input on AIN0 with default settings
    char config_data[3];
    config_data[0] = CONFIG_REG;
    config_data[1] = 0xC1;  // MSB: OS: start, MUX: AINP=AIN0 / AINN=GND, PGA = ±6.144V, MODE = Single
    config_data[2] = 0x83;  // LSB: DR = 1600 SPS, COMP_MODE = traditional, COMP_POL = active low, COMP_LAT = non-latching, COMP_QUE = disable

    if (i2cWriteDevice(handle, config_data, 3) != 0) {
        fprintf(stderr, "Failed to write configuration\n");
        i2cClose(handle);
        gpioTerminate();
        return -1;
    }

    // Wait for the conversion to complete (depends on the data rate)
    gpioDelay(1000); // Delay in microseconds (e.g., 1ms for 1600SPS)

    // Read the conversion result
    char read_data[2];
    if (i2cReadI2CBlockData(handle, CONVERSION_REG, read_data, 2) != 2) {
        fprintf(stderr, "Failed to read conversion result\n");
        i2cClose(handle);
        gpioTerminate();
        return -1;
    }

    // Combine the two bytes into a single 12-bit value
    int16_t raw_adc = ((read_data[0] << 4) | (read_data[1] >> 4));

    // Handle negative values (ADS1015 output is in 2's complement)
    if (raw_adc > 0x07FF) {
        raw_adc |= 0xF000;
    }

    // Close I2C connection
    i2cClose(handle);

    // Terminate pigpio
    gpioTerminate();

    // Return the ADC result
    return raw_adc;
}

int main() {
    int sample = read_ads1015_sample(1, ADS1015_ADDRESS);
    if (sample >= 0) {
        printf("ADC Sample: %d\n", sample);
    } else {
        printf("Error reading ADC sample\n");
    }
    return 0;
}

