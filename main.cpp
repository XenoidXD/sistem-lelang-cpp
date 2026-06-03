#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
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
//  BERSIHKAN TERMINAL & SYSTEM PAUSE
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
    if (jml_akun >= 100) { cout << "Database akun penuh!\n"; return; }
    Akun a;
    cout << "\n--- DAFTAR AKUN BARU ---\nUsername: "; cin >> a.username;
    for (int i = 0; i < jml_akun; i++)
        if (db_akun[i].username == a.username) {
            cout << "Username sudah terdaftar!\n"; return;
        }
    cout << "Password: "; cin >> a.password;

    int roleChoice;
    cout << "Pilih Role:\n[1] Peserta\n[2] Admin\nPilihan: "; cin >> roleChoice;

    if (roleChoice == 2) {
        string kode;
        cout << "Kode Admin: "; cin >> kode;
        if (kode == ADMIN_CODE) a.role = "ADMIN";
        else { cout << "Kode salah! Jadi Peserta.\n"; a.role = "PESERTA"; }
    } else a.role = "PESERTA";

    db_akun[jml_akun] = a; 
    jml_akun++;

    // Simpan akun ke file
    ofstream fAkun("akun.csv");
    for (int i = 0; i < jml_akun; i++)
        fAkun << db_akun[i].username << ","
              << db_akun[i].password << ","
              << db_akun[i].role << "\n";
    fAkun.close();

    cout << "Akun berhasil dibuat!\n";
}

void login() {
    string usr, pwd;
    cout << "\n--- LOGIN ---\nUsername: "; cin >> usr;
    cout << "Password: "; cin >> pwd;
    for (int i = 0; i < jml_akun; i++)
        if (db_akun[i].username == usr && db_akun[i].password == pwd) {
            currentUser = db_akun[i].username;
            currentRole = db_akun[i].role;
            cout << "Login berhasil, " << currentUser << "!\n";
            return;
        }
    cout << "Gagal login!\n";
}

