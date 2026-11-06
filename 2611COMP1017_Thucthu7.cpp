#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cctype>
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace std;

// ==================== MAU ANSI ====================
const string RESET = "\033[0m";               const string DAM = "\033[1m";
const string HONG = "\033[38;5;175m";         const string DO = "\033[38;5;167m";
const string VANG = "\033[38;5;222m";         const string CAM = "\033[38;5;208m";
const string TIM = "\033[38;5;141m";          const string TIM_VIOLET = "\033[38;5;99m";
const string XANH_DUONG = "\033[38;5;75m";    const string XANH_LA = "\033[38;5;114m";
const string TRANG = "\033[38;5;252m";        const string XAM = "\033[38;5;245m";

// ==================== HANG SO ====================
const int NOI_DIA = 1;          // nguon hang noi dia (giam 50%)
const int NHAP_KHAU = 0;        // nguon hang nhap khau
const int RONG_BANG = 134;      // do rong bang danh sach

// Nem ra khi het du lieu dau vao (vd: nguoi dung nhan Ctrl+Z / Ctrl+D)
struct KetThucNhap {};

// ==================== HAM HO TRO: CHUOI & SO ====================
// Bat che do hien thi mau ANSI cho CMD/PowerShell tren Windows 10 tro len
void BatMauConsole() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD cheDo = 0;
    if (h != INVALID_HANDLE_VALUE && GetConsoleMode(h, &cheDo)) {
        SetConsoleMode(h, cheDo | 0x0004); // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    }
#endif
}

string CatKhoangTrang(const string& s) {
    size_t dau = s.find_first_not_of(" \t\r\n");
    if (dau == string::npos) return "";
    size_t cuoi = s.find_last_not_of(" \t\r\n");
    return s.substr(dau, cuoi - dau + 1);
}

string ChuThuong(string s) { for (char& c : s) c = (char)tolower((unsigned char)c); return s; }
string ChuHoa(string s)    { for (char& c : s) c = (char)toupper((unsigned char)c); return s; }

// Kiem tra chuoi s co chua tu khoa (khong phan biet hoa thuong)
bool Chua(const string& s, const string& tuKhoa) {
    return ChuThuong(s).find(ChuThuong(tuKhoa)) != string::npos;
}

// Cat bot chuoi qua dai de khong lam vo khung bang
string CatChuoi(const string& s, int toiDa) {
    if ((int)s.length() <= toiDa) return s;
    return s.substr(0, toiDa - 3) + "...";
}

// So thuc -> chuoi (dung fixed + setprecision), bo so 0 thua: 55.0 -> "55", 32.50 -> "32.5"
string SoThucThanhChuoi(double x, int soLe = 1) {
    ostringstream oss;
    oss << fixed << setprecision(soLe) << x;
    string s = oss.str();
    if (s.find('.') != string::npos) {
        while (s.back() == '0') s.pop_back();
        if (s.back() == '.') s.pop_back();
    }
    return s;
}

// Dinh dang tien VND: 27702500 -> "27,702,500"
string DinhDangTien(double tien) {
    ostringstream oss;
    oss << fixed << setprecision(0) << tien;
    string s = oss.str();
    int batDau = (!s.empty() && s[0] == '-') ? 1 : 0;
    for (int i = (int)s.length() - 3; i > batDau; i -= 3) s.insert(i, ",");
    return s;
}

// ==================== HAM HO TRO: GIAO DIEN ====================
void InThongBao(const string& s, const string& mau) { cout << mau << DAM << s << RESET << "\n\n"; }
void InCanhBao(const string& s) { cout << VANG << DAM << "[?] " << s << RESET; }
void InDuongNgang(int n) { cout << XAM << "+" << string(n - 2, '-') << "+" << RESET << "\n"; }
void InPhanCach() { cout << XAM << "\n" << string(RONG_BANG, '-') << RESET << "\n"; }

// In 1 o trong bang. doDaiThuc dung khi chuoi s co ma mau (ma mau khong chiem cho tren man hinh)
void InO(const string& s, int rong, const string& mau = RESET, bool canPhai = false,
         bool canGiua = false, int doDaiThuc = -1) {
    int len = (doDaiThuc != -1) ? doDaiThuc : (int)s.length();
    int trong = max(0, rong - 2 - len);
    int trai = canGiua ? trong / 2 : (canPhai ? trong : 0);
    int phai = trong - trai;
    cout << " " << mau << string(trai, ' ') << s << string(phai, ' ')
         << RESET << " " << XAM << "|" << RESET;
}

