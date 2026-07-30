#ifndef HAMMINGCORE_H
#define HAMMINGCORE_H

#include <vector>
#include <cstdint>

class HammingCore {
public:
    // f bloğu: Veriyi (Data In) alır, K bitlerini hesaplar ve belleğe yazılacak M+K uzunluğundaki diziyi döner.
    static std::vector<uint8_t> generateCodeword(const std::vector<uint8_t>& dataBits);

    // Compare bloğu: Bellekten okunan veriyi alır ve sendrom kelimesini (tam sayı olarak) hesaplar.
    static int calculateSyndrome(const std::vector<uint8_t>& memoryData);

    // Corrector bloğu: Hata sinyali varsa (sendrom > 0), ilgili biti tersine çevirerek düzeltir.
    static void correctError(std::vector<uint8_t>& memoryData, int syndrome);

private:
    // Yardımcı alt fonksiyonlar
    static bool isPowerOfTwo(int n);
    static int calculateParityBits(int dataSize);
};

#endif // HAMMINGCORE_H