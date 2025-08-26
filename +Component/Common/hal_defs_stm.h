
/*  ------------------------------------------------------------------------------------------------
                                               Macros
    ------------------------------------------------------------------------------------------------
*/


#define TRUE 1
#define FALSE 0


#ifndef BV
    #define BV(n)      (1 << (n))
#endif


#ifndef MIN
    #define MIN(n,m)   (((n) < (m)) ? (n) : (m))
#endif

#ifndef MAX
    #define MAX(n,m)   (((n) < (m)) ? (m) : (n))
#endif

#ifndef ABS
    #define ABS(n)     (((n) < 0) ? -(n) : (n))
#endif


#define BUILD_UINT8(hiByte, loByte) \
    ((uint8_t)(((loByte) & 0x0F) + (((hiByte) & 0x0F) << 4)))

#define HI_UINT8(a) (((a) >> 4) & 0x0F)
#define LO_UINT8(a) ((a) & 0x0F)



#define BUILD_UINT16(loByte, hiByte) \
    ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)



#define BREAK_UINT32( var, ByteNum ) \
    (uint8_t)((uint32_t)(((var) >>((ByteNum) * 8)) & 0x00FF))

#define BUILD_UINT32(Byte0, Byte1, Byte2, Byte3) \
    ((uint32_t)((uint32_t)((Byte0) & 0x00FF) \
                + ((uint32_t)((Byte1) & 0x00FF) << 8) \
                + ((uint32_t)((Byte2) & 0x00FF) << 16) \
                + ((uint32_t)((Byte3) & 0x00FF) << 24)))


#define st(x)      do { x } while (__LINE__ == -1)

