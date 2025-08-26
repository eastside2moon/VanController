



#define   TYPE_TCP   "TCP"
#define   TYPE_UDP   "UDP"





typedef struct server_info_t
{
    char *addr;
    char *port;

} server_info_t;



uint16_t *SOCKET_SendLen ( void );
char *SOCKET_SendBuf ( void );


///////////////////////////////////////////////

uint8_t Socket_SendData ( char *payload, uint16_t len, uint8_t clog  );

//////////////////////////////////////////////////////


uint8_t app_TcpSts ( uint8_t sts );


server_info_t  *SERVER_Info ( void );
uint8_t Socket_Start ( char *type, char *addr, char *port );



