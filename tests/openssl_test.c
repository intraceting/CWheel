/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include "goodutil/getargs.h"
#include "goodutil/rsa.h"
#include "goodutil/aes.h"
#include "goodutil/ssl.h"
#include "goodutil/socket.h"

#ifdef HEADER_RSA_H

void test_rsa(good_tree_t *opt)
{
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    ERR_load_crypto_strings();
    ERR_load_RSA_strings();

    RSA *key = NULL;
    RSA *prikey = NULL;
    RSA *pubkey = NULL;
    int chk;
#if 1
    prikey = good_rsa_from_file(
        good_option_get(opt, "--rsa-key-prifile", 0, ""),
        1,
        good_option_get(opt, "--rsa-key-pwd", 0, ""));
    pubkey = good_rsa_from_file(
        good_option_get(opt, "--rsa-key-pubfile", 0, ""),
        0,
        //good_option_get(opt, "--rsa-key-pwd", 0, ""));
        NULL);

    assert(prikey && pubkey);
#else

        key = good_rsa_create(2048,RSA_F4);

        int chk = good_rsa_to_file(
            good_option_get(opt, "--rsa-key-prifile", 0, ""),
            key,
            1,
            good_option_get(opt, "--rsa-key-pwd", 0, ""));

        assert(chk > 0);

        chk = good_rsa_to_file(
            good_option_get(opt, "--rsa-key-pubfile", 0, ""),
            key,
            0,
            good_option_get(opt, "--rsa-key-pwd", 0, ""));

        assert(chk > 0);

#endif 

    char *buf1 = (char*) good_heap_alloc(1000);
    char *buf2 = (char*) good_heap_alloc(2000);
    char *buf3 = (char*) good_heap_alloc(2000);

    memset(buf1,'a',1000);

    ssize_t m = good_rsa_ecb_encrypt(buf2,buf1,1000,pubkey,0,RSA_NO_PADDING);

    int e = ERR_get_error();
    char szErrMsg[1024] = {0};
    char *pTmp = NULL;
    pTmp = ERR_error_string(e,szErrMsg); 

    printf("%s\n",pTmp);

    assert(m>0);

    chk = good_rsa_ecb_decrypt(buf3,buf2,m,prikey,1,RSA_NO_PADDING);

    assert(chk >0);



    assert(memcmp(buf1,buf3,1000)==0);


    good_heap_free(buf1);
    good_heap_free(buf2);
    good_heap_free(buf3);


    RSA_free(key);
    RSA_free(prikey);
    RSA_free(pubkey);
}

#endif //HEADER_RSA_H

#ifdef HEADER_AES_H

void test_aes(good_tree_t *opt)
{
    AES_KEY ek,dk;
    AES_KEY ek2,dk2;
    uint8_t iv[AES_BLOCK_SIZE * 4] = {0};

    assert(good_aes_set_key(&ek,"abcde",5,9,1)>0);
    assert(good_aes_set_key(&dk,"abcde",5,9,0)>0);

    assert(good_aes_set_key(&ek2,"qwert",5,9,1)>0);
    assert(good_aes_set_key(&dk2,"qwert",5,9,0)>0);

    assert(good_aes_set_iv(iv,"12345789344wewqerwreqwer",20,9)>0);

    char *buf1 = (char*) good_heap_alloc(1000);
    char *buf2 = (char*) good_heap_alloc(2000);
    char *buf3 = (char*) good_heap_alloc(2000);

    memset(buf1,'a',1000);

    int len = AES_BLOCK_SIZE * 16;
    AES_bi_ige_encrypt(buf1,buf2,len,&ek,&ek2,iv,AES_ENCRYPT);

    AES_bi_ige_encrypt(buf2,buf3,len,&dk,&dk2,iv,AES_DECRYPT);


    assert(memcmp(buf1,buf3,len)==0);


   good_heap_free(buf1);
    good_heap_free(buf2);
    good_heap_free(buf3);

}

#endif //HEADER_AES_H

#ifdef HEADER_SSL_H

void test_ssl(good_tree_t *opt)
{
   SSL_library_init();
   OpenSSL_add_all_algorithms();
   SSL_load_error_strings();  

    SSL_CTX * ctx = good_ssl_ctx_alloc(11,0);

    int chk = good_ssl_ctx_load_cert(ctx, NULL,
                                     good_option_get(opt, "--rsa-key-prifile", 0, ""),
                                     good_option_get(opt, "--rsa-key-pwd", 0, ""));

    assert(chk == 0);

    SSL* s = good_ssl_alloc(ctx);


    good_sockaddr_t addr={0};
    assert(good_sockaddr_from_string(&addr,"www.taobao.com:443",1)==0);

    int c = good_socket(addr.family,0);
    
    assert(good_connect(c,&addr,10000)==0);

    assert(good_ssl_handshake(c,s,0,10000)==0);


    good_closep(&c);

    good_ssl_freep(&s);

    good_ssl_ctx_freep(&ctx);
}

#endif //HEADER_SSL_H

int main(int argc, char **argv)
{

    good_tree_t *t = good_tree_alloc(NULL);

    good_getargs(t,argc,argv,"--");

#ifdef HEADER_RSA_H

    test_rsa(t);
    
#endif //HEADER_RSA_H

#ifdef HEADER_AES_H

    test_aes(t);

#endif //HEADER_AES_H

#ifdef HEADER_SSL_H

    test_ssl(t);

#endif //HEADER_SSL_H

    good_tree_free(&t);

    return 0;
}