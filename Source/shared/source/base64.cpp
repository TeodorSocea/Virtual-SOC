#include "../headers/base64.h"

//credit to user mtrw on StackOverflow for the encode and decode functions
char *base64(const unsigned char *input, int length)
{
    const auto pl = 4 * ((length + 2) / 3);
    auto output = reinterpret_cast<char *>(calloc(pl + 1, 1)); //+1 for the terminating null that EVP_EncodeBlock adds on
    const auto ol = EVP_EncodeBlock(reinterpret_cast<unsigned char *>(output), input, length);
    if (pl != ol)
    {
        std::cerr << "Whoops, encode predicted " << pl << " but we got " << ol << "\n";
    }
    return output;
}

unsigned char *decode64(const char *input, int length)
{
    const auto pl = 3 * length / 4;
    auto output = reinterpret_cast<unsigned char *>(calloc(pl + 1, 1));
    const auto ol = EVP_DecodeBlock(output, reinterpret_cast<const unsigned char *>(input), length);
    if (pl != ol)
    {
        std::cerr << "Whoops, decode predicted " << pl << " but we got " << ol << "\n";
    }
    return output;
}