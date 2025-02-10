#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#define RED_PIN 12          // Pinos do LED RGB
#define SERVO_PIN 22        // Pino do motor servo

// Função para configurar PWM no pino especificado com um período desejado
void configure_pwm(uint gpio, float period_ms) 
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_config config = pwm_get_default_config();
    float clkdiv = clock_get_hz(clk_sys) / (1000.0f / period_ms * 65536);
    pwm_config_set_clkdiv(&config, clkdiv);  // Define a divisão do clock
    pwm_init(slice_num, &config, true);
    pwm_set_wrap(slice_num, 65535);  // Define o valor máximo do contador PWM
}

// Função para definir o ciclo ativo em microsegundos
void set_pwm_us(uint gpio, uint16_t us) 
{
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint32_t clock = clock_get_hz(clk_sys);
    uint32_t top = pwm_hw->slice[slice_num].top;
    uint32_t level = (us * (top + 1)) / 20000;  // 20000us é o período de 20ms

    // Adiciona mensagens de depuração
    printf("set_pwm_us - gpio: %d, us: %d, clock: %d, top: %d, level: %d\n", gpio, us, clock, top, level);

    pwm_set_gpio_level(gpio, level);
}

// Função para definir o ciclo ativo em porcentagem (0-100) no pino
void set_pwm_porcent(uint gpio, uint8_t percent) 
{
    uint16_t value = (uint16_t)((percent / 100.0) * 10000);
    printf("set_pwm_porcent - gpio: %d, percent: %d, value: %d\n", gpio, percent, value);
    pwm_set_gpio_level(gpio, value);
}

// Função para aumentar a intensidade gradualmente
void increase_pwm(uint gpio, uint8_t start_percent, uint8_t end_percent, uint delay_ms) 
{
    for (int8_t i = start_percent; i <= end_percent; i++) 
    {
        set_pwm_porcent(gpio, i);
        sleep_ms(delay_ms);
    }
}

// Função para diminuir a intensidade gradualmente
void decrease_pwm(uint gpio, uint8_t start_percent, uint8_t end_percent, uint delay_ms) 
{
    for (int8_t i = start_percent; i >= end_percent; i--) 
    {
        set_pwm_porcent(gpio, i);
        sleep_ms(delay_ms);
    }
}

// Função para mover o pwm para uma posição específica
void move_pwm(uint gpio, uint16_t us, uint delay_ms) 
{
    set_pwm_us(gpio, us);
    sleep_ms(delay_ms);
}

int main() 
{
    stdio_init_all();
    //gpio_init(RED_PIN);
    //gpio_init(SERVO_PIN);
    
    // Configura PWM para um período de 20ms (50Hz)
    configure_pwm(RED_PIN, 20.0f);
    configure_pwm(SERVO_PIN, 20.0f);  

    while (1) 
    {
        // Move para 180º (2400 us) e espera 2 segundos
        printf("Movendo para 180º\n");
        move_pwm(SERVO_PIN, 2400, 2000);

        // Move para 90º (1470 us) e espera 2 segundos
        printf("Movendo para 90º\n");
        move_pwm(SERVO_PIN, 1470, 2000);

        // Move para 0º (500 us) e espera 2 segundos
        printf("Movendo para 0º\n");
        move_pwm(SERVO_PIN, 500, 2000);

        // Move lentamente de 0º a 180º e volta
        increase_pwm(SERVO_PIN, 0, 100, 50);
        decrease_pwm(SERVO_PIN, 100, 0, 50);
    }

    return 0;
}