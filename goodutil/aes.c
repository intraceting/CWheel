/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include "aes.h"

size_t good_aes_set_key(AES_KEY *key, const void *pwd, size_t len, uint8_t padding, int encrypt)
{
    uint8_t key_buf[32] = {0};
    size_t key_bits = 0;
    int chk;

    assert(key != NULL && pwd != NULL && len > 0);

    if (len <= 16)
    {
        memset(key_buf, padding, 16);
        memcpy(key_buf, pwd, len);
        key_bits = 128;
    }
    else if (len <= 24)
    {
        memset(key_buf, padding, 24);
        memcpy(key_buf, pwd, len);
        key_bits = 192;
    }
    else if (len <= 32)
    {
        memset(key_buf, padding, 32);
        memcpy(key_buf, pwd, len);
        key_bits = 256;
    }

    if (key_bits)
    {
        if (encrypt)
            chk = AES_set_encrypt_key(key_buf, key_bits, key);
        else
            chk = AES_set_decrypt_key(key_buf, key_bits, key);

        assert(chk == 0);
    }

    return key_bits;
}

size_t good_aes_set_iv(uint8_t *iv, const void *salt, size_t len, uint8_t padding)
{
    size_t iv_bytes = 0;

    assert(iv != NULL && salt != NULL && len > 0);

    if (len <= AES_BLOCK_SIZE)
    {
        memset(iv, padding, AES_BLOCK_SIZE);
        memcpy(iv, salt,len);
        iv_bytes = AES_BLOCK_SIZE;
    }
    else if (len <= AES_BLOCK_SIZE * 2)
    {
        memset(iv, padding, AES_BLOCK_SIZE * 2);
        memcpy(iv, salt, len);
        iv_bytes = AES_BLOCK_SIZE * 2;
    }
    else if (len <= AES_BLOCK_SIZE * 4)
    {
        memset(iv, padding, AES_BLOCK_SIZE * 4);
        memcpy(iv, salt, len);
        iv_bytes = AES_BLOCK_SIZE * 4;
    }

    return iv_bytes;
}

ssize_t good_aes_ecb_encrypt(void *dst, const void *src, size_t len, const AES_KEY *key)
{
    size_t blocks;
    size_t fixlen;
    uint8_t fix_buf[AES_BLOCK_SIZE];

    assert(len >0 && key != NULL);

    blocks = len / AES_BLOCK_SIZE;
    fixlen = len % AES_BLOCK_SIZE;

    if (!dst || !src)
        goto final;

    for (size_t i = 0; i < blocks; i++)
    {
        AES_encrypt(GOOD_PTR2U8PTR(src, i * AES_BLOCK_SIZE),GOOD_PTR2I8PTR(dst, i * AES_BLOCK_SIZE),key);
    }

    if (fixlen > 0)
    {
        memcpy(fix_buf, GOOD_PTR2U8PTR(src, blocks * AES_BLOCK_SIZE), fixlen);
        AES_encrypt(fix_buf, GOOD_PTR2I8PTR(dst, blocks * AES_BLOCK_SIZE), key);
    }

final:

    return good_align(len, AES_BLOCK_SIZE);
}

int good_aes_ecb_decrypt(void *dst, const void *src, size_t len, const AES_KEY *key)
{
    size_t blocks;
    size_t fixlen;

    assert(dst != NULL && src != NULL && len > 0 && key != NULL);

    blocks = len / AES_BLOCK_SIZE;
    fixlen = len % AES_BLOCK_SIZE;

    if (fixlen != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

    for (size_t i = 0; i < blocks; i++)
    {
        AES_decrypt(GOOD_PTR2U8PTR(src, i * AES_BLOCK_SIZE), GOOD_PTR2I8PTR(dst, i * AES_BLOCK_SIZE), key);
    }

    return 1;

final_error:

    return -1;
}

ssize_t good_aes_ige_encrypt(void *dst, const void *src, size_t len,
                             const AES_KEY *key, const uint8_t iv[AES_BLOCK_SIZE * 2])
{
    uint8_t iv_buf[AES_BLOCK_SIZE * 2];

    assert(len >0 && key != NULL && iv != NULL);

    if((len % AES_BLOCK_SIZE) != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

    if (!dst || !src)
        goto final;

    memcpy(iv_buf, iv, AES_BLOCK_SIZE * 2);
    
    AES_ige_encrypt(GOOD_PTR2U8PTR(src, 0),GOOD_PTR2U8PTR(dst, 0),len,key,iv_buf, AES_ENCRYPT);

final:

    return good_align(len, AES_BLOCK_SIZE);

final_error:

    return -1;
}

int good_aes_ige_decrypt(void *dst, const void *src, size_t len,
                         const AES_KEY *key, const uint8_t iv[AES_BLOCK_SIZE * 2])
{
    size_t blocks;
    size_t fixlen;
    uint8_t iv_buf[AES_BLOCK_SIZE * 2];

    assert(dst != NULL && src != NULL && len > 0 && key != NULL && iv != NULL);

    blocks = len / AES_BLOCK_SIZE;
    fixlen = len % AES_BLOCK_SIZE;

    if (fixlen != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

    memcpy(iv_buf, iv, AES_BLOCK_SIZE * 2);

    AES_ige_encrypt(GOOD_PTR2U8PTR(src, 0),GOOD_PTR2U8PTR(dst, 0),len,key,iv_buf, AES_DECRYPT);
    
    return 1;

final_error:

    return -1;
}

ssize_t good_aes_bi_ige_encrypt(void *dst, const void *src, size_t len,
                                const AES_KEY *key, const AES_KEY *key2, const uint8_t iv[AES_BLOCK_SIZE * 4])
{
    uint8_t iv_buf[AES_BLOCK_SIZE * 4];

    assert(len >0 && key != NULL && key2 != NULL && iv != NULL);

    if((len % AES_BLOCK_SIZE) != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

    if (!dst || !src)
        goto final;

    memcpy(iv_buf, iv, AES_BLOCK_SIZE * 4);
    
    AES_bi_ige_encrypt(GOOD_PTR2U8PTR(src, 0),GOOD_PTR2U8PTR(dst, 0),len,key,key2,iv_buf, AES_ENCRYPT);

final:

    return good_align(len, AES_BLOCK_SIZE);

final_error:

    return -1;
}

int good_aes_bi_ige_decrypt(void *dst, const void *src, size_t len,
                            const AES_KEY *key, const AES_KEY *key2, const uint8_t iv[AES_BLOCK_SIZE * 4])
{
    size_t blocks;
    size_t fixlen;
    uint8_t iv_buf[AES_BLOCK_SIZE * 2];

    assert(dst != NULL && src != NULL && len > 0 && key != NULL && key2 != NULL && iv != NULL);

    blocks = len / AES_BLOCK_SIZE;
    fixlen = len % AES_BLOCK_SIZE;

    if (fixlen != 0)
        GOOD_ERRNO_AND_GOTO1(EINVAL, final_error);

    memcpy(iv_buf, iv, AES_BLOCK_SIZE * 2);

    AES_bi_ige_encrypt(GOOD_PTR2U8PTR(src, 0),GOOD_PTR2U8PTR(dst, 0),len,key,key2,iv_buf, AES_DECRYPT);
    
    return 1;

final_error:

    return -1;
}