#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

// ============================================================
//  FUNGSI BANTU STRING & ANGKA  (bagian rekan)
// ============================================================
void salinTeks(char tujuan[], const char asal[]) {
    int i = 0;
    while (asal[i] != '\0') { tujuan[i] = asal[i]; i++; }
    tujuan[i] = '\0';
}

bool teksSama(const char teks1[], const char teks2[]) {
    int i = 0;
    while (teks1[i] != '\0' && teks2[i] != '\0') {
        if (teks1[i] != teks2[i]) return false;
        i++;
    }
    return teks1[i] == teks2[i];
}

long long teksKeAngka(const char teks[]) {
    long long hasil = 0;
    int i = 0;
    while (teks[i] != '\0') {
        if (teks[i] >= '0' && teks[i] <= '9')
            hasil = hasil * 10 + (teks[i] - '0');
        i++;
    }
    return hasil;
}

// -- Tambahan untuk modul 5,6,7 (gaya sama: tanpa cstring) --

// Membandingkan dua teks seperti strcmp: <0, 0, atau >0
int bandingTeks(const char a[], const char b[]) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return (a[i] < b[i]) ? -1 : 1;
        i++;
    }
    if (a[i] == '\0' && b[i] == '\0') return 0;
    return (a[i] == '\0') ? -1 : 1;
}

// Cek apakah 'kata' terdapat di dalam 'teks' (substring, untuk pencarian nama)
bool teksMengandung(const char teks[], const char kata[]) {
    if (kata[0] == '\0') return true;
    for (int i = 0; teks[i] != '\0'; i++) {
        int j = 0;
        while (kata[j] != '\0' && teks[i + j] == kata[j]) j++;
        if (kata[j] == '\0') return true;
    }
    return false;
}

// Ubah time_t menjadi teks "YYYY-MM-DD HH:MM" untuk ditampilkan
void formatWaktu(time_t t, char buffer[]) {
    struct tm* info = localtime(&t);
    strftime(buffer, 20, "%Y-%m-%d %H:%M", info);
}

// ============================================================
//  STRUCT  (bagian rekan)
// ============================================================
struct Akun {
    char username[50];
    char password[50];
    char role[20];
};

struct Barang {
    int id;
    char nama[100];
    char deskripsi[200];
    char kategori[50];
    long long harga_awal;
    long long harga_tertinggi;
    char pemenang[50];
    time_t waktu_tutup;
    int status;                 // 0=aktif, 1=tutup
};

struct Tawaran {
    int id_barang;
    char username[50];
    long long nominal;
    time_t waktu;
};

// ============================================================
//  VARIABEL GLOBAL  (bagian rekan)
// ============================================================
Akun db_akun[100];      int jml_akun = 0;
Barang db_barang[100];  int jml_barang = 0;
Tawaran db_tawaran[100]; int jml_tawaran = 0;

char currentUser[50] = "";
char currentRole[20] = "";
const char ADMIN_CODE[] = "ADMIN123";

