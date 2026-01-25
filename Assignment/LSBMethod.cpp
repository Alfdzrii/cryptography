#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

// membaca file BMP menjadi vector of bytes (char)
bool readBMP(const string& filename, vector<unsigned char>& header, vector<unsigned char>& data) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Gagal membuka file! Pastikan nama file benar." << endl;
        return false;
    }

    // Header BMP standar 54 byte
    header.resize(54);
    file.read(reinterpret_cast<char*>(header.data()), 54);

    if (header[0] != 'B' || header[1] != 'M') {
        cerr << "Bukan file BMP yang valid!" << endl;
        return false;
    }

    // Baca sisa data (piksel)
    data.assign(istreambuf_iterator<char>(file), istreambuf_iterator<char>());
    file.close();
    return true;
}

// menyimpan kembali data ke file BMP baru
void writeBMP(const string& filename, const vector<unsigned char>& header, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Gagal membuat file output!" << endl;
        return;
    }
    file.write(reinterpret_cast<const char*>(header.data()), header.size());
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    file.close();
    cout << "Gambar berhasil disimpan sebagai: " << filename << endl;
}

// masukan pesan ke piksel (LSB)
void embedMessage(vector<unsigned char>& data, const string& message) {
    // karakter null ('\0') di akhir untuk tanda berhenti
    string msg = message + '\0';

    if (data.size() < msg.length() * 8) {
        cout << "Gambar terlalu kecil untuk menampung pesan ini!" << endl;
        return;
    }

    int pixelIndex = 0;
    for (char c : msg) {
        for (int i = 7; i >= 0; --i) {
            int bitVal = (c >> i) & 1;

            // LSB replacement
            data[pixelIndex] &= 254; // Set bit terakhir jadi 0
            data[pixelIndex] |= bitVal; // Isi bit terakhir dengan bit pesan

            pixelIndex++;
        }
    }
}

// Mengekstrak pesan dari data piksel
string extractMessage(const vector<unsigned char>& data) {
    string message = "";
    char c = 0;
    int bitCount = 0;

    for (size_t i = 0; i < data.size(); i++) {
        // Ambil LSB dari data piksel
        int bitVal = data[i] & 1;

        // Masukkan ke char c
        c = (c << 1) | bitVal;
        bitCount++;

        if (bitCount == 8) {
            if (c == '\0') break; // Berhenti jika ketemu penanda akhir
            message += c;
            c = 0;
            bitCount = 0;
        }
    }
    return message;
}

int main() {
    string filename, message;
    vector<unsigned char> header;
    vector<unsigned char> pixelData;
    int choice;

    cout << "=== Program Steganografi LSB (File BMP) ===\n";
    cout << "1. Sembunyikan Pesan (Hide)\n";
    cout << "2. Baca Pesan (Reveal)\n";
    cout << "Pilihan (1/2): ";
    cin >> choice;

    cout << "Masukkan nama file gambar (.bmp): ";
    cin >> filename;

    if (readBMP(filename, header, pixelData)) {
        if (choice == 1) {
            cout << "Masukkan pesan rahasia (tanpa spasi atau gunakan _): ";
            cin >> message;

            embedMessage(pixelData, message);
            writeBMP("stego_output.bmp", header, pixelData);
            cout << "Pesan berhasil disembunyikan di 'stego_output.bmp'.\n";
        }
        else if (choice == 2) {
            string hiddenMsg = extractMessage(pixelData);
            cout << "\nPesan yang ditemukan: " << hiddenMsg << endl;
        }
    }

    return 0;
}