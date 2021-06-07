/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#ifndef GOODUTIL_OPENSSL_H
#define GOODUTIL_OPENSSL_H

#include "general.h"

/******************************************************************************************************/

#ifdef HEADER_AES_H

/**
 *
 * 设置密钥
 * 
 * @param pwd 密钥的指针。
 * @param len 密钥的长度(字节），1～32之间，自动对齐到16、24、32三类长度，不足部分用padding补齐。
 * @param padding 填充字符。
 * @param encrypt !0 加密密钥，0 解密密钥。
 * 
 * @return > 0 成功(密钥长度(bits))，<= 0 失败。
 * 
*/
size_t good_openssl_aes_set_key(AES_KEY *key, const void *pwd, size_t len, uint8_t padding, int encrypt);

/**
 * 设置向量
 * 
 * @param salt “盐”的指针。
 * @param len “盐”的长度(字节），1～32之间，自动对齐到16、24、32三类长度，不足部分用padding补齐。
 * @param padding 填充字符。
 * 
 * @return > 0 成功(向量长度(字节))，<= 0 失败。
 * 
*/
size_t good_openssl_aes_set_iv(uint8_t *iv, const void *salt, size_t len, uint8_t padding);



#endif //HEADER_AES_H

/******************************************************************************************************/


#ifdef HEADER_RSA_H

/**
 * 计算补充数据(盐)长度(字节)。
 * 
 * @param padding 见RSA_*_PADDING。
 * 
 * @return >=0 成功(长度)，< 0 失败。
*/
int good_openssl_rsa_padding_size(int padding);

/**
 * 创建RSA密钥对象。
 * 
 * @param bits KEY的长度(bits)
 * @param e 指数，见RSA_3/RSA_F4
 * 
 * @return !NULL(0) 成功(对象的指针)，NULL(0) 失败。
*/
RSA *good_openssl_rsa_create(int bits, unsigned long e);

/**
 * 从文件导入密钥，并创建密钥对象。
 * 
 * @param type !0 私钥，0 公钥。
 * @param pwd 密钥密码的指针，NULL(0)忽略。
 * 
 * @return !NULL(0) 成功(对象的指针)，NULL(0) 失败。
*/
RSA *good_openssl_rsa_from_fp(FILE *fp,int type,const char *pwd);

/**
 * 从文件导入密钥，并创建密钥对象。
 * 
 * @return !NULL(0) 成功(对象的指针)，NULL(0) 失败。
*/
RSA *good_openssl_rsa_from_file(const char *file,int type,const char *pwd);

/**
 * 向文件导出密钥。
 * 
 * @param pwd 密钥密码的指针，NULL(0)忽略。私钥有效。
 * 
 * @return > 0 成功，<= 0 失败。
*/
int good_openssl_rsa_to_fp(FILE *fp, RSA *key, int type, const char *pwd);

/**
 * 向文件导出密钥。
 * 
 * @return > 0 成功，<= 0 失败。
*/
int good_openssl_rsa_to_file(const char *file, RSA *key, int type, const char *pwd);

/**
 * 加密。
 * 
 * @param dst 密文的指针。
 * @param src 明文的指针。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功，<= 0 失败。
 * 
 */
int good_openssl_rsa_encrypt(void *dst, const void *src, int len, RSA *key, int type, int padding);

/**
 * 解密。
 * 
 * @param dst 明文的指针。
 * @param src 密文的指针。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功，<= 0 失败。
 * 
 */
int good_openssl_rsa_decrypt(void *dst, const void *src, int len, RSA *key, int type, int padding);

/**
 * 加密，ECB模式。
 * 
 * @param dst 密文的指针，NULL(0) 计算密文的长度。
 * @param src 明文的指针，NULL(0) 计算密文的长度。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功(密文的长度)，<= 0 失败。
 * 
 */
ssize_t good_openssl_rsa_ecb_encrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding);

/**
 * 解密，ECB模式。
 * 
 * @param dst 明文的指针，可用空间至少与密文长度的相等。
 * @param src 密文的指针。
 * @param len 长度。
 * @param key  
 * @param type
 * @param padding 见RSA_*_PADDING。
 * 
 * @return > 0 成功，<= 0 失败。
 */
int good_openssl_rsa_ecb_decrypt(void *dst, const void *src, size_t len, RSA *key, int type, int padding);