// ============================================================
//  LOAD & SAVE DATA  (bagian rekan)
//  CATATAN: blok tawaran.csv DITAMBAHKAN untuk modul 6 & 7
//  (riwayat penawaran). Sampaikan ke rekan agar selaras.
// ============================================================
void loadData() {
    char buffer[255];
    jml_akun = 0; jml_barang = 0; jml_tawaran = 0;

    ifstream fAkun("akun.csv");
    if (fAkun.is_open()) {
        while (fAkun.getline(db_akun[jml_akun].username, 50, ',')) {
            fAkun.getline(db_akun[jml_akun].password, 50, ',');
            fAkun.getline(db_akun[jml_akun].role, 20, '\n');
            jml_akun++;
        }
        fAkun.close();
    }

    ifstream fBarang("barang.csv");
    if (fBarang.is_open()) {
        while (fBarang.getline(buffer, 255, ',')) {
            db_barang[jml_barang].id = teksKeAngka(buffer);
            fBarang.getline(db_barang[jml_barang].nama, 100, ',');
            fBarang.getline(db_barang[jml_barang].deskripsi, 200, ',');
            fBarang.getline(db_barang[jml_barang].kategori, 50, ',');
            fBarang.getline(buffer, 255, ','); db_barang[jml_barang].harga_awal = teksKeAngka(buffer);
            fBarang.getline(buffer, 255, ','); db_barang[jml_barang].harga_tertinggi = teksKeAngka(buffer);
            fBarang.getline(db_barang[jml_barang].pemenang, 50, ',');
            fBarang.getline(buffer, 255, ','); db_barang[jml_barang].waktu_tutup = teksKeAngka(buffer);
            fBarang.getline(buffer, 255, '\n'); db_barang[jml_barang].status = teksKeAngka(buffer);
            jml_barang++;
        }
        fBarang.close();
    }

    // -- TAMBAHAN: muat riwayat tawaran --
    ifstream fTawaran("tawaran.csv");
    if (fTawaran.is_open()) {
        while (fTawaran.getline(buffer, 255, ',')) {
            db_tawaran[jml_tawaran].id_barang = teksKeAngka(buffer);
            fTawaran.getline(db_tawaran[jml_tawaran].username, 50, ',');
            fTawaran.getline(buffer, 255, ','); db_tawaran[jml_tawaran].nominal = teksKeAngka(buffer);
            fTawaran.getline(buffer, 255, '\n'); db_tawaran[jml_tawaran].waktu = teksKeAngka(buffer);
            jml_tawaran++;
        }
        fTawaran.close();
    }
}

void saveData() {
    ofstream fAkun("akun.csv");
    for (int i = 0; i < jml_akun; i++)
        fAkun << db_akun[i].username << "," << db_akun[i].password << "," << db_akun[i].role << "\n";
    fAkun.close();

    ofstream fBarang("barang.csv");
    for (int i = 0; i < jml_barang; i++) {
        fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                << db_barang[i].status << "\n";
    }
    fBarang.close();

    // -- TAMBAHAN: simpan riwayat tawaran --
    ofstream fTawaran("tawaran.csv");
    for (int i = 0; i < jml_tawaran; i++)
        fTawaran << db_tawaran[i].id_barang << "," << db_tawaran[i].username << ","
                 << db_tawaran[i].nominal << "," << db_tawaran[i].waktu << "\n";
    fTawaran.close();
}

void checkExpiredAuctions() {
    time_t now = time(0);
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 0 && now >= db_barang[i].waktu_tutup)
            db_barang[i].status = 1;
}

// ============================================================
//  DAFTAR AKUN & LOGIN  (bagian rekan)
// ============================================================
void daftarAkun() {
    if (jml_akun >= 100) { cout << "Database akun penuh!\n"; return; }
    Akun a;
    cout << "\n--- DAFTAR AKUN BARU ---\nUsername: ";
    cin >> a.username;
    for (int i = 0; i < jml_akun; i++)
        if (teksSama(db_akun[i].username, a.username)) {
            cout << "Username sudah terdaftar!\n"; return;
        }
    cout << "Password: "; cin >> a.password;
    int roleChoice;
    cout << "Pilih Role:\n[1] Peserta\n[2] Admin\nPilihan: "; cin >> roleChoice;
    if (roleChoice == 2) {
        char kode[50];
        cout << "Kode Admin: "; cin >> kode;
        if (teksSama(kode, ADMIN_CODE)) salinTeks(a.role, "ADMIN");
        else { cout << "Kode salah! Jadi Peserta.\n"; salinTeks(a.role, "PESERTA"); }
    } else salinTeks(a.role, "PESERTA");
    db_akun[jml_akun] = a; jml_akun++;
    saveData();
    cout << "Akun berhasil dibuat!\n";
}

void login() {
    char usr[50], pwd[50];
    cout << "\n--- LOGIN ---\nUsername: "; cin >> usr;
    cout << "Password: "; cin >> pwd;
    for (int i = 0; i < jml_akun; i++)
        if (teksSama(db_akun[i].username, usr) && teksSama(db_akun[i].password, pwd)) {
            salinTeks(currentUser, db_akun[i].username);
            salinTeks(currentRole, db_akun[i].role);
            cout << "Login berhasil, " << currentUser << "!\n";
            return;
        }
    cout << "Gagal login!\n";
}

