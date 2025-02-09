# FishCare – IoT Fish Feeder

## Introdução

FishCare é um sistema IoT inovador para a automação e monitoramento de aquários. Inspirado na história de Bob, um peixe que sofria com a alimentação irregular e condições ambientais inadequadas, o projeto busca garantir o bem-estar dos peixes mesmo na ausência do dono. O sistema automatiza a alimentação, controla a iluminação do aquário e monitora condições ambientais, permitindo o controle remoto via um app iOS.

## Objetivos

- **Automatizar a Alimentação:** Programação de horários para acionar o motor de passo e liberar a ração automaticamente.
- **Monitoramento Ambiental:** Medir e exibir a temperatura do aquário em tempo real, garantindo um ambiente ideal.
- **Controle de Iluminação:** Ajustar a fita LED RGB para economizar energia e proporcionar um ambiente adequado.
- **Interface Remota:** Disponibilizar um app iOS para configuração e monitoramento, com comunicação via Wi-Fi.
- **Persistência de Dados:** Salvar as configurações e registros na memória flash para que as informações permaneçam mesmo após reinicializações.

## Funcionalidades

- **Alimentação Automática:** Libera a ração nos horários programados, usando um motor de passo controlado pela Raspberry Pi Pico W.
- **Monitoramento de Temperatura:** O sensor DS18B20 à prova d'água fornece leituras precisas da temperatura do aquário.
- **Controle de Iluminação:** Ajuste da fita LED RGB para criar ambientes ideais e reduzir o consumo de energia.
- **Interface Remota (App iOS):** Configuração de horários, ajuste de cores e visualização de dados (temperatura, status, etc.) por meio de endpoints HTTP.
- **Salvamento na Memória Flash:** As configurações são armazenadas para persistir entre reinicializações.
- **Uso do Display OLED e Botões:** Oferece uma interface local para interação e visualização das informações.

## Arquitetura do Sistema

### Hardware

- **Raspberry Pi Pico W:** Processador com conectividade Wi-Fi.
- **Motor de Passo 28BYJ-48 + Driver ULN2003:** Acionamento do sistema de alimentação.
- **Sensor de Temperatura DS18B20:** Monitoramento ambiental.
- **Fita LED RGB:** Controle de iluminação.
- **Display OLED:** Exibição local de informações.
- **Fonte 5V 2A com Conector P4:** Alimentação estável para o sistema.

### Software/Firmware

- **Firmware Modular:** Código em C organizado em bibliotecas (Wi-Fi, OLED, Motor, Temperatura, etc.).
- **Endpoints HTTP:** Comunicação com o app iOS para configuração e monitoramento.
- **App iOS em SwiftUI:** Interface remota para controlar o sistema e visualizar dados.


## Conclusão

O FishCare demonstra como a integração entre hardware e software pode transformar a manutenção de aquários, automatizando a alimentação, monitorando as condições ambientais e oferecendo controle remoto intuitivo.  
**Benefícios:**  
- Alimentação regular e programada para os peixes.  
- Monitoramento em tempo real que garante um ambiente ideal.  
- Economia de energia e facilidade de gerenciamento via app.  
**Impacto:**  
- Redução de estresse para os peixes e maior tranquilidade para os donos.  
- Uma solução escalável com potencial para integrações futuras.
