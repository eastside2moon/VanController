
//c lib

#include <string.h>
#include <stdlib.h>
#include <time.h>


//mcu



//other


#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"

#include "socket_buf.h"
#include "uart1_print.h"
#include "prj_config.h"

#include "chip.h"

#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "cJSON.h"

#include "net_communicate.h"
#include "utils_hmac.h"
#include "utils_md5.h"
#include "utils_sha1.h"

#include "sht20.h"

#include "mpu6050.h"


#include "led.h"
#include "oled.h"



extern DMA_HandleTypeDef hdma_usart3_rx;
extern ADC_HandleTypeDef hadc1;




uint16_t adc_value[2];



typedef enum
{

    MQTT_LOG,
    MQTT_PING,
    MQTT_CNET,

} mqtt_sts_t;


static  mqtt_sts_t 	mqtt_sts =  MQTT_LOG;
static uint8_t ping_cnt = 0;

static uint16_t  pak_seq_sd =   0;
static uint16_t  pak_seq_rv = 0;


static uint8_t sts_led = 0;




#define   CLEAN_SESSION    1
#define   KEEP_ALIVE   180


#define APP_PAY_LEN   1024
#define APP_BUF_LEN   APP_PAY_LEN+64

static uint8_t payload[APP_PAY_LEN];
static uint16_t payloadlen ;
static unsigned char	 mqtt_buf[APP_BUF_LEN];
static uint16_t	 mqtt_len;




////主要的3个mqtt功能函数
static char Mqtt_FuncPing ( void );
static char Mqtt_FuncLog ( void );
static char Mqtt_FuncPub ( void );

static char Mqtt_process (  uint8_t *buf, int buflen );
static char Mqtt_process_payload (  uint8_t *buf, int buflen );
static char Mqtt_SendJson ( char type, cJSON *mqtt_pay );




void TASK_App ( void const *argument );


//应用任务----------------------------------------
void TASK_App ( void const *argument )
{
    CHIP_RstFlagInit();



    OLED_Init();
    OLED_DisStringFmt (	1,  "RVcontroller" );
    OLED_DisStringFmt (	2, "%s", DEVICENAME	);
    OLED_DisStringFmt (	3,  "3333333333333333"	);
    OLED_DisStringFmt (	4,  "4444444444444444"	);


    if(MPU6050_Exist() == 1)
        USART1_Printf( "MPU6050: Ok\r\n", strlen("MPU6050: Ok\r\n"), 20);
    else
        USART1_Printf( "MPU6050: Err\r\n", strlen("MPU6050: Err\r\n"), 20);

    if(mpu6050_info.device_ok)
    {
        if(MPU6050_Init() == 0)
            USART1_Printf( "MPU6050_Init Ok\r\n", strlen("MPU6050_Init Ok\r\n"), 20);
    }



    while  ( NET_Status() == 0 )
    {
        //    USART1_Printf ( "\r\n wait network"    );
        osDelay ( 2000 );
    }


    for ( ;; )
    {

        OLED_DisStringFmt (	3, "Rssi:%d", LTE_ReadRssi()	);
        OLED_DisStringFmt (	4, "SeqRv:%d", pak_seq_rv	);

		
        if (    mqtt_sts ==  MQTT_CNET      )
        {

            TickType_t xLastWakeTime;
            xLastWakeTime = xTaskGetTickCount ();

            ////////////////////

            mqtt_sts =  MQTT_LOG ;      /////  如果发数据失败就去重新登录
            USART1_Printf (	"\r\n pub111"	);

            Mqtt_FuncPub();
            //////////

            vTaskDelayUntil( &xLastWakeTime,  5 * 1000 );         ///  5秒一次

        }

        else if (    mqtt_sts ==  MQTT_PING      )           ////发送失败还是先ping一下，不要直接重连
        {

            mqtt_sts =  MQTT_LOG ;
            Mqtt_FuncPing();     /////  如果ping不通就去log
            osDelay (  2000  );           //////2秒
        }

        else
            /////尝试登录
        {

            mqtt_sts =  MQTT_PING ;
            ////////配置到服务器地址
            Socket_Start (	TYPE_TCP,	PRODUCTKEY ".iot-as-mqtt.cn-shanghai.aliyuncs.com"	, "1883"	);
            Mqtt_FuncLog();
            osDelay ( 2000  );           //////2秒一次
        }
    }
}





