# linksim

A assignment for data comunication discipline. It must simulate the link layer of OSI model.

## Requisitos

- Controle de erro
- Controle de fluxo
- Erros de bit e de rajada
- Perda de quadros
- Quadros fora de ordem
- Transferência de imagem
- Interface gráfica (extra)
- Transferência de vídeo (extra)

## Contratos

### Contrato: trocar quadros via socket

- [X] Criar dois exectáveis com SDL2
- [ ] Criar dois sockets e conecta-los
- [ ] Trocar mensagens entre executáveis

### Contrato: ler arquivo de vídeo

- [ ] Ler arquivo por stdin inteiramente
- [ ] Transmitir arquivo do primeiro pro segundo executável via socket
- [ ] Imprimir arquivo no final

### Contrato: introduzir camada de rede

- [ ] Encapsular leitura e impressão do arquivo
- [ ] Enviar e receber partes via socket em cada executável
- [ ] Ir checando arquivo transmitido com original

### Contrato: introduzir camada de enlace

- [ ] Criar módulo que corresponde ao enlace
- [ ] Numa única chamada:
    - [ ] Repassar quadros recebidos do socket
    - [ ] Enviar dados pra camada superior
    - [ ] Responder com outro quadro e envia-lo via socket
- [ ] Tamanho máximo do quadro configurável

### Contrato: aperfeiçoar enlace com Selective Repeat ARQ

- [ ] Novo campo no cabeçalho, tipo de resposta, com piggyback
    - [ ] Resposta 0x: nada a enviar
    - [ ] Resposta 1x: enviando algum quadro
    - [ ] Resposta x0: nada a confirmar
    - [ ] Resposta x1: confirmando algum quadro
- [ ] Adicionar campo com ID do quadro enviado
- [ ] Adicionar campo com ID do quadro recebido, é o ACK
- [ ] Adicionar janela deslizante de envio
    - [ ] Alimentar lista com dados do arquivo
    - [ ] Índice do quadro sendo enviado
    - [ ] Estado de deslize da janela
    - [ ] Enviar quadros conforme
    - [ ] Deslizar quadro conforme ACKs
- [ ] Adicionar janela deslizante de recebimento 
    - [ ] Preencher lista com dados dos quadros recebidos
    - [ ] Descartar quadros repetidos
    - [ ] Estado de deslize da janela
    - [ ] Escrever arquivo e deslizar janela, responder com ACKs
- [ ] Comprimento da janela deslizante configurável

### Contrato: introduzir camada física

- [ ] Criar módulo que faz meio-campo entre sockets e enlace
- [ ] Criar lista de quadros e outra paralela de tempos de atraso
- [ ] Armazenar quadros lidos via socket na lista e associar tempo
- [ ] Calcular tempo delta e passar como argumento
- [ ] Passar quadros pro enlace após esgotar tempo
- [ ] Tempo de atraso mínimo e máximo configurável
- [ ] Perder quadros aleatoriamente
- [ ] Chance de perda configurável

### Contrato: aperfeiçoar enlace com detecção de erro CRC-32

- [ ] Adicionar campo no cabeçalho com digestão do CRC-32
- [ ] Calcular e armazenar CRC-32 antes de envio
- [ ] Calcular e comparar CRC-32 após recebimento
- [ ] Descartar quadro em caso de erro, leitura só se tudo OK
- [ ] Ler próximo quadro como dado se não detectar bit stuffing

### Contrato: aperfeiçoar camada física com simulação de erros

- [ ] Inverter um bit, erro de bit
- [ ] Predefinir erros de rajada e efetuá-los
- [ ] Persistir erro de rajada no fim dum quadro no início doutro
- [ ] Chances de erros configuráveis

### Contrato: estatísticas

- [ ] Da camada física
    - [ ] Quantos quadros perdidos
    - [ ] Quantos erros de bit
    - [ ] Quantos erros em rajada e comprimento
    - [ ] Quantos erros em rajada multiquadro
    - [ ] Quantos quadros transmitidos
    - [ ] Quantos bits transmitidos
- [ ] Da camada de enlace
    - [ ] Quantos quadros descartados
    - [ ] Quantos quadros repetidos
- [ ] Da execução inteira
    - [ ] Transmissão correta
    - [ ] Tempo de execução
    - [ ] Peso do arquivo

### Contrato: interface gráfica

- [ ] Compilar exemplo Nuklear + SDL2
- [ ] Adicionar botão para iniciar transmissão
- [ ] Imprimir estatísticas na tela enquanto executa
- [ ] Ler e escrever arquivo considerando o loop infinito
- [ ] Adicionar widgets para configurar transmissão

### Contrato: ler arquivo "largado" na janela

- [ ] Solicitar largar arquivo se não for iniciado em modo filter
- [ ] Ler arquivo, considerar loop infinito
- [ ] Só transmitir, sem salvar

### Contrato: embutir libmpv na janela

- [ ] DDDDDDDDDDDDDD
- [ ] ??????????????
- [ ] 66666666666666

### Contrato: modo piggyback

- [ ] 8=======D~~~~~