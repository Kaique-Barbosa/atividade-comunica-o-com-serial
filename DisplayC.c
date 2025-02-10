/*
 * Por: Kaique Barbosa
 *    Comunicação serial com I2C
 *
 * Uso da interface I2C para comunicação com o Display OLED
 *
 * Código baseado na aula do professor Wilton, com modificações para minha solução.
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
 
 #define PORTA_I2C i2c1
 #define PINO_SDA 14
 #define PINO_SCL 15
 #define ENDERECO_OLED 0x3C
 #define TEMPO_ESPERA 2500
 
 ssd1306_t tela_oled;
 bool estado_cor = true;
 
 const uint botao_verde = 5;
 const uint botao_azul = 6;
 
 const uint led_vermelho = 13;
 const uint led_azul = 12;
 const uint led_verde = 11;
 
 uint8_t intensidade_vermelho = 0;
 uint8_t intensidade_verde = 0;
 uint8_t intensidade_azul = 20;
 
 #define RGBW_ATIVO false
 #define PINO_WS2812 7
 #define TEMPO_ATUALIZACAO 4
 
 void configurar_botoes();
 void configurar_leds();
 void atualizar_tela();
 void acionar_led_rgb(uint8_t r, uint8_t g, uint8_t b, char caractere);
 void tratar_interrupcao_gpio(uint gpio, uint32_t eventos);
 void capturar_caractere();
 
 static volatile uint32_t contador = 1;
 static volatile uint32_t ultimo_tempo = 0;
 static volatile bool exibir_mensagem_verde = false;
 static volatile bool exibir_mensagem_azul = false;
 
 void configurar_botoes() {
     gpio_init(botao_verde);
     gpio_init(botao_azul);
 
     gpio_set_dir(botao_verde, GPIO_IN);
     gpio_set_dir(botao_azul, GPIO_IN);
 
     gpio_pull_up(botao_verde);
     gpio_pull_up(botao_azul);
 }
 
 void configurar_leds() {
     gpio_init(led_verde);
     gpio_set_dir(led_verde, GPIO_OUT);
     gpio_init(led_azul);
     gpio_set_dir(led_azul, GPIO_OUT);
 }
 
 void atualizar_tela() {
     ssd1306_fill(&tela_oled, false);
 
     if (exibir_mensagem_verde) {
         ssd1306_draw_string(&tela_oled, "LED VERDE LIGADO", 10, 20);
     } else if (exibir_mensagem_azul) {
         ssd1306_draw_string(&tela_oled, "LED AZUL LIGADO", 10, 20);
     }
 
     ssd1306_send_data(&tela_oled);
 }
 
 void tratar_interrupcao_gpio(uint gpio, uint32_t eventos) {
     uint32_t tempo_atual = to_us_since_boot(get_absolute_time());
 
     if (tempo_atual - ultimo_tempo > 200000) {
         ultimo_tempo = tempo_atual;
         
         if (gpio == botao_verde) {
             gpio_put(led_verde, !gpio_get(led_verde));
             exibir_mensagem_verde = !exibir_mensagem_verde;
             atualizar_tela();
         } else if (gpio == botao_azul) {
             gpio_put(led_azul, !gpio_get(led_azul));
             exibir_mensagem_azul = !exibir_mensagem_azul;
             atualizar_tela();
         }
         
         contador++;
     }
 }
 
 void capturar_caractere() {
     char caractere;
     
     if (scanf("%c", &caractere) == 1) {
         if ((caractere >= 'a' && caractere <= 'z') || (caractere >= 'A' && caractere <= 'Z')) {
             ssd1306_draw_char(&tela_oled, caractere, 60, 40);
         } else if (caractere >= '0' && caractere <= '9') {
             ssd1306_draw_char(&tela_oled, caractere, 60, 40);
             acionar_led_rgb(intensidade_vermelho, intensidade_verde, intensidade_azul, caractere);
         }
     }
     
     ssd1306_send_data(&tela_oled);
 }
 
 static inline void definir_pixel(uint32_t pixel_cor) {
     pio_sm_put_blocking(pio0, 0, pixel_cor << 8u);
 }
 
 static inline uint32_t converter_rgb_u32(uint8_t r, uint8_t g, uint8_t b) {
     return ((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b);
 }
 
 void acionar_led_rgb(uint8_t r, uint8_t g, uint8_t b, char caractere) {
     uint32_t cor_rgb = converter_rgb_u32(r, g, b);
     int numero = caractere - '0';
     
     if (numero >= 0 && numero <= 9) {
         for (int i = 0; i < NUM_PIXELS; i++) {
             definir_pixel(led_buffers[numero][i] ? cor_rgb : 0);
         }
     }
 }
 
 int main() {
     stdio_init_all();
     
     PIO pio = pio0;
     int sm = 0;
     uint offset = pio_add_program(pio, &ws2812_program);
     ws2812_program_init(pio, sm, offset, PINO_WS2812, 800000, RGBW_ATIVO);
 
     i2c_init(PORTA_I2C, 400 * 1000);
     gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
     gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(PINO_SDA);
     gpio_pull_up(PINO_SCL);
     
     ssd1306_init(&tela_oled, WIDTH, HEIGHT, false, ENDERECO_OLED, PORTA_I2C);
     ssd1306_config(&tela_oled);
     ssd1306_send_data(&tela_oled);
     
     ssd1306_fill(&tela_oled, false);
     ssd1306_send_data(&tela_oled);
     
     configurar_botoes();
     configurar_leds();
 
     gpio_set_irq_enabled_with_callback(botao_verde, GPIO_IRQ_EDGE_FALL, true, &tratar_interrupcao_gpio);
     gpio_set_irq_enabled_with_callback(botao_azul, GPIO_IRQ_EDGE_FALL, true, &tratar_interrupcao_gpio);
     
     while (true) {
         estado_cor = !estado_cor;
         if (stdio_usb_connected()) {
             capturar_caractere();
         }
     }
 }
 