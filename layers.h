#ifndef __LAYERS_H__
#define __LAYERS_H__

#include "rnd.h"
#include <time.h>

#define MAX_STREAM 1000

struct upper
{
    unsigned char *data;
    size_t         size;
    size_t         read;
    size_t         written;
};

struct physical
{
    int fd;
    rnd_pcg_t random;
    unsigned char *streams[MAX_STREAM];
    size_t sizes[MAX_STREAM];
    uint64_t timeout[MAX_STREAM];
    int total_streams;
};

struct link
{
    struct upper *here;
    struct upper *there;
    struct physical physical;
    struct frame_control* frame_control;
    int frame_size;
    int received;
};

struct frame_control
{
    int send_frame;
    int awaiting_frame_id;     //Receiver Sliding Window
    int sliding_window_start;  //Sender Sliding Window Start
    int sliding_window_end;    //Sender Sliding Window End
    struct frame* frame_list;    
    clock_t* frame_timeouts_list;
    int* frame_confirmation_list; //0 -> Aguardando confirmação, 1 -> Confirmado, 2 -> Reenvio/envio
};
#endif