// ============================================================
//  MENU LAPORAN (sub-menu admin)
// ============================================================
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

        if (pil == 1) {
            // Laporan barang aktif, diurutkan harga tertinggi
            Barang tmp[100]; int n = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) tmp[n++] = db_barang[i];
            if (n == 0) { cout << "\nTidak ada barang aktif.\n"; continue; }
            sortByHarga(tmp, n);
            cout << "\n[ LAPORAN BARANG AKTIF - Urut Harga ]\n";
            for (int i = 0; i < n; i++)
                cout << "ID: " << tmp[i].id << " | " << tmp[i].nama
                     << " | Rp" << tmp[i].harga_tertinggi
                     << " | Pemenang sementara: " << tmp[i].pemenang << "\n";

        } else if (pil == 2) {
            // Laporan barang yang sudah tutup
            cout << "\n[ LAPORAN BARANG SUDAH TUTUP ]\n";
            int n = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 1) {
                    cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama;
                    if (db_barang[i].pemenang != "-" && !db_barang[i].pemenang.empty())
                        cout << " | Pemenang: " << db_barang[i].pemenang
                             << " (Rp" << db_barang[i].harga_tertinggi << ")";
                    else cout << " | Tanpa penawar";
                    cout << "\n"; n++;
                }
            if (n == 0) cout << "  Belum ada barang ditutup.\n";

        } else if (pil == 3) {
            // Laporan semua riwayat penawaran
            cout << "\n[ LAPORAN SEMUA RIWAYAT PENAWARAN ]\n";
            if (jml_tawaran == 0) { cout << "  Belum ada penawaran.\n"; continue; }
            for (int i = 0; i < jml_tawaran; i++) {
                char w[20];
                struct tm* info = localtime(&db_tawaran[i].waktu);
                strftime(w, 20, "%Y-%m-%d %H:%M", info);
                cout << "Barang ID " << db_tawaran[i].id_barang
                     << " | " << db_tawaran[i].username
                     << " | Rp" << db_tawaran[i].nominal
                     << " | " << w << "\n";
            }

        } else if (pil == 4) {
            // Ekspor laporan ke CSV (file I/O dengan fstream)
            ofstream f("laporan.csv");
            if (!f.is_open()) { cout << "Gagal membuat file.\n"; continue; }
            f << "ID;Nama;Kategori;Harga Awal;Harga Tertinggi;Pemenang;Status\n";
            for (int i = 0; i < jml_barang; i++)
                f << db_barang[i].id << ";" << db_barang[i].nama << ";"
                  << db_barang[i].kategori << ";" << db_barang[i].harga_awal << ";"
                  << db_barang[i].harga_tertinggi << ";" << db_barang[i].pemenang << ";"
                  << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
            f.close();
            cout << "\nLaporan diekspor ke 'laporan.csv'.\n";

        } else if (pil != 5) {
            cout << "Pilihan tidak valid!\n";
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

        cout << "\n=== MENU ADMIN (" << currentUser << ") ===\n";
        cout << "[1] Tambah Barang\n";
        cout << "[2] Tutup Lelang Manual\n";
        cout << "[3] Lihat Semua Barang\n";
        cout << "[4] Hapus Barang\n";
        cout << "[5] Laporan\n";
        cout << "[6] Logout\nPilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            // Tambah barang baru
            if (jml_barang >= 100) { cout << "Database barang penuh!\n"; continue; }
            Barang b;
            b.id = (jml_barang == 0) ? 1 : db_barang[jml_barang - 1].id + 1;
            cin.ignore();
            cout << "\n--- TAMBAH BARANG (ID " << b.id << ") ---\n";
            cout << "Nama Barang: "; getline(cin, b.nama);
            cout << "Deskripsi  : "; getline(cin, b.deskripsi);
            cout << "Kategori   : "; getline(cin, b.kategori);
            cout << "Harga Awal : "; cin >> b.harga_awal;
            int durasi;
            cout << "Durasi (Jam): "; cin >> durasi;

            b.harga_tertinggi = b.harga_awal;
            b.pemenang  = "-";
            b.status    = 0;
            b.waktu_tutup = time(0) + ((time_t)durasi * 3600);

            db_barang[jml_barang] = b; jml_barang++;

            // Simpan barang ke file
            ofstream fBarang("barang.csv");
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
            cout << "Barang tersimpan! Lelang tutup pada: " << w << "\n";

        } else if (pilihan == 2) {
            // Tutup lelang manual
            cout << "\n--- TUTUP LELANG MANUAL ---\n";
            int ada = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) {
                    cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                         << " | Max Bid: Rp" << db_barang[i].harga_tertinggi << "\n";
                    ada++;
                }
            if (ada == 0) { cout << "Tidak ada lelang aktif.\n"; continue; }

            int id;
            cout << "ID barang yang ingin ditutup: "; cin >> id;
            int idx = searchBarangById(id);
            if (idx == -1) { cout << "ID tidak ditemukan.\n"; continue; }
            if (db_barang[idx].status != 0) { cout << "Barang sudah tutup.\n"; continue; }

            db_barang[idx].status = 1;
            cout << "\nLelang '" << db_barang[idx].nama << "' ditutup.\n";
            if (db_barang[idx].pemenang != "-" && !db_barang[idx].pemenang.empty())
                cout << "PEMENANG: " << db_barang[idx].pemenang
                     << " (Rp" << db_barang[idx].harga_tertinggi << ")\n";
            else cout << "Tanpa penawar.\n";

            // Simpan perubahan status ke file
            ofstream fBarang("barang.csv");
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();

        } else if (pilihan == 3) {
            // Lihat semua barang dengan pilihan urutan
            if (jml_barang == 0) { cout << "\nBelum ada barang.\n"; continue; }
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

        } else if (pilihan == 4) {
            // Hapus barang
            if (jml_barang == 0) { cout << "\nBelum ada barang.\n"; continue; }
            cout << "\n--- HAPUS BARANG ---\n";
            for (int i = 0; i < jml_barang; i++)
                cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                     << " | " << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";

            int id;
            cout << "ID barang yang ingin dihapus: "; cin >> id;
            int idx = searchBarangById(id);
            if (idx == -1) { cout << "ID tidak ditemukan.\n"; continue; }

            for (int i = idx; i < jml_barang - 1; i++) db_barang[i] = db_barang[i + 1];
            jml_barang--;

            // Simpan setelah hapus
            ofstream fBarang("barang.csv");
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();
            cout << "Barang ID " << id << " berhasil dihapus.\n";

        } else if (pilihan == 5) {
            menuLaporan();
        } else if (pilihan == 6) {
            cout << "Logout berhasil.\n";
        } else {
            cout << "Pilihan tidak valid!\n";
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

        cout << "\n=== MENU PESERTA (" << currentUser << ") ===\n";
        cout << "[1] Lihat Lelang Aktif\n";
        cout << "[2] Cari Barang\n";
        cout << "[3] Tawar Barang\n";
        cout << "[4] Aktivitas Saya\n";
        cout << "[5] Logout\nPilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            // Lihat semua lelang yang masih aktif
            Barang tmp[100]; int n = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) tmp[n++] = db_barang[i];
            if (n == 0) { cout << "\nTidak ada lelang aktif.\n"; continue; }

            int pil;
            cout << "\n--- LELANG AKTIF ---\n";
            cout << "[1] Urutkan Harga (tertinggi)\n[2] Urutkan Nama (A-Z)\n[3] Tanpa urut\nPilih: ";
            cin >> pil;
            if (pil == 1) sortByHarga(tmp, n);
            else if (pil == 2) sortByNama(tmp, n);

            cout << "\n";
            for (int i = 0; i < n; i++) {
                char w[20];
                struct tm* info = localtime(&tmp[i].waktu_tutup);
                strftime(w, 20, "%Y-%m-%d %H:%M", info);
                cout << "ID: " << tmp[i].id << " | " << tmp[i].nama
                     << " | " << tmp[i].kategori
                     << " | Max Bid: Rp" << tmp[i].harga_tertinggi
                     << " | Tutup: " << w << "\n";
            }
            cout << "Total: " << n << " lelang aktif.\n";

        } else if (pilihan == 2) {
            // Cari barang — Sequential Search berdasarkan nama
            if (jml_barang == 0) { cout << "\nBelum ada barang.\n"; continue; }
            cout << "\n--- CARI BARANG (Sequential Search) ---\n";
            string kata;
            cin.ignore();
            cout << "Kata kunci nama: "; getline(cin, kata);

            bool ketemu = false;
            cout << "\nHasil pencarian:\n";
            // Sequential Search: cek tiap elemen satu per satu
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].nama.find(kata) != string::npos) {
                    cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                         << " | Rp" << db_barang[i].harga_tertinggi
                         << " | " << (db_barang[i].status == 0 ? "AKTIF" : "TUTUP") << "\n";
                    ketemu = true;
                }
            if (!ketemu) cout << "Tidak ditemukan.\n";

        } else if (pilihan == 3) {
            // Tawar barang
            if (jml_tawaran >= 100) { cout << "Penyimpanan tawaran penuh.\n"; continue; }
            cout << "\n--- TAWAR BARANG ---\n";
            int ada = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0) {
                    cout << "ID: " << db_barang[i].id << " | " << db_barang[i].nama
                         << " | Max Bid: Rp" << db_barang[i].harga_tertinggi << "\n";
                    ada++;
                }
            if (ada == 0) { cout << "Tidak ada lelang aktif.\n"; continue; }

            int id; long long nominal;
            cout << "ID Barang: "; cin >> id;
            int idx = searchBarangById(id);
            if (idx == -1 || db_barang[idx].status == 1) {
                cout << "Barang tidak ada atau sudah tutup!\n"; continue;
            }

            long long batas = db_barang[idx].harga_tertinggi;
            cout << "Minimal tawaran: Rp" << batas + 1 << "\nNominal Anda: ";
            cin >> nominal;
            if (nominal <= batas) { cout << "Tawaran terlalu rendah!\n"; continue; }

            // Update barang
            db_barang[idx].harga_tertinggi = nominal;
            db_barang[idx].pemenang = currentUser;

            // Catat riwayat tawaran
            db_tawaran[jml_tawaran].id_barang = id;
            db_tawaran[jml_tawaran].username  = currentUser;
            db_tawaran[jml_tawaran].nominal   = nominal;
            db_tawaran[jml_tawaran].waktu     = time(0);
            jml_tawaran++;

            // Simpan barang dan tawaran ke file
            ofstream fBarang("barang.csv");
            for (int i = 0; i < jml_barang; i++)
                fBarang << db_barang[i].id << "," << db_barang[i].nama << ","
                        << db_barang[i].deskripsi << "," << db_barang[i].kategori << ","
                        << db_barang[i].harga_awal << "," << db_barang[i].harga_tertinggi << ","
                        << db_barang[i].pemenang << "," << db_barang[i].waktu_tutup << ","
                        << db_barang[i].status << "\n";
            fBarang.close();

            ofstream fTawaran("tawaran.csv");
            for (int i = 0; i < jml_tawaran; i++)
                fTawaran << db_tawaran[i].id_barang << "," << db_tawaran[i].username << ","
                         << db_tawaran[i].nominal << "," << db_tawaran[i].waktu << "\n";
            fTawaran.close();

            cout << "Tawaran masuk! Anda memimpin lelang ini.\n";

        } else if (pilihan == 4) {
            // Aktivitas saya — riwayat dan posisi saat ini
            cout << "\n--- AKTIVITAS SAYA (" << currentUser << ") ---\n";
            cout << "\n[ Riwayat Penawaran ]\n";
            int adaT = 0;
            for (int i = 0; i < jml_tawaran; i++)
                if (db_tawaran[i].username == currentUser) {
                    char w[20];
                    struct tm* info = localtime(&db_tawaran[i].waktu);
                    strftime(w, 20, "%Y-%m-%d %H:%M", info);
                    cout << "  Barang ID " << db_tawaran[i].id_barang
                         << " | Rp" << db_tawaran[i].nominal
                         << " | " << w << "\n";
                    adaT++;
                }
            if (adaT == 0) cout << "  Belum ada penawaran.\n";

            cout << "\n[ Barang yang Sedang Anda Pimpin ]\n";
            int adaP = 0;
            for (int i = 0; i < jml_barang; i++)
                if (db_barang[i].status == 0 && db_barang[i].pemenang == currentUser) {
                    cout << "  ID: " << db_barang[i].id << " | " << db_barang[i].nama
                         << " (Rp" << db_barang[i].harga_tertinggi << ")\n";
                    adaP++;
                }
            if (adaP == 0) cout << "  Tidak sedang memimpin lelang apapun.\n";

        } else if (pilihan == 5) {
            cout << "Logout berhasil.\n";
        } else {
            cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 5);

    currentUser = "";
    currentRole = "";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    system("clear");

    // Muat data dari file saat program dimulai
    string buf;

    ifstream fAkun("akun.csv");
    if (fAkun.is_open()) {
        while (getline(fAkun, db_akun[jml_akun].username, ',')) {
            getline(fAkun, db_akun[jml_akun].password, ',');
            getline(fAkun, db_akun[jml_akun].role);
            jml_akun++;
        }
        fAkun.close();
    }

    ifstream fBarang("barang.csv");
    if (fBarang.is_open()) {
        while (getline(fBarang, buf, ',')) {
            db_barang[jml_barang].id = stoi(buf);
            getline(fBarang, db_barang[jml_barang].nama, ',');
            getline(fBarang, db_barang[jml_barang].deskripsi, ',');
            getline(fBarang, db_barang[jml_barang].kategori, ',');
            getline(fBarang, buf, ','); db_barang[jml_barang].harga_awal       = stoll(buf);
            getline(fBarang, buf, ','); db_barang[jml_barang].harga_tertinggi  = stoll(buf);
            getline(fBarang, db_barang[jml_barang].pemenang, ',');
            getline(fBarang, buf, ','); db_barang[jml_barang].waktu_tutup = (time_t)stoll(buf);
            getline(fBarang, buf);     db_barang[jml_barang].status       = stoi(buf);
            jml_barang++;
        }
        fBarang.close();
    }

    ifstream fTawaran("tawaran.csv");
    if (fTawaran.is_open()) {
        while (getline(fTawaran, buf, ',')) {
            db_tawaran[jml_tawaran].id_barang = stoi(buf);
            getline(fTawaran, db_tawaran[jml_tawaran].username, ',');
            getline(fTawaran, buf, ','); db_tawaran[jml_tawaran].nominal = stoll(buf);
            getline(fTawaran, buf);     db_tawaran[jml_tawaran].waktu   = (time_t)stoll(buf);
            jml_tawaran++;
        }
        fTawaran.close();
    }

    // Loop utama program
    int pilihan;
    do {
        // Cek expired saat di menu utama juga
        time_t now = time(0);
        for (int i = 0; i < jml_barang; i++)
            if (db_barang[i].status == 0 && now >= db_barang[i].waktu_tutup)
                db_barang[i].status = 1;

        cout << "\n=== SISTEM LELANG (SILELANG) ===\n";
        cout << "[1] Login\n[2] Daftar\n[3] Keluar\nPilih: ";
        cin >> pilihan;

        if (pilihan == 1) {
            login();
            if (!currentUser.empty()) {
                if (currentRole == "ADMIN") menuAdmin();
                else menuPeserta();
            }
        } else if (pilihan == 2) {
            daftarAkun();
        } else if (pilihan != 3) {
            cout << "Pilihan tidak valid!\n";
        }
    } while (pilihan != 3);

    cout << "Program selesai.\n";
    return 0;
}