void InTieuDe(const string& title, int rong = 76) {
    string khung = "+" + string(rong - 2, '-') + "+";
    int khoangTrang = rong - 2 - (int)title.length();
    int trai = khoangTrang / 2;
    int phai = khoangTrang - trai;

    cout << "\n" << HONG << DAM
         << khung << "\n"
         << "|" << string(trai, ' ') << title << string(phai, ' ') << "|\n"
         << khung << RESET << "\n";
}

void InNhanNhap(const string& nhan, const string& lc = "") {
    cout << XANH_DUONG << DAM << " - " << left << setw(26) << nhan << RESET;
    if (!lc.empty()) {
        cout << " (" << lc << ")";
    }
    cout << ": ";
}

// In 1 dong thong tin chi tiet (dung cho toan tu <<)
void InDongChiTiet(ostream& os, const string& nhan, const string& giaTri, const string& mau) {
    os << XAM << "   > " << RESET << TRANG << left << setw(18) << nhan << RESET
       << ": " << mau << DAM << giaTri << RESET << "\n";
}

// ==================== HAM HO TRO: NHAP LIEU AN TOAN ====================
// Doc ca dong de tranh loi troi bo dem khi tron lan >> va getline
string DocDong(istream& is) {
    string s;
    if (!getline(is, s)) throw KetThucNhap();
    return CatKhoangTrang(s);
}

// Chuyen chuoi -> so nguyen; that bai neu co ky tu thua (vd: "12abc")
bool ChuyenSoNguyen(const string& s, int& x) {
    stringstream ss(s);
    char du;
    return (ss >> x) && !(ss >> du);
}

bool ChuyenSoThuc(string s, double& x) {
    for (char& c : s) if (c == ',') c = '.';   // chap nhan ca "55,5"
    stringstream ss(s);
    char du;
    return (ss >> x) && !(ss >> du);
}

string NhapChuoi(istream& is, const string& nhan, const string& loi, const string& lc = "") {
    while (true) {
        InNhanNhap(nhan, lc);
        string s = DocDong(is);
        if (!s.empty()) return s;
        InThongBao("[!] Loi: " + loi, DO);
    }
}

int NhapSoNguyen(istream& is, const string& nhan, int nhoNhat, int lonNhat,
                 const string& loi, const string& lc = "") {
    while (true) {
        InNhanNhap(nhan, lc);
        int x;
        if (ChuyenSoNguyen(DocDong(is), x) && x >= nhoNhat && x <= lonNhat) return x;
        InThongBao("[!] Loi: " + loi, DO);
    }
}

double NhapSoThuc(istream& is, const string& nhan, double nhoNhat, double lonNhat,
                  const string& loi, const string& lc = "") {
    while (true) {
        InNhanNhap(nhan, lc);
        double x;
        if (ChuyenSoThuc(DocDong(is), x) && x >= nhoNhat && x <= lonNhat) return x;
        InThongBao("[!] Loi: " + loi, DO);
    }
}

// ==================== LOP CO SO (LOP TRUU TUONG) ====================
class ThietBi {
private:
    string maTB;
    string tenTB;
    int nguonHang;      // 1: Noi dia, 0: Nhap khau

protected:
    // Cac ham dung chung cho lop con khi in 1 dong cua bang
    void InCacCotChung() const {
        InO(maTB, 10, MauLoai(), false, true);
        InO(CatChuoi(tenTB, 24), 26, TRANG);
        InO(LayTenNguon(), 14, (LaHangNoiDia() ? XANH_LA : CAM), false, true);
        InO(LayLoai(), 13, MauLoai(), false, true);
    }
    void InCotThongSo(const string& coMau, const string& tho) const {
        InO(coMau, 38, RESET, false, false, (int)tho.length());
    }
    void InCotGia() const {
        InO(DinhDangTien(TinhTien()), 19, VANG, true);
    }

public:
    // ----- Ham khoi tao / huy -----
    ThietBi() : maTB(""), tenTB(""), nguonHang(NOI_DIA) {}
    ThietBi(string ma, string ten, int nguon) : maTB(ma), tenTB(ten), nguonHang(nguon) {}
    ThietBi(const ThietBi& tb) : maTB(tb.maTB), tenTB(tb.tenTB), nguonHang(tb.nguonHang) {}
    virtual ~ThietBi() {}

    // ----- Getter / Setter -----
    string getMaTB() const { return maTB; }
    void setMaTB(string ma) { maTB = ma; }

    string getTenTB() const { return tenTB; }
    void setTenTB(string ten) { tenTB = ten; }

    int getNguonHang() const { return nguonHang; }
    void setNguonHang(int nh) { if (nh == NOI_DIA || nh == NHAP_KHAU) nguonHang = nh; }

    bool LaHangNoiDia() const { return nguonHang == NOI_DIA; }
    string LayTenNguon() const { return LaHangNoiDia() ? "Noi dia" : "Nhap khau"; }

