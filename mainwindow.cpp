#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Radio butonlar tıklandığında arayüzün dinamik olarak güncellenmesini bağlıyoruz
    connect(ui->Radio8, &QRadioButton::toggled, this, &MainWindow::on_radio_toggled);
    connect(ui->Radio16, &QRadioButton::toggled, this, &MainWindow::on_radio_toggled);
    connect(ui->Radio32, &QRadioButton::toggled, this, &MainWindow::on_radio_toggled);

    // İlk açılışta varsayılan olarak 8-bit seçili gelsin
    ui->Radio8->setChecked(true);
    updateMemoryLayout(8);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 8, 16 veya 32 bit seçildiğinde çalışan fonksiyon
void MainWindow::on_radio_toggled(bool checked) {
    if (!checked) return;

    if (ui->Radio8->isChecked()) updateMemoryLayout(8);
    else if (ui->Radio16->isChecked()) updateMemoryLayout(16);
    else if (ui->Radio32->isChecked()) updateMemoryLayout(32);
}

// Ekrana dinamik buton dizen fonksiyon
void MainWindow::updateMemoryLayout(int dataSize) {
    // 1. Önce eski butonlar varsa arayüzden temizle
    for (QPushButton* btn : memoryButtons) {
        delete btn;
    }
    memoryButtons.clear();
    currentCodeword.clear();

    // 2. Veri boyutuna göre toplam Hamming boyutunu hesapla (M + K)
    int k = 0;
    while ((1 << k) < (dataSize + k + 1)) k++;
    int totalSize = dataSize + k;

    // 3. memoryLayout içerisine yan yana butonları ekle
    // Çizdiğin QFrame'in içine yatay bir yerleşim düzeni atıyoruz
    if(ui->memoryLayout->layout() == nullptr) {
        // Eğer layout yoksa yeni oluştur (Yatay düzen)
        auto* layout = new QHBoxLayout(ui->memoryLayout);
        ui->memoryLayout->setLayout(layout);
    }

    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->memoryLayout->layout());

    for (int i = 0; i < totalSize; ++i) {
        QPushButton* btn = new QPushButton("0", this);
        btn->setCheckable(true); // Buton tıklandığında basılı kalabilsin
        btn->setMinimumSize(30, 30);
        btn->setMaximumSize(40, 40);

        // Tasarım ipucu: Parite bitlerini görsel olarak ayırt etmek için renk verelim (2'nin kuvvetleri)
        int pos = i + 1;
        if ((pos & (pos - 1)) == 0) {
            btn->setStyleSheet("background-color: #3498db; color: white; font-weight: bold;"); // Parite bitleri Mavi
        } else {
            btn->setStyleSheet("background-color: #bdc3c7; font-weight: bold;"); // Veri bitleri Gri
        }

        // Butona tıklandığında tetiklenecek fonksiyonu bağlıyoruz
        connect(btn, &QPushButton::clicked, this, &MainWindow::on_bitButton_clicked);

        layout->addWidget(btn);
        memoryButtons.push_back(btn);
        currentCodeword.push_back(0);
    }
}

