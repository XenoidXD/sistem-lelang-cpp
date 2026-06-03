#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
using namespace std;

// ============================================================
//  STRUCT
// ============================================================
struct Akun {
    string username;
    string password;
    string role;
};

struct Barang {
    int id;
    string nama;
    string deskripsi;
    string kategori;
    long long harga_awal;
    long long harga_tertinggi;
    string pemenang;
    time_t waktu_tutup;
    int status;   // 0=aktif, 1=tutup
};

struct Tawaran {
    int id_barang;
    string username;
    long long nominal;
    time_t waktu;
};

// ============================================================
//  VARIABEL GLOBAL
// ============================================================
Akun    db_akun[100];    int jml_akun    = 0;
Barang  db_barang[100];  int jml_barang  = 0;
Tawaran db_tawaran[100]; int jml_tawaran = 0;

string currentUser = "";
string currentRole = "";
const string ADMIN_CODE = "ADMIN123";

// ============================================================
//  UTILITAS TERMINAL
// ============================================================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\nTekan Enter untuk melanjutkan...";
    cin.ignore();
    cin.get();
}

// ============================================================
//  KONVERSI STRING KE ANGKA
//  (pengganti stoi/stoll agar kompatibel dengan compiler lama)
// ============================================================

// Konversi string ke int — pengganti stoi()
int toInt(const string& s) {
    int hasil = 0;
    size_t i = 0;
    bool negatif = false;
    if (s[i] == '-') { negatif = true; i++; }
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
        hasil = hasil * 10 + (s[i] - '0');
        i++;
    }
    return negatif ? -hasil : hasil;
}

// Konversi string ke long long — pengganti stoll()
long long toLongLong(const string& s) {
    long long hasil = 0;
    size_t i = 0;
    bool negatif = false;
    if (s[i] == '-') { negatif = true; i++; }
    while (i < s.size() && s[i] >= '0' && s[i] <= '9') {
        hasil = hasil * 10 + (s[i] - '0');
        i++;
    }
    return negatif ? -hasil : hasil;
}

// ============================================================
//  SORTING
// ============================================================

// Bubble Sort — descending berdasarkan harga tertinggi
void sortByHarga(Barang arr[], int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j].harga_tertinggi < arr[j + 1].harga_tertinggi) {
                Barang tmp = arr[j];
                arr[j]     = arr[j + 1];
                arr[j + 1] = tmp;
            }
}

// Insertion Sort — ascending berdasarkan nama (alfabetis)
void sortByNama(Barang arr[], int n) {
    for (int i = 1; i < n; i++) {
        Barang kunci = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].nama > kunci.nama) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = kunci;
    }
}

// ============================================================
//  SEARCHING
// ============================================================

// Sequential Search — mencari barang berdasarkan ID
// Return: index di db_barang, atau -1 kalau tidak ditemukan
int searchBarangById(int id) {
    int i = 0;
    while (i < jml_barang && db_barang[i].id != id)
        i++;
    if (i < jml_barang) return i;
    return -1;
}

// ============================================================
//  DAFTAR & LOGIN
// ============================================================
void daftarAkun() {
    clearScreen();
    if (jml_akun >= 100) { cout << "Database akun penuh!\n"; pause(); return; }
    Akun a;
    cout << "=== DAFTAR AKUN BARU ===\n";
    cout << "Username : "; cin >> a.username;
    for (int i = 0; i < jml_akun; i++)
        if (db_akun[i].username == a.username) {
            cout << "Username sudah terdaftar!\n"; pause(); return;
        }
    cout << "Password : "; cin >> a.password;

    int roleChoice;
    cout << "\nPilih Role:\n[1] Peserta\n[2] Admin\nPilihan: "; cin >> roleChoice;

    if (roleChoice == 2) {
        string kode;
        cout << "Kode Admin: "; cin >> kode;
        if (kode == ADMIN_CODE) a.role = "ADMIN";
        else { cout << "Kode salah! Didaftarkan sebagai Peserta.\n"; a.role = "PESERTA"; }
    } else a.role = "PESERTA";

    db_akun[jml_akun] = a;
    jml_akun++;

    // Simpan akun ke file (baris pertama = header)
    ofstream fAkun("akun.csv");
    fAkun << "username,password,role\n";
    for (int i = 0; i < jml_akun; i++)
        fAkun << db_akun[i].username << ","
              << db_akun[i].password << ","
              << db_akun[i].role << "\n";
    fAkun.close();

    cout << "\nAkun berhasil dibuat sebagai " << a.role << "!\n";
    pause();
}

