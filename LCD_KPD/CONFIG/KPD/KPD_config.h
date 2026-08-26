#ifndef KPD_CONFIG_H_
#define KPD_CONFIG_H_

/* Keypad Port Configuration */
#define KPD_PORT DIO_PORTD

/* Keypad Row Pins */
#define KPD_R0_PIN DIO_PIN0
#define KPD_R1_PIN DIO_PIN1
#define KPD_R2_PIN DIO_PIN2
#define KPD_R3_PIN DIO_PIN3

/* Keypad Column Pins */
#define KPD_C0_PIN DIO_PIN4
#define KPD_C1_PIN DIO_PIN5
#define KPD_C2_PIN DIO_PIN6
#define KPD_C3_PIN DIO_PIN7

/* Keypad Values Mapping */
#define KPD_KEYS { \
    {'1', '2', '3', 'A'}, \
    {'4', '5', '6', 'B'}, \
    {'7', '8', '9', 'C'}, \
    {'*', '0', '#', 'D'}  \
}

#endif /* KPD_CONFIG_H_ */
