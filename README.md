# Frell display

## PINOUT

**T\_** prefix - touch panel.

| Pin | Function                                            | Comment                      |
| --- | --------------------------------------------------- | ---------------------------- |
| 15  | DATA TEMPERATURE SENSOR EXTERNAL                    | Data line for external temp. |
| X   | DATA TEMPERATURE SENSOR INTERNAL                    | Data line for internal temp. |
| X   | BATTERY VOLTAGE DATA                                |                              |
| XX  | DC                                                  | XXXXXX                       |
| XX  | CS                                                  | Chip select for display      |
| XX  | RESET                                               | XXXXXX                       |
| XX  | SDI(MOSI) T_DIN                                     | XXXXXX                       |
| XX  | SCK(CLOCK) T_CLK                                    | XXXXXX                       |
| XX  | T_DO                                                | XXXXXX                       |
| XX  | T_CS                                                | Chip select for touch        |
| XX  | T_IRQ                                               | XXXXXX                       |
| 3V3 | VCC[ILI9341] LED[ILI9341] VCC[EXTERNAL_TEMP_SENSOR] | XXXXXX                       |
| GND | GND[ILI9341] GND[EXTERNAL_TEMP_SENSOR]              | XXXXXX                       |
