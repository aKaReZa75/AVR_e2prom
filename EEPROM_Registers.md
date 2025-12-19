# **EEPROM Registers and Control Operations – ATmega328**

This document explains how to use the **EEPROM control registers** on the ATmega328 microcontroller to perform read, write, and erase operations on the internal non-volatile memory. It covers the register structure, control sequences, timing specifications, and best practices for reliable EEPROM operations.

---

## **Overview**

The ATmega328 microcontroller includes **1024 bytes** (1 KB) of internal EEPROM for non-volatile data storage. EEPROM operations are controlled through three dedicated registers:

- **EEAR** – EEPROM Address Register
- **EEDR** – EEPROM Data Register  
- **EECR** – EEPROM Control Register

---

## **EEPROM Specifications**

| Parameter              | Value                                  |
|------------------------|----------------------------------------|
| **EEPROM Size**        | 1024 bytes (ATmega328P)                |
| **Address Range**      | 0x0000 to 0x03FF (0 to 1023)           |
| **Write Time**         | ~3.3 ms (typical at 25°C)              |
| **Erase Time**         | ~1.8 ms (typical at 25°C)              |
| **Erase+Write Time**   | ~3.4 ms (atomic operation)             |
| **Write Endurance**    | 100,000 cycles minimum per location    |
| **Data Retention**     | 20 years at 85°C, 100 years at 25°C    |
| **Operating Voltage**  | Same as MCU (1.8V - 5.5V for ATmega328)|

---

## **EEAR – EEPROM Address Register**

The EEPROM Address Register holds the address of the EEPROM location to be accessed. For ATmega328P with 1024 bytes of EEPROM, only the lower 10 bits are used.

### **EEARH (High Byte)**
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|-----|---|---|---|---|---|---|---|---|
|     | – | – | – | – | – | – | EEAR9 | EEAR8 |

### **EEARL (Low Byte)**
| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|-----|---|---|---|---|---|---|---|---|
|     | EEAR7 | EEAR6 | EEAR5 | EEAR4 | EEAR3 | EEAR2 | EEAR1 | EEAR0 |

**Setting Address Example:**
```c
/* Set EEPROM address to 0x0100 (256) */
EEAR = 0x0100;

/* Or set high and low bytes separately */
EEARH = 0x01;
EEARL = 0x00;
```

> [!NOTE]
> For ATmega328P, valid addresses are 0x0000 to 0x03FF (0 to 1023). Addresses beyond this range are undefined.

---

## **EEDR – EEPROM Data Register**

The EEPROM Data Register holds the data byte to be written to or read from EEPROM.

| Bit | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
|-----|---|---|---|---|---|---|---|---|
|     | EEDR7 | EEDR6 | EEDR5 | EEDR4 | EEDR3 | EEDR2 | EEDR1 | EEDR0 |

**Data Access Example:**
```c
/* Write data */
EEDR = 0xAB; // Load data byte to write

/* Read data */
uint8_t data = EEDR; // Read data byte after read operation
```

---

## **EECR – EEPROM Control Register**

The EEPROM Control Register manages read, write, and erase operations.

| Bit | Name  | Description                                           |
|-----|-------|-------------------------------------------------------|
| 7-6 | –     | Reserved (always read as zero)                        |
| 5   | EEPM1 | EEPROM Programming Mode bit 1                         |
| 4   | EEPM0 | EEPROM Programming Mode bit 0                         |
| 3   | EERIE | EEPROM Ready Interrupt Enable                         |
| 2   | EEMPE | EEPROM Master Program Enable (write enable)           |
| 1   | EEPE  | EEPROM Program Enable (starts write/erase)            |
| 0   | EERE  | EEPROM Read Enable (starts read)                      |

---

## **EEPROM Programming Modes (EEPM1:EEPM0)**

The programming mode bits determine how EEPROM write operations are performed:

