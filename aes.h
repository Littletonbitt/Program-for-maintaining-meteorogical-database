#ifndef __AES_H__
#define __AES_H__

#include <iostream>
#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

class Myaes{
	public:
		Myaes();
		Myaes(const std::string& key_str, const std::string& iv_str);
		~Myaes();
		std::string encrypt(const std::string& b);
		std::string decrypt(const std::string& e);
		void handleErrors();
	private:
		std::vector<unsigned char> key;
		std::vector<unsigned char> iv;
};
#endif