// ============================================================
//  SORTING  (mengurutkan SALINAN array, tidak mengubah db asli)
// ============================================================
// Bubble Sort - descending berdasarkan harga tertinggi
void sortByHarga(Barang arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j].harga_tertinggi < arr[j + 1].harga_tertinggi) {
                Barang temp = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = temp;
            }
}
// Insertion Sort - ascending berdasarkan nama (alfabetis)
void sortByNama(Barang arr[], int n) {
    for (int i = 1; i < n; i++) {
        Barang kunci = arr[i];
        int j = i - 1;
        while (j >= 0 && bandingTeks(arr[j].nama, kunci.nama) > 0) {
            arr[j + 1] = arr[j]; j--;
        }
        arr[j + 1] = kunci;
    }
}

// ============================================================
//  SEARCHING
// ============================================================
// Binary Search berdasarkan ID (db_barang terurut naik by id)
int searchBarangById(int id) {
    int kiri = 0, kanan = jml_barang - 1;
    while (kiri <= kanan) {
        int tengah = (kiri + kanan) / 2;
        if (db_barang[tengah].id == id) return tengah;
        else if (db_barang[tengah].id < id) kiri = tengah + 1;
        else kanan = tengah - 1;
    }
    return -1;
}

// ============================================================
//  MODUL 5: FITUR ADMIN
// ============================================================
void tambahBarang() {
    if (jml_barang >= 100) { cout << "Database barang penuh!\n"; return; }
    Barang b;
    b.id = (jml_barang == 0) ? 1 : db_barang[jml_barang - 1].id + 1;
    cin.ignore();
    cout << "\n--- TAMBAH BARANG (ID " << b.id << ") ---\n";
    cout << "Nama Barang: "; cin.getline(b.nama, 100);
    cout << "Deskripsi  : "; cin.getline(b.deskripsi, 200);
    cout << "Kategori   : "; cin.getline(b.kategori, 50);
    cout << "Harga Awal : "; cin >> b.harga_awal;
    int durasi;
    cout << "Durasi (Jam): "; cin >> durasi;

    b.harga_tertinggi = b.harga_awal;
    salinTeks(b.pemenang, "-");
    b.status = 0;
    b.waktu_tutup = time(0) + ((time_t)durasi * 3600);

    db_barang[jml_barang] = b; jml_barang++;
    saveData();

    char w[20]; formatWaktu(b.waktu_tutup, w);
    cout << "Barang tersimpan! Lelang tutup pada: " << w << "\n";
}

void tutupLelangManual() {
    cout << "\n--- TUTUP LELANG MANUAL ---\n";
    int ada = 0;
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 0) {
            cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                      << " | Max Bid: Rp" << db_barang[i].harga_tertinggi << "\n";
            ada++;
        }
    if (ada == 0) { cout << "Tidak ada lelang aktif.\n"; return; }

    int id;
    cout << "ID barang yang ingin ditutup: "; cin >> id;
    int idx = searchBarangById(id);
    if (idx == -1) { cout << "ID tidak ditemukan.\n"; return; }
    if (db_barang[idx].status != 0) { cout << "Barang sudah tutup.\n"; return; }

    db_barang[idx].status = 1;
    cout << "\nLelang '" << db_barang[idx].nama << "' ditutup.\n";
    if (teksSama(db_barang[idx].pemenang, "-") == false &&
        db_barang[idx].pemenang[0] != '\0')
        cout << "PEMENANG: " << db_barang[idx].pemenang
                  << " (Rp" << db_barang[idx].harga_tertinggi << ")\n";
    else
        cout << "Tanpa penawar.\n";
    saveData();
}

void lihatSemuaBarang() {
    if (jml_barang == 0) { cout << "\nBelum ada barang.\n"; return; }
    Barang tmp[100];
    for (int i = 0; i < jml_barang; i++) tmp[i] = db_barang[i];

    int pil;
    cout << "\n--- LIHAT SEMUA BARANG ---\n";
    cout << "[1] Urutkan Nama (A-Z)\n[2] Urutkan Harga (tertinggi)\n[3] Tanpa urut\nPilih: ";
    cin >> pil;
    if (pil == 1) sortByNama(tmp, jml_barang);
    else if (pil == 2) sortByHarga(tmp, jml_barang);

    cout << "\n";
    for (int i = 0; i < jml_barang; i++)
        cout << "ID: " << tmp[i].id << " | " << tmp[i].nama
                  << " | Rp" << tmp[i].harga_tertinggi
                  << " | " << (tmp[i].status == 0 ? "AKTIF" : "TUTUP")
                  << " | Pemenang: " << tmp[i].pemenang << "\n";
    cout << "Total: " << jml_barang << " barang.\n";
}