    // ----- Phuong thuc thuan ao: moi lop con bat buoc ghi de (DA HINH) -----
    virtual double TinhTien() const = 0;
    virtual void HienThiThongSoKyThuat() const = 0;
    virtual string LayLoai() const = 0;
    virtual string MauLoai() const = 0;
    virtual string MauThongSo() const = 0;

    // ----- Nhap / xuat ao: lop con goi lai phan cua lop cha roi bo sung phan rieng -----
    virtual void Nhap(istream& is) {
        tenTB = NhapChuoi(is, "Ten thiet bi", "Ten thiet bi khong duoc de trong.");
        string lc = XANH_LA + DAM + "1: Noi dia" + RESET + ", " + CAM + DAM + "0: Nhap khau" + RESET;
        nguonHang = NhapSoNguyen(is, "Nguon hang", NHAP_KHAU, NOI_DIA,
                                 "Chi nhap 1 hoac 0. Vui long thu lai.", lc);
    }

    virtual void Xuat(ostream& os) const {
        InDongChiTiet(os, "Ma thiet bi", maTB, MauLoai());
        InDongChiTiet(os, "Ten thiet bi", tenTB, TRANG);
        InDongChiTiet(os, "Loai thiet bi", LayLoai(), MauLoai());
        InDongChiTiet(os, "Nguon hang", LayTenNguon(), (LaHangNoiDia() ? XANH_LA : CAM));
    }

    // ----- Nap chong toan tu: goi ham ao nen dung duoc voi con tro ThietBi* -----
    friend istream& operator>>(istream& is, ThietBi& tb) {
        tb.Nhap(is);
        return is;
    }

    friend ostream& operator<<(ostream& os, const ThietBi& tb) {
        tb.Xuat(os);
        string gia = DinhDangTien(tb.TinhTien()) + " VND";
        if (tb.LaHangNoiDia()) gia += " (da giam 50% hang noi dia)";
        InDongChiTiet(os, "Gia ban", gia, VANG);
        return os;
    }
};

// ==================== DIEN THOAI ====================
class DienThoai : public ThietBi {
private:
    string heDieuHanh;   // "iOS" hoac "Android"
    int dungLuongPin;    // don vi mAh

public:
    DienThoai() : ThietBi(), heDieuHanh("Android"), dungLuongPin(4500) {}

    DienThoai(string ma, string ten, int nguon, string hdh, int pin)
        : ThietBi(ma, ten, nguon), heDieuHanh(hdh), dungLuongPin(pin) {}

    DienThoai(const DienThoai& dt) : ThietBi(dt), heDieuHanh(dt.heDieuHanh), dungLuongPin(dt.dungLuongPin) {}

    ~DienThoai() override {}

    string getHeDieuHanh() const { return heDieuHanh; }
    void setHeDieuHanh(string hdh) { heDieuHanh = hdh; }

    int getDungLuongPin() const { return dungLuongPin; }
    void setDungLuongPin(int pin) { if (pin > 0) dungLuongPin = pin; }

    // Gia = 5,000,000 + phu phi HDH (iOS: 11,600,000 | Android: 5,000,000) + 2,500 x Pin
    double TinhTien() const override {
        double giaCoBan = 5000000;
        double phuPhiHDH = (heDieuHanh == "iOS") ? 11600000 : 5000000;
        double gia = giaCoBan + phuPhiHDH + 2500.0 * dungLuongPin;
        if (LaHangNoiDia()) gia *= 0.5;
        return gia;
    }

    string LayLoai() const override { return "Dien thoai"; }
    string MauLoai() const override { return XANH_DUONG; }
    string MauThongSo() const override { return (heDieuHanh == "iOS" ? TIM : XANH_LA); }

    void HienThiThongSoKyThuat() const override {
        InCacCotChung();
        string pin = to_string(dungLuongPin) + " mAh";
        string tho = "HDH: " + heDieuHanh + " | Pin: " + pin;
        string coMau = TRANG + "HDH: " + MauThongSo() + DAM + heDieuHanh + RESET
                     + TRANG + " | Pin: " + pin + RESET;
        InCotThongSo(coMau, tho);
        InCotGia();
    }

    void Nhap(istream& is) override {
        ThietBi::Nhap(is);

        while (true) {
            string lc = TIM + DAM + "iOS" + RESET + "/" + XANH_LA + DAM + "Android" + RESET;
            InNhanNhap("He dieu hanh", lc);
            string hdh = ChuThuong(DocDong(is));
            if (hdh == "ios")     { heDieuHanh = "iOS"; break; }
            if (hdh == "android") { heDieuHanh = "Android"; break; }
            InThongBao("[!] Loi: Hay nhap dung iOS hoac Android.", DO);
        }

        dungLuongPin = NhapSoNguyen(is, "Dung luong pin (mAh)", 1, 20000,
                                    "Pin phai la so nguyen duong (1 - 20000 mAh).");
    }

