

Example Programs for the ETM32GG-STK3700 Development Board
==========================================================

In all examples, a direct access to registers approach was used. It means that no library besides CMSIS was used.

* 01-Blink: Blink LEDs with direct access to registers
* 02-Blink: Blink LEDs with a HAL for GPIO
* 03-Blink: Blink LEDs with a HAL for LEDs above GPIO HAL
* 04-Blink: Blink LEDs with a HAL for LEDs without a HAL for GPIO 
* 05-Blink: Blink LEDs changing the core frequency
* 06-Blink: Blink LEDs using SysTick
* 07-Blink: Blink LEDs using Time Triggered Tasks



Programas exemplos para a Placa de Desenvolvimento ETM32GG-STK3700
==================================================================

Em todos os exemplos, foi usado acesso direto a registradores, ou seja, não foram usadas
bibliotecas de fabricantes.

* 01-Blink: pisca leds usando acesso direto a registradores
* 02-Blink: pisca leds usando uma camada de abstracao de hardware (HAL) para a GPIO.
* 03-Blink: pisca leds usando uma HAL para os LEDs usando a HAL de GPIO.
* 04-Blink: pisca leds usando uma HAL para os LEDs (mesma interface) sem uusar a HAL para GPIO.
* 05-Blink: pisca leds variando a frequencia de relogio do processador
* 06-Blink: pisca leds usando interrupção periódica SysTick
* 07-Blink: pisca leds usando Time Triggered Tasks