/* Aufgabe 2.1 — Der misstrauische Kernel: der Lehr-Dispatcher aus dem Handout als Modell.
 *
 * Das Programm läuft im User Mode und spielt nach, was der Lehr-Kernel nach der
 * Trap-Instruktion tut: Es prüft die Nummer, springt über die Tabelle und ruft im Fall
 * von sys_write die Pufferprüfung auf.
 *
 *   ./dispatch_sim N              System Call Nummer N (Puffer für sys_write: buf=0x1000, n=16)
 *   ./dispatch_sim puffer BUF N   sys_write mit diesem Puffer, BUF und N dezimal, hexadezimal
 *                                 (0x…) oder relativ zu K (K, K+0x1000, K-8)
 *
 * Bearbeitet werden nur die beiden Funktionen syscall_dispatch und puffer_erlaubt.
 * Alles darunter ist Technik des Modells.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSYS 4

/* Adressen ab K gehören dem Kernel (Sachinformation im Handout). */
#define K 0x8000000000000000UL

_Static_assert(sizeof(unsigned long) == 8, "Das Modell setzt 64-Bit-Adressen voraus");

/* Ein Eintrag der Tabelle; a1..a3 sind die Registerinhalte, die das Programm übergibt. */
typedef long (*sysfn)(unsigned long a1, unsigned long a2, unsigned long a3);

/* Technik des Modells (unten definiert): springt über die Tabelle wie
 * syscall_table[num](a1, a2, a3) im Lehr-Kernel. */
static long tabelle_aufrufen(long num, unsigned long a1, unsigned long a2, unsigned long a3);


/* (a) Der Verteiler aus dem Handout — läuft im Lehr-Kernel im Kernel Mode. */
static long syscall_dispatch(long num, unsigned long a1, unsigned long a2, unsigned long a3)
{
    if (num > NSYS)                     /* (a) */
        return -1;                      /* ungültige Nummer */
    return tabelle_aufrufen(num, a1, a2, a3);
}


/* (b) Darf sys_write n Bytes ab Adresse buf lesen?  1 = erlaubt, 0 = abgelehnt. */
static int puffer_erlaubt(unsigned long buf, unsigned long n)
{
    /* TODO (b): Prüfung ergänzen. */
    (void) buf;
    (void) n;
    return 1;
}


/* ---------------------------------------------------------------------------------------
 * Ab hier: Technik des Modells, für die Aufgabe nicht nötig.
 * ------------------------------------------------------------------------------------- */

static int erreicht;   /* 1, sobald der Sprung irgendwo angekommen ist */

static long sys_read(unsigned long a1, unsigned long a2, unsigned long a3)
{
    (void) a1; (void) a2; (void) a3;
    erreicht = 1; printf("sys_read erreicht\n");
    return 0;
}

/* write(fd, buf, n): n Bytes ab Adresse buf an Datei oder Gerät fd */
static long sys_write(unsigned long fd, unsigned long buf, unsigned long n)
{
    (void) fd;
    erreicht = 1; printf("sys_write erreicht\n");
    if (!puffer_erlaubt(buf, n)) {
        printf("Puffer buf=0x%lx, n=0x%lx: abgelehnt\n", buf, n);
        return -1;
    }
    printf("Puffer buf=0x%lx, n=0x%lx: erlaubt\n", buf, n);
    return 0;                           /* hier würde device_send die n Bytes lesen */
}

static long sys_open(unsigned long a1, unsigned long a2, unsigned long a3)
{
    (void) a1; (void) a2; (void) a3;
    erreicht = 1; printf("sys_open erreicht\n");
    return 0;
}

static long sys_exit(unsigned long a1, unsigned long a2, unsigned long a3)
{
    (void) a1; (void) a2; (void) a3;
    erreicht = 1; printf("sys_exit erreicht\n");
    return 0;
}

/* Was im Kernel-Speicher direkt neben der Tabelle liegt. */
static long speicher_davor(unsigned long a1, unsigned long a2, unsigned long a3)
{
    (void) a1; (void) a2; (void) a3;
    erreicht = 1;
    printf("nimmt den Wert direkt vor der Tabelle als Sprungziel (im Kernel Mode) — "
           "dort liegt anderer Kernel-Speicher\n");
    return 0;
}

