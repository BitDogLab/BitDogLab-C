# PicoW BLE Temp Sensor

Este projeto é um exemplo educacional de como utilizar o Raspberry Pi Pico W para criar um sensor de temperatura que se comunica via Bluetooth Low Energy (BLE).

O objetivo deste projeto é ensinar aos estudantes como configurar e programar o Raspberry Pi Pico W para ler dados do sensor de temperatura integrado e transmiti-los via BLE para um dispositivo receptor.

## Fatos importantes sobre Bluetooth Low Energy (BLE)

- Foca no baixo consumo de energia e dispositivos alimentados por bateria
- Opera na banda ISM de 2.4 GHz
- Alcance: Pode operar a partir de alguns metros ate 1 Km (utilizando o método de recuperação de dados FEC), configurável
- Consumo de energia: Reduzido, o rádio fica desligado e basicamente é ligado apenas para transmitir e receber dados. De modo geral, a bateria pode durar meses
- BLE vs Bluetooth clássico:
  - Dispositivos BLE e BT clássicos não são compatíveis
  - BLE é projetado para transferências de dados em rajadas e de baixa largura de banda
  - A maioria dos novos recursos e atualizações na especificação desde 2010 são direcionados ao BLE
- [Mais informações](https://www.bluetooth.com/learn-about-bluetooth/tech-overview/)

## Materiais Necessários

- Raspberry Pi Pico W (2 unidades, se desejar usar uma como receptor)
- Protoboard e jumpers (opcional, se desejar testar com outros sensores)
- Computador com ambiente de desenvolvimento configurado (ex: VSCode com Pico SDK)
- Dispositivo receptor com suporte a BLE (ex: smartphone, tablet, ou outra placa Raspberry Pi Pico W)

## Configuração do Ambiente

1. Instale o VSCode no seu computador.
2. Configure o Pico SDK seguindo as instruções oficiais.
3. Conecte o Raspberry Pi Pico W ao computador via USB.
4. Configure o VSCode para programar o Raspberry Pi Pico W.

## Montagem do Circuito

1. Conecte o Raspberry Pi Pico W ao computador via USB.
2. (Opcional) Se desejar testar com outros sensores, conecte-os ao Raspberry Pi Pico W utilizando a protoboard e os jumpers.

## Programação

1. Abra o VSCode e carregue o projeto utilizando o Pico SDK.
2. Compile o código utilizando o comando:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
3. Carregue o código no Raspberry Pi Pico W utilizando o comando:
    ```sh
    sudo picotool load server.uf2
    ```
4. Para o dispositivo receptor, compile e carregue o código do client.c utilizando os mesmos comandos acima, substituindo `server.uf2` por `client.uf2`.
5. Teste o projeto utilizando um dispositivo receptor para verificar se os dados de temperatura estão sendo transmitidos corretamente.

## Atividade para os Estudantes

1. **Montagem do Circuito**: Os estudantes devem montar o circuito conforme as instruções fornecidas.
2. **Implementação do Código**: Os estudantes devem implementar o código para ler os dados do sensor e transmitir via BLE.
3. **Teste e Validação**: Os estudantes devem testar o projeto utilizando um dispositivo receptor para verificar se os dados de temperatura estão sendo transmitidos corretamente.
4. **Desafio Adicional**: Os estudantes devem modificar o código para incluir uma funcionalidade de alerta. Quando a temperatura ultrapassar um determinado limite, o dispositivo deve enviar uma notificação via BLE para o receptor.

## Recursos Adicionais

- [Documentação do Raspberry Pi Pico W](https://www.raspberrypi.org/documentation/pico/getting-started/)
- [Exemplos de código para sensores de temperatura](https://github.com/adafruit/Adafruit_CircuitPython_DHT)

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para abrir issues e pull requests.
