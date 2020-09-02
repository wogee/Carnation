#ifndef _GPIO_H_
#define _GPIO_H_


#define OUTPUT             1            //���
#define INPUT              0            //����

#define HIGH              1           // �ߵ�ƽ
#define LOW               0           // �͵�ƽ



void 	GpioSetDir(uint16_t io, uint8_t dir);
void 	GpioSetLev(uint16_t io,uint8_t lev);
uint8_t 	GpioGetLev(uint16_t io);




#endif

