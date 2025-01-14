#include "crc.h"

uint16_t Modbus_Caluation_CRC16(uint8_t *Buff, uint8_t nSize)
{
       uint16_t m_Crc;
       uint16_t m_InitCrc = 0xffff;
       uint8_t i,j;
       //u8 lCrcBuff,hCrcBuff;
       for(i=0; i<nSize; i++)
       {
              m_InitCrc ^= Buff[i];
              for(j=0; j<8; j++)
              {
                     m_Crc = m_InitCrc;
                     m_InitCrc >>= 1;
                     if(m_Crc&0x0001)
                            m_InitCrc ^= 0xa001;
              }
       }
       return m_InitCrc;
}