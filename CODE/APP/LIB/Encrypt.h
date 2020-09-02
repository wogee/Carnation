#ifndef _ENCRYPT_H_
#define _ENCRYPT_H_

//���ݼ���
void Encrypt(const uint8_t* input, const uint8_t* key, uint8_t *output, const uint32_t length);

//���ݽ���
void Decrypt(const uint8_t* input, const uint8_t* key, uint8_t *output, const uint32_t length);

//�ͻ����ת��Կ
uint64_t CalcKey(uint32_t nCustomerID);

#endif