#endif //EADER_RSA_H

/******************************************************************************************************/

#ifdef HEADER_HMAC_H

/**
 * HMAC支持的算法。
*/
enum _good_openssl_hmac_type
{
    GOOD_OPENSSL_HMAC_MD2 = 1,
#define GOOD_OPENSSL_HMAC_MD2 GOOD_OPENSSL_HMAC_MD2

    GOOD_OPENSSL_HMAC_MD4 = 2,
#define GOOD_OPENSSL_HMAC_MD4 GOOD_OPENSSL_HMAC_MD4

    GOOD_OPENSSL_HMAC_MD5 = 3,
#define GOOD_OPENSSL_HMAC_MD5 GOOD_OPENSSL_HMAC_MD5

    GOOD_OPENSSL_HMAC_SHA = 4,
#define GOOD_OPENSSL_HMAC_SHA GOOD_OPENSSL_HMAC_SHA

    GOOD_OPENSSL_HMAC_SHA1 = 5,
#define GOOD_OPENSSL_HMAC_SHA1 GOOD_OPENSSL_HMAC_SHA1

    GOOD_OPENSSL_HMAC_SHA224 = 6,
#define GOOD_OPENSSL_HMAC_SHA224 GOOD_OPENSSL_HMAC_SHA224

    GOOD_OPENSSL_HMAC_SHA256 = 7,
#define GOOD_OPENSSL_HMAC_SHA256 GOOD_OPENSSL_HMAC_SHA256

    GOOD_OPENSSL_HMAC_SHA384 = 8,
#define GOOD_OPENSSL_HMAC_SHA384 GOOD_OPENSSL_HMAC_SHA384

    GOOD_OPENSSL_HMAC_SHA512 = 9,
#define GOOD_OPENSSL_HMAC_SHA512 GOOD_OPENSSL_HMAC_SHA512

    GOOD_OPENSSL_HMAC_RIPEMD160 = 10,
#define GOOD_OPENSSL_HMAC_RIPEMD160 GOOD_OPENSSL_HMAC_RIPEMD160

    GOOD_OPENSSL_HMAC_WHIRLPOOL = 11
#define GOOD_OPENSSL_HMAC_WHIRLPOOL GOOD_OPENSSL_HMAC_WHIRLPOOL
};

/**
 * 初始化环境。
 * 
 *  @return 0 成功，!0 失败。
*/
int good_openssl_hmac_init(HMAC_CTX *hmac,const void *key, int len,int type);


#endif //HEADER_HMAC_H

/******************************************************************************************************/


/******************************************************************************************************/

#ifdef HEADER_SSL_H

/**
 * 释放CTX句柄。
 * 
*/
void good_openssl_ctx_freep(SSL_CTX **ctx);

/**
 * 创建CTX句柄。
 * 
 * TLSv1.0              = 10
 * TLSv1.1              = 11
 * TLSv1.2              = 12
 * SSLv2/SSLv3          = 23
 * SSLv3                = 30
 * 
 * @param version 版本号。
 * @param server !0 服务端环境，0 客户端环境。
 * 
 * @return !NULL(0) 成功(句柄)，NULL(0) 失败。
*/
SSL_CTX* good_openssl_ctx_alloc(int version,int server);

/**
 * 加载证书、私钥。
 * 
 * @param cert 证书文件的指针。
 * @param key 私钥文件的指针，NULL(0) 忽略。
 * @param pwd 密码的指针，NULL(0) 忽略。
 * 
 * @return 0 成功(句柄)，-1 失败。
*/
int good_openssl_ctx_load_cert(SSL_CTX *ctx,const char *cert,const char *key,const char *pwd);

/**
 * 释放SSL句柄。
*/
void good_openssl_ssl_freep(SSL **ssl);

/**
 * 创建SSL句柄。
 * 
 * @return !NULL(0) 成功(句柄)，NULL(0) 失败。
*/
SSL *good_openssl_ssl_alloc(SSL_CTX *ctx);

/**
 * 握手。
 * 
 * @param fd 文件或SOCKET句柄。
 * @param timeout 超时(毫秒)。
 * 
 * @return 0 成功(句柄)，-1 失败。
*/
int good_openssl_ssl_handshake(int fd, SSL *ssl, int server, time_t timeout);

#endif //HEADER_SSL_H

#endif //GOODUTIL_OPENSSL_H