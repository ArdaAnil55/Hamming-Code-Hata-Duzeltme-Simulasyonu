#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>
#include "HammingCore.h" // Backend sınıfımızı dahil ediyoruz

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnGenerate_clicked();      // "Kodu Oluştur" butonuna basılınca çalışacak
    void on_bitButton_clicked();        // Bellekteki herhangi bir bit butonuna basılınca çalışacak
    void on_radio_toggled(bool checked); // 8, 16, 32 bit seçimleri değişince çalışacak

private:
    Ui::MainWindow *ui;

    // Bellekteki bitleri temsil eden butonları dinamik olarak bu vektörde saklayacağız
    std::vector<QPushButton*> memoryButtons;

    // Bellekteki o anki ham bit durumunu (0 veya 1) tutacak vektör
    std::vector<uint8_t> currentCodeword;

    // Arayüzü temizleyip yeniden bit butonları dizen yardımcı fonksiyon
    void updateMemoryLayout(int dataSize);

    // Sendromu hesaplayıp arayüzdeki çıkışları güncelleyen fonksiyon (Compare & Corrector simülasyonu)
    void processAndDisplay();
};
#endif // MAINWINDOW_H