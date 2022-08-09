# linksim

An assignment for data comunication discipline. It must simulate the link layer of OSI model.

## Requisitos

- Controle de erro
- Controle de fluxo
- Erros de bit e de rajada
- Perda de quadros
- Quadros fora de ordem
- Transferência de imagem
- Interface gráfica (extra)
- Transferência de vídeo (extra)

## Compilação e arquivos

Requisitos:

- SDL2
- Make com extensões da GNU
- Acesso a utilitário `rm` (para o alvo `clean`)

Desenvolvido e testado com:

- Ubuntu (pelo WSL2)
- Windows pelo MSYS2

Arquivos para retrocompatibilidade com Windows:

- `fcntl.h`
- `sys/socket.h`
- `unistd.h`
- `socket.c`

IMPORTANTE: escolher `PLATFORM` no arquivo `Makefile` antes de compilar.

## Contratos

### Contrato: trocar quadros via socket

- [X] Criar dois sockets e conecta-los
- [X] Criar duas threads com SDL2
- [X] Trocar mensagens entre threads via socket

### Contrato: ler arquivo de vídeo

- [X] Ler arquivo por argumento inteiramente
- [X] Transmitir arquivo da primeira pra segunda thread via socket
- [X] Comparar se arquivos transmitidos são iguais

### Contrato: introduzir camada de rede

- [X] Encapsular leitura do arquivo
- [X] Trocar partes via socket usando camada em cada thread
- [X] Encapsular checagem do arquivo transmitido com original

### Contrato: introduzir camada de enlace

- [X] Criar módulo que corresponde ao enlace
- [X] Numa única chamada:
    - [X] Repassar quadros recebidos do socket
    - [X] Enviar dados pra camada superior
    - [X] Responder com outro quadro e envia-lo via socket
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

- [X] 8=======D~~~~~