static long speicher_dahinter(unsigned long a1, unsigned long a2, unsigned long a3)
{
    (void) a1; (void) a2; (void) a3;
    erreicht = 1;
    printf("nimmt den Wert direkt hinter der Tabelle als Sprungziel (im Kernel Mode) — "
           "dort liegt anderer Kernel-Speicher\n");
    return 0;
}

/* Der Kernel-Speicher im Modell: die Tabelle mit vier Einträgen, davor und dahinter je ein
 * Eintrag anderer Kernel-Speicher. Die Tabelle ist ein Ausschnitt dieses Arrays. */
static sysfn kernel_speicher[] = {
    speicher_davor, sys_read, sys_write, sys_open, sys_exit, speicher_dahinter
};
#define MODELL_EINTRAEGE (sizeof kernel_speicher / sizeof kernel_speicher[0])
static sysfn *const syscall_table = kernel_speicher + 1;

static long tabelle_aufrufen(long num, unsigned long a1, unsigned long a2, unsigned long a3)
{
    /* Position im Modell-Array, vorzeichenlos gerechnet (definierter Wraparound modulo 2^64,
     * auch für sehr große und negative Nummern). */
    unsigned long i = (unsigned long) num + 1UL;

    if (i >= MODELL_EINTRAEGE) {
        erreicht = 1;
        printf("nimmt einen Wert weit neben der Tabelle als Sprungziel (im Kernel Mode), "
               "im Modell nicht nachgebildet\n");
        return 0;
    }
    return syscall_table[num](a1, a2, a3);
}

static void gebrauch(void)
{
    fprintf(stderr,
            "Aufruf: ./dispatch_sim N             System Call Nummer N\n"
            "        ./dispatch_sim puffer BUF N  sys_write mit Puffer BUF und Länge N\n"
            "BUF und N: dezimal, hexadezimal (0x1000) oder relativ zu K (K, K+0x1000, K-8)\n");
}

/* Nummer lesen: ganze Zahl, auch negativ. 0 bei Erfolg. */
static int lies_nummer(const char *s, long *wert)
{
    char *ende;

    errno = 0;
    *wert = strtol(s, &ende, 10);
    return (*s == '\0' || *ende != '\0' || errno == ERANGE) ? -1 : 0;
}

/* Nichtnegative Zahl lesen: dezimal oder hexadezimal mit 0x, ohne Vorzeichen. 0 bei Erfolg. */
static int lies_zahl(const char *s, unsigned long *wert)
{
    char *ende;
    int basis = 10;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        basis = 16;
        s += 2;
    }
    if (!((*s >= '0' && *s <= '9') || (basis == 16 && strchr("abcdefABCDEF", *s) != NULL && *s != '\0')))
        return -1;
    errno = 0;
    *wert = strtoul(s, &ende, basis);
    return (*ende != '\0' || errno == ERANGE) ? -1 : 0;
}

/* Adresse oder Länge lesen: Zahl, K, K+Zahl oder K-Zahl (modulo 2^64 gerechnet). */
static int lies_wert(const char *s, unsigned long *wert)
{
    unsigned long d;

    if (s[0] != 'K')
        return lies_zahl(s, wert);
    if (s[1] == '\0') { *wert = K; return 0; }
    if ((s[1] != '+' && s[1] != '-') || lies_zahl(s + 2, &d) != 0)
        return -1;
    *wert = (s[1] == '+') ? K + d : K - d;
    return 0;
}

int main(int argc, char *argv[])
{
    long num;
    unsigned long a2 = 0x1000UL, a3 = 16UL;   /* Puffer für sys_write */

    if (argc == 2 && lies_nummer(argv[1], &num) == 0) {
        /* Nummer aus der Kommandozeile */
    } else if (argc == 4 && strcmp(argv[1], "puffer") == 0
               && lies_wert(argv[2], &a2) == 0 && lies_wert(argv[3], &a3) == 0) {
        num = 1;                                 /* sys_write */
    } else {
        gebrauch();
        return 2;
    }

    printf("num=%ld: ", num);
    fflush(stdout);
    syscall_dispatch(num, 1UL, a2, a3);
    if (!erreicht)
        printf("abgelehnt\n");
    return 0;
}
