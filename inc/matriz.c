// Buffer para armazenar quais LEDs estão ligados matriz 5x5
#define NUM_PIXELS 25
bool led_buffer0[NUM_PIXELS] = {
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 1, 
    1, 0, 0, 0, 1, 
    1, 0, 0, 0, 1, 
    1, 1, 1, 1, 1
  };
  
  bool led_buffer1[NUM_PIXELS] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0
  };
  
  bool led_buffer2[NUM_PIXELS] = {
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1, 
    0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1
  };
  
  bool led_buffer3[NUM_PIXELS] = {
    1, 1, 1, 1, 1, 
    0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 
    0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1
  };
  
  bool led_buffer4[NUM_PIXELS]={
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 1, 1, 1, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
  };
  
  bool led_buffer5[NUM_PIXELS]={
    1, 1, 1, 1, 1, 
    0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1
  };
  
  bool led_buffer6[NUM_PIXELS]={
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 0, 
    1, 1, 1, 1, 1
  };
  
  bool led_buffer7[NUM_PIXELS] = {
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 1, 1, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 1, 1
  };
  
  bool led_buffer8[NUM_PIXELS] = {
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 1, 
    1, 1, 1, 1, 1
  };
  
  bool led_buffer9[NUM_PIXELS] = {
    1, 0, 0, 0, 0, 
    0, 0, 0, 0, 1, 
    1, 1, 1, 1, 1, 
    1, 0, 0, 0, 1, 
    1, 1, 1, 1, 1
  };
  
  bool* led_buffers[] = {led_buffer0, led_buffer1, led_buffer2, led_buffer3, led_buffer4, led_buffer5, led_buffer6, led_buffer7, led_buffer8, led_buffer9};
  int current_buffer_index = 0;