| EEPM1 | EEPM0 | Mode               | Description                                    | Time      |
|-------|-------|--------------------|------------------------------------------------|-----------|
| 0     | 0     | Erase and Write    | Atomic erase then write (default)              | ~3.4 ms   |
| 0     | 1     | Erase Only         | Erase byte to 0xFF                             | ~1.8 ms   |
| 1     | 0     | Write Only         | Write without erase (only bits 1→0)            | ~1.8 ms   |
| 1     | 1     | Reserved           | Do not use                                     | –         |

**Recommended Mode:**
```c
/* Set mode 00 for atomic erase+write (most common) */
bitClear(EECR, EEPM0);
bitClear(EECR, EEPM1);
```

> [!IMPORTANT]
> **Mode 00 (Erase and Write)** is the safest and most commonly used mode. It ensures data integrity by automatically erasing before writing.

---

## **EEPROM Write Operation Sequence**

Writing to EEPROM requires a specific sequence to prevent accidental writes:

```c
void eeprom_write_byte(uint16_t address, uint8_t data)
{
    /* 1. Wait for completion of previous write */
    while(EECR & (1<<EEPE));
    
    /* 2. Set up address and data registers */
    EEAR = address;
    EEDR = data;
    
    /* 3. Set programming mode (00 = erase+write) */
    EECR &= ~((1<<EEPM1) | (1<<EEPM0));
    
    /* 4. Write logical one to EEMPE (Master Program Enable) */
    EECR |= (1<<EEMPE);
    
    /* 5. Within 4 clock cycles, write EEPE to start operation */
    EECR |= (1<<EEPE);
    
    /* Hardware automatically clears EEPE when write completes */
}
```

### **Critical Timing Requirement:**
The `EEPE` bit must be set **within 4 CPU clock cycles** after setting `EEMPE`. This safety mechanism prevents accidental EEPROM writes.

**At 16 MHz:** 4 cycles = 0.25 µs  
**At 8 MHz:** 4 cycles = 0.5 µs

---

## **EEPROM Read Operation Sequence**

Reading from EEPROM is faster and simpler than writing:

```c
uint8_t eeprom_read_byte(uint16_t address)
{
    /* 1. Wait for completion of previous write */
    while(EECR & (1<<EEPE));
    
    /* 2. Set up address register */
    EEAR = address;
    
    /* 3. Start EEPROM read by writing EERE */
    EECR |= (1<<EERE);
    
    /* 4. Return data from data register */
    return EEDR;
}
```

**Read Timing:**
- Read operation completes in approximately **4 CPU clock cycles**
- Much faster than write operations (~3.3 ms)
- No waiting required after setting EERE

---

## **Atomic Access to EECR**

The EECR register controls hardware operations and must be accessed atomically to prevent corruption:

### **Why Atomic Access Matters:**
- Multiple bits must be set in correct sequence
- Interrupts could modify EECR between operations
- Incorrect sequence can prevent write or cause data corruption

### **Safe Write Sequence:**
```c
/* Disable interrupts during critical section */
cli(); // Disable global interrupts

/* Set EEMPE */
EECR |= (1<<EEMPE);

/* Set EEPE within 4 cycles */
EECR |= (1<<EEPE);

sei(); // Re-enable global interrupts
```

---

## **EEPROM Ready Interrupt**

The EEPROM Ready Interrupt (EERIE) can be enabled to notify when a write operation completes:

```c
/* Enable EEPROM Ready Interrupt */
EECR |= (1<<EERIE);
sei(); // Enable global interrupts

/* Interrupt Service Routine */
ISR(EE_READY_vect)
{
    /* Called when EEPROM write completes */
    // Perform post-write actions here
}
```

**Benefits:**
- Non-blocking EEPROM writes
- CPU can perform other tasks during 3.3 ms write time
- Useful in time-critical applications

---

## **Preventing EEPROM Corruption During Power Loss**

EEPROM writes take ~3.3 ms to complete. If power is lost during this time, the byte being written may be corrupted.

### **Protection Strategies:**

1. **Use Update Instead of Write:**
   ```c
   /* Only write if value changed (e2prom library) */
   e2prom_uint8_Upate(address, value);
   ```