void hapusBarang() {
    if (jml_barang == 0) { cout << "\nBelum ada barang.\n"; return; }
    cout << "\n--- HAPUS BARANG ---\n";
    for (int i = 0; i < jml_barang; i++)
        cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                  << " | " << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";

    int id;
    cout << "ID barang yang ingin dihapus: "; cin >> id;
    int idx = searchBarangById(id);
    if (idx == -1) { cout << "ID tidak ditemukan.\n"; return; }

    for (int i = idx; i < jml_barang - 1; i++) db_barang[i] = db_barang[i + 1];
    jml_barang--;
    saveData();
    cout << "Barang ID " << id << " berhasil dihapus.\n";
}

// ============================================================
//  MODUL 6: FITUR PESERTA
// ============================================================
void lihatLelangAktif() {
    Barang tmp[100];
    int n = 0;
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 0) tmp[n++] = db_barang[i];
    if (n == 0) { cout << "\nTidak ada lelang aktif.\n"; return; }

    int pil;
    cout << "\n--- LELANG AKTIF ---\n";
    cout << "[1] Urutkan Harga (tertinggi)\n[2] Urutkan Nama (A-Z)\n[3] Tanpa urut\nPilih: ";
    cin >> pil;
    if (pil == 1) sortByHarga(tmp, n);
    else if (pil == 2) sortByNama(tmp, n);

    cout << "\n";
    for (int i = 0; i < n; i++) {
        char w[20]; formatWaktu(tmp[i].waktu_tutup, w);
        cout << "ID: " << tmp[i].id << " | " << tmp[i].nama
                  << " | " << tmp[i].kategori
                  << " | Max Bid: Rp" << tmp[i].harga_tertinggi
                  << " | Tutup: " << w << "\n";
    }
    cout << "Total: " << n << " lelang aktif.\n";
}

void cariBarang() {
    if (jml_barang == 0) { cout << "\nBelum ada barang.\n"; return; }
    int pil;
    cout << "\n--- CARI BARANG ---\n";
    cout << "[1] Cari Nama (Sequential Search)\n[2] Cari ID (Binary Search)\nPilih: ";
    cin >> pil;

    if (pil == 1) {
        char kata[100];
        cin.ignore();
        cout << "Kata kunci nama: "; cin.getline(kata, 100);
        bool ketemu = false;
        cout << "\nHasil pencarian:\n";
        for (int i = 0; i < jml_barang; i++)
            if (teksMengandung(db_barang[i].nama, kata)) {
                cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                          << " | Rp" << db_barang[i].harga_tertinggi
                          << " | " << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
                ketemu = true;
            }
        if (!ketemu) cout << "Tidak ditemukan.\n";

    } else if (pil == 2) {
        int id;
        cout << "Masukkan ID barang: "; cin >> id;
        int idx = searchBarangById(id);
        if (idx == -1) { cout << "ID tidak ditemukan.\n"; return; }
        char w[20]; formatWaktu(db_barang[idx].waktu_tutup, w);
        cout << "\nBarang ditemukan:\n";
        cout << "ID        : " << db_barang[idx].id << "\n";
        cout << "Nama      : " << db_barang[idx].nama << "\n";
        cout << "Deskripsi : " << db_barang[idx].deskripsi << "\n";
        cout << "Kategori  : " << db_barang[idx].kategori << "\n";
        cout << "Max Bid   : Rp" << db_barang[idx].harga_tertinggi << "\n";
        cout << "Status    : " << (db_barang[idx].status == 0 ? "AKTIF" : "TUTUP") << "\n";
        cout << "Tutup     : " << w << "\n";
    } else cout << "Pilihan tidak valid.\n";
}

