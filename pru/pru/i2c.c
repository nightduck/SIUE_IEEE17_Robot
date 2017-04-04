//*************************************************
//*                I2C FUNCTIONS                  *
//*************************************************
// Returns a handle for i2c device at "addr" on bus "bus"

int i2c_open(unsigned char bus, unsigned char addr)
{
  int file;
  char filename[16];

  return(file);
}

// Write out a data buffer to i2c device

int i2c_write(int handle, unsigned char* buf, unsigned int length)
{
  
  return(length);
}

//  Write out a single byte to i2c device

int i2c_write_byte(int handle, unsigned char val)
{
  
  return(1);
}

// Read a specified number of bytes from i2c device

int i2c_read(int handle, unsigned char* buf, unsigned int length)
{
 
  return(length);
}

// Read a single byte from the device

int i2c_read_byte(int handle, unsigned char* val)
{

  return(1);
}

// Close the handle to the device

int i2c_close(int handle)
{
  if ((close(handle)) != 0)
  {
    return(-1);
  }
  return(0);
}

// Write and read

int i2c_write_read(int handle,
                   unsigned char addr_w, unsigned char *buf_w, unsigned int len_w,
                   unsigned char addr_r, unsigned char *buf_r, unsigned int len_r)
{

  return(len_r);
}


