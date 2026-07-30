# 🧮 BilgMimariProje — Hamming Kodu (SEC) Hata Düzeltme Simülatörü

![Qt](https://img.shields.io/badge/Qt-6%20%7C%205-41CD52?logo=qt&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?logo=cmake&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-informational)

**BilgMimariProje**, Bilgisayar Mimarisi derslerinde işlenen **Hamming SEC (Single Error Correction)** yöntemini uçtan uca simüle eden bir Qt Widgets masaüstü uygulamasıdır. Girilen ikili veriden parite bitlerini üretir, oluşan kod kelimesini "bellek" üzerinde interaktif butonlarla görselleştirir, dilediğiniz bite tıklayarak yapay hata enjekte etmenizi sağlar ve hatayı **anlık olarak** tespit edip düzeltir.

## 📋 İçindekiler

- [Nasıl Çalışır](#nasıl-çalışır)
- [Özellikler](#özellikler)
- [Gereksinimler](#gereksinimler)
- [Kurulum ve Derleme](#kurulum-ve-derleme)
- [Kullanım](#kullanım)
- [Proje Yapısı](#proje-yapısı)
- [Algoritma Detayları](#algoritma-detayları)
- [Notlar ve Sınırlamalar](#notlar-ve-sınırlamalar)
- [Lisans](#lisans)

## Nasıl Çalışır

Uygulama, klasik Hamming hata düzeltme devresindeki üç bloğu birebir simüle eder; bu bloklar `HammingCore` sınıfındaki fonksiyonlara karşılık gelir:

```
 Data In (M bit)
        │
        ▼
   ┌──────────┐
   │    f     │   generateCodeword() → parite bitlerini hesaplar
   └────┬─────┘
        │  Kod kelimesi (M+K bit)
        ▼
   ┌──────────┐
   │  Memory  │   Arayüzde renkli butonlar olarak gösterilir
   └────┬─────┘
        │  (bir bite tıklanarak hata enjekte edilebilir)
        ▼
   ┌──────────┐
   │ Compare  │   calculateSyndrome() → sendromu hesaplar
   └────┬─────┘
        ├───────────► Hata Sinyali / Sendrom Değeri
        ▼
   ┌───────────┐
   │ Corrector │   correctError() → hatalı biti tersine çevirir
   └─────┬─────┘
         ▼
   Data Out (M bit)
```

## Özellikler

- 🔘 **8 / 16 / 32 bit** veri uzunluğu seçimi
- ⚡ Girilen veriden otomatik **Hamming kod kelimesi** üretimi
- 🎨 Bellek bitlerinin görsel gösterimi — parite bitleri **mavi**, veri bitleri **gri**
- 🖱️ Herhangi bir bite tıklayarak **yapay hata enjeksiyonu** (kırmızı ile vurgulanır)
- 🔍 Gerçek zamanlı **sendrom hesaplama** ve hata tespiti
- 🛠️ Tek bit hataların **otomatik düzeltilmesi**
- 📤 Düzeltilmiş veri bitlerinin (**Data Out**) ayıklanıp gösterilmesi

## Gereksinimler

| Bileşen | Sürüm |
|---|---|
| Qt | 6.x veya 5.x (Widgets modülü) |
| CMake | 3.16+ |
| Derleyici | C++17 destekli (MSVC, GCC veya Clang) |

## Kurulum ve Derleme

### Qt Creator ile

1. Qt Creator'ı açın.
2. **File > Open File or Project** ile `CMakeLists.txt` dosyasını seçin.
3. Uygun bir Qt kiti (kit) seçerek projeyi configure edin.
4. `Ctrl+R` ile derleyip çalıştırın.

### Komut satırından

```bash
mkdir build && cd build
cmake ..
# Qt otomatik bulunamazsa:
# cmake .. -DCMAKE_PREFIX_PATH="<Qt-kurulum-yolunuz>"
cmake --build .
./BilgMimariProje        # Linux / macOS
# Windows: build klasöründeki BilgMimariProje.exe dosyasını çalıştırın
```

## Kullanım

1. **"Veri Uzunluğu Seçimi"** panelinden 8, 16 veya 32 bit seçin.
2. **"Giriş Verisi (Binary)"** kutusuna seçtiğiniz uzunlukta, yalnızca `0` ve `1`'lerden oluşan bir dizi girin.
3. **"Kodu Oluştur"** butonuna tıklayın — parite bitleri hesaplanır ve kod kelimesi alttaki bellek alanında butonlar halinde belirir.
4. Herhangi bir **bit butonuna tıklayarak** yapay hata enjekte edin (buton kırmızıya döner).
5. Uygulama anında **Sendrom Değeri**, **Hata Durumu** ve düzeltilmiş **Çıkış Verisi**'ni günceller.

## Proje Yapısı

```
BilgMimariProje/
├── CMakeLists.txt        # Derleme yapılandırması
├── main.cpp               # Uygulama giriş noktası
├── mainwindow.h/.cpp/.ui  # GUI mantığı ve arayüz tasarımı
└── hammingcore.h/.cpp      # Hamming SEC algoritması (f / Compare / Corrector)
```

## Algoritma Detayları

Gerekli parite biti sayısı **K**, aşağıdaki koşulu sağlayan en küçük değer olacak şekilde hesaplanır:

```
2^K ≥ M + K + 1      (M: veri biti sayısı)
```

Uygulamada desteklenen üç boyut için sonuçlar:

| Veri (M) | Parite (K) | Toplam Kod Kelimesi (M+K) |
|---|---|---|
| 8  | 4 | 12 |
| 16 | 5 | 21 |
| 32 | 6 | 38 |

**Bit yerleşimi:** 1-tabanlı pozisyonlardan 2'nin kuvveti olanlar (1, 2, 4, 8, 16…) parite bitlerine ayrılır; kalan tüm pozisyonlar sırasıyla veri bitleriyle doldurulur. Örnek olarak 12 bitlik (8 veri biti) bir kod kelimesinin yerleşimi:

| Pozisyon | Tür |
|---|---|
| 1  | P1 (parite) |
| 2  | P2 (parite) |
| 3  | D1 (veri) |
| 4  | P4 (parite) |
| 5  | D2 (veri) |
| 6  | D3 (veri) |
| 7  | D4 (veri) |
| 8  | P8 (parite) |
| 9  | D5 (veri) |
| 10 | D6 (veri) |
| 11 | D7 (veri) |
| 12 | D8 (veri) |

Her `Pi` parite biti, pozisyon numarasının ikili gösteriminde ilgili biti `1` olan tüm konumların **XOR**'u alınarak hesaplanır (örn. **P1**, tek numaralı pozisyonların — 1, 3, 5, 7, 9, 11 — XOR'udur).

**Sendrom / hata düzeltme:** Aynı parite grupları, bu kez parite bitinin kendisi de dahil edilerek yeniden XOR'lanır. Sonuç `0` ise veri bozulmamıştır; `0`'dan farklıysa elde edilen sendrom değeri **doğrudan hatalı bitin 1-tabanlı pozisyonunu verir** — Hamming kodunun asıl gücü de burada yatar.

## Notlar ve Sınırlamalar

- Bu implementasyon yalnızca **SEC (Single Error Correction)** sağlar; genel bir ek parite biti (P0) bulunmadığından **çift bit hatalarını güvenilir şekilde tespit edemez** (SECDED değildir).
- Giriş doğrulaması yalnızca uzunluk ve karakterlerin `0`/`1` olup olmadığını kontrol eder.

## Lisans

Bu proje [MIT Lisansı](LICENSE) ile lisanslanmıştır.
