#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

//Wybrane kolory 
const Fl_Color KOLOR_TLO = fl_rgb_color(241, 247, 237);  
const Fl_Color KOLOR_PRZYCISKOW = fl_rgb_color(121, 166, 119);  
const Fl_Color KOLOR_TEKSTU = fl_rgb_color(36, 62, 54);  

void pokazOknoPowiadomien(char* wiadomosc);

//Klasa glowna
class Uzytkownik {
public:
    string login, pin, numerKonta;
    int saldo;

    //Konstruktor klasy Uzytkownik
    Uzytkownik(string log, string pin, string numKo, int sal)
        : login(log), pin(pin), numerKonta(numKo), saldo(sal) {
    }

    virtual ~Uzytkownik() {}
    
    virtual double maksymalnaKwotaPrzelewu() const = 0; // Funkcja wirtualna do uzyskania limitu przelewu
};


//Klasa dziedziczona po Uzytkownik
class UzytkownikIndywidualny : public Uzytkownik {
public:

    //Konstruktor klasy UzytkownikIndywidualny na bazie konstruktora klasy Uzytkownik
    UzytkownikIndywidualny(string log, string pin, string numKo, int sal)
        : Uzytkownik(log, pin, numKo, sal) {
    }

    double maksymalnaKwotaPrzelewu() const override {
        return 1000.0; 
    }

    virtual ~UzytkownikIndywidualny() {
        pokazOknoPowiadomien("Uzytkownik indywidualny wylogowany poprawnie.");
    }
};


//Klasa dziedziczona po Uzytkownik
class UzytkownikFirmowy : public Uzytkownik {
public:

    //Konstruktor klasy UzytkownikFirmowy na bazie konstruktora klasy Uzytkownik
    UzytkownikFirmowy(string log, string pin, string numKo, int sal)
        : Uzytkownik(log, pin, numKo, sal) {
    }

    double maksymalnaKwotaPrzelewu() const override {
        return 10000.0; 
    }

    virtual ~UzytkownikFirmowy() {
        pokazOknoPowiadomien("Uzytkownik firmowy wylogowany poprawnie.");
    }
};


// Deklaracja obiektów
vector<Uzytkownik*> uzytkownicy = {
    new UzytkownikIndywidualny("indy1", "1234", "1234567890", 5000),
    new UzytkownikIndywidualny("indy2", "2345", "2345678901", 3000),
    new UzytkownikFirmowy("firma1", "1234", "0987654321", 100000),
    new UzytkownikFirmowy("firma2", "2345", "9876543210", 50000)
};

Uzytkownik* zalogowanyUzytkownik = nullptr;

// Deklaracja okien
Fl_Window* ekranLogowania;
Fl_Window* ekranGlowny;
Fl_Window* oknoPrzelewu;
Fl_Window* oknoUstawien;
Fl_Window* oknoBlik;
Fl_Window* oknoWiadomosci;

// WskaŸniki na pola w oknie przelewu
Fl_Input* poleNumerKontaPrzelew;
Fl_Input* poleKwotaPrzelew;

// Zmienne dla rozmiaru i lokalizacji okien
const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 350;
const int SCREEN_CENTER_X = (Fl::w() - WINDOW_WIDTH) / 2;
const int SCREEN_CENTER_Y = (Fl::h() - WINDOW_HEIGHT) / 2;

//Funkcja ustawiaj¹ca wygl¹d przycisków
void ustawStylPrzycisku(Fl_Button* przycisk) {
    przycisk->color(KOLOR_PRZYCISKOW);
    przycisk->labelcolor(KOLOR_TEKSTU);
    przycisk->labelfont(FL_HELVETICA_BOLD);
    przycisk->labelsize(14);
}

// Funkcja ustawiaj¹ca wygl¹d Boxa
void ustawStylBoxa(Fl_Box* box, int rozmiar) {
    box->labelcolor(KOLOR_TEKSTU);
    box->labelfont(FL_HELVETICA_BOLD);
    box->labelsize(rozmiar);
}

// Funkcja zamykania okna
void zamknijOkno(Fl_Widget* widget, void* okno) {
    Fl_Window* win = (Fl_Window*)okno;
    win->hide();
}

// Funkcje sluzace do otwierania okien
void pokazEkranLogowania();
void pokazEkranGlowny();
void pokazOknoPrzelewu();
void pokazOknoUstawien();
void pokazOknoBlik();

