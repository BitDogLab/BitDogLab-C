# Driver MPU6050

Driver adaptado de [raspberrypi pico-examples](https://github.com/raspberrypi/pico-examples/blob/master/i2c/mpu6050_i2c/mpu6050_i2c.c)

## Arquivos

- `mpu6050_i2c.h` – Header com as declarações das funções
- `mpu6050_i2c.c` – Implementação das funções do mpu6050 via I2C

## Como usar

1. Copie os arquivos `mpu6050_i2c.h` e `mpu6050_i2c.c` para a pasta do seu projeto principal.
2. No seu arquivo `main.c`, inclua o header:

   ```c
   #include "mpu6050_i2c.h"

3. Inicialize o driver e use a função de leitura:

   ```c
   mpu6050_setup_i2c();  // Configura o barramento I2C
   mpu6050_reset();      // Reinicia o sensor

   int16_t accel[3], gyro[3], temp;
   mpu6050_read_raw(accel, gyro, &temp); // Faz a leitura e armazena nas variaveis

## Funções disponíveis

| Função                | Descrição                                               |
|---------------------- |-------------------------------------------------------- |
| `mpu6050_setup_i2c()` | Configura o I2C para comunicação com o sensor           |
| `mpu6050_reset()`     | Reseta o sensor para o estado inicial                   |
| `mpu6050_read_raw()`  | Lê os valores brutos de aceleração, giroscópio e temperatura |


## Exemplo de uso

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "mpu6050_i2c.h"

int main() {
    stdio_init_all();           // Inicializa USB serial
    mpu6050_setup_i2c();       // Configura barramento I2C
    mpu6050_reset();           // Reinicia o sensor

    int16_t accel[3], gyro[3], temp;

    while (1) {
        mpu6050_read_raw(accel, gyro, &temp); // Armazena valores lidos nas variaveis

        printf("Accel X: %d, Y: %d, Z: %d\n", accel[0], accel[1], accel[2]);
        sleep_ms(1000); // espera 1 segundo
    }
}
```

## Observacao

Caso deseje usar o código disponível no repositório:
https://github.com/raspberrypi/pico-examples/blob/master/i2c/mpu6050_i2c/mpu6050_i2c.c
e também disponível no PDF p582-585
https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf

Substituir a variável "i2c_default" pelas variáveis "i2c0" ou "i2c1".
Substituir as variáveis  "PICO_DEFAULT_I2C_SDA_PIN" e "PICO_DEFAULT_I2C_SCL_PIN" por "0" e "1" (caso i2c0), ou por "2" e "3" (caso i2c1).

Tabela resumo:
| Interface  | Pino SDA  | Pino SCL  |
|------------|-----------|-----------|
| i2c0       | 0         | 1         |
| i2c1       | 2         | 3         |

Não esquecer de usar a opção "i2c0/i2c1" no código que corresponda ao conector físico onde conectou o sensor.