void login() {
    clearScreen();
    string usr, pwd;
    cout << "=== LOGIN ===\n";
    cout << "Username : "; cin >> usr;
    cout << "Password : "; cin >> pwd;
    for (int i = 0; i < jml_akun; i++)
        if (db_akun[i].username == usr && db_akun[i].password == pwd) {
            currentUser = db_akun[i].username;
            currentRole = db_akun[i].role;
            cout << "\nLogin berhasil! Selamat datang, " << currentUser << ".\n";
            pause();
            return;
        }
    cout << "\nUsername atau password salah!\n";
    pause();
}

// ============================================================
//  MENU LAPORAN (sub-menu admin)
// ============================================================
void menuLaporan() {
    int pil;
    do {
        clearScreen();
        cout << "=== LAPORAN ===\n";
        cout << "[1] Barang Aktif (urut harga)\n";
        cout << "[2] Barang Sudah Tutup + Pemenang\n";
        cout << "[3] Semua Riwayat Penawaran\n";
        cout << "[4] Ekspor ke laporan.csv\n";
        cout << "[5] Kembali\nPilih: ";
        cin >> pil;

        if (pil == 1) {
            clearScreen();
            Barang tmp[100]; int n = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) tmp[n++] = db_barang[i];
            if (n == 0) { cout << "Tidak ada barang aktif.\n"; pause(); continue; }
            sortByHarga(tmp, n);
            cout << "=== LAPORAN BARANG AKTIF (Urut Harga) ===\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(14) << "Harga"
                 << setw(12) << "Sementara"
                 << "\n";
            cout << string(63, '-') << "\n";
            for (int i = 0; i < n; i++)
                cout << left
                     << setw(4)  << tmp[i].id
                     << setw(33) << tmp[i].nama.substr(0, 31)
                     << "Rp" << setw(12) << tmp[i].harga_tertinggi
                     << setw(12) << tmp[i].pemenang << "\n";
            cout << string(63, '-') << "\n";
            cout << "Total: " << n << " barang aktif.\n";
            pause();

        } else if (pil == 2) {
            clearScreen();
            cout << "=== LAPORAN BARANG SUDAH TUTUP ===\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(14) << "Harga Akhir"
                 << setw(12) << "Pemenang"
                 << "\n";
            cout << string(63, '-') << "\n";
            int n = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 1) {
                    cout << left
                         << setw(4)  << db_barang[i].id
                         << setw(33) << db_barang[i].nama.substr(0, 31);
                    if (db_barang[i].pemenang != "-" && !db_barang[i].pemenang.empty())
                        cout << "Rp" << setw(12) << db_barang[i].harga_tertinggi
                             << setw(12) << db_barang[i].pemenang;
                    else
                        cout << setw(14) << "-" << setw(12) << "Tanpa penawar";
                    cout << "\n";
                    n++;
                }
            if (n == 0) cout << "Belum ada barang yang ditutup.\n";
            cout << string(63, '-') << "\n";
            pause();

        } else if (pil == 3) {
            clearScreen();
            cout << "=== SEMUA RIWAYAT PENAWARAN ===\n";
            if (jml_tawaran == 0) { cout << "Belum ada penawaran.\n"; pause(); continue; }
            // Header kolom
            cout << left
                 << setw(10) << "Barang ID"
                 << setw(12) << "Username"
                 << setw(14) << "Nominal"
                 << setw(17) << "Waktu"
                 << "\n";
            cout << string(53, '-') << "\n";
            for (int i = 0; i < jml_tawaran; i++) {
                char w[20];
                struct tm* info = localtime(&db_tawaran[i].waktu);
                strftime(w, 20, "%Y-%m-%d %H:%M", info);
                cout << left
                     << setw(10) << db_tawaran[i].id_barang
                     << setw(12) << db_tawaran[i].username
                     << "Rp" << setw(12) << db_tawaran[i].nominal
                     << setw(17) << w << "\n";
            }
            cout << string(53, '-') << "\n";
            pause();

        } else if (pil == 4) {
            // Ekspor laporan ke CSV dengan header
            ofstream f("laporan.csv");
            if (!f.is_open()) { cout << "Gagal membuat file.\n"; pause(); continue; }
            f << "ID;Nama;Kategori;Harga Awal;Harga Tertinggi;Pemenang;Status\n";
            for (int i = 0; i < jml_barang; i++)
                f << db_barang[i].id << ";" << db_barang[i].nama << ";"
                  << db_barang[i].kategori << ";" << db_barang[i].harga_awal << ";"
                  << db_barang[i].harga_tertinggi << ";" << db_barang[i].pemenang << ";"
                  << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
            f.close();
            cout << "\nLaporan diekspor ke 'laporan.csv'.\n";
            pause();

        } else if (pil != 5) {
            cout << "Pilihan tidak valid!\n";
            pause();
        }
    } while (pil != 5);
}

