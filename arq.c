#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "arq.h"
#include "layers.h"

int GoBackN_Receiver(struct frame* frame, struct link* link)
{    
    struct physical* physical      = &link->physical;
    struct frame_control* fcontrol = link->frame_control;
    struct upper* there            = link->here;

    unsigned char payload[MAX_FRAME_SIZE];
    
    int writing_index = 0;

    //Verifica se era o frame esperado
    if(fcontrol->awaiting_frame_id == frame->id_frame)
    {   
        struct frame ack;
        ack.fcode = FCODE_ACK;
        ack.id_ack = frame.id_frame;
        frame_deframe(ack);
        phisical_send(physical, ack.bytes, ack.n_byte);
        fcontrol->awaiting_frame_id = next_frame_id(fcontrol->awaiting_frame_id);
        upper_write(there, frame->data, frame->data_size);
        return 1; //Quadro recebido, ack retornado.
    }
    else{
        return 0; // Não queria esse quadro!
    }

}

//ADICIONAR FRAME_CONTROL NO LINK
int GoBackN_Sender(struct frame* frame, struct link* link)
{
    struct physical* physical      = &link->physical;
    struct frame_control* fcontrol = link->frame_control;
    struct upper* here             = link->here;

    int f_id = 0;
    int oldest_frame_waiting_id = 0; 

    //Received
    //Id é o payload de um frame com fcode == FCODE_ACK
    if(frame->fcode == FCODE_ACK)
    {
        fcontrol->frame_confirmation_list[frame->id_ack] = 1;
        for(int iteration = fcontrol->sliding_window_start; iteration != next_frame_id(frame->id_ack); next_frame_id(iteration))
        {
            free(fcontrol->frame_list[iteration]);
            fcontrol->frame_list[iteration] = NULL;
            fcontrol->frame_confirmation_list[iteration] = 1;
            fcontrol->sliding_window_start = next_frame_id(fcontrol->sliding_window_start);
            fcontrol->sliding_window_end = next_frame_id(fcontrol->sliding_window_end); 
        }
    }
    //

    //Verifica timeouts e se um frame já foi confirmado
    for(int idframe = fcontrol->sliding_window_start; idframe <= fcontrol->sliding_window_end; idframe = next_frame_id(idframe))
    {
        if(fcontrol->frame_confirmation_list[idframe] == 0)
        {
            if((fcontrol->frame_timeouts_list[idframe] + TIMEOUT) < clock())
            {              
                fcontrol->frame_confirmation_list[idframe] = 2;
                break;  //retirar no selective repeat arq
            }
        }
        else if(fcontrol->frame_confirmation_list[idframe] == 1)
        {           
            //IDs de frames já confirmados são reciclados
            fcontrol->frame_confirmation_list[idframe] = 2;

            struct frame new_frame;
            new_frame.id_frame = idframe;
            new_frame.fcode = FCODE_USERDATA;
            unsigned char* data_chunk;
            int size = upper_read(here, &data_chunk, MAX_FRAME_SIZE);
            new_frame.datasize = size;
            memcpy(new_frame.data, data_chunk, size); 
            frame_deframe(&new_frame);
            fcontrol->frame_list[idframe] = new_frame; //INFO = dados para transmitir
        }        
    }  

    int send_id = fcontrol->sliding_window_start;
    while(send_id <= fcontrol->sliding_window_end)
    {
        if(fcontrol->frame_confirmation_list[send_id] == 2)
        {
            phisical_send(physical, frame_list[send_id].bytes, frame_list[send_id].n_byte);

            send_frame(send_frame);
            send_id = next_frame_id(send_id);
            fcontrol->frame_confirmation_list[send_id] = 0;
            fcontrol->frame_timeouts_list[send_id] = clock();
            break;
        }
        send_id = next_frame_id(send_id);
    }
}

//Go-Back-N
static int next_frame_id(int frame_index){
    if(frame_index + 1 >= SLIDING_WINDOW_SIZE)
    {
        return 0;
    }
    else 
    {
        return frame_index = frame_index + 1;
    }
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
    struct frame_control* fcontrol = (struct frame_control*) malloc(sizeof(struct frame_control));
    fcontrol->frame_list = (unsigned char**) malloc(sizeof(unsigned char*) * sliding_window_size);
    fcontrol->frame_timeouts_list = (clock_t*) malloc(sizeof(clock_t) * sliding_window_size); 
    fcontrol->frame_confirmation_list = (int*) malloc(sizeof(int) * sliding_window_size); 
    fcontrol->sliding_window_start = 0;
    fcontrol->sliding_window_end = SLIDING_WINDOW_SIZE - 2; //Size = 255 então last = 253 pois precisa ter 1 a menos.
    
    for(int fc_id = 0; fc_id < SLIDING_WINDOW_SIZE; fc_id++)
    {
        fcontrol->frame_confirmation_list[fc_id] = 1;
    }
    
    return &fcontrol;
}

void destroy_frame_control(struct frame_control* fcontrol){
    for(int i = 0; i < SLIDING_WINDOW_SIZE; i++)
    {
        free(fcontrol->frame_list[i])
    }
    free(fcontrol->frame_list);
    free(fcontrol->frame_timeouts_list);
    free(fcontrol->frame_confirmation_list);
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