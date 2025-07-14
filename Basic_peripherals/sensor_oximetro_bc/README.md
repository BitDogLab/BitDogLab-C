# Sensor Oxímetro e Batimentos Cardíacos

## Arquivos de driver para o módulo sensor oxímetro e batimentos cardíacos MAX30102

Os códigos aqui incluídos foram desenvolvidos com propósitos didáticos, para uso ao longo do Curso de Capacitação em Sistemas Embarcados - Embarcatech.

Para usar os códigos, conecte o módulo MAX30102 (OXI BAT), usando um conector JST SH de 4 fios, ao port I2C 0 da BitDogLab.

Os valores medidos/processados podem ser lidos via Serial Monitor.

> [!Warning]
> Os códigos desta pasta foram feitos para operação com o módulo **MAX30102**. Portanto, pode não ser compatível com um módulo **MAX30101**.

### Os códigos:
> `oxi_bat_sensor_raw.c`
> Recebe os valores de IR e RED lidos pelo sensor e os exibe via Serial Monitor.

> `oximeter_heart_rate.c`
> Recebe os valores de IR e RED lidos pelo sensor, os processa em valores de SpO2 (em %) e BPM, e os exibe via Serial Monitor.
