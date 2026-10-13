import utime 
from lcd_api import LcdApi 
from machine import I2C 
 class I2C_Lcd(LcdApi): 
    def __init__(self, i2c, i2c_addr, num_lines, num_columns): 
        self.i2c = i2c 
        self.i2c_addr = i2c_addr 
        self.i2c.writeto(self.i2c_addr, bytearray([0])) 
        utime.sleep_ms(20) 
        self.i2c.writeto(self.i2c_addr, bytearray([0x03 << 4])) 
 utime.sleep_ms(5) 
        self.i2c.writeto(self.i2c_addr, bytearray([0x03 << 4])) 
        utime.sleep_ms(1) 
        self.i2c.writeto(self.i2c_addr, bytearray([0x03 << 4])) 
        self.i2c.writeto(self.i2c_addr, bytearray([0x02 << 4])) 
        super().__init__(num_lines, num_columns) 
   def hal_write_init_nibble(self, nibble): 
        self.hal_write_command(nibble) 
  def hal_backlight_on(self): 
        self.i2c.writeto(self.i2c_addr, bytearray([0x08])) 
  def hal_backlight_off(self): 
        self.i2c.writeto(self.i2c_addr, bytearray([0x00])) 
    def hal_write_command(self, cmd): 
        self.i2c.writeto(self.i2c_addr, bytearray([(cmd & 0xF0) | 0x04 | 0x08])) 
        self.i2c.writeto(self.i2c_addr, bytearray([(cmd & 0xF0) | 0x08])) 
        self.i2c.writeto(self.i2c_addr, bytearray([((cmd << 4) & 0xF0) | 0x04 | 0x08])) 
        self.i2c.writeto(self.i2c_addr, bytearray([((cmd << 4) & 0xF0) | 0x08])) 
  def hal_write_data(self, data): 
        self.i2c.writeto(self.i2c_addr, bytearray([(data & 0xF0) | 0x05 | 0x08])) 
        self.i2c.writeto(self.i2c_addr, bytearray([(data & 0xF0) | 0x01 | 0x08])) 
        self.i2c.writeto(self.i2c_addr, bytearray([((data << 4) & 0xF0) | 0x05 | 0x08])) 
        self.i2c.writeto(self.i2c_addr, bytearray([((data << 4) & 0xF0) | 0x01 | 0x08]))