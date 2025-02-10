/*
 * Por: Kaique Barbosa
 *    Comunicação serial com I2C
 *  
 * Uso da interface I2C para comunicação com o Display OLED
 *  
 * Usei o código que foi disponibilizado na aula do professor Wilton como base e construi minha solução
 * 
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "hardware/clocks.h"
#include "inc/font.h"
#include "hardware/pio.h"
#include "inc/matriz.c"
#include "ws2812.pio.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define tempo 2500
ssd1306_t ssd; // Inicializa o  display
bool cor = true;


const uint button_A = 5;
const uint button_B = 6;

const uint led_red_pin = 13;
const uint led_blue_pin = 12;
const uint led_green_pin = 11;

// Variável global para armazenar a cor 
uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 0; // Intensidade do verde
uint8_t led_b = 20; // Intensidade do azul

#define IS_RGBW false
#define WS2812_PIN 7
#define tempoM 4

void set_one_led(uint8_t r, uint8_t g, uint8_t b, char c);


void inicializar_botoes(){
  gpio_init(button_A);
  gpio_init(button_B);

  gpio_set_dir(button_A, GPIO_IN);
  gpio_set_dir(button_B, GPIO_IN);

  gpio_pull_up(button_A);
  gpio_pull_up(button_B);
}

void inicializar_led(){
  gpio_init(led_green_pin);              // Inicializa o pino do LED
  gpio_set_dir(led_green_pin, GPIO_OUT); // Configura o pino como saída
  gpio_init(led_blue_pin);              // Inicializa o pino do LED
  gpio_set_dir(led_blue_pin, GPIO_OUT); // Configura o pino como saída
}

static volatile uint32_t a = 1;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
static volatile bool mostrar_mensagem_A = false;
static volatile bool mostrar_mensagem_B = false;


void atualizar_display() {
  ssd1306_fill(&ssd, false);  // Limpa o display

  if (mostrar_mensagem_A) {
      ssd1306_draw_string(&ssd, "LED GREEN ON", 10, 20);
  } else if (mostrar_mensagem_B) {
      ssd1306_draw_string(&ssd, "LED BLUE ON", 10, 20);
  }

  ssd1306_send_data(&ssd);
}



// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    printf("A = %d\n", a);
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento
        if(gpio == button_A){
          gpio_put(led_green_pin, !gpio_get(led_green_pin));
          printf("Botão A pressionado - LED verde alterado\n");

          // **Alternar estado da mensagem**
          mostrar_mensagem_A = !mostrar_mensagem_A;
          
          atualizar_display();
        }else if(gpio == button_B){
          gpio_put(led_blue_pin, !gpio_get(led_blue_pin));
          printf("Botão B pressionado - LED azul alterado\n");
          mostrar_mensagem_B = !mostrar_mensagem_B;
          atualizar_display();
        }

        a++;                                     // incrementa a variavel de verificação
    }
}

void get_caracter(){
  char c;
  //ssd1306_fill(&ssd, !cor); // Limpa o display
  if(scanf("%c",&c) ==1){
    if(c >= 'a' && c <= 'z'){
      ssd1306_draw_char(&ssd, c, 60, 40);
    }else if(c >= 'A' && c <= 'z'){
      ssd1306_draw_char(&ssd, c, 60, 40);
    }else if(c >= '0' && c <= '9'){
      ssd1306_draw_char(&ssd, c, 60, 40);
      set_one_led(led_r, led_g, led_b, c);

    }
  }

  ssd1306_send_data(&ssd);

}


static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b);
}



void set_one_led(uint8_t r, uint8_t g, uint8_t b, char c)
{
    uint32_t color = urgb_u32(r, g, b);
    int num = c - '0'; 
    // Verifica se o número está dentro do intervalo válido
    if (num >= 0 && num <= 9) {
        for (int i = 0; i < NUM_PIXELS; i++) {
            if (led_buffers[num][i]) { 
                put_pixel(color); // Acende o LED se estiver no padrão
            } else {
                put_pixel(0); // Apaga o LED se não estiver no padrão
            }
        }
    }
}


int main()
{
  stdio_init_all(); // Inicializa comunicação USB CDC para monitor serial


  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);

  ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);



  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  //ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);
  inicializar_botoes();
  inicializar_led();

  // Configuração da interrupção com callback
  gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  gpio_set_irq_enabled_with_callback(button_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

  while (true)
  {
    cor = !cor;
    if(stdio_usb_connected()){
      get_caracter();

    }
  }
}