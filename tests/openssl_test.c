/*
 * This file is part of ABTK.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include "abtkutil/getargs.h"
#include "abtkutil/openssl.h"
#include "abtkutil/socket.h"


#ifdef HEADER_RSA_H

void test_rsa(abtk_tree_t *opt)
{
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    ERR_load_crypto_strings();
    ERR_load_RSA_strings();

    RSA *key = NULL;
    RSA *prikey = NULL;
    RSA *pubkey = NULL;
    int chk;
#if 0
    prikey = abtk_openssl_rsa_from_file(
        abtk_option_get(opt, "--rsa-key-prifile", 0, ""),
        1,
        abtk_option_get(opt, "--rsa-key-pwd", 0, ""));
    pubkey = abtk_openssl_rsa_from_file(
        abtk_option_get(opt, "--rsa-key-pubfile", 0, ""),
        0,
        //abtk_option_get(opt, "--rsa-key-pwd", 0, ""));
        NULL);

    assert(prikey && pubkey);
#else

        key = abtk_openssl_rsa_create(2048,RSA_F4);

        chk = abtk_openssl_rsa_to_file(
            abtk_option_get(opt, "--rsa-key-prifile", 0, ""),
            key,
            1,
            abtk_option_get(opt, "--rsa-key-pwd", 0, ""));

        assert(chk > 0);

        chk = abtk_openssl_rsa_to_file(
            abtk_option_get(opt, "--rsa-key-pubfile", 0, ""),
            key,
            0,
            abtk_option_get(opt, "--rsa-key-pwd", 0, ""));

        assert(chk > 0);

#endif 

    char *buf1 = (char*) abtk_heap_alloc(1000);
    char *buf2 = (char*) abtk_heap_alloc(2000);
    char *buf3 = (char*) abtk_heap_alloc(2000);

    memset(buf1,'a',1000);

    ssize_t m = abtk_openssl_rsa_ecb_encrypt(buf2,buf1,1000,pubkey,0,RSA_NO_PADDING);

    int e = ERR_get_error();
    char szErrMsg[1024] = {0};
    char *pTmp = NULL;
    pTmp = ERR_error_string(e,szErrMsg); 

    printf("%s\n",pTmp);

    assert(m>0);

    chk = abtk_openssl_rsa_ecb_decrypt(buf3,buf2,m,prikey,1,RSA_NO_PADDING);

    assert(chk >0);



    assert(memcmp(buf1,buf3,1000)==0);


    abtk_heap_free(buf1);
    abtk_heap_free(buf2);
    abtk_heap_free(buf3);


    RSA_free(key);
    RSA_free(prikey);
    RSA_free(pubkey);
}

#endif //HEADER_RSA_H

#ifdef HEADER_AES_H

void test_aes(abtk_tree_t *opt)
{
    AES_KEY ek,dk;
    AES_KEY ek2,dk2;
    uint8_t iv[AES_BLOCK_SIZE * 4] = {0};

    assert(abtk_openssl_aes_set_key(&ek,"abcde",5,9,1)>0);
    assert(abtk_openssl_aes_set_key(&dk,"abcde",5,9,0)>0);

    assert(abtk_openssl_aes_set_key(&ek2,"qwert",5,9,1)>0);
    assert(abtk_openssl_aes_set_key(&dk2,"qwert",5,9,0)>0);

    assert(abtk_openssl_aes_set_iv(iv,"12345789344wewqerwreqwer",20,9)>0);

    char *buf1 = (char*) abtk_heap_alloc(1000);
    char *buf2 = (char*) abtk_heap_alloc(2000);
    char *buf3 = (char*) abtk_heap_alloc(2000);

    memset(buf1,'a',1000);

    int len = AES_BLOCK_SIZE * 16;
    AES_bi_ige_encrypt(buf1,buf2,len,&ek,&ek2,iv,AES_ENCRYPT);

    AES_bi_ige_encrypt(buf2,buf3,len,&dk,&dk2,iv,AES_DECRYPT);


    assert(memcmp(buf1,buf3,len)==0);


   abtk_heap_free(buf1);
    abtk_heap_free(buf2);
    abtk_heap_free(buf3);

}

#endif //HEADER_AES_H

#ifdef HEADER_SSL_H

void test_ssl(abtk_tree_t *opt)
{
   SSL_library_init();
   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();  
   const SSL_METHOD *method = TLSv1_2_client_method();

    SSL_CTX * ctx = SSL_CTX_new(method);

    int chk = abtk_openssl_ctx_load_cert(ctx, NULL,
                                     abtk_option_get(opt, "--rsa-key-prifile", 0, ""),
                                     abtk_option_get(opt, "--rsa-key-pwd", 0, ""));

    assert(chk == 0);

    SSL* s = abtk_openssl_ssl_alloc(ctx);


    // abtk_sockaddr_t addr={0};
    // assert(abtk_sockaddr_from_string(&addr,"www.taobao.com:443",1)==0);

    // int c = abtk_socket(addr.family,0);
    
    // assert(abtk_connect(c,&addr,10000)==0);

    // assert(abtk_openssl_ssl_handshake(c,s,0,10000)==0);


    // abtk_closep(&c);

    abtk_openssl_ssl_freep(&s);

    SSL_CTX_free(ctx);
}

#endif //HEADER_SSL_H

#ifdef HEADER_HMAC_H

void test_hmac(abtk_tree_t *opt)
{
    HMAC_CTX hmac;

    /*123456*/
    assert(abtk_openssl_hmac_init(&hmac,"123456",6,ABTK_OPENSSL_HMAC_SHA256)==0);

    HMAC_Update(&hmac,"123456",6);

    uint8_t buf[100]={0};
    int len;

    HMAC_Final(&hmac,buf,&len);

    for(int i = 0;i<len;i++)
        printf("%02x",buf[i]);
    printf("\n");

    HMAC_CTX_cleanup(&hmac);

    HMAC_CTX hmac2;

    /*123457*/
    assert(abtk_openssl_hmac_init(&hmac2,"123457",6,ABTK_OPENSSL_HMAC_SHA256)==0);

    HMAC_Update(&hmac2,"123456",6);

    uint8_t buf2[100]={0};
    int len2;

    HMAC_Final(&hmac2,buf2,&len2);

    for(int i = 0;i<len2;i++)
        printf("%02x",buf2[i]);
    printf("\n");

    HMAC_CTX_cleanup(&hmac2);

}

#endif //HEADER_HMAC_H

int main(int argc, char **argv)
{

    abtk_tree_t *t = abtk_tree_alloc(NULL);

    abtk_getargs(t,argc,argv,"--");

#ifdef HEADER_RSA_H

    test_rsa(t);
    
#endif //HEADER_RSA_H

#ifdef HEADER_AES_H

    test_aes(t);

#endif //HEADER_AES_H

#ifdef HEADER_SSL_H

    test_ssl(t);

#endif //HEADER_SSL_H

#ifdef HEADER_HMAC_H

    test_hmac(t);

#endif //HEADER_HMAC_H

    abtk_tree_free(&t);

    return 0;
}