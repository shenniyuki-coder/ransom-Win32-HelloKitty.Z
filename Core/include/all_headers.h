#ifndef ALL_HEADERS_H
#define ALL_HEADERS_H

// -----------------------
// Standard / STL Headers
// -----------------------
#include <iostream>     // std::cout, std::cerr
#include <fstream>      // std::ifstream, std::ofstream
#include <vector>       // std::vector
#include <filesystem>   // std::filesystem::recursive_directory_iterator
#include <cassert>      // assert

// -----------------------
// Crypto++ Headers
// -----------------------
#include "cryptopp/aes.h"                 // AES block cipher
#include "cryptopp/gcm.h"                 // GCM mode
#include "cryptopp/filters.h"             // StringSource, AuthenticatedEncryptionFilter, VectorSink
#include "cryptopp/osrng.h"               // AutoSeededRandomPool

// -----------------------
// Polymorphic Engine Header
// -----------------------
#include "polymorphic_engine.h"           // polymorphic_encrypt(), polymorphic_decrypt()

#endif // ALL_HEADERS_H