static char Mqtt_FuncLog ( void )
{
    char temp_ret;
    MQTTPacket_connectData  ConnectData =
    {
        {'M', 'Q', 'T', 'C'}, 0, 4, {NULL, {0, NULL }},
        KEEP_ALIVE, CLEAN_SESSION, 0,
        MQTTPacket_willOptions_initializer,
        {NULL, {0, NULL }}, { NULL, {0, NULL }}
    };
    ///////初始化 MQTT登录信息12
    char 	temp[128];
    char 	clientid[128];
    char 	Passward[128];

    ///////初始化 MQTT登录信息

    memset ( clientid, 0, 128 );
    sprintf ( clientid, "%.24s|securemode=3,signmethod=hmacsha1|", CHIP_GetId() );
    ConnectData.clientID.cstring  = clientid;
    ConnectData.username.cstring 	= 	 DEVICENAME"&"PRODUCTKEY;
    memset ( temp, 0, 128 );                                                                 //临时缓冲区全部清零
    memset ( Passward, 0, 128 );                                                                 //临时缓冲区全部清零
    //构建加密时的明文

    sprintf ( temp, "clientId%.24sdeviceName%sproductKey%s", CHIP_GetId(), DEVICENAME, PRODUCTKEY ); //构建加密时的明文
    utils_hmac_sha1 ( temp, strlen ( temp ), Passward, DEVICESECRE, DEVICESECRE_LEN );       //以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
                                 
    ConnectData.password.cstring 	= 	Passward;

    USART1_Printf (  "\r\nLog Req" );
    mqtt_len = MQTTSerialize_connect ( &mqtt_buf[0], APP_BUF_LEN, &ConnectData );
    temp_ret = Socket_SendData (    ( char * ) &mqtt_buf[0],   mqtt_len, 0  );


    return temp_ret;
}





static char Mqtt_FuncPub ( void )
{
    char temp_ret;

    cJSON 	*pub_json, *temp_json, *temp_json1  ;
    ///////////////////////////////////////////////////////////////////////////////////
    //  SHT20_GetValue();
    SHT20_GetValue();
    MPU6050_Read_Accel_Gyro();
    {
        // 1.发送常规数据，用type3，没有时间戳
        pub_json = cJSON_CreateObject();


        USART1_Printf (  "\r\n pub_json: %P",   pub_json  );
        cJSON_AddStringToObject ( pub_json, "method", "thing.event.property.post" );
        cJSON_AddItemToObject (  pub_json,  "params",   temp_json = cJSON_CreateObject() );
        /////////////////////////// //////公共信息

		
        cJSON_AddNumberToObject ( temp_json, "WorkSwitch", 1  );		
		    cJSON_AddNumberToObject ( temp_json, "Led_sts", sts_led  );
			
        cJSON_AddNumberToObject ( temp_json, "Temperature", sht20_info.tempreture  );
        cJSON_AddNumberToObject ( temp_json, "Humidity", sht20_info.humidity  );

	/////  添加坐标数据	
        cJSON_AddItemToObject (  temp_json,  "GeoLocation",   temp_json1 = cJSON_CreateObject() );	

	
		
        cJSON_AddNumberToObject ( temp_json1, "longitude", 106.470637 );
        cJSON_AddNumberToObject ( temp_json1, "latitude",   29.625926  );
        cJSON_AddNumberToObject ( temp_json1, "CoordinateSystem", 1 );

	


        cJSON_AddNumberToObject ( temp_json, "Light", HAL_ADC_GetValue(	&hadc1	) );
        cJSON_AddNumberToObject ( temp_json, "Axis_X",	-mpu6050_info.accel[0] );
        cJSON_AddNumberToObject ( temp_json, "Axis_Y",	-mpu6050_info.accel[1] );
        cJSON_AddNumberToObject ( temp_json, "Axis_Z", 	-mpu6050_info.accel[2]  );
        HAL_ADC_Start(	&hadc1	);


        //封装，发送
        temp_ret =    Mqtt_SendJson ( 3, pub_json );


    }
    return temp_ret;
}


