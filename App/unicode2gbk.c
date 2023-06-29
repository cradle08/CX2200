/*------------------------------------------------------------------------*/
/* Unicode - OEM code bidirectional converter  (C)ChaN, 2015              */
/* CP936 (Simplified Chinese GBK)                                         */
/*------------------------------------------------------------------------*/

#include "unicode2gbk.h"
#include "Common.h"


#if USE_RL_FLASH_FS



//将UNI2GBK码表加载到SDRAM中
uint16_t* uni2gbk = (uint16_t*)SDRAM_UNI2GBK_ADDR;
uint32_t Uni2GbkLen = 87172;


/*
*	单个unicode 转 gbk
*/
static uint16_t UnicodeToGBK_One(uint16_t chr)
{
	const uint16_t *p;
	uint16_t c;
	int i, n, li, hi;

	if (chr < 0x80) {	/* ASCII */
		c = chr;
	} else {
		/* Unicode to gbk code */
		p = uni2gbk;
		hi = Uni2GbkLen / 4 - 1;
		
		//
		li = 0;
		for (n = 16; n; n--) {
			i = li + (hi - li) / 2;
			if (chr == p[i * 2]) break;
			if (chr > p[i * 2])
				li = i;
			else
				hi = i;
		}
		c = n ? p[i * 2 + 1] : 0;
	}

	return c;
}


#else
    #include "ff.h"

#endif



/*
*   unicode 转 GBK
*/
void UnicodeToGBK(uint16_t *pGBK, uint16_t *pUncodeBuffer)
{
   
    uint16_t usTemp = 0;
    uint8_t *pIndex = (uint8_t*)pGBK;
    
    while(*pUncodeBuffer != 0)
    {
#if USE_RL_FLASH_FS
        usTemp = UnicodeToGBK_One(*pUncodeBuffer);
#else
        usTemp = ff_convert(*pUncodeBuffer, 0);
#endif
        if(usTemp > 0x80)
        {
            *pIndex = usTemp >> 8;
             pIndex++;
            *pIndex = usTemp;
             pIndex++;
        }else{
           *pIndex = usTemp;
            pIndex++;
        }
        pUncodeBuffer++;
    }
}



/*
*   UTF-8  转为 unicode
*/
void UTFToUnicode(uint16_t *pUncodeBuffer, uint8_t *pUTF8Buffer)
{
    uint8_t *pUncode = (uint8_t *)pUncodeBuffer;
    
    while(*pUTF8Buffer != 0)
    {
        if(*pUTF8Buffer > 0x80)
        {
            *pUncode = ((pUTF8Buffer[1] & 0x03)<<6) | (pUTF8Buffer[2] & 0x3F);
             pUncode++;
            *pUncode = ((pUTF8Buffer[0] & 0x0F)<<4) | ((pUTF8Buffer[1] & 0x3C)>>2);
   
            pUTF8Buffer += 3;
        }else{
            //*pUncodeBuffer = *pUTF8Buffer;
             //pUTF8Buffer++;        
             *pUncode = *pUTF8Buffer;
              pUncode++;
             *pUncode = 0;
              pUncode++;
            
             pUTF8Buffer++; 
        }
        pUncodeBuffer++;
        pUncode = (uint8_t *)pUncodeBuffer;      
    }
}






