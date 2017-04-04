//
// I2C functions for PRU 0
//

int i2c_open(unsigned char bus, unsigned char addr);

int i2c_write(int handle, unsigned char* buf, unsigned int length);

int i2c_read(int handle, unsigned char* buf, unsigned int length);

int i2c_write_read(int handle,
                   unsigned char addr_w, unsigned char *buf_w, unsigned int len_w,
                   unsigned char addr_r, unsigned char *buf_r, unsigned int len_r);

int i2c_write_byte(int handle, unsigned char val);

int i2c_read_byte(int handle, unsigned char* val);

int i2c_close(int handle);


