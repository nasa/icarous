
#include "port_lib.h"
#include "port_lib_version.h"




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 Port_LibInit(void)
{

    OS_printf ("PORT_LIB Initialized.  Version %d.%d.%d.%d\n",
                PORT_LIB_MAJOR_VERSION,
                PORT_LIB_MINOR_VERSION,
                PORT_LIB_REVISION,
                PORT_LIB_MISSION_REV);

    return CFE_SUCCESS;

}


void InitializeSocketPort(port_t* prt){
    int32_t                     CFE_SB_status;
    uint16_t                   size;

    memset(&prt->self_addr, 0, sizeof(prt->self_addr));
    prt->self_addr.sin_family      = AF_INET;
    prt->self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    prt->self_addr.sin_port        = htons(prt->portin);

    // Open a UDP socket
    if ( (prt->sockId = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        OS_printf("couldn't open socket\n");
    }

    // Bind the socket to a specific port
    if (bind(prt->sockId, (struct sockaddr *)&prt->self_addr, sizeof(prt->self_addr)) < 0) {
        // Handle case where binding failed.
        OS_printf("couldn't bind socket\n");
    }

    // Setup output port address
    memset(&prt->target_addr, 0, sizeof(prt->target_addr));
    prt->target_addr.sin_family      = AF_INET;
    prt->target_addr.sin_addr.s_addr = inet_addr(prt->target);
    prt->target_addr.sin_port        = htons(prt->portout);

    fcntl(prt->sockId, F_SETFL, O_NONBLOCK);
}

int InitializeSerialPort(port_t* prt,bool should_block){

    prt->id = open (prt->target, O_RDWR | O_NOCTTY | O_SYNC);
    if (prt->id < 0)
    {
        OS_printf("Error operning port\n");
        return -1;
    }

    struct termios tty;
    memset (&tty, 0, sizeof tty);
    if (tcgetattr (prt->id, &tty) != 0)
    {
        OS_printf("error in tcgetattr 1\n");
        return -1;
    }

    /* Select baud rate */
    uint64_t brate = B57600;
    switch(prt->baudrate){
        case 9600 : {
             brate = B9600;
             break;
        }
        case 19200 : {
             brate = B19200;
             break;
        }
        case 57600 : {
             brate = B57600;
             break;
        }
        case 115200 : {
             brate = B115200;
             break;
        }
    }
    cfsetospeed (&tty, brate);
    cfsetispeed (&tty, brate);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 /* 8-bit characters */
    tty.c_cflag &= ~PARENB;             /* no parity bit */
    tty.c_cflag &= ~CSTOPB;             /* only need 1 stop bit */
    //tty.c_cflag |= CRTSCTS;         /* Enable hardware flow control */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN]  = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 0.25;                      // 0.5 seconds read timeout


    if (tcsetattr (prt->id, TCSANOW, &tty) != 0)
    {
        OS_printf("error from tcsetattr 2\n");
        return -1;
    }

    OS_printf("Opened serial port %s\n",prt->target);

}

int readPort(port_t* prt){
    int n = 0;
    if (prt->portType == SOCKET){
        memset(prt->recvbuffer, 0, BUFFER_LENGTH);
        if(prt->portout == 0) {
            n = recvfrom(prt->sockId, (void *) prt->recvbuffer, BUFFER_LENGTH, 0, (struct sockaddr *) &prt->target_addr,
                         &prt->recvlen);
            prt->portout = ntohs(prt->target_addr.sin_port);
        }else{
            n = recvfrom(prt->sockId, (void *) prt->recvbuffer, BUFFER_LENGTH, 0, NULL, NULL);
        }
    }else if(prt->portType == SERIAL){
        n = read (prt->id, prt->recvbuffer, BUFFER_LENGTH);
    }else{

    }
    return n;
}

void writeData(port_t* prt,char* sendbuffer,int datalength){
    if(prt->portType == SOCKET){
        int n = sendto(prt->sockId, sendbuffer, datalength, 0, (struct sockaddr*)&prt->target_addr, sizeof (struct sockaddr_in));
    }else if(prt->portType == SERIAL){
        for(int i=0;i<datalength;i++){
            char c = sendbuffer[i];
            write(prt->id,&c,1);
        }
    }else{
        // unimplemented port type
    }
}

void writeMavlinkData(port_t *prt,mavlink_message_t* message){
    char sendbuffer[300];
    uint16_t datalen = mavlink_msg_to_send_buffer((uint8_t*)sendbuffer, message);
    writeData(prt,sendbuffer,datalen);
}

/************************/
/*  End of File Comment */
/************************/
