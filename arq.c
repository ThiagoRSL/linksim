#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "frame.h"

#define PAYLOAD "A"
#define ID_FRAME_ACK_RECEBIDO "ID DO FRAME CUJO O RECEBIMENTO FOI CONFIRMADO"
#define INFO = "DADOSDOARQUIVOFODAS"
#define DADO_A_SER_ESCRITO = "BYTE ATUAL DO ARQUIVO SENDO ADICIONADO AO FRAME PARA SER ENVIADO;"
main()
{

}

int GoBackN_Receiver(struct frame* frame, struct link* link)
{    
    struct physical* physical      = link->physical;
    struct frame_control* fcontrol = link->frame_control;
    struct upper* there             = link->here;

    unsigned char[MAX_FRAME_SIZE] payload;
    int awaiting_frame_id = 0; //armazenar em link
    
    int writing_index = 0;

    //Verifica se era o frame esperado
    if(frame[FRAME_ID_INDEX] == frame.id_frame)
    {   
        struct frame ack;
        ack.fcode = FCODE_ACK;
        ack.id_ack = frame.id_frame;
        frame_deframe(ack);
        phisical_send(physical, ack.bytes, ack.n_byte);
        return 1;
    }

    upper_write(there, frame.data, fram.data_size);
}

//ADICIONAR FRAME_CONTROL NO LINK
void GoBackN_Sender(struct frame* frm, struct link* link)
{
    struct physical* physical      = link->physical;
    struct frame_control* fcontrol = link->frame_control;
    struct upper* here             = link->here;

    int f_id = 0;
    int oldest_frame_waiting_id = 0; 

    //Received
    //Id é o payload de um frame com fcode == FCODE_ACK
    if(frm.fcode == FCODE_ACK)
    {
        fcontrol.frame_confirmation_list[frm.id_ack] = 1;
        for(int iteration = fcontrol.sliding_window_start; iteration != next_frame_id(frm.id_ack); next_frame_id(iteration))
        {
            free(fcontrol.frame_list[iteration]);
            fcontrol.frame_list[iteration] = null;
            fcontrol.frame_confirmation_list[iteration] = 1;
            fcontrol.sliding_window_start = next_frame_id(fcontrol.sliding_window_start);
            fcontrol.sliding_window_end = next_frame_id(fcontrol.sliding_window_end); 
        }
    }
    //

    //Verifica timeouts e se um frame já foi confirmado
    for(int idframe = fcontrol.sliding_window_start; idframe <= sliding_window_end; idframe = next_frame_id(idframe))
    {
        if(fcontrol.frame_confirmation_list[idframe] == 0)
        {
            if((fcontrol.frame_timeouts_list[idframe] + TIMEOUT) < clock())
            {              
                fcontrol.frame_confirmation_list[idframe] = 2;
                break;  //retirar no selective repeat arq
            }
        }
        else if(fcontrol.frame_confirmation_list[idframe] == 1)
        {           
            //IDs de frames já confirmados são reciclados
            fcontrol.frame_confirmation_list[idframe] = 2;

            struct frame new_frame;
            new_frame.id_frame = idframe;
            new_frame.fcode = FCODE_USERDATA;
            unsigned char* data_chunk;
            int size = upper_read(here, &data_chunk, MAX_FRAME_SIZE);
            new_frame.datasize = size;
            memcpy(new_frame.data, data_chunk, size); 
            frame_deframe(&new_frame);

            phisical_send(physical, new_frame.bytes, new_frame.n_byte);

            fcontrol.frame_list[idframe] = frame_deframe(f_id, FCODE_USERDATA, INFO); //INFO = dados para transmitir
        }        
    }  

    int send_id = fcontrol.sliding_window_start;
    while(send_id < fcontrol.sliding_window_end)
    {
        if(fcontrol.frame_confirmation_list[send_id] == 2)
        {
            phisical_send(physical, frame, );
            send_frame(send_frame);
            send_frame = next_frame_id(send_frame);
            fcontrol.frame_timeouts_list[send_frame] = clock();
            break;
        }
        send_id = next_frame_id(send_id);
    }
}

//Go-Back-N
static void next_frame_id(int frame_index){
    if(frame_index + 1 > (POW(2, (double) SLINDING_WINDOW_POT) - 1))
    {
        return 0;
    }
    else 
    {
        return frame_index = frame_index + 1;
    }
}

unsigned char* enframe(int id, unsigned char fcode, unsigned char* info)
{
    unsigned char* frame = (unsigned char*) malloc(sizeof(unsigned char) * MAX_FRAME_SIZE);

    frame[0] = FLAG;
    frame[FRAME_FCODE_INDEX] = fcode;
    frame[FRAME_ID_INDEX] = id;

    current_index = 3;

    while(current_index < total_size - 1){
        if(DADO_A_SER_ESCRITO == ESC || DADO_A_SER_ESCRITO == FLAG){       // SE O DADO FOR @ ou ESC
            frame[current_index] = ESC;                                  // INSERE ESC
            current_index = current_index + 1;
        }
        frame[current_index] = DADO_A_SER_ESCRITO;                      // acrescentar dado
    }

    frame[total_size - 1] = FLAG; //finalizar frame
    return frame;
}

int bit_to_int(char* bit_string){
    int v = 0;
    int i = strlen(bit_string); 
    while(bit_string[i] != '\0')
    {
        if(bit_string[i] == '1')
        {
            v += pow((double) 2, (double) i);
        }
    }
    return v;
}


struct frame_control* initialize_frame_control(){
    struct frame_control fcontrol;
    fcontrol.frame_list = (unsigned char**) malloc(sizeof(unsigned char*) * sliding_window_size);
    fcontrol.frame_timeouts_list = (clock_t*) malloc(sizeof(clock_t) * sliding_window_size); 
    fcontrol.frame_confirmation_list = (int*) malloc(sizeof(int) * sliding_window_size); 
    fcontrol.sliding_window_start = 0;
    fcontrol.sliding_window_end = (POW(2, (double) SLINDING_WINDOW_POT) - 1) - 1;
    
    for(int fc_id = 0; fc_id < SLIDING_WINDOW_SIZE; fc_id++)
    {
        fcontrol.frame_confirmation_list[fc_id] = 1;
    }
    
    return fcontrol;
}

void destroy_frame_control(struct frame_control* fcontrol){
    for(int i = 0; i < SLIDING_WINDOW_SIZE; i++)
    {
        free(fcontrol.frame_list[i])
    }
    free(fcontrol.frame_list);
    free(fcontrol.frame_timeouts_list);
    free(fcontrol.frame_confirmation_list);
    free(fcontrol);
}
/*

/*
//Selective Repeat
static void next_frame_id(int frame_index){
    if(frame_index + 1 > POW(2, (double) (SLINDING_WINDOW_POT - 1))) 
    {
        return 0;
    }
    else 
    {
        return frame_index = frame_index + 1;
    }
}
*/