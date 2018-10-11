#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base64_encoder.h"
#include "base64_decoder.h"
#include "md5.h"
#include "sha1.h"

int main()
{
	//base64编码解码
	int nEncodeLen;
	char* pEncodeBuffer = base64_encode((const uint8_t*)"Hello World.", strlen("Hello World."), &nEncodeLen);
	printf("Encode[Base64]:%s\n", pEncodeBuffer);

	int nDecodeLen;
	char* pDecodeBuffer = base64_decode((const uint8_t*)pEncodeBuffer, nEncodeLen, &nDecodeLen);
	printf("Decode[Base64]:%s\n", pDecodeBuffer);

	base64_decode_free(pDecodeBuffer);
	base64_encode_free(pEncodeBuffer);
	
	//md5编码解码 - 16个字节的二进制数据(一个字节是2个16进制数，所以是32位16进制数。)
	unsigned char pMd5Buffer[MD5_HASHSIZE];
	md5("Hello World.", strlen("Hello World."), (char*)pMd5Buffer);
	for (int i = 0; i < MD5_HASHSIZE; i++) {
		printf("%x ", pMd5Buffer[i]);
	}
	printf("\n");

	//sha1编码解码
	int nSha1Len;
	unsigned char pSha1Buffer[128];
	crypt_sha1((const uint8_t*)"Hello World.", strlen("Hello World."), pSha1Buffer, &nSha1Len);
	for (int i = 0; i < nSha1Len; i++) {
		printf("%x ", pSha1Buffer[i]);
	}
	printf("\n");

    system("pause");
	return 0;
}