// Funkcja umozliwiajaca logowanie
void cbZaloguj(Fl_Widget* widget, void*) {
    Fl_Input* poleLogin = (Fl_Input*)widget->parent()->child(0);
    Fl_Input* polePin = (Fl_Input*)widget->parent()->child(1);
    string login = poleLogin->value();
    string pin = polePin->value();

    for (auto* uzytkownik : uzytkownicy) {
        if (uzytkownik->login == login && uzytkownik->pin == pin) {
            // Tworzymy nowy obiekt zalogowanego u¿ytkownika jako dynamiczn¹ kopiê
            if (dynamic_cast<UzytkownikIndywidualny*>(uzytkownik)) {
                zalogowanyUzytkownik = new UzytkownikIndywidualny(*dynamic_cast<UzytkownikIndywidualny*>(uzytkownik));
            }
            else if (dynamic_cast<UzytkownikFirmowy*>(uzytkownik)) {
                zalogowanyUzytkownik = new UzytkownikFirmowy(*dynamic_cast<UzytkownikFirmowy*>(uzytkownik));
            }

            ekranLogowania->hide();
            pokazEkranGlowny();
            return;
        }
    }
    pokazOknoPowiadomien("Bledny login lub PIN.");
}

// Funkcja umozliwiajaca wylogowanie
void cbWyloguj(Fl_Widget* widget, void*) {
    ekranGlowny->hide();
    oknoUstawien->hide();
    pokazEkranLogowania();
    if (zalogowanyUzytkownik) {
        delete zalogowanyUzytkownik;
        zalogowanyUzytkownik = nullptr;
    }
}

// Funkcja realizacji przelewu
void cbPrzelew(Fl_Widget* widget, void*) {
    string numerKonta = poleNumerKontaPrzelew->value();
    double kwota = atof(poleKwotaPrzelew->value());

    if (numerKonta.empty() || kwota <= 0) {
        pokazOknoPowiadomien("Podano nieprawidlowe dane.");
        return;
    }

    if (numerKonta.length() < 10) {
        pokazOknoPowiadomien("Blad: nieprawidlowy numer konta.");
        return;
    }

    if (zalogowanyUzytkownik->saldo < kwota) {
        pokazOknoPowiadomien("Brak wystarczajacych srodkow.");
        return;
    }

    if (kwota > zalogowanyUzytkownik->maksymalnaKwotaPrzelewu()) {
        pokazOknoPowiadomien("Kwota przekracza limit dla Twojego konta.");
        return;
    }

    zalogowanyUzytkownik->saldo -= kwota;
    oknoPrzelewu->hide();
    pokazEkranGlowny();
    pokazOknoPowiadomien("Przelew zrealizowany.");
}

// Funkcja tworzenia przycisku powrotu "<-"
void dodajPrzyciskPowrotu(Fl_Window* okno) {
    Fl_Button* powrotButton = new Fl_Button(10, 10, 30, 30, "<-");
    powrotButton->callback(zamknijOkno, okno);
    ustawStylPrzycisku(powrotButton);
}

// Funkcja tworzenia przycisku zamkniecia "X"
void dodajPrzyciskZamykania(Fl_Window* okno) {
    Fl_Button* zamknijButton = new Fl_Button(WINDOW_WIDTH - 40, 10, 30, 30, "X");
    zamknijButton->color(FL_RED);
    zamknijButton->callback(zamknijOkno, okno);
}