static char Mqtt_FuncPing ( void )
{
    char temp_ret;
    USART1_Printf ( "\r\nPing Req" );

    if (   ++ping_cnt > 3  )
    {
        ping_cnt = 0;
        mqtt_sts =  MQTT_LOG;
    }

    mqtt_len = MQTTSerialize_pingreq ( &mqtt_buf[0], APP_BUF_LEN );
    temp_ret = Socket_SendData (    ( char * ) &mqtt_buf[0],   mqtt_len, 0  );

    return temp_ret;
}








char Mqtt_SendJson ( char type, cJSON *mqtt_pay )
{
    char temp_ret = 1;
    static   char 	*mqtt_body;
    ////////////这里非常重要，一定要初始化里面的长度值为0 ，不然后面封装的时候就乱了，容易搞乱内存
    ///    MQTTString topicString = {"$dp", {0, NULL }};
    MQTTString topicString = {"/sys/"PRODUCTKEY"/"DEVICENAME"/thing/event/property/post", {0, NULL }};
    //////////////////////////////////////////////////////////
    ////  1.把json转换成字符串，然后删除json
    //	USART1_Printf (  "\r\n pub_111111"    );
    mqtt_body	=   cJSON_Print ( mqtt_pay );
    cJSON_Delete ( mqtt_pay );
    //////////////////////////////////////////////////////////
    memcpy ( &payload[0], mqtt_body,    strlen ( mqtt_body )  );
    payloadlen = 	strlen ( mqtt_body ) ;
    /////////////////////////////////////////
    mqtt_len = MQTTSerialize_publish ( &mqtt_buf[0],   APP_BUF_LEN, 0, 1, 0, pak_seq_sd++, topicString, ( uint8_t * ) payload, payloadlen );
    free ( mqtt_body );
    temp_ret = Socket_SendData (    ( char * ) &mqtt_buf[0],   mqtt_len, 0  );
    return temp_ret;
}






