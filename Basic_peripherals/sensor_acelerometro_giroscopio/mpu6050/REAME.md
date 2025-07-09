# Driver MPU6050

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
   mpu6050_read_raw(accel, gyro, &temp);

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
        mpu6050_read_raw(accel, gyro, &temp);

        printf("Accel X: %d, Y: %d, Z: %d\n", accel[0], accel[1], accel[2]);
        sleep_ms(1000); // espera 1 segundo
    }
}