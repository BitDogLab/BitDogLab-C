### 📜 Exemplo: Botão com Interrupção, LED RGB, e Tarefa de RTOS (BitDogLab)

**Exemplo prático na BitDogLab:**
- Uma interrupção de botão (GPIO) sinaliza um evento com uma flag.
- O loop principal (executor cíclico) verifica essa flag e acende o LED RGB.
- Uma tarefa do RTOS cuida da atualização do display OLED e comunicação USB.

---

**Estrutura geral:**
- Interrupção de GPIO: Aciona uma flag global.
- Loop principal: Verifica a flag e acende o LED RGB.
- Tarefa RTOS: Atualiza o display OLED e gerencia comunicação USB.

---

**🛠️ Resumo Técnico**  
| Elemento       | Descrição                                                           |
|----------------|---------------------------------------------------------------------|
| Button ISR     | Captura borda de descida e seta a flag.                             |
| Loop (Idle Hook)| Verifica a flag e controla o LED RGB.                               |
| Tarefa RTOS    | Atualiza o display OLED e envia status via USB.                     |
| Uso de FreeRTOS| Separação clara entre eventos assíncronos e tarefas de serviço.     |

---

**📌 Observações práticas:**
- gpio_set_irq_enabled_with_callback já cuida de registrar seu handler de interrupção.
- A flag precisa ser volatile para sincronização entre ISR e o loop principal.
- O Idle Hook (vApplicationIdleHook) é ótimo para um "executor cíclico" leve se você não quiser criar outra tarefa RTOS para verificar flags.

---

**📂 Estrutura esperada de pastas:**

abordagem_hibrida/  
├── CMakeLists.txt  
├── main.c  
├── include/  
│   ├── ssd1306.h  
│   └── usb_comm.h  
├── lib/  
│   ├── ssd1306/  
│   │   ├── ssd1306.c  
│   │   └── ssd1306.h  
│   ├── usb_comm/  
│   │   ├── usb_comm.c  
│   │   └── usb_comm.h  
│   └── FreeRTOS-Kernel/  
│       ├── CMakeLists.txt  
│       └── (resto do FreeRTOS)  
└── build/  
    └── (onde será gerado)  

---

**⚡ Notas Rápidas**
- FreeRTOS-Kernel: você precisa clonar ou copiar o kernel do FreeRTOS para dentro de lib/FreeRTOS-Kernel/ e colocar um CMakeLists.txt lá também.
- ssd1306.c e usb_comm.c são bibliotecas básicas que precisam implementar inicialização e operação do display e da USB.
- hardware_gpio e hardware_irq são módulos específicos da Pico SDK que lidam com GPIOs e interrupções.

