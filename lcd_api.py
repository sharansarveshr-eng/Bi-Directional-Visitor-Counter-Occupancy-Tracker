import utime 
from machine import I2C, Pin 
from pico_i2c_lcd import I2C_Lcd 
i2c = I2C(0, sda=Pin(0), scl=Pin(1), freq=400000) 
lcd = I2C_Lcd(i2c, 0x27, 4, 16) 
pir_entry = Pin(14, Pin.IN) 
pir_exit = Pin(15, Pin.IN) 
total_inside = 0 
total_entered = 0 
total_exited = 0 
 def update_display(): 
    lcd.clear() 
    # Row 0: Header 
    lcd.move_to(0, 0) 
    lcd.putstr(" OBJECT TRACKER ") 
    # Row 1: Current status 
    lcd.move_to(0, 1) 
    lcd.putstr(f"INSIDE: {total_inside}") 
    lcd.move_to(0, 2) 
    lcd.putstr(f"IN: {total_entered}") 
    lcd.move_to(0, 3) 
    lcd.putstr(f"OUT: {total_exited}") 

update_display() 
print("System Ready...") 
 while True: 
    if pir_entry.value() == 1: 
        timeout = 0 
        while timeout < 20: # 2 second window 
            if pir_exit.value() == 1: 
                total_inside += 1 
                total_entered += 1 
                update_display() 
                print(f"Entry Detected! Inside: {total_inside}") 
                utime.sleep(2) # Delay to allow person to fully pass 
                break 
            utime.sleep(0.1) 
            timeout += 1 
    elif pir_exit.value() == 1: 
        timeout = 0 
        while timeout < 20: 
            if pir_entry.value() == 1: 
                if total_inside > 0: 
                    total_inside -= 1 
                total_exited += 1 
                update_display() 
                print(f"Exit Detected! Inside: {total_inside}") 
                utime.sleep(2) 
                break 

            utime.sleep(0.1) 
            timeout += 1 
          utime.sleep(0.1)