// ============================================================
//  MENU ADMIN
// ============================================================
void menuAdmin() {
    int pilihan;
    do {
        // Cek otomatis lelang yang sudah expired
        time_t now = time(0);
        for (int i = 0; i < jml_barang; i++)
            if (db_barang[i].status == 0 && now >= db_barang[i].waktu_tutup)
                db_barang[i].status = 1;

        clearScreen();
        cout << "=== MENU ADMIN (" << currentUser << ") ===\n";
        cout << "[1] Tambah Barang\n";
        cout << "[2] Tutup Lelang Manual\n";
        cout << "[3] Lihat Semua Barang\n";
        cout << "[4] Hapus Barang\n";
        cout << "[5] Laporan\n";
        cout << "[6] Logout\nPilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            clearScreen();
            if (jml_barang >= 100) { cout << "Database barang penuh!\n"; pause(); continue; }
            Barang b;
            b.id = (jml_barang == 0) ? 1 : db_barang[jml_barang - 1].id + 1;
            cin.ignore();
            cout << "=== TAMBAH BARANG (ID " << b.id << ") ===\n";
            cout << "Nama Barang  : "; getline(cin, b.nama);
            cout << "Deskripsi    : "; getline(cin, b.deskripsi);
            cout << "Kategori     : "; getline(cin, b.kategori);
            cout << "Harga Awal   : Rp"; cin >> b.harga_awal;
            int durasi;
            cout << "Durasi (Jam) : "; cin >> durasi;

            b.harga_tertinggi = b.harga_awal;
            b.pemenang        = "-";
            b.status          = 0;
            b.waktu_tutup     = time(0) + ((time_t)durasi * 3600);

            db_barang[jml_barang] = b; jml_barang++;

            // Simpan barang ke file (baris pertama = header)
            ofstream fBarang("barang.csv");
            fBarang << "id,nama,deskripsi,kategori,harga_awal,harga_tertinggi,pemenang,waktu_tutup,status\n";
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();

            char w[20];
            struct tm* info = localtime(&b.waktu_tutup);
            strftime(w, 20, "%Y-%m-%d %H:%M", info);
            cout << "\nBarang tersimpan! Lelang tutup pada: " << w << "\n";
            pause();

        } else if (pilihan == 2) {
            clearScreen();
            cout << "=== TUTUP LELANG MANUAL ===\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(14) << "Max Bid"
                 << "\n";
            cout << string(51, '-') << "\n";
            int ada = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) {
                    cout << left
                         << setw(4)  << db_barang[i].id
                         << setw(33) << db_barang[i].nama.substr(0, 31)
                         << "Rp" << db_barang[i].harga_tertinggi << "\n";
                    ada++;
                }
            if (ada == 0) { cout << "Tidak ada lelang aktif.\n"; pause(); continue; }
            cout << string(51, '-') << "\n";

            int id;
            cout << "ID barang yang ingin ditutup: "; cin >> id;
            int idx = searchBarangById(id);
            if (idx == -1) { cout << "ID tidak ditemukan.\n"; pause(); continue; }
            if (db_barang[idx].status != 0) { cout << "Barang sudah tutup.\n"; pause(); continue; }

            db_barang[idx].status = 1;
            cout << "\nLelang '" << db_barang[idx].nama << "' berhasil ditutup.\n";
            if (db_barang[idx].pemenang != "-" && !db_barang[idx].pemenang.empty())
                cout << "Pemenang : " << db_barang[idx].pemenang
                     << " (Rp" << db_barang[idx].harga_tertinggi << ")\n";
            else cout << "Tidak ada penawar.\n";

            // Simpan perubahan status ke file
            ofstream fBarang("barang.csv");
            fBarang << "id,nama,deskripsi,kategori,harga_awal,harga_tertinggi,pemenang,waktu_tutup,status\n";
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();
            pause();

        } else if (pilihan == 3) {
            clearScreen();
            if (jml_barang == 0) { cout << "Belum ada barang.\n"; pause(); continue; }
            Barang tmp[100];
            for (int i = 0; i < jml_barang; i++) tmp[i] = db_barang[i];

            int pil;
            cout << "=== LIHAT SEMUA BARANG ===\n";
            cout << "[1] Urutkan Nama (A-Z)\n[2] Urutkan Harga (tertinggi)\n[3] Tanpa urut\nPilih: ";
            cin >> pil;
            if (pil == 1) sortByNama(tmp, jml_barang);
            else if (pil == 2) sortByHarga(tmp, jml_barang);

            cout << "\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(12) << "Kategori"
                 << setw(14) << "Max Bid"
                 << setw(7)  << "Status"
                 << setw(12) << "Pemenang"
                 << "\n";
            cout << string(82, '-') << "\n";
            for (int i = 0; i < jml_barang; i++)
                cout << left
                     << setw(4)  << tmp[i].id
                     << setw(33) << tmp[i].nama.substr(0, 31)
                     << setw(12) << tmp[i].kategori.substr(0, 10)
                     << "Rp" << setw(12) << tmp[i].harga_tertinggi
                     << setw(7)  << (tmp[i].status == 0 ? "AKTIF" : "TUTUP")
                     << setw(12) << tmp[i].pemenang << "\n";
            cout << string(82, '-') << "\n";
            cout << "Total: " << jml_barang << " barang.\n";
            pause();

        } else if (pilihan == 4) {
            clearScreen();
            if (jml_barang == 0) { cout << "Belum ada barang.\n"; pause(); continue; }
            cout << "=== HAPUS BARANG ===\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(7)  << "Status"
                 << "\n";
            cout << string(44, '-') << "\n";
            for (int i = 0; i < jml_barang; i++)
                cout << left
                     << setw(4)  << db_barang[i].id
                     << setw(33) << db_barang[i].nama.substr(0, 31)
                     << setw(7)  << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
            cout << string(44, '-') << "\n";

            int id;
            cout << "ID barang yang ingin dihapus: "; cin >> id;
            int idx = searchBarangById(id);
            if (idx == -1) { cout << "ID tidak ditemukan.\n"; pause(); continue; }

            for (int i = idx; i < jml_barang - 1; i++) db_barang[i] = db_barang[i + 1];
            jml_barang--;

            // Simpan setelah hapus
            ofstream fBarang("barang.csv");
            fBarang << "id,nama,deskripsi,kategori,harga_awal,harga_tertinggi,pemenang,waktu_tutup,status\n";
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();
            cout << "Barang ID " << id << " berhasil dihapus.\n";
            pause();

        } else if (pilihan == 5) {
            menuLaporan();
        } else if (pilihan == 6) {
            cout << "\nSampai jumpa, " << currentUser << "!\n";
            pause();
        } else {
            cout << "Pilihan tidak valid!\n";
            pause();
        }
    } while (pilihan != 6);

    currentUser = "";
    currentRole = "";
}

