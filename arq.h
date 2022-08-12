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


unsigned char* bitify(struct frame frm);
int bit_to_int(char* bit_string);
struct frame_control* initialize_frame_control();
void destroy_frame_control(struct frame_control* fcontrol);

int GoBackN_Receiver(struct frame* frame, struct link* link);
int GoBackN_Sender(struct frame* frame, struct link* link);

/*
    frame
    int fcode;                        //01 = ACK 10 = Userdata, 11 = piggybank
    char* payload;                    //Info
*/