    void Xuat(ostream& os) const override {
        ThietBi::Xuat(os);
        InDongChiTiet(os, "He dieu hanh", heDieuHanh, MauThongSo());
        InDongChiTiet(os, "Dung luong pin", to_string(dungLuongPin) + " mAh", TRANG);
    }
};

// ==================== MAY TINH ====================
class MayTinh : public ThietBi {
private:
    string CPU;
    int RAM;             // don vi GB

public:
    MayTinh() : ThietBi(), CPU("Intel i5"), RAM(16) {}

    MayTinh(string ma, string ten, int nguon, string cpu, int ram)
        : ThietBi(ma, ten, nguon), CPU(cpu), RAM(ram) {}

    MayTinh(const MayTinh& mt) : ThietBi(mt), CPU(mt.CPU), RAM(mt.RAM) {}

    ~MayTinh() override {}

    string getCPU() const { return CPU; }
    void setCPU(string cpu) { CPU = cpu; }

    int getRAM() const { return RAM; }
    void setRAM(int ram) { if (ram > 0) RAM = ram; }

    // Xac dinh hang CPU: "Intel" / "AMD" / "Apple" / "" (khong hop le)
    string LayHangCPU() const {
        if (Chua(CPU, "intel")) return "Intel";
        if (Chua(CPU, "amd") || Chua(CPU, "ryzen")) return "AMD";
        if (Chua(CPU, "apple")) return "Apple";
        return "";
    }

    // Phu phi theo dong CPU (chi tra cuu trong dung hang de tranh nham)
    double PhuPhiCPU() const {
        string hang = LayHangCPU();
        if (hang == "Intel") {
            if (Chua(CPU, "i9")) return 14000000;
            if (Chua(CPU, "i7")) return 9600000;
            if (Chua(CPU, "i5")) return 5000000;
            if (Chua(CPU, "i3")) return 3200000;
        } else if (hang == "AMD") {
            if (Chua(CPU, "ryzen 9")) return 14000000;
            if (Chua(CPU, "ryzen 7")) return 9600000;
            if (Chua(CPU, "ryzen 5")) return 5000000;
            if (Chua(CPU, "ryzen 3")) return 3200000;
        } else if (hang == "Apple") {
            if (Chua(CPU, "m5")) return 18000000;
            if (Chua(CPU, "m4")) return 15000000;
            if (Chua(CPU, "m3")) return 12000000;
            if (Chua(CPU, "m2")) return 10000000;
            if (Chua(CPU, "m1")) return 8000000;
        }
        return 2000000; // dong CPU pho thong / khong xac dinh
    }

    // Gia = 8,000,000 + phu phi CPU + 500,000 x RAM
    double TinhTien() const override {
        double giaCoBan = 8000000;
        double gia = giaCoBan + PhuPhiCPU() + 500000.0 * RAM;
        if (LaHangNoiDia()) gia *= 0.5;
        return gia;
    }

    string LayLoai() const override { return "May tinh"; }
    string MauLoai() const override { return CAM; }
    string MauThongSo() const override {
        string hang = LayHangCPU();
        if (hang == "Intel") return XANH_DUONG;
        if (hang == "AMD")   return DO;
        if (hang == "Apple") return XAM;
        return RESET;
    }

    void HienThiThongSoKyThuat() const override {
        InCacCotChung();
        string ram = " | RAM: " + to_string(RAM) + " GB";
        int choTrongCPU = 36 - 5 - (int)ram.length();          // 36 = do rong noi dung cot
        string cpu = CatChuoi(CPU, choTrongCPU);
        string tho = "CPU: " + cpu + ram;
        string coMau = TRANG + "CPU: " + MauThongSo() + DAM + cpu + RESET + TRANG + ram + RESET;
        InCotThongSo(coMau, tho);
        InCotGia();
    }

    void Nhap(istream& is) override {
        ThietBi::Nhap(is);

        while (true) {
            string lc = XANH_DUONG + DAM + "Intel" + RESET + "/" + DO + DAM + "AMD" + RESET + "/" + XAM + DAM + "Apple" + RESET;
            CPU = NhapChuoi(is, "CPU", "CPU khong duoc de trong.", lc);
            if (CPU.length() > 30) {
                InThongBao("[!] Loi: Ten CPU toi da 30 ky tu.", DO);
            } else if (LayHangCPU().empty()) {
                InThongBao("[!] Loi: CPU can thuoc Intel, AMD hoac Apple (vd: Intel i5, AMD Ryzen 7, Apple M3).", DO);
            } else {
                break;
            }
        }

        RAM = NhapSoNguyen(is, "RAM (GB)", 1, 256, "RAM phai la so nguyen duong (1 - 256 GB).");
    }