char Mqtt_process (  uint8_t *buf, int buflen )
{
    int index = 0;
    int rem_length = 0;
    MQTTHeader header = {0};
    header.byte = buf[index++];
    index += MQTTPacket_decodeBuf ( ( unsigned char * ) &buf[index], &rem_length );
    //                 USART1_Printf (  "\r\n type %d",     header.bits.type );
    USART1_PrintfBuf ( ( uint8_t * ) buf,  buflen );

    ///////////////////////////////////////////////////////////////////////
    switch ( header.bits.type )
    {
    case PINGRESP:
        USART1_Printf (  "\r\n Ping_OK"  );
        mqtt_sts =  MQTT_CNET;
        //                _MAIN_APP_RSM;
        break;

    case  CONNACK:
    {
        unsigned char sessionPresent, connack_rc;

        if ( MQTTDeserialize_connack ( &sessionPresent, &connack_rc, buf, buflen ) == 1 )
        {
            ///登录成功后订阅
            if ( connack_rc == 0x00 )
            {
                USART1_Printf (  "\r\nLog OK"  );
                mqtt_sts =  MQTT_CNET;
                //            _MAIN_APP_RSM;
            }

            else
            {
                USART1_Printf (  "\r\nLog failed " );
            }
        }
    }
    break;

    case SUBACK:   ///订阅成功后就去PUB数据点，
    {
        unsigned short packetid;
        int maxcount = 1, count = 0;
        int grantedQoSs[1];

        if ( MQTTDeserialize_suback ( &packetid, maxcount, &count, grantedQoSs, buf, buflen ) == 1 )
        {
            USART1_Printf (  "\r\nSub OK " );
            mqtt_sts =  MQTT_CNET;

        }
    }
    break;

    case PUBACK:
    {
        unsigned char packettype, dup;
        unsigned short packetid;

        if ( MQTTDeserialize_ack ( &packettype, &dup, &packetid, buf, buflen ) == 1 )
        {
            pak_seq_rv = packetid;
            mqtt_sts =  MQTT_CNET;
            USART1_Printf (  "\r\n Pub_ok: %d ", pak_seq_rv );
        }
    }
    break;

    case PUBLISH:    //收到PUBLISH  可能是命令，可能是订阅的消息,反正就是下发下来的
    {
        unsigned char dup, retained, *payload;
        unsigned short packetid;
        int qos, payloadlen;
        //           static 	unsigned short rcv_cnt = 0;
        MQTTString topicName = MQTTString_initializer;

        if ( MQTTDeserialize_publish ( &dup, &qos, &retained, &packetid, &topicName,
                                       &payload, &payloadlen, buf, buflen ) == 1 )
        {
            ///////////////QOS1  需要回复确认
            if ( qos == 1 )
            {
                mqtt_len = 	MQTTSerialize_puback ( &mqtt_buf[0], APP_BUF_LEN, packetid );
                Socket_SendData (    ( char * ) &mqtt_buf[0],   mqtt_len, 0 );
            }

            /////通过串口打印出来
            USART1_Printf (  "\r\nRcv:%d" , payloadlen  );
            USART1_Printf (  "\r\n%.*s" ,  payloadlen, payload  );
            ////通过屏幕显示出来
//            OLED_DisStringFmt (		1,  "Rcv:%d" , payloadlen   );
//            OLED_DisStringFmt (		2,  "%.*s" ,  payloadlen, payload  );
//            LED_Blink ( HAL_BEEP, 1, 2 );
           Mqtt_process_payload(    payload, payloadlen     );
        }
//extern 	osThreadId_t myTask_appHandle;	
//		
//		 xTaskAbortDelay ( myTask_appHandle );
        break;
    }

    default:
        break;
    }

    return 0;
}


char Mqtt_process_payload (  uint8_t *buf, int buflen )
{

cJSON   *Alink_JSON ,*temp_json ,*params_json;

			
            /////通过串口打印出来
        //    USART1_Printf (  "\r\nRcv:%d" , payloadlen  );
      //      USART1_Printf (  "\r\n%s" ,  buf  );
            ////通过屏幕显示出来
//            OLED_DisStringFmt (		1,  "Rcv:%d" , payloadlen   );
//      
//            LED_Blink ( HAL_BEEP, 1, 2 );
	

	
	Alink_JSON =   cJSON_Parse(  (const char *) buf	);

	
	
	
	
	
if (  Alink_JSON   )
{
//  OLED_DisStringFmt (		2,  "%.16s" ,  buf   );
	temp_json =   cJSON_GetObjectItem(Alink_JSON,  "params");

  USART1_Printf (  "\r\n%s" ,  cJSON_Print(  temp_json )  );
// OLED_DisStringFmt (		2,  "%.16s" , cJSON_Print(  temp_json ) );

	
	
	
/////如果是	LED命令
params_json =   cJSON_GetObjectItem(temp_json,  "Led_sts");

	if (  params_json   )
    {

	sts_led   =   params_json->valueint;
	 LED_Set ( HAL_LED_4,    sts_led );
			
    }
	
///////如果是	风扇命令
//params_json =   cJSON_GetObjectItem(temp_json,  "Fan_sts");

//	if (  params_json   )
//    {
//	sts_fan  =   params_json->valueint;
//HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, sts_fan); 	
//		
//    }	
///////如果是	风扇命令
//params_json =   cJSON_GetObjectItem(temp_json,  "Volume");

//	if (  params_json   )
//    {
//			sts_volume  =   params_json->valueint;
//				
//    }
	

}
	





	
	cJSON_Delete(Alink_JSON);
	
	return 0;
}


void App_RcvData (   uint8_t *app_data,       uint16_t len )
{

    Mqtt_process (  app_data, len );


    USART1_Printf (  "\r\nRcvapp:%d" , len  );


}