// Funkcja wyswietlania okna logowania
void pokazEkranLogowania() {
    ekranLogowania = new Fl_Window(SCREEN_CENTER_X, SCREEN_CENTER_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
    ekranLogowania->border(0);
    ekranLogowania->color(KOLOR_TLO);

    Fl_Input* poleLogin = new Fl_Input(150, 80, 200, 30, "Login:");
    poleLogin->labelcolor(KOLOR_TEKSTU);

    Fl_Input* polePin = new Fl_Input(150, 120, 200, 30, "PIN:");
    polePin->type(FL_SECRET_INPUT);
    polePin->labelcolor(KOLOR_TEKSTU);
    

    Fl_Button* loginButton = new Fl_Button(200, 180, 100, 30, "Zaloguj");
    loginButton->callback(cbZaloguj);
    ustawStylPrzycisku(loginButton);

    dodajPrzyciskZamykania(ekranLogowania);

    ekranLogowania->end();
    ekranLogowania->show();
}

// Funkcja wyswietlania okna glownego
void pokazEkranGlowny() {
    ekranGlowny = new Fl_Window(SCREEN_CENTER_X, SCREEN_CENTER_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
    ekranGlowny->border(0);
    ekranGlowny->color(KOLOR_TLO);

    ostringstream saldoStream;
    saldoStream << "Saldo: " << to_string(static_cast<int>(zalogowanyUzytkownik->saldo)) << ".00 PLN";
    Fl_Box* saldoBox = new Fl_Box(50, 50, 400, 30);
    saldoBox->copy_label(saldoStream.str().c_str());
    saldoBox->align(FL_ALIGN_CENTER);
    ustawStylBoxa(saldoBox, 16);

    Fl_Button* przelewButton = new Fl_Button(150, 100, 200, 30, "Nowy Przelew");
    przelewButton->callback([](Fl_Widget*, void*) { pokazOknoPrzelewu(); });
    ustawStylPrzycisku(przelewButton);

    Fl_Button* blikButton = new Fl_Button(150, 150, 200, 30, "Generuj BLIK");
    blikButton->callback([](Fl_Widget*, void*) { pokazOknoBlik(); });
    ustawStylPrzycisku(blikButton);

    Fl_Button* ustawieniaButton = new Fl_Button(10, 10, 100, 30, "Ustawienia");
    ustawieniaButton->callback([](Fl_Widget*, void*) { pokazOknoUstawien(); });
    ustawStylPrzycisku(ustawieniaButton);

    dodajPrzyciskZamykania(ekranGlowny);

    ekranGlowny->end();
    ekranGlowny->show();
}

// Funkcja wyswietlania okna przelewu
void pokazOknoPrzelewu() {
    oknoPrzelewu = new Fl_Window(SCREEN_CENTER_X, SCREEN_CENTER_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
    oknoPrzelewu->border(0);
    oknoPrzelewu->color(KOLOR_TLO);

    dodajPrzyciskPowrotu(oknoPrzelewu);

    Fl_Box* numerKontaBox = new Fl_Box(50, 50, 400, 30, "Wprowadz dane do przelewu:");
    ustawStylBoxa(numerKontaBox, 14);

    poleNumerKontaPrzelew = new Fl_Input(150, 100, 200, 30, "Numer Konta:");
    poleKwotaPrzelew = new Fl_Input(150, 150, 200, 30, "Kwota:");
    poleNumerKontaPrzelew->labelcolor(KOLOR_TEKSTU);
    poleKwotaPrzelew->labelcolor(KOLOR_TEKSTU);

    Fl_Button* wykonajPrzelew = new Fl_Button(150, 200, 200, 30, "Wykonaj");
    wykonajPrzelew->callback(cbPrzelew);
    ustawStylPrzycisku(wykonajPrzelew);

    oknoPrzelewu->end();
    oknoPrzelewu->show();
}

// Funckja wyswietlania okna ustawieñ
void pokazOknoUstawien() {
    oknoUstawien = new Fl_Window(SCREEN_CENTER_X, SCREEN_CENTER_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
    oknoUstawien->border(0);
    oknoUstawien->color(KOLOR_TLO);

    dodajPrzyciskPowrotu(oknoUstawien);

    Fl_Box* numerKontaBox = new Fl_Box(50, 100, 400, 30);
    numerKontaBox->align(FL_ALIGN_CENTER);
    numerKontaBox->copy_label(("Numer konta: " + zalogowanyUzytkownik->numerKonta).c_str());
    ustawStylBoxa(numerKontaBox, 18);

    Fl_Button* wylogujButton = new Fl_Button(150, 200, 200, 30, "Wyloguj");
    wylogujButton->callback(cbWyloguj);
    ustawStylPrzycisku(wylogujButton);

    oknoUstawien->end();
    oknoUstawien->show();
}

// Funkcja wysiwetlania okna BLIK
void pokazOknoBlik() {
    oknoBlik = new Fl_Window(SCREEN_CENTER_X, SCREEN_CENTER_Y, WINDOW_WIDTH, WINDOW_HEIGHT);
    oknoBlik->border(0);
    oknoBlik->color(KOLOR_TLO);

    dodajPrzyciskPowrotu(oknoBlik);

    srand(time(nullptr));
    int blikKod = rand() % 900000 + 200000;
    string kod = "Twoj kod BLIK: " + to_string(blikKod);

    Fl_Box* kodBox = new Fl_Box(50, 100, 400, 30);
    kodBox->copy_label(kod.c_str());
    kodBox->align(FL_ALIGN_CENTER);
    ustawStylBoxa(kodBox, 16);

    oknoBlik->end();
    oknoBlik->show();
}

void pokazOknoPowiadomien(char* wiadomosc) {
    Fl_Window* oknoWiadomosci = new Fl_Window(518,350,500,150);
    oknoWiadomosci->border(0);
    oknoWiadomosci->color(KOLOR_TEKSTU);

    dodajPrzyciskZamykania(oknoWiadomosci);

    Fl_Box* wiadomoscBox = new Fl_Box(50, 75, 400, 30, wiadomosc);
    wiadomoscBox->align(FL_ALIGN_CENTER);
    ustawStylBoxa(wiadomoscBox, 16);
    wiadomoscBox->labelcolor(KOLOR_TLO);

    oknoWiadomosci->end();
    oknoWiadomosci->set_modal();
    oknoWiadomosci->show();
}

int main() {
    pokazEkranLogowania(); //Program startuje z ekranem logowania, ka¿de nastêpne okno jest odpalane przez u¿ywane funkcje.
    return Fl::run();
}
