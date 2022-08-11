#define ESC 27                  //ESC
#define FLAG 64                 //@

#define FRAME_FCODE_INDEX 1
#define FRAME_ID_INDEX 2



#define FRAME_RESERVED_SIZE 3
#define MAX_FRAME_SIZE 512
#define SLIDING_WINDOW_SIZE 255   //(2 ^ n) - 1

#define TIME_BETWEEN_FRAMES 25
#define TIMEOUT 0.5 // deve ser pelo menos 2x TIME_BETWEEN_FRAMES + tempo de processamento + 2x tempo de propagação

#define FCODE_USERDATA 2
#define FCODE_ACK 1
#define FCODE_PIGGYBANKING 3
/*ASCII

ESC = 33 (00011011) 
@ = 64 (01000000)

*/
//Frame code: 11 = Piggybank, 01 = ACK, 10 = Info, outras possibilidas = controle;
struct frame_control
{
    int send_frame;
    int sliding_window_start;
    int sliding_window_end;
    unsigned char** frame_list;    
    clock_t* frame_timeouts_list;
    int* frame_confirmation_list; //0 -> Aguardando confirmação, 1 -> Confirmado, 2 -> Reenvio/envio

};

unsigned char* bitify(struct frame frm);
int bit_to_int(char* bit_string);
struct frame_control* initialize_frame_control();
void destroy_frame_control(struct frame_control* fcontrol);

void GoBackN_Receiver(struct frame* frm, struct link* link)
void GoBackN_Sender(struct frame* frm, struct link* link)

/*
    frame
    int fcode;                        //01 = ACK 10 = Userdata, 11 = piggybank
    char* payload;                    //Info
*/