// "Kodu Oluştur" Butonuna Basıldığında (Data In -> f -> Memory)
void MainWindow::on_btnGenerate_clicked() {
    QString text = ui->inputData->text();
    int expectedSize = ui->Radio8->isChecked() ? 8 : (ui->Radio16->isChecked() ? 16 : 32);

    // Basit bir girdi kontrolü
    if (text.length() != expectedSize) {
        QMessageBox::warning(this, "Hata", QString("Lütfen tam olarak %1 bitlik binary veri girin!").arg(expectedSize));
        return;
    }

    std::vector<uint8_t> dataBits;
    for (int i = 0; i < text.length(); ++i) {
        if (text[i] != '0' && text[i] != '1') {
            QMessageBox::warning(this, "Hata", "Veri sadece 0 veya 1'lerden oluşmalıdır!");
            return;
        }
        dataBits.push_back(text[i].toLatin1() - '0');
    }

    // Backend kütüphanemizi çağırıp Hamming kodunu hesaplatıyoruz (f fonksiyonu)
    currentCodeword = HammingCore::generateCodeword(dataBits);

    // Hesaplanan kod kelimesini ekrandaki Bellek (Memory) butonlarına yansıtıyoruz
    for (size_t i = 0; i < currentCodeword.size(); ++i) {
        memoryButtons[i]->setText(QString::number(currentCodeword[i]));
        memoryButtons[i]->setChecked(false); // Hata durumunu sıfırla

        // Renkleri varsayılana döndür
        int pos = i + 1;
        if ((pos & (pos - 1)) == 0) memoryButtons[i]->setStyleSheet("background-color: #3498db; color: white; font-weight: bold;");
        else memoryButtons[i]->setStyleSheet("background-color: #bdc3c7; font-weight: bold;");
    }

    // Çıkışları hesapla ve güncelle
    processAndDisplay();
}

// Bellekteki Herhangi Bir Bit Butonuna Basıldığında (Yapay Hata Enjeksiyonu)
void MainWindow::on_bitButton_clicked() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton) return;

    // Hangi butona tıklandığını vektördeki indeksinden buluyoruz
    for (size_t i = 0; i < memoryButtons.size(); ++i) {
        if (memoryButtons[i] == clickedButton) {
            // Yapay olarak o biti tersine çeviriyoruz (XOR mantığı)
            currentCodeword[i] ^= 1;
            clickedButton->setText(QString::number(currentCodeword[i]));

            // Eğer bit orijinalinden farklıysa (yani hata enjekte edildiyse) butonu kırmızı yap
            if (clickedButton->isChecked()) {
                clickedButton->setStyleSheet("background-color: #e74c3c; color: white; font-weight: bold;"); // Hatalı Bit Kırmızı
            } else {
                int pos = i + 1;
                if ((pos & (pos - 1)) == 0) clickedButton->setStyleSheet("background-color: #3498db; color: white; font-weight: bold;");
                else clickedButton->setStyleSheet("background-color: #bdc3c7; font-weight: bold;");
            }
            break;
        }
    }

    // Hata oluşturulduğu an anlık olarak Compare ve Corrector bloklarını çalıştır
    processAndDisplay();
}

// Şemadaki Compare ve Corrector Bloklarının Tetiklenmesi
void MainWindow::processAndDisplay() {
    // 1. Compare Bloğu: Sendrom hesapla
    int syndrome = HammingCore::calculateSyndrome(currentCodeword);

    // Sendromu ikilik tabanda ekrana bas (Örn: 0101)
    ui->lblSyndrome->setText(QString::number(syndrome, 2).rightJustified(4, '0') + QString(" (İndeks: %1)").arg(syndrome));

    // 2. Error Signal Durumu
    if (syndrome == 0) {
        ui->lblErrorSignal->setText("Hata Durumu: Hata Yok (Sistem Temiz)");
        ui->lblErrorSignal->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->lblErrorSignal->setText(QString("Hata Durumu: 1-Bit Hata Algılandı! (Bit %1)").arg(syndrome));
        ui->lblErrorSignal->setStyleSheet("color: red; font-weight: bold;");
    }

    // 3. Corrector Bloğu: Hatalı veriyi düzeltip Data Out'a gönderme simülasyonu
    std::vector<uint8_t> correctedWord = currentCodeword;
    HammingCore::correctError(correctedWord, syndrome); // Eğer hata varsa arka planda düzeltiliyor

    // Düzeltilmiş kelimeden sadece veri bitlerini (M) ayıklayıp Data Out olarak basıyoruz
    QString dataOutStr = "";
    for (size_t i = 1; i <= correctedWord.size(); ++i) {
        if ((i & (i - 1)) != 0) { // 2'nin kuvveti olmayan veri bitlerini seç
            dataOutStr += QString::number(correctedWord[i - 1]);
        }
    }
    ui->lblDataOut->setText(dataOutStr);
}