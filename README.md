# 🌫️ Sistema IoT de Monitoramento da Qualidade do Ar (ESP8266)

Sistema embarcado para **monitoramento da qualidade do ar com leituras periódicas**, utilizando **sensores MQ-135 e MQ-7**, executando em **ESP8266**, com exibição local em **display OLED** e envio dos dados para **Supabase** e **ThingSpeak** via Wi-Fi.

---

## 🎯 Objetivo do Projeto

Desenvolver um sistema IoT capaz de **coletar, processar, exibir e registrar dados ambientais**, permitindo o acompanhamento remoto da **qualidade do ar** para fins de monitoramento, análise e estudos ambientais.

O projeto simula um cenário real de **Internet das Coisas (IoT)**, integrando hardware, firmware e serviços em nuvem.

---

## ⚙️ Funcionalidades

- 🌫️ Leitura de gases e poluentes com **MQ-135**
- 🔥 Leitura digital do **MQ-7** (estado 0/1; não mede concentração em ppm)
- 📟 Exibição de dados em **display OLED (SSD1306)**
- 📊 Rotina de desenho de gráfico (a atualização do display precisa ser revisada)
- 📡 Envio de dados para **Supabase (REST API)**
- ☁️ Envio de dados para **ThingSpeak**
- 📶 Conexão Wi-Fi automática
- ⏱️ Envio periódico de dados (intervalo configurável)

---

## 🧱 Tecnologias Utilizadas

### 🔌 Hardware
- ESP8266
- Sensor MQ-135 (qualidade do ar)
- Sensor MQ-7 (monóxido de carbono)
- Display OLED SSD1306 (128x64)
- LED indicador
- Fonte 5V

### 💻 Software / Firmware
- Arduino (C/C++)
- ESP8266WiFi
- ESP8266HTTPClient
- Adafruit GFX
- Adafruit SSD1306
- WiFiClientSecure

### ☁️ Cloud
- Supabase (armazenamento dos dados)
- ThingSpeak (visualização e histórico)

---

## 🖥️ Arquitetura do Sistema

[Sensores MQ-135 / MQ-7]
↓
[ESP8266]
[Display OLED Local]
↓
[Wi-Fi Connection]
↓ ↓
[Supabase] [ThingSpeak]

## Configuração e execução

1. Instale o suporte à placa ESP8266 na Arduino IDE e as bibliotecas Adafruit GFX e Adafruit SSD1306.
2. Abra `Monitoramento-Ar.ino`. Se a IDE solicitar uma pasta para o sketch, aceite e mantenha os arquivos de configuração nessa mesma pasta.
3. Copie `secrets.example.h` para `secrets.h` e preencha a rede Wi-Fi e as configurações dos serviços. Somente o exemplo deve ir para o Git.
4. No Supabase, configure a tabela `Sensores` com os campos usados pelo firmware: `MQ135`, `MQ7`, `Qualidade` e `TimeStamp`. Revise tipos e políticas de acesso antes de enviar dados.
5. Configure o canal do ThingSpeak; a versão atual envia apenas a leitura do MQ-135 ao Field 1.
6. Selecione a placa e a porta, compile e envie. O monitor serial usa 9600 baud.

O firmware usa A0 para o MQ-135 e D0 para a saída digital do MQ-7. O intervalo atual entre ciclos é `delay(300000)`, equivalente a cinco minutos, além do tempo das requisições.

## Limites do protótipo

A classificação usa faixas fixas da leitura analógica do MQ-135, sem conversão calibrada para concentração. O MQ-7 retorna apenas um estado digital. A função de gráfico é chamada após a atualização do display; o gráfico ainda precisa de ajuste para aparecer corretamente.

O código atual usa HTTP no ThingSpeak e desabilita a validação do certificado no cliente Supabase. A configuração de transporte precisa ser revisada antes de uso fora do protótipo.

## Credenciais

`secrets.h` está ignorado pelo Git. Não use uma chave administrativa/service-role do Supabase no dispositivo. Utilize permissões mínimas e políticas adequadas para a tabela.

Se senhas ou chaves de escrita já foram publicadas, substitua-as nos respectivos serviços. A remoção do arquivo atual não apaga o histórico Git. As chaves e o cabeçalho de autorização não são mais impressos no monitor serial.
