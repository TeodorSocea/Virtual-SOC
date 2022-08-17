#pragma once
#include <iostream>
#include <openssl/evp.h>

char *base64(const unsigned char *input, int length);
unsigned char *decode64(const char *input, int length);