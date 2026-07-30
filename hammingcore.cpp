#include "HammingCore.h"
#include <cmath>

// Bir sayının 2'nin kuvveti olup olmadığını bitwise (AND) ile çok hızlı kontrol ederiz.
bool HammingCore::isPowerOfTwo(int n) {
    return (n > 0) && ((n & (n - 1)) == 0);
}

// M uzunluğundaki veri için gereken K (parite) bit sayısını hesaplar: 2^K >= M + K + 1
int HammingCore::calculateParityBits(int dataSize) {
    int k = 0;
    while ((1 << k) < (dataSize + k + 1)) {
        k++;
    }
    return k;
}

std::vector<uint8_t> HammingCore::generateCodeword(const std::vector<uint8_t>& dataBits) {
    int m = dataBits.size(); // Veri bitleri (M)
    int k = calculateParityBits(m); // Parite bitleri (K)
    int totalSize = m + k;

    // Belleğe yazılacak veri (M+K), başlangıçta hepsi 0.
    std::vector<uint8_t> codeword(totalSize, 0);

    // 1. Adım: Veri bitlerini (Data In) [cite: 11] 2'nin kuvveti OLMAYAN pozisyonlara yerleştir.
    int dataIndex = 0;
    for (int i = 1; i <= totalSize; ++i) {
        if (!isPowerOfTwo(i)) {
            // 1-tabanlı indeks 'i'yi, 0-tabanlı vector indeksine (i-1) çeviriyoruz
            codeword[i - 1] = dataBits[dataIndex++];
        }
    }

    // 2. Adım: Parite bitlerini hesapla (f fonksiyonu)
    for (int i = 0; i < k; ++i) {
        int parityPos = (1 << i); // 1, 2, 4, 8...
        uint8_t parityValue = 0;

        // XOR işlemi ile ilgili bitleri kontrol et
        for (int j = 1; j <= totalSize; ++j) {
            // j. pozisyonun ikilik yazılımında parityPos biti 1 ise, bu bit parite hesabına dahildir.
            if (j & parityPos) {
                // parityPos'un kendisini atlıyoruz, sadece diğer bitleri XOR'luyoruz
                if (j != parityPos) {
                    parityValue ^= codeword[j - 1];
                }
            }
        }
        codeword[parityPos - 1] = parityValue; // Hesaplanan pariteyi yerine yaz
    }

    return codeword;
}

int HammingCore::calculateSyndrome(const std::vector<uint8_t>& memoryData) {
    int totalSize = memoryData.size();
    int syndrome = 0;
    int k = 0;

    // Toplam boyuttan K sayısını bul
    while ((1 << k) <= totalSize) {
        k++;
    }

    // Compare bloğu: Bellekteki okunan veriler üzerinden parite kontrolü yap
    for (int i = 0; i < k; ++i) {
        int parityPos = (1 << i);
        uint8_t parityValue = 0;

        for (int j = 1; j <= totalSize; ++j) {
            if (j & parityPos) {
                parityValue ^= memoryData[j - 1];
            }
        }

        // Eğer bu parite grubunda hata varsa, sendrom kelimesinin o bitini 1 yap
        if (parityValue != 0) {
            syndrome |= parityPos;
        }
    }

    return syndrome;
}

void HammingCore::correctError(std::vector<uint8_t>& memoryData, int syndrome) {
    // Corrector bloğu: Sendrom 0'dan büyükse ve dizinin boyutu içindeyse bir hata tespit edilmiştir.
    if (syndrome > 0 && syndrome <= memoryData.size()) {
        // Hatalı biti XOR (^) ile tersine çevir (0 ise 1, 1 ise 0 yap)
        memoryData[syndrome - 1] ^= 1;
    }
}