void tawarBarang() {
    if (jml_tawaran >= 100) { cout << "Penyimpanan tawaran penuh.\n"; return; }
    cout << "\n--- TAWAR BARANG ---\n";
    int ada = 0;
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 0) {
            cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                      << " | Max Bid: Rp" << db_barang[i].harga_tertinggi << "\n";
            ada++;
        }
    if (ada == 0) { cout << "Tidak ada lelang aktif.\n"; return; }

    int id; long long nominal;
    cout << "ID Barang: "; cin >> id;
    int idx = searchBarangById(id);
    if (idx == -1 || db_barang[idx].status == 1) {
        cout << "Barang tidak ada atau sudah tutup!\n"; return;
    }

    long long batas = db_barang[idx].harga_tertinggi;
    cout << "Minimal tawaran: Rp" << batas + 1 << "\nNominal Anda: ";
    cin >> nominal;
    if (nominal <= batas) { cout << "Tawaran terlalu rendah!\n"; return; }

    // Update barang
    db_barang[idx].harga_tertinggi = nominal;
    salinTeks(db_barang[idx].pemenang, currentUser);

    // Catat riwayat tawaran
    Tawaran t;
    t.id_barang = id;
    salinTeks(t.username, currentUser);
    t.nominal = nominal;
    t.waktu = time(0);
    db_tawaran[jml_tawaran] = t; jml_tawaran++;

    saveData();
    cout << "Tawaran masuk! Anda memimpin lelang ini.\n";
}

void aktivitasSaya() {
    cout << "\n--- AKTIVITAS SAYA (" << currentUser << ") ---\n";
    cout << "\n[ Riwayat Penawaran ]\n";
    int adaT = 0;
    for (int i = 0; i < jml_tawaran; i++)
        if (teksSama(db_tawaran[i].username, currentUser)) {
            char w[20]; formatWaktu(db_tawaran[i].waktu, w);
            cout << "  Barang ID " << db_tawaran[i].id_barang
                      << " | Rp" << db_tawaran[i].nominal
                      << " | " << w << "\n";
            adaT++;
        }
    if (adaT == 0) cout << "  Belum ada penawaran.\n";

    cout << "\n[ Barang yang Sedang Anda Pimpin ]\n";
    int adaP = 0;
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 0 && teksSama(db_barang[i].pemenang, currentUser)) {
            cout << "  ID: " << db_barang[i].id << " | " << db_barang[i].nama
                      << " (Rp" << db_barang[i].harga_tertinggi << ")\n";
            adaP++;
        }
    if (adaP == 0) cout << "  Tidak sedang memimpin lelang apapun.\n";
}

// ============================================================
//  MODUL 7: LAPORAN
// ============================================================
void laporanAktif() {
    Barang tmp[100]; int n = 0;
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 0) tmp[n++] = db_barang[i];
    if (n == 0) { cout << "\nTidak ada barang aktif.\n"; return; }
    sortByHarga(tmp, n);
    cout << "\n[ LAPORAN BARANG AKTIF - Urut Harga ]\n";
    for (int i = 0; i < n; i++)
        cout << "ID: " << tmp[i].id << " | " << tmp[i].nama
                  << " | Rp" << tmp[i].harga_tertinggi
                  << " | Pemenang sementara: " << tmp[i].pemenang << "\n";
}

void laporanTutup() {
    cout << "\n[ LAPORAN BARANG SUDAH TUTUP ]\n";
    int n = 0;
    for (int i = 0; i < jml_barang; i++)
        if (db_barang[i].status == 1) {
            cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama;
            if (teksSama(db_barang[i].pemenang, "-") == false &&
                db_barang[i].pemenang[0] != '\0')
                cout << " | Pemenang: " << db_barang[i].pemenang
                          << " (Rp" << db_barang[i].harga_tertinggi << ")";
            else cout << " | Tanpa penawar";
            cout << "\n";
            n++;
        }
    if (n == 0) cout << "  Belum ada barang ditutup.\n";
}

void laporanRiwayat() {
    cout << "\n[ LAPORAN SEMUA RIWAYAT PENAWARAN ]\n";
    if (jml_tawaran == 0) { cout << "  Belum ada penawaran.\n"; return; }
    for (int i = 0; i < jml_tawaran; i++) {
        char w[20]; formatWaktu(db_tawaran[i].waktu, w);
        cout << "Barang ID " << db_tawaran[i].id_barang
                  << " | " << db_tawaran[i].username
                  << " | Rp" << db_tawaran[i].nominal
                  << " | " << w << "\n";
    }
}