2. **Implement Checksums:**
   ```c
   typedef struct {
       uint8_t data;
       uint8_t checksum;
   } eeprom_entry_t;
   
   void write_with_checksum(uint16_t addr, uint8_t data) {
       e2prom_uint8_Write(addr, data);
       e2prom_uint8_Write(addr + 1, ~data); // Inverse as checksum
   }
   ```

3. **Use Duplicate Storage:**
   ```c
   /* Write critical data to two locations */
   e2prom_uint8_Write(0x0000, critical_data);
   e2prom_uint8_Write(0x0200, critical_data);
   ```

4. **Enable Brown-Out Detection (BOD):**
   - Configure BOD fuse to reset MCU before voltage drops too low
   - Prevents writes during unstable power conditions

---

## **EEPROM Write Endurance and Wear-Leveling**

Each EEPROM location is guaranteed for **100,000 write/erase cycles** minimum.

### **Wear-Leveling Techniques:**

**1. Read-Modify-Write Pattern:**
```c
/* Bad: Always writes */
e2prom_uint8_Write(addr, new_value);

/* Good: Only writes if changed */
e2prom_uint8_Upate(addr, new_value);
```

**2. Rotating Storage:**
```c
/* Spread writes across multiple locations */
uint8_t write_index = 0;

void store_sensor_data(uint8_t data) {
    uint16_t address = 0x0100 + write_index;
    e2prom_uint8_Write(address, data);
    write_index = (write_index + 1) % 100; // Rotate through 100 locations
}
```

**3. Journaling/Circular Buffer:**
```c
/* Keep pointer to current write position */
uint16_t current_position = 0;

void append_log_entry(uint8_t entry) {
    e2prom_uint8_Write(current_position, entry);
    current_position = (current_position + 1) % 1024;
}
```

---

## **EEPROM Register Access Examples**

### **Example 1: Direct Register Access**
```c
#include <avr/io.h>
#include <util/delay.h>

void write_eeprom_direct(uint16_t addr, uint8_t data) {
    /* Wait for previous write */
    while(EECR & (1<<EEPE));
    
    /* Setup address and data */
    EEAR = addr;
    EEDR = data;
    
    /* Configure for erase+write */
    EECR &= ~((1<<EEPM1) | (1<<EEPM0));
    
    /* Start write sequence */
    cli();
    EECR |= (1<<EEMPE);
    EECR |= (1<<EEPE);
    sei();
}

uint8_t read_eeprom_direct(uint16_t addr) {
    /* Wait for previous write */
    while(EECR & (1<<EEPE));
    
    /* Setup address */
    EEAR = addr;
    
    /* Start read */
    EECR |= (1<<EERE);
    
    /* Return data */
    return EEDR;
}
```

### **Example 2: Using e2prom Library**
```c
#include "aKaReZa.h"
#include "e2prom.h"

int main(void) {
    /* Write operations */
    e2prom_uint8_Write(0x0000, 42);
    e2prom_uint16_Write(0x0010, 1234);
    e2prom_float_Write(0x0020, 3.14159);
    
    /* Read operations */
    uint8_t val8;
    uint16_t val16;
    float val_float;
    
    e2prom_uint8_Read(0x0000, &val8);
    e2prom_uint16_Read(0x0010, &val16);
    e2prom_float_Read(0x0020, &val_float);
    
    /* Update with wear-leveling */
    e2prom_uint8_Upate(0x0000, 99); // Only writes if changed
    
    while(1) {
        // Main loop
    }
}
```

