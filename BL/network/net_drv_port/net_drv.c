#include "net_drv.h"
extern ring_buffer_t rb;

/*connect to server ,this func will be call in a while block*/
int connect_to_TCPServer(){


}

/*send msg to TCPServer,this fuc will be call in network hadler*/
int TCP_send_msg(){
    

}

/*throw msg to ring buffer,this fuc should be call in a interrupt*/
int TCP_read_msg(ring_buffer_t* rb){

    /*
    int data;
    int len;
    len=get_msg_from_server(data);
    ring_buffer_write(rb,data,len);
    */
}



