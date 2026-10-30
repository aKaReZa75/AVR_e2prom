# E2PROM API Reference

This section provides detailed descriptions of the functions used to interact with the internal EEPROM of AVR microcontrollers (specifically ATMEGA328). These functions are part of a custom library for handling EEPROM read, write, and update operations with support for multiple data types including unsigned integers, signed integers, floats, and byte arrays. Each function is explained with its purpose and usage, accompanied by code examples to demonstrate how they can be implemented in your projects.

> [!NOTE]
> The library and all of its APIs provided below have been developed by myself.
> This library utilizes various macros defined in the `aKaReZa.h` header file, which are designed to simplify bitwise operations and register manipulations.
> Detailed descriptions of these macros can be found at the following link:
> [https://github.com/aKaReZa75/AVR/blob/main/Macros.md](https://github.com/aKaReZa75/AVR/blob/main/Macros.md)

| Setting                  | Value                                  |
| ------------------------ | -------------------------------------- |
| **Microcontroller**      | ATmega328P (compatible with AVR family)|
| **EEPROM Size**          | 1024 bytes (ATmega328P)                |
| **Write Time**           | ~3.3 ms per byte                       |
| **Endurance**            | 100,000 write/erase cycles minimum     |
| **Data Retention**       | 20 years at 85°C, 100 years at 25°C   |
| **Byte Order**           | Little-endian                          |
| **Operation Mode**       | Atomic erase-and-write                 |

---

## **Library Features**

- ✅ **Multi-Type Support**: uint8, uint16, uint32, int8, int16, int32, float, and byte arrays
- ✅ **Wear-Leveling**: Update functions that write only when values change
- ✅ **Atomic Operations**: Hardware-controlled erase-and-write cycle
- ✅ **Block Operations**: Read/write/update multiple bytes efficiently
- ✅ **Format Function**: Erase entire EEPROM to factory state (0xFF)

---

## **Unsigned Integer Write Functions**

### **Write Unsigned 8-bit Value**

```c
void e2prom_uint8_Write(uint16_t _address, uint8_t _Value);
```

* Writes a single unsigned byte to EEPROM.
* Atomic erase-and-write operation using AVR hardware controller.
* Blocks for approximately 3.3 ms during write.

**Parameters:**
- `_address`: EEPROM address (0 to 1023 for ATmega328P)
- `_Value`: 8-bit unsigned value to write (0 to 255)

**Example:**

```c
#include "aKaReZa.h"
#include "e2prom.h"

int main(void) 
{
    e2prom_uint8_Write(0x0000, 42); /**< Write value 42 to address 0 */
    
    while(1)
    {
        /* Your code here */
    }
}
```

---

### **Write Unsigned 16-bit Value**

```c
void e2prom_uint16_Write(uint16_t _address, uint16_t _Value);
```

* Writes a 16-bit unsigned integer to EEPROM (2 consecutive bytes).
* Stores in little-endian format: LSB at `_address`, MSB at `_address+1`.
* Total write time: ~6.6 ms (2 × 3.3 ms).

**Parameters:**
- `_address`: Starting EEPROM address (needs 2 consecutive bytes)
- `_Value`: 16-bit unsigned value to write (0 to 65535)

**Example:**

```c
e2prom_uint16_Write(0x0010, 1234); /**< Write 1234 to address 16-17 */
```

---

### **Write Unsigned 32-bit Value**

```c
void e2prom_uint32_Write(uint16_t _address, uint32_t _Value);
```

* Writes a 32-bit unsigned integer to EEPROM (4 consecutive bytes).
* Stores in little-endian format.
* Total write time: ~13.2 ms (4 × 3.3 ms).

**Parameters:**
- `_address`: Starting EEPROM address (needs 4 consecutive bytes)
- `_Value`: 32-bit unsigned value to write (0 to 4,294,967,295)

**Example:**

```c
e2prom_uint32_Write(0x0020, 1000000UL); /**< Write 1,000,000 to address 32-35 */
```

---

### **Write Float Value**

```c
void e2prom_float_Write(uint16_t _address, float _Value);
```

* Writes an IEEE 754 single-precision floating-point value to EEPROM (4 consecutive bytes).
* Stores in little-endian format.
* Total write time: ~13.2 ms (4 × 3.3 ms).

**Parameters:**
- `_address`: Starting EEPROM address (needs 4 consecutive bytes)
- `_Value`: Float value to write

**Example:**

```c
e2prom_float_Write(0x0100, 3.14159); /**< Write Pi value to address 256-259 */
```

---

### **Write Block of Bytes**

```c
void e2prom_block_Write(uint16_t _address, uint8_t *_Data, uint16_t _length);
```

* Writes an array of bytes to EEPROM sequentially.
* Total write time: `_length × 3.3 ms`.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Data`: Pointer to source data array
- `_length`: Number of bytes to write

**Example:**

```c
uint8_t myData[5] = {10, 20, 30, 40, 50};
e2prom_block_Write(0x0050, myData, 5); /**< Write 5 bytes starting at address 80 */
```

---

## **Unsigned Integer Read Functions**

### **Read Unsigned 8-bit Value**

```c
void e2prom_uint8_Read(uint16_t _adress, uint8_t* _Value);
```

* Reads a single unsigned byte from EEPROM.
* Fast operation (~4 CPU cycles after read enable).

**Parameters:**
- `_adress`: EEPROM address to read from
- `_Value`: Pointer to store the read value

**Example:**

```c
uint8_t readValue;
e2prom_uint8_Read(0x0000, &readValue); /**< Read byte from address 0 */
```

---

### **Read Unsigned 16-bit Value**

```c
void e2prom_uint16_Read(uint16_t _adress, uint16_t* _Value);
```

* Reads a 16-bit unsigned integer from EEPROM (2 consecutive bytes).
* Reconstructs value from little-endian format.

**Parameters:**
- `_adress`: Starting EEPROM address
- `_Value`: Pointer to store the read value

**Example:**

```c
uint16_t readValue;
e2prom_uint16_Read(0x0010, &readValue); /**< Read 16-bit value from address 16-17 */
```

---

### **Read Unsigned 32-bit Value**

```c
void e2prom_uint32_Read(uint16_t _adress, uint32_t* _Value);
```

* Reads a 32-bit unsigned integer from EEPROM (4 consecutive bytes).
* Reconstructs value from little-endian format.

**Parameters:**
- `_adress`: Starting EEPROM address
- `_Value`: Pointer to store the read value

**Example:**

```c
uint32_t readValue;
e2prom_uint32_Read(0x0020, &readValue); /**< Read 32-bit value from address 32-35 */
```

---

### **Read Float Value**

```c
void e2prom_float_Read(uint16_t _adress, float* _Value);
```

* Reads an IEEE 754 floating-point value from EEPROM (4 consecutive bytes).
* Reconstructs float from little-endian byte representation.

**Parameters:**
- `_adress`: Starting EEPROM address
- `_Value`: Pointer to store the read value

**Example:**

```c
float readValue;
e2prom_float_Read(0x0100, &readValue); /**< Read float from address 256-259 */
```

---

### **Read Block of Bytes**

```c
void e2prom_block_Read(uint16_t _address, uint8_t *_Data, uint16_t _length);
```

* Reads an array of bytes from EEPROM sequentially.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Data`: Pointer to destination buffer
- `_length`: Number of bytes to read

**Example:**

```c
uint8_t myBuffer[5];
e2prom_block_Read(0x0050, myBuffer, 5); /**< Read 5 bytes starting from address 80 */
```

---

## **Unsigned Integer Update Functions (Wear-Leveling)**

### **Update Unsigned 8-bit Value**

```c
void e2prom_uint8_Update(uint16_t _address, uint8_t _Value);
```

* Updates an 8-bit value only if it differs from the current stored value.
* Extends EEPROM lifespan by avoiding unnecessary write cycles.
* Reads current value first, then writes only if different.

**Parameters:**
- `_address`: EEPROM address
- `_Value`: New value to write

**Example:**

```c
e2prom_uint8_Update(0x0000, 99); /**< Write only if current value is not 99 */
```

---

### **Update Unsigned 16-bit Value**

```c
void e2prom_uint16_Update(uint16_t _address, uint16_t _Value);
```

* Updates a 16-bit value with wear-leveling optimization.
* Saves up to 2 write cycles (~6.6 ms) if value is unchanged.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: New value to write

**Example:**

```c
e2prom_uint16_Update(0x0010, 5678); /**< Update only if different from stored value */
```

---

### **Update Unsigned 32-bit Value**

```c
void e2prom_uint32_Update(uint16_t _address, uint32_t _Value);
```

* Updates a 32-bit value with wear-leveling optimization.
* Saves up to 4 write cycles (~13.2 ms) if value is unchanged.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: New value to write

**Example:**

```c
e2prom_uint32_Update(0x0020, 2000000UL); /**< Update only if different */
```

---

### **Update Float Value**

```c
void e2prom_float_Update(uint16_t _address, float _Value);
```

* Updates a float value with epsilon comparison for equality.
* Uses tolerance of 0.0000001 for float comparison.
* Writes only if difference is significant.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: New float value to write

**Example:**

```c
e2prom_float_Update(0x0100, 2.71828); /**< Update only if significantly different */
```

---

### **Update Block of Bytes**

```c
void e2prom_block_Update(uint16_t _address, uint8_t *_Data, uint16_t _length);
```

* Updates multiple bytes, writing only those that have changed.
* Most efficient for partial block updates.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Data`: Pointer to new data array
- `_length`: Number of bytes to update

**Example:**

```c
uint8_t newData[5] = {11, 22, 33, 44, 55};
e2prom_block_Update(0x0050, newData, 5); /**< Update only changed bytes */
```

---

## **Signed Integer Write Functions**

### **Write Signed 8-bit Value**

```c
void e2prom_int8_Write(uint16_t _address, int8_t _Value);
```

* Writes a signed 8-bit integer to EEPROM.
* Range: -128 to 127.

**Parameters:**
- `_address`: EEPROM address
- `_Value`: Signed 8-bit value to write

**Example:**

```c
e2prom_int8_Write(0x0200, -50); /**< Write -50 to address 512 */
```

---

### **Write Signed 16-bit Value**

```c
void e2prom_int16_Write(uint16_t _address, int16_t _Value);
```

* Writes a signed 16-bit integer to EEPROM (2 consecutive bytes).
* Range: -32768 to 32767.
* Stores in little-endian format.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: Signed 16-bit value to write

**Example:**

```c
e2prom_int16_Write(0x0210, -1234); /**< Write -1234 to address 528-529 */
```

---

### **Write Signed 32-bit Value**

```c
void e2prom_int32_Write(uint16_t _address, int32_t _Value);
```

* Writes a signed 32-bit integer to EEPROM (4 consecutive bytes).
* Range: -2,147,483,648 to 2,147,483,647.
* Stores in little-endian format.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: Signed 32-bit value to write

**Example:**

```c
e2prom_int32_Write(0x0220, -1000000L); /**< Write -1,000,000 to address 544-547 */
```

---

## **Signed Integer Read Functions**

### **Read Signed 8-bit Value**

```c
void e2prom_int8_Read(uint16_t _address, int8_t* _Value);
```

* Reads a signed 8-bit integer from EEPROM.

**Parameters:**
- `_address`: EEPROM address
- `_Value`: Pointer to store the read value

**Example:**

```c
int8_t temperature;
e2prom_int8_Read(0x0200, &temperature); /**< Read signed byte from address 512 */
```

---

### **Read Signed 16-bit Value**

```c
void e2prom_int16_Read(uint16_t _address, int16_t* _Value);
```

* Reads a signed 16-bit integer from EEPROM (2 consecutive bytes).

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: Pointer to store the read value

**Example:**

```c
int16_t offset;
e2prom_int16_Read(0x0210, &offset); /**< Read signed 16-bit value from address 528-529 */
```

---

### **Read Signed 32-bit Value**

```c
void e2prom_int32_Read(uint16_t _address, int32_t* _Value);
```

* Reads a signed 32-bit integer from EEPROM (4 consecutive bytes).

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: Pointer to store the read value

**Example:**

```c
int32_t largeValue;
e2prom_int32_Read(0x0220, &largeValue); /**< Read signed 32-bit value from address 544-547 */
```

---

## **Signed Integer Update Functions (Wear-Leveling)**

### **Update Signed 8-bit Value**

```c
void e2prom_int8_Update(uint16_t _address, int8_t _Value);
```

* Updates a signed 8-bit value with wear-leveling optimization.

**Parameters:**
- `_address`: EEPROM address
- `_Value`: New signed value to write

**Example:**

```c
e2prom_int8_Update(0x0200, -25); /**< Update only if different from stored value */
```

---

### **Update Signed 16-bit Value**

```c
void e2prom_int16_Update(uint16_t _address, int16_t _Value);
```

* Updates a signed 16-bit value with wear-leveling optimization.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: New signed value to write

**Example:**

```c
e2prom_int16_Update(0x0210, -5000); /**< Update only if different */
```

---

### **Update Signed 32-bit Value**

```c
void e2prom_int32_Update(uint16_t _address, int32_t _Value);
```

* Updates a signed 32-bit value with wear-leveling optimization.
* Saves up to 4 write cycles (~13.2 ms) if value is unchanged.

**Parameters:**
- `_address`: Starting EEPROM address
- `_Value`: New signed value to write

**Example:**

```c
e2prom_int32_Update(0x0220, -500000L); /**< Update only if different */
```

---

## **Utility Functions**

### **Format EEPROM**

```c
void e2prom_Format(void);
```

* Erases entire EEPROM by writing 0xFF to all addresses.
* Resets EEPROM to factory blank state.
* **Warning**: This operation takes several seconds and cannot be interrupted.

**Time Required:**
- ATmega328P (1024 bytes): ~3.4 seconds
- ATmega2560 (4096 bytes): ~13.5 seconds
- ATmega168 (512 bytes): ~1.7 seconds

**Example:**

```c
e2prom_Format(); /**< Erase entire EEPROM - takes ~3.4 seconds for ATmega328P */
```

---

## **Summary**

| API Function             | Description                                                  |
| ------------------------ | ------------------------------------------------------------ |
| `e2prom_uint8_Write`     | Write unsigned 8-bit value to EEPROM                         |
| `e2prom_uint16_Write`    | Write unsigned 16-bit value (2 bytes, little-endian)         |
| `e2prom_uint32_Write`    | Write unsigned 32-bit value (4 bytes, little-endian)         |
| `e2prom_float_Write`     | Write IEEE 754 float value (4 bytes)                         |
| `e2prom_block_Write`     | Write array of bytes sequentially                            |
| `e2prom_uint8_Read`      | Read unsigned 8-bit value from EEPROM                        |
| `e2prom_uint16_Read`     | Read unsigned 16-bit value (2 bytes)                         |
| `e2prom_uint32_Read`     | Read unsigned 32-bit value (4 bytes)                         |
| `e2prom_float_Read`      | Read IEEE 754 float value (4 bytes)                          |
| `e2prom_block_Read`      | Read array of bytes sequentially                             |
| `e2prom_uint8_Update`     | Update 8-bit value (write only if different)                 |
| `e2prom_uint16_Update`    | Update 16-bit value with wear-leveling                       |
| `e2prom_uint32_Update`    | Update 32-bit value with wear-leveling                       |
| `e2prom_float_Update`     | Update float value with epsilon comparison                   |
| `e2prom_block_Update`     | Update block of bytes (byte-by-byte comparison)              |
| `e2prom_int8_Write`      | Write signed 8-bit value to EEPROM                           |
| `e2prom_int16_Write`     | Write signed 16-bit value (2 bytes, little-endian)           |
| `e2prom_int32_Write`     | Write signed 32-bit value (4 bytes, little-endian)           |
| `e2prom_int8_Read`       | Read signed 8-bit value from EEPROM                          |
| `e2prom_int16_Read`      | Read signed 16-bit value (2 bytes)                           |
| `e2prom_int32_Read`      | Read signed 32-bit value (4 bytes)                           |
| `e2prom_int8_Update`      | Update signed 8-bit value with wear-leveling                 |
| `e2prom_int16_Update`     | Update signed 16-bit value with wear-leveling                |
| `e2prom_int32_Update`     | Update signed 32-bit value with wear-leveling                |
| `e2prom_Format`          | Erase entire EEPROM (write 0xFF to all addresses)            |

---

## **Complete Example**

```c
#include "aKaReZa.h"
#include "e2prom.h"

int main(void) 
{
    /**< Configuration data structure */
    typedef struct {
        uint16_t deviceID;
        float calibrationValue;
        int16_t offset;
        uint8_t mode;
    } Config_t;
    
    Config_t myConfig = {
        .deviceID = 1234,
        .calibrationValue = 1.5,
        .offset = -100,
        .mode = 3
    };
    
    /**< Write configuration to EEPROM */
    e2prom_uint16_Write(0x0000, myConfig.deviceID);
    e2prom_float_Write(0x0002, myConfig.calibrationValue);
    e2prom_int16_Write(0x0006, myConfig.offset);
    e2prom_uint8_Write(0x0008, myConfig.mode);
    
    while(1)
    {
        /**< Read configuration from EEPROM */
        Config_t readConfig;
        e2prom_uint16_Read(0x0000, &readConfig.deviceID);
        e2prom_float_Read(0x0002, &readConfig.calibrationValue);
        e2prom_int16_Read(0x0006, &readConfig.offset);
        e2prom_uint8_Read(0x0008, &readConfig.mode);
        
        /**< Update only if value changed (wear-leveling) */
        if (readConfig.mode != 5)
        {
            e2prom_uint8_Update(0x0008, 5); /**< Write only if different */
        }
        
        /**< Wait some time before next operation */
        _delay_ms(1000);
    }
}
```

---

> [!IMPORTANT]
> **Best Practices:**
> - Always use Update functions (`e2prom_xxx_Update`) instead of Write functions when possible to extend EEPROM lifespan.
> - EEPROM has a limited write endurance of 100,000 cycles per location.
> - Multi-byte values are stored in little-endian format (LSB first).
> - Each write operation takes ~3.3 ms and blocks execution.
> - Power loss during write may corrupt only the byte being written.
> - Use `e2prom_Format()` carefully as it takes several seconds to complete.

---

> [!TIP]
> **Memory Organization Tips:**
> - Reserve address 0x0000-0x000F for critical system configuration
> - Use address 0x0010-0x00FF for user settings
> - Reserve address 0x0100+ for runtime data storage
> - Keep a memory map document for your EEPROM layout
> - Consider using checksums for critical data validation

---

# 🌟 Support Me
If you found this repository useful:
- Subscribe to my [YouTube Channel](https://www.youtube.com/@aKaReZa75).
- Share this repository with others.
- Give this repository and my other repositories a star.
- Follow my [GitHub account](https://github.com/aKaReZa75).

# ✉️ Contact Me
Feel free to reach out to me through any of the following platforms:
- 📧 [Email: aKaReZa75@gmail.com](mailto:aKaReZa75@gmail.com)
- 🎥 [YouTube: @aKaReZa75](https://www.youtube.com/@aKaReZa75)
- 💼 [LinkedIn: @akareza75](https://www.linkedin.com/in/akareza75)
