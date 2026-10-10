/* Aufgabe 2.1 (c) — Der misstrauische Kernel: Was antwortet der echte Linux-Kernel?
 *
 * Zwei Aufrufe mit ungültigen Werten. Beide Stellen
 * sind mit TODO markiert; zunächst stehen dort harmlose, gültige Werte.
 * Für jeden Aufruf gibt das Programm den Rückgabewert rc und errno aus.
 *
 *   make && ./syscall_probe
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

/* Name zu einer errno-Nummer (nur die häufigsten) */
static const char *errname(int e)
{
    switch (e) {
    case 0:      return "0";
    case EBADF:  return "EBADF";
    case EFAULT: return "EFAULT";
    case EINVAL: return "EINVAL";
    case ENOSYS: return "ENOSYS";
    case EPERM:  return "EPERM";
    default:     return "anderer";
    }
}

/* Eine Ergebniszeile: Aufruf, rc, errno-Name und Text. errno zählt nur bei rc < 0. */
static void zeile(const char *aufruf, long rc, int e)
{
    if (rc >= 0)
        e = 0;
    printf("%-36s rc=%ld errno=%s (%s)\n", aufruf, rc, errname(e),
           e ? strerror(e) : "kein Fehler");
}

int main(void)
{
    char text[64];
    long rc;
    int e;

    /* (1) Ein System Call mit einer Nummer, die es nicht gibt.
     * TODO: SYS_getpid durch eine solche Nummer ersetzen (frei gewählt). */
    long nummer = SYS_getpid;

    errno = 0;
    rc = syscall(nummer);
    e = errno;
    snprintf(text, sizeof text, "syscall(%ld)", nummer);
    zeile(text, rc, e);

    /* (2) write mit einer Adresse, die kein Puffer des Programms ist.
     * TODO: Adresse ersetzen — weit oben im Adressraum,
     *       z. B. (const void *) 0xfffffffffffff000UL */
    const void *adresse = "0123456789abcdef";

    fflush(stdout);                     /* Ausgabe von printf vor die von write */
    errno = 0;
    rc = write(1, adresse, 16);
    e = errno;
    if (rc > 0)
        printf("\n");                   /* die 16 geschriebenen Bytes abschließen */
    snprintf(text, sizeof text, "write(1, %p, 16)", adresse);
    zeile(text, rc, e);

    return 0;
}