    void Xuat(ostream& os) const override {
        ThietBi::Xuat(os);
        InDongChiTiet(os, "CPU", CPU, MauThongSo());
        InDongChiTiet(os, "RAM", to_string(RAM) + " GB", TRANG);
    }
};

// ==================== TV ====================
class TV : public ThietBi {
private:
    double kichThuocManHinh;   // don vi inch

public:
    TV() : ThietBi(), kichThuocManHinh(43.0) {}

    TV(string ma, string ten, int nguon, double kichThuoc)
        : ThietBi(ma, ten, nguon), kichThuocManHinh(kichThuoc) {}

    TV(const TV& tv) : ThietBi(tv), kichThuocManHinh(tv.kichThuocManHinh) {}

    ~TV() override {}

    double getKichThuocManHinh() const { return kichThuocManHinh; }
    void setKichThuocManHinh(double kt) { if (kt > 0) kichThuocManHinh = kt; }

    // Gia = 3,200,000 + phu phi theo nhom kich thuoc + 200,000 x inch
    double TinhTien() const override {
        double giaCoBan = 3200000;
        double phuPhi = 0;
        if (kichThuocManHinh < 43) phuPhi = 1000000;
        else if (kichThuocManHinh < 55) phuPhi = 3200000;
        else if (kichThuocManHinh < 65) phuPhi = 6800000;
        else phuPhi = 12000000;

        double gia = giaCoBan + phuPhi + 200000.0 * kichThuocManHinh;
        if (LaHangNoiDia()) gia *= 0.5;
        return gia;
    }

    string LayLoai() const override { return "TV"; }
    string MauLoai() const override { return TIM; }
    string MauThongSo() const override { return HONG; }

    void HienThiThongSoKyThuat() const override {
        InCacCotChung();
        string kichThuoc = SoThucThanhChuoi(kichThuocManHinh) + " inch";
        string tho = "Kich thuoc: " + kichThuoc;
        string coMau = TRANG + "Kich thuoc: " + MauThongSo() + DAM + kichThuoc + RESET;
        InCotThongSo(coMau, tho);
        InCotGia();
    }

    void Nhap(istream& is) override {
        ThietBi::Nhap(is);
        kichThuocManHinh = NhapSoThuc(is, "Kich thuoc man hinh (inch)", 15, 120,
                                      "Kich thuoc man hinh phai tu 15 den 120 inch.");
    }

    void Xuat(ostream& os) const override {
        ThietBi::Xuat(os);
        InDongChiTiet(os, "Kich thuoc", SoThucThanhChuoi(kichThuocManHinh) + " inch", MauThongSo());
    }
};

// ==================== DANH SACH THIET BI ====================
class DanhSachThietBi {
private:
    vector<ThietBi*> ds;

    ThietBi* TimTheoMa(const string& ma) const {
        for (ThietBi* tb : ds) {
            if (tb->getMaTB() == ma) return tb;
        }
        return nullptr;
    }

    // Tao doi tuong dung loai theo lua chon (1: Dien thoai, 2: May tinh, 3: TV)
    static ThietBi* TaoThietBi(int loai) {
        switch (loai) {
        case 1:  return new DienThoai();
        case 2:  return new MayTinh();
        default: return new TV();
        }
    }

    // In bang thiet bi - goi HienThiThongSoKyThuat() qua con tro lop cha (lien ket dong)
    void InBang(const vector<ThietBi*>& dsIn, const string& tieuDe) const {
        InTieuDe(tieuDe, RONG_BANG);
        cout << "\n";
        InDuongNgang(RONG_BANG);

        cout << XAM << "|" << RESET;
        InO("STT", 6, TIM, false, true);
        InO("MA TB", 10, TIM, false, true);
        InO("TEN THIET BI", 26, TIM, false, true);
        InO("NGUON HANG", 14, TIM, false, true);
        InO("LOAI", 13, TIM, false, true);
        InO("THONG SO KY THUAT", 38, TIM, false, true);
        InO("GIA BAN (VND)", 19, TIM, false, true);
        cout << "\n";
        InDuongNgang(RONG_BANG);

        for (size_t i = 0; i < dsIn.size(); ++i) {
            cout << XAM << "|" << RESET;
            InO(to_string(i + 1), 6, TRANG, false, true);
            dsIn[i]->HienThiThongSoKyThuat();
            cout << "\n";
            InDuongNgang(RONG_BANG);
        }
    }

public:
    DanhSachThietBi() {}