void eksporLaporan() {
    ofstream f("laporan.csv");
    if (!f.is_open()) { cout << "Gagal membuat file.\n"; return; }
    f << "ID;Nama;Kategori;Harga Awal;Harga Tertinggi;Pemenang;Status\n";
    for (int i = 0; i < jml_barang; i++)
        f << db_barang[i].id << ";" << db_barang[i].nama << ";"
          << db_barang[i].kategori << ";" << db_barang[i].harga_awal << ";"
          << db_barang[i].harga_tertinggi << ";" << db_barang[i].pemenang << ";"
          << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
    f.close();
    cout << "\nLaporan diekspor ke 'laporan.csv' (bisa dibuka di Excel).\n";
}

void menuLaporan() {
    int pil;
    do {
        cout << "\n=== LAPORAN ===\n";
        cout << "[1] Barang Aktif (urut harga)\n";
        cout << "[2] Barang Sudah Tutup + Pemenang\n";
        cout << "[3] Semua Riwayat Penawaran\n";
        cout << "[4] Ekspor ke laporan.csv\n";
        cout << "[5] Kembali\nPilih: ";
        cin >> pil;
        switch (pil) {
            case 1: laporanAktif();   break;
            case 2: laporanTutup();   break;
            case 3: laporanRiwayat(); break;
            case 4: eksporLaporan();  break;
            case 5: break;
            default: cout << "Pilihan tidak valid!\n";
        }
    } while (pil != 5);
}

// ============================================================
//  MODUL 4: MENU & NAVIGASI
// ============================================================
void menuAdmin() {
    int pilihan;
    do {
        checkExpiredAuctions();
        cout << "\n=== MENU ADMIN (" << currentUser << ") ===\n";
        cout << "[1] Tambah Barang\n";
        cout << "[2] Tutup Lelang Manual\n";
        cout << "[3] Lihat Semua Barang\n";
        cout << "[4] Hapus Barang\n";
        cout << "[5] Laporan\n";
        cout << "[6] Logout\nPilih: ";
        cin >> pilihan;
        switch (pilihan) {
            case 1: tambahBarang();      break;
            case 2: tutupLelangManual(); break;
            case 3: lihatSemuaBarang();  break;
            case 4: hapusBarang();       break;
            case 5: menuLaporan();       break;
            case 6: cout << "Logout berhasil.\n"; break;
            default: cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 6);
    salinTeks(currentUser, "");
    salinTeks(currentRole, "");
}

void menuPeserta() {
    int pilihan;
    do {
        checkExpiredAuctions();
        cout << "\n=== MENU PESERTA (" << currentUser << ") ===\n";
        cout << "[1] Lihat Lelang Aktif\n";
        cout << "[2] Cari Barang\n";
        cout << "[3] Tawar Barang\n";
        cout << "[4] Aktivitas Saya\n";
        cout << "[5] Logout\nPilih: ";
        cin >> pilihan;
        switch (pilihan) {
            case 1: lihatLelangAktif(); break;
            case 2: cariBarang();       break;
            case 3: tawarBarang();      break;
            case 4: aktivitasSaya();    break;
            case 5: cout << "Logout berhasil.\n"; break;
            default: cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 5);
    salinTeks(currentUser, "");
    salinTeks(currentRole, "");
}

int main() {
    system("clear");
    loadData();
    int pilihan;
    do {
        checkExpiredAuctions();
        cout << "\n=== SISTEM LELANG (SILELANG) ===\n";
        cout << "[1] Login\n[2] Daftar\n[3] Keluar\nPilih: ";
        cin >> pilihan;
        if (pilihan == 1) {
            login();
            if (teksSama(currentUser, "") == false) {
                if (teksSama(currentRole, "ADMIN")) menuAdmin();
                else menuPeserta();
            }
        } else if (pilihan == 2) {
            daftarAkun();
        }
    } while (pilihan != 3);
    saveData();
    cout << "Program selesai.\n";
    return 0;
}