### **Example 3: Multi-Byte Structure**
```c
typedef struct {
    uint16_t id;
    float temperature;
    uint8_t status;
    uint8_t checksum;
} sensor_config_t;

#define CONFIG_ADDR 0x0100

void save_config(sensor_config_t *config) {
    uint16_t addr = CONFIG_ADDR;
    
    /* Calculate checksum */
    config->checksum = config->id + config->status;
    
    /* Write structure members */
    e2prom_uint16_Write(addr, config->id);
    addr += 2;
    
    e2prom_float_Write(addr, config->temperature);
    addr += 4;
    
    e2prom_uint8_Write(addr, config->status);
    addr += 1;
    
    e2prom_uint8_Write(addr, config->checksum);
}

bool load_config(sensor_config_t *config) {
    uint16_t addr = CONFIG_ADDR;
    
    /* Read structure members */
    e2prom_uint16_Read(addr, &config->id);
    addr += 2;
    
    e2prom_float_Read(addr, &config->temperature);
    addr += 4;
    
    e2prom_uint8_Read(addr, &config->status);
    addr += 1;
    
    e2prom_uint8_Read(addr, &config->checksum);
    
    /* Verify checksum */
    uint8_t calc_checksum = config->id + config->status;
    return (calc_checksum == config->checksum);
}
```

---

## **Timing Diagrams**

### **Write Operation Timing:**
```
Time:     0 ms          3.3 ms         6.6 ms
          |               |               |
Write 1:  [====ERASE+WRITE====]
          EEPE=1 -----> EEPE=0 (done)
          
Write 2:                [====ERASE+WRITE====]
                        Wait... EEPE=1 -----> EEPE=0
```

### **Read Operation Timing:**
```
Time:     0 µs     0.25 µs
          |         |
Read:     [=READ=]
          EERE=1   Data ready in EEDR
```

---

## **Common EEPROM Operations Summary**

| Operation               | Registers Used      | Time     | Description                    |
|-------------------------|---------------------|----------|--------------------------------|
| Write byte              | EEAR, EEDR, EECR    | ~3.3 ms  | Atomic erase+write             |
| Read byte               | EEAR, EEDR, EECR    | ~4 cycles| Fast read operation            |
| Erase byte              | EEAR, EECR          | ~1.8 ms  | Set byte to 0xFF               |
| Write without erase     | EEAR, EEDR, EECR    | ~1.8 ms  | Can only clear bits (1→0)      |
| Update byte             | All + logic         | 0-3.3 ms | Write only if different        |
| Format (1024 bytes)     | All in loop         | ~3.4 sec | Write 0xFF to all locations    |

---

## **Best Practices**

✅ **DO:**
- Use `e2prom_xxx_Upate()` functions to reduce wear
- Wait for EEPE to clear before starting new write
- Use atomic access for EECR modifications
- Enable Brown-Out Detection (BOD) for critical applications
- Implement checksums for critical data
- Test EEPROM operations at temperature extremes

❌ **DON'T:**
- Write to EEPROM in tight loops (causes excessive wear)
- Ignore the 4-cycle timing requirement for EEMPE→EEPE
- Access EEPROM during interrupt routines without protection
- Exceed 100,000 writes to the same location
- Rely on EEPROM for rapidly changing data

---

## **Troubleshooting**

| Problem                          | Possible Cause                        | Solution                                      |
|----------------------------------|---------------------------------------|-----------------------------------------------|
| Data not written                 | EEMPE→EEPE timing violation           | Set EEPE within 4 cycles of EEMPE            |
| Write takes too long             | Previous write not completed          | Check EEPE bit before starting new write     |
| Data corruption                  | Interrupt modified EECR               | Use cli()/sei() during write sequence        |
| Unexpected values read           | Address out of range                  | Verify address is 0x0000-0x03FF              |
| EEPROM wear-out                  | Too many writes to same location      | Implement wear-leveling techniques           |

---

## **Register Quick Reference**

```c
/* EEPROM Registers */
EEAR   // EEPROM Address Register (10-bit for ATmega328P)
EEDR   // EEPROM Data Register (8-bit)
EECR   // EEPROM Control Register

/* EECR Bits */
#define EERE   0  // EEPROM Read Enable
#define EEPE   1  // EEPROM Program Enable
#define EEMPE  2  // EEPROM Master Program Enable
#define EERIE  3  // EEPROM Ready Interrupt Enable
#define EEPM0  4  // EEPROM Programming Mode bit 0
#define EEPM1  5  // EEPROM Programming Mode bit 1
```

---

> [!TIP]
> The e2prom library handles all register operations automatically. Use direct register access only when implementing custom EEPROM drivers or optimizing for specific use cases.

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