    // Danh sach so huu cac con tro -> cam sao chep de tranh giai phong 2 lan
    DanhSachThietBi(const DanhSachThietBi&) = delete;
    DanhSachThietBi& operator=(const DanhSachThietBi&) = delete;

    ~DanhSachThietBi() {
        for (ThietBi* tb : ds) delete tb;
        ds.clear();
    }

    int SoLuong() const { return (int)ds.size(); }

    bool KiemTraTrungMa(const string& ma) const { return TimTheoMa(ma) != nullptr; }

    void ThemThietBi(ThietBi* tb) { ds.push_back(tb); }

    void NhapDanhSach() {
        InTieuDe("THEM THIET BI MOI");
        int n = NhapSoNguyen(cin, "So luong thiet bi can them", 1, 100,
                             "So luong phai la so nguyen duong (1 - 100).");

        for (int i = 0; i < n; ++i) {
            InTieuDe("THIET BI SO " + to_string(i + 1));

            string lc = XANH_DUONG + DAM + "1: Dien thoai" + RESET + ", " + CAM + DAM + "2: May tinh" + RESET + ", " + TIM + DAM + "3: TV" + RESET;
            int loai = NhapSoNguyen(cin, "Loai thiet bi", 1, 3, "Chi chon 1, 2 hoac 3.", lc);

            string ma;
            while (true) {
                InNhanNhap("Ma thiet bi", XAM + "toi da 8 ky tu" + RESET);
                ma = ChuHoa(DocDong(cin));
                if (ma.empty())                        InThongBao("[!] Loi: Ma thiet bi khong duoc de trong.", DO);
                else if (ma.find(' ') != string::npos) InThongBao("[!] Loi: Ma thiet bi khong duoc chua khoang trang.", DO);
                else if (ma.length() > 8)              InThongBao("[!] Loi: Ma thiet bi toi da 8 ky tu.", DO);
                else if (KiemTraTrungMa(ma))           InThongBao("[!] Loi: Ma thiet bi nay da ton tai. Vui long nhap lai.", DO);
                else break;
            }

            ThietBi* tb = TaoThietBi(loai);   // con tro lop cha tro den doi tuong lop con
            tb->setMaTB(ma);
            cin >> *tb;                        // goi Nhap() dung lop con (da hinh)

            ThemThietBi(tb);
            cout << "\n";
            InThongBao("[+] THEM THIET BI THANH CONG! Gia ban: " + DinhDangTien(tb->TinhTien()) + " VND", XANH_LA);
        }
    }

    void TimKiemThietBi() const {
        if (ds.empty()) {
            InThongBao("[!] DANH SACH THIET BI HIEN DANG TRONG!", DO);
            return;
        }

        InTieuDe("TIM KIEM THIET BI");
        string maCanTim = ChuHoa(NhapChuoi(cin, "Nhap ma thiet bi can tim", "Ma thiet bi khong duoc de trong."));

        ThietBi* tb = TimTheoMa(maCanTim);
        if (tb == nullptr) {
            InThongBao("[!] Khong tim thay thiet bi co ma: " + maCanTim, DO);
            return;
        }

        InBang(vector<ThietBi*>(1, tb), "KET QUA TIM KIEM");
        cout << "\n" << HONG << DAM << " THONG TIN CHI TIET:" << RESET << "\n";
        cout << *tb;                           // toan tu << goi Xuat() cua dung lop con
    }

    void XoaThietBi() {
        if (ds.empty()) {
            InThongBao("[!] DANH SACH THIET BI HIEN DANG TRONG!", DO);
            return;
        }

        InTieuDe("XOA THIET BI");
        string maCanXoa = ChuHoa(NhapChuoi(cin, "Nhap ma thiet bi can xoa", "Ma thiet bi khong duoc de trong."));

        for (auto it = ds.begin(); it != ds.end(); ++it) {
            if ((*it)->getMaTB() != maCanXoa) continue;

            cout << "\n" << HONG << DAM << " THIET BI SE BI XOA:" << RESET << "\n"
                 << **it << "\n";
            InCanhBao("Ban co chac chan muon xoa thiet bi nay? (Y/N): ");
            string xacNhan = ChuHoa(DocDong(cin));
            cout << "\n";

            if (xacNhan == "Y") {
                delete *it;                    // goi destructor ao -> giai phong dung lop con
                ds.erase(it);
                InThongBao("[+] XOA THIET BI THANH CONG!", XANH_LA);
            } else {
                InThongBao("[-] Da huy thao tac xoa.", VANG);
            }
            return;
        }

        InThongBao("[!] Khong tim thay thiet bi co ma: " + maCanXoa, DO);
    }