// ============================================================
//  MENU PESERTA
// ============================================================
void menuPeserta() {
    int pilihan;
    do {
        // Cek otomatis lelang yang sudah expired
        time_t now = time(0);
        for (int i = 0; i < jml_barang; i++)
            if (db_barang[i].status == 0 && now >= db_barang[i].waktu_tutup)
                db_barang[i].status = 1;

        clearScreen();
        cout << "=== MENU PESERTA (" << currentUser << ") ===\n";
        cout << "[1] Lihat Lelang Aktif\n";
        cout << "[2] Cari Barang\n";
        cout << "[3] Tawar Barang\n";
        cout << "[4] Aktivitas Saya\n";
        cout << "[5] Logout\nPilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            clearScreen();
            Barang tmp[100]; int n = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) tmp[n++] = db_barang[i];
            if (n == 0) { cout << "Tidak ada lelang aktif saat ini.\n"; pause(); continue; }

            int pil;
            cout << "=== LELANG AKTIF ===\n";
            cout << "[1] Urutkan Harga (tertinggi)\n[2] Urutkan Nama (A-Z)\n[3] Tanpa urut\nPilih: ";
            cin >> pil;
            if (pil == 1) sortByHarga(tmp, n);
            else if (pil == 2) sortByNama(tmp, n);

            cout << "\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(12) << "Kategori"
                 << setw(14) << "Max Bid"
                 << setw(17) << "Tutup"
                 << "\n";
            cout << string(80, '-') << "\n";
            for (int i = 0; i < n; i++) {
                char w[20];
                struct tm* info = localtime(&tmp[i].waktu_tutup);
                strftime(w, 20, "%Y-%m-%d %H:%M", info);
                cout << left
                     << setw(4)  << tmp[i].id
                     << setw(33) << tmp[i].nama.substr(0, 31)
                     << setw(12) << tmp[i].kategori.substr(0, 10)
                     << "Rp" << setw(12) << tmp[i].harga_tertinggi
                     << setw(17) << w << "\n";
            }
            cout << string(80, '-') << "\n";
            cout << "Total: " << n << " lelang aktif.\n";
            pause();

        } else if (pilihan == 2) {
            clearScreen();
            if (jml_barang == 0) { cout << "Belum ada barang.\n"; pause(); continue; }
            cout << "=== CARI BARANG (Sequential Search) ===\n";
            string kata;
            cin.ignore();
            cout << "Kata kunci nama: "; getline(cin, kata);

            bool ketemu = false;
            cout << "\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(12) << "Kategori"
                 << setw(14) << "Max Bid"
                 << setw(6)  << "Status"
                 << "\n";
            cout << string(69, '-') << "\n";
            // Sequential Search: telusuri tiap elemen satu per satu
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].nama.find(kata) != string::npos) {
                    cout << left
                         << setw(4)  << db_barang[i].id
                         << setw(33) << db_barang[i].nama.substr(0, 31)
                         << setw(12) << db_barang[i].kategori.substr(0, 10)
                         << "Rp" << setw(12) << db_barang[i].harga_tertinggi
                         << setw(6)  << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
                    ketemu = true;
                }
            if (!ketemu) cout << "Tidak ada barang yang cocok.\n";
            cout << string(69, '-') << "\n";
            pause();

        } else if (pilihan == 3) {
            clearScreen();
            if (jml_tawaran >= 100) { cout << "Penyimpanan tawaran penuh.\n"; pause(); continue; }
            cout << "=== TAWAR BARANG ===\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(14) << "Max Bid"
                 << "\n";
            cout << string(51, '-') << "\n";
            int ada = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) {
                    cout << left
                         << setw(4)  << db_barang[i].id
                         << setw(33) << db_barang[i].nama.substr(0, 31)
                         << "Rp" << db_barang[i].harga_tertinggi << "\n";
                    ada++;
                }
            if (ada == 0) { cout << "Tidak ada lelang aktif.\n"; pause(); continue; }
            cout << string(51, '-') << "\n";

            int id; long long nominal;
            cout << "ID Barang   : "; cin >> id;
            int idx = searchBarangById(id);
            if (idx == -1 || db_barang[idx].status == 1) {
                cout << "Barang tidak ada atau sudah tutup!\n"; pause(); continue;
            }

            long long batas = db_barang[idx].harga_tertinggi;
            cout << "Minimal bid : Rp" << batas + 1 << "\n";
            cout << "Tawaran Anda: Rp"; cin >> nominal;
            if (nominal <= batas) { cout << "Tawaran terlalu rendah!\n"; pause(); continue; }

            // Update barang
            db_barang[idx].harga_tertinggi = nominal;
            db_barang[idx].pemenang        = currentUser;

            // Catat riwayat tawaran
            db_tawaran[jml_tawaran].id_barang = id;
            db_tawaran[jml_tawaran].username  = currentUser;
            db_tawaran[jml_tawaran].nominal   = nominal;
            db_tawaran[jml_tawaran].waktu     = time(0);
            jml_tawaran++;

            // Simpan barang ke file
            ofstream fBarang("barang.csv");
            fBarang << "id,nama,deskripsi,kategori,harga_awal,harga_tertinggi,pemenang,waktu_tutup,status\n";
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();

            // Simpan tawaran ke file (baris pertama = header)
            ofstream fTawaran("tawaran.csv");
            fTawaran << "id_barang,username,nominal,waktu\n";
            for (int i = 0; i < jml_tawaran; i++)
                fTawaran << db_tawaran[i].id_barang << "," << db_tawaran[i].username << ","
                         << db_tawaran[i].nominal << "," << db_tawaran[i].waktu << "\n";
            fTawaran.close();

            cout << "\nTawaran Rp" << nominal << " berhasil masuk!\n";
            cout << "Anda sedang memimpin lelang '" << db_barang[idx].nama << "'.\n";
            pause();

        } else if (pilihan == 4) {
            clearScreen();
            cout << "=== AKTIVITAS SAYA (" << currentUser << ") ===\n";

            cout << "\n[ Riwayat Penawaran ]\n";
            // Header kolom
            cout << left
                 << setw(10) << "Barang ID"
                 << setw(12) << "Username"
                 << setw(14) << "Nominal"
                 << setw(17) << "Waktu"
                 << "\n";
            cout << string(53, '-') << "\n";
            int adaT = 0;
            for (int i = 0; i < jml_tawaran; i++)
                if (db_tawaran[i].username == currentUser) {
                    char w[20];
                    struct tm* info = localtime(&db_tawaran[i].waktu);
                    strftime(w, 20, "%Y-%m-%d %H:%M", info);
                    cout << left
                         << setw(10) << db_tawaran[i].id_barang
                         << setw(12) << db_tawaran[i].username
                         << "Rp" << setw(12) << db_tawaran[i].nominal
                         << setw(17) << w << "\n";
                    adaT++;
                }
            if (adaT == 0) cout << "Belum ada penawaran.\n";
            cout << string(53, '-') << "\n";

            cout << "\n[ Lelang yang Sedang Anda Pimpin ]\n";
            // Header kolom
            cout << left
                 << setw(4)  << "ID"
                 << setw(33) << "Nama"
                 << setw(14) << "Tawaran Anda"
                 << "\n";
            cout << string(51, '-') << "\n";
            int adaP = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0 && db_barang[i].pemenang == currentUser) {
                    cout << left
                         << setw(4)  << db_barang[i].id
                         << setw(33) << db_barang[i].nama.substr(0, 31)
                         << "Rp" << db_barang[i].harga_tertinggi << "\n";
                    adaP++;
                }
            if (adaP == 0) cout << "Tidak sedang memimpin lelang apapun.\n";
            cout << string(51, '-') << "\n";
            pause();

        } else if (pilihan == 5) {
            cout << "\nSampai jumpa, " << currentUser << "!\n";
            pause();
        } else {
            cout << "Pilihan tidak valid!\n";
            pause();
        }
    } while (pilihan != 5);

    currentUser = "";
    currentRole = "";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    clearScreen();

    // Muat data dari file
    string buf;

    ifstream fAkun("akun.csv");
    if (fAkun.is_open()) {
        getline(fAkun, buf); // skip header
        while (getline(fAkun, db_akun[jml_akun].username, ',')) {
            getline(fAkun, db_akun[jml_akun].password, ',');
            getline(fAkun, db_akun[jml_akun].role);
            jml_akun++;
        }
        fAkun.close();
    }

    ifstream fBarang("barang.csv");
    if (fBarang.is_open()) {
        getline(fBarang, buf); // skip header
        while (getline(fBarang, buf, ',')) {
            db_barang[jml_barang].id = toInt(buf);
            getline(fBarang, db_barang[jml_barang].nama, ',');
            getline(fBarang, db_barang[jml_barang].deskripsi, ',');
            getline(fBarang, db_barang[jml_barang].kategori, ',');
            getline(fBarang, buf, ','); db_barang[jml_barang].harga_awal      = toLongLong(buf);
            getline(fBarang, buf, ','); db_barang[jml_barang].harga_tertinggi = toLongLong(buf);
            getline(fBarang, db_barang[jml_barang].pemenang, ',');
            getline(fBarang, buf, ','); db_barang[jml_barang].waktu_tutup = (time_t)toLongLong(buf);
            getline(fBarang, buf);     db_barang[jml_barang].status       = toInt(buf);
            jml_barang++;
        }
        fBarang.close();
    }

    ifstream fTawaran("tawaran.csv");
    if (fTawaran.is_open()) {
        getline(fTawaran, buf); // skip header
        while (getline(fTawaran, buf, ',')) {
            db_tawaran[jml_tawaran].id_barang = toInt(buf);
            getline(fTawaran, db_tawaran[jml_tawaran].username, ',');
            getline(fTawaran, buf, ','); db_tawaran[jml_tawaran].nominal = toLongLong(buf);
            getline(fTawaran, buf);     db_tawaran[jml_tawaran].waktu   = (time_t)toLongLong(buf);
            jml_tawaran++;
        }
        fTawaran.close();
    }

    // Loop utama program
    int pilihan;
    do {
        // Cek expired di menu utama
        time_t now = time(0);
        for (int i = 0; i < jml_barang; i++)
            if (db_barang[i].status == 0 && now >= db_barang[i].waktu_tutup)
                db_barang[i].status = 1;

        clearScreen();
        cout << "=== SILELANG — Sistem Informasi Lelang Barang Bekas ===\n";
        cout << "[1] Login\n[2] Daftar Akun\n[3] Keluar\nPilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            login();
            if (!currentUser.empty()) {
                if (currentRole == "ADMIN") menuAdmin();
                else menuPeserta();
            }
        } else if (pilihan == 2) {
            daftarAkun();
        } else if (pilihan == 3) {
            clearScreen();
            cout << "Terima kasih telah menggunakan SILELANG. Sampai jumpa!\n\n";
        } else {
            cout << "Pilihan tidak valid!\n";
            pause();
        }
    } while (pilihan != 3);

    return 0;
}