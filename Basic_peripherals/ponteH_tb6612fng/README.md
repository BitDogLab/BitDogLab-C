# Driver Ponte H TB6612FNG

Este diretório contém um driver simples para controlar motores DC usando a ponte H TB6612FNG com a BitDogLab v6.

## Arquivos

- `motor.h` – Declarações das funções e definição dos pinos utilizados
- `motor.c` – Implementação das funções de controle dos motores

## Como usar

1. Copie os arquivos `motor.h` e `motor.c` para a pasta do seu projeto principal.
2. No seu `main.c`, inclua o header do driver:

   ```c
   #include "motor.h"
   ```

3. No início do programa, configure e habilite os motores:

   ```c
   motor_setup();      // Inicializa GPIOs e PWM
   motor_enable();     // Libera a ponte H para funcionamento
   ```
    
4. Para controlar os motores durante a execução:

   ```c
   // Define velocidade (0 até 65535) e direção (true=frente, false=tras)
   motor_set_both_level(20000, true);
   ```

## Funções disponíveis

| Função                | Descrição                                               |
|---------------------- |-------------------------------------------------------- |
| `motor_setup()` | Inicializa GPIOs e PWM |
| `motor_enable()`     | Ativa a ponte H (pin20(STBY)=true) |
| `motor_disable()`  | Desativa a ponte H (pin20(STBY)=false) |
| `motor_set_left_level(level, forward)`  | Define nível de PWM e direção para motor esquerdo |
| `motor_set_right_level(level, forward)` | Define nível de PWM e direção para motor direiro |
| `motor_set_both_level(level, forward)`  | Define nível de PWM e direção para os dois motores |

## Exemplo básico

   ```c
   #include <stdio.h>
   #include "pico/stdlib.h"
   #include "motor.h"
   #include <math.h> // para fabsf e fminf
   
   int main() {
       stdio_init_all();
       motor_setup();
       motor_enable();

       // Exemplo: comando de controle com valor negativo = girar para tras
       float control_signal = -0.30f * 255.0f;

       bool forward = control_signal > 0; // obtem direcao
       float magnitude = fabsf(control_signal); // obtem modulo do sinal
       float limitado = fminf(magnitude, 255.0f); // limita ao maximo de 255
       uint16_t level = (uint16_t)limitado << 8; // converte e ajusta escala
       
       motor_set_both_level(level, forward); // envia comando para motores

       sleep_ms(1000); // mantem ligado por 1 segundo

       motor_set_both_level(0, true); // para os motores

       while (1) {
           tight_loop_contents(); // mantém o programa rodando
       }
   }
   ```

## Observações

- Ajuste o valor de level conforme a velocidade desejada (até ~65535).
- Os pinos usados estão definidos no motor.h conforme montagem com cabo flat de 14 vias no conector IDC 2x7 (se a montagem for diferente ajuste os pinos)