    void XuatDanhSach() const {
        if (ds.empty()) {
            InThongBao("[!] DANH SACH THIET BI HIEN DANG TRONG!", DO);
            return;
        }

        InBang(ds, "DANH SACH THIET BI");

        double tongTien = 0;
        for (ThietBi* tb : ds) tongTien += tb->TinhTien();

        string nhan = "Tong gia tri hang hoa (" + to_string(ds.size()) + " thiet bi): ";
        string giaTri = DinhDangTien(tongTien) + " VND";
        cout << "\n" << setw(RONG_BANG - (int)(nhan.length() + giaTri.length()) - 1) << ""
             << XANH_DUONG << DAM << nhan << VANG << DAM << giaTri << RESET << "\n";
    }

    void ThongKe() const {
        if (ds.empty()) {
            InThongBao("[!] DANH SACH THIET BI HIEN DANG TRONG!", DO);
            return;
        }

        // Gom nhom theo LayLoai() -> them loai thiet bi moi khong can sua ham nay
        struct NhomLoai {
            string ten, mau;
            int soLuong, noiDia, nhapKhau;
            double tongTien;
            NhomLoai(string t, string m) : ten(t), mau(m), soLuong(0), noiDia(0), nhapKhau(0), tongTien(0) {}
        };
        vector<NhomLoai> nhom;
        double tongTatCa = 0;
        ThietBi* datNhat = ds[0];
        ThietBi* reNhat = ds[0];

        for (ThietBi* tb : ds) {
            size_t k = 0;
            while (k < nhom.size() && nhom[k].ten != tb->LayLoai()) ++k;
            if (k == nhom.size()) nhom.push_back(NhomLoai(tb->LayLoai(), tb->MauLoai()));

            double gia = tb->TinhTien();
            nhom[k].soLuong++;
            if (tb->LaHangNoiDia()) nhom[k].noiDia++; else nhom[k].nhapKhau++;
            nhom[k].tongTien += gia;
            tongTatCa += gia;
            if (gia > datNhat->TinhTien()) datNhat = tb;
            if (gia < reNhat->TinhTien()) reNhat = tb;
        }

        InTieuDe("THONG KE HANG HOA", RONG_BANG);
        cout << "\n";
        InDuongNgang(RONG_BANG);
        cout << XAM << "|" << RESET;
        InO("LOAI THIET BI", 24, TIM, false, true);
        InO("SO LUONG", 16, TIM, false, true);
        InO("NOI DIA", 16, TIM, false, true);
        InO("NHAP KHAU", 16, TIM, false, true);
        InO("TONG GIA TRI (VND)", 34, TIM, false, true);
        InO("TI TRONG (%)", 21, TIM, false, true);
        cout << "\n";
        InDuongNgang(RONG_BANG);

        int tongNoiDia = 0, tongNhapKhau = 0;
        for (const NhomLoai& g : nhom) {
            ostringstream tiTrong;
            tiTrong << fixed << setprecision(1) << (g.tongTien * 100.0 / tongTatCa);
            cout << XAM << "|" << RESET;
            InO(g.ten, 24, g.mau, false, true);
            InO(to_string(g.soLuong), 16, TRANG, false, true);
            InO(to_string(g.noiDia), 16, XANH_LA, false, true);
            InO(to_string(g.nhapKhau), 16, CAM, false, true);
            InO(DinhDangTien(g.tongTien), 34, VANG, true);
            InO(tiTrong.str(), 21, TRANG, true);
            cout << "\n";
            tongNoiDia += g.noiDia;
            tongNhapKhau += g.nhapKhau;
        }
        InDuongNgang(RONG_BANG);
        cout << XAM << "|" << RESET;
        InO("TONG CONG", 24, HONG + DAM, false, true);
        InO(to_string(ds.size()), 16, HONG + DAM, false, true);
        InO(to_string(tongNoiDia), 16, HONG + DAM, false, true);
        InO(to_string(tongNhapKhau), 16, HONG + DAM, false, true);
        InO(DinhDangTien(tongTatCa), 34, VANG + DAM, true);
        InO("100.0", 21, HONG + DAM, true);
        cout << "\n";
        InDuongNgang(RONG_BANG);

        cout << "\n";
        cout << XANH_DUONG << DAM << " - " << left << setw(26) << "Gia ban trung binh" << RESET << ": "
             << VANG << DAM << DinhDangTien(tongTatCa / ds.size()) << " VND" << RESET << "\n";
        cout << XANH_DUONG << DAM << " - " << left << setw(26) << "Thiet bi dat nhat" << RESET << ": "
             << datNhat->MauLoai() << DAM << datNhat->getMaTB() << RESET << TRANG << " - " << datNhat->getTenTB()
             << " - " << RESET << VANG << DAM << DinhDangTien(datNhat->TinhTien()) << " VND" << RESET << "\n";
        cout << XANH_DUONG << DAM << " - " << left << setw(26) << "Thiet bi re nhat" << RESET << ": "
             << reNhat->MauLoai() << DAM << reNhat->getMaTB() << RESET << TRANG << " - " << reNhat->getTenTB()
             << " - " << RESET << VANG << DAM << DinhDangTien(reNhat->TinhTien()) << " VND" << RESET << "\n";
    }

    // Nap san du lieu mau (dung constructor co tham so) de demo nhanh
    void NapDuLieuMau() {
        vector<ThietBi*> mau = {
            new DienThoai("DT01", "iPhone 15 Pro Max",      NHAP_KHAU, "iOS", 4441),
            new DienThoai("DT02", "Samsung Galaxy A55",     NOI_DIA,   "Android", 5000),
            new MayTinh  ("MT01", "MacBook Air 13",         NHAP_KHAU, "Apple M3", 16),
            new MayTinh  ("MT02", "Dell Inspiron 15",       NOI_DIA,   "Intel i5-1335U", 8),
            new MayTinh  ("MT03", "ASUS ROG Strix G16",     NHAP_KHAU, "Intel i9-14900HX", 32),
            new TV       ("TV01", "Samsung Crystal UHD 55", NOI_DIA,   55),
            new TV       ("TV02", "Sony Bravia 7 65",       NHAP_KHAU, 65)
        };

        int daThem = 0;
        for (ThietBi* tb : mau) {
            if (KiemTraTrungMa(tb->getMaTB())) { delete tb; continue; }
            ThemThietBi(tb);
            daThem++;
        }

        if (daThem > 0) InThongBao("[+] DA NAP " + to_string(daThem) + " THIET BI MAU!", XANH_LA);
        else InThongBao("[!] Du lieu mau da duoc nap truoc do (trung ma thiet bi).", VANG);
    }
};

// ==================== MENU CHINH ====================
void InDongMenu(const string& noiDung, const string& mau) {
    int rong = 48;
    string nd = (int)noiDung.length() > rong ? noiDung.substr(0, rong) : noiDung;
    cout << string(49, ' ') << mau << "|" << left << setw(rong) << nd << "|" << RESET << "\n";
}

void HienThiMenu() {
    int le = 49;
    cout << "\n" << string(le, ' ') << TIM_VIOLET << "+------------------------------------------------+\n" << RESET;
    cout << string(le, ' ') << TIM_VIOLET << "|" << setw(32) << right << "QUAN LY HANG HOA" << setw(17) << "|" << "\n" << RESET;
    cout << string(le, ' ') << TIM_VIOLET << "+------------------------------------------------+\n" << RESET;
    InDongMenu("  [1]  Xem danh sach thiet bi", XANH_DUONG);
    InDongMenu("  [2]  Them thiet bi moi", XANH_LA);
    InDongMenu("  [3]  Tim kiem thiet bi theo ma", VANG);
    InDongMenu("  [4]  Xoa thiet bi theo ma", DO);
    InDongMenu("  [5]  Thong ke hang hoa", HONG);
    InDongMenu("  [6]  Nap du lieu mau (demo)", TIM);
    InDongMenu("  [0]  Thoat chuong trinh", CAM);
    cout << string(le, ' ') << TIM_VIOLET << "+------------------------------------------------+\n" << RESET;
}

int main() {
    BatMauConsole();
    DanhSachThietBi ql;
    int chon = -1;

    try {
        do {
            InPhanCach();
            HienThiMenu();
            InPhanCach();

            while (true) {
                cout << HONG << "                         LUA CHON CUA BAN: "
                     << RESET;
                if (ChuyenSoNguyen(DocDong(cin), chon) && chon >= 0 && chon <= 6)
                    break;
                InThongBao("                     [!] LUA CHON KHONG HOP LE. "
                           "VUI LONG NHAP TU 0 DEN 6.", DO);
            }

            InPhanCach();
            switch (chon) {
            case 1: ql.XuatDanhSach(); break;
            case 2: ql.NhapDanhSach(); break;
            case 3: ql.TimKiemThietBi(); break;
            case 4: ql.XoaThietBi(); break;
            case 5: ql.ThongKe(); break;
            case 6: ql.NapDuLieuMau(); break;
            case 0:
                InThongBao("                     [*] THOAT CHUONG TRINH THANH CONG!",
                           XANH_LA);
                break;
            }

            if (chon != 0) InPhanCach();

        } while (chon != 0);
    } catch (const KetThucNhap&) {
        cout << "\n";
        InThongBao("                     [*] KET THUC DU LIEU NHAP - "
                   "THOAT CHUONG TRINH.", VANG);
    }

    cout << RESET;
    return 0;
}

