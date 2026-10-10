#!/usr/bin/env bash
# Prüft dispatch_sim: welche Nummern welche Funktion erreichen und welche Puffer sys_write
# annimmt. Aufruf: make test   oder   ./test.sh   (übersetzt selbst)
set -u
export LC_ALL=C
cd "$(dirname "$0")" || exit 1
make -s all || { echo "Übersetzen fehlgeschlagen"; exit 1; }
pass=0; fail=0

# check "Beschreibung" "erwartet" "gemessen" — ein leerer Messwert ist immer FAIL
check() {
    local b=$((50 + $(printf '%s' "$1" | tr -cd '\200-\277' | wc -c)))   # Breite in Zeichen
    if [ -n "$3" ] && [ "$2" = "$3" ]; then
        printf '  PASS  %-*s %s\n' "$b" "$1" "$3"; pass=$((pass + 1))
    else
        printf '  FAIL  %-*s erwartet: %s | gemessen: %s\n' "$b" "$1" "$2" "${3:-(leer)}"; fail=$((fail + 1))
    fi
}

# nummer N → was der Aufruf mit Nummer N erreicht, zusammengefasst:
#   „sys_… erreicht“, „abgelehnt“, sonst „angenommen (Sprung)“; Abbruch des Programms → Exit-Status
nummer() {
    local aus st z
    aus=$(./dispatch_sim "$1" 2>&1); st=$?
    if [ "$st" -ne 0 ]; then echo "Programm endete mit Exit-Status $st"; return; fi
    z=$(printf '%s\n' "$aus" | sed -n "s/^num=$1: //p" | head -1)
    case "$z" in
        '')                       ;;
        abgelehnt|sys_*' erreicht') echo "$z" ;;
        *)                        echo "angenommen (Sprung)" ;;
    esac
}
# puffer BUF N → „erlaubt“ oder „abgelehnt“; Abbruch des Programms → Exit-Status
puffer() {
    local aus st
    aus=$(./dispatch_sim puffer "$1" "$2" 2>&1); st=$?
    if [ "$st" -ne 0 ]; then echo "Programm endete mit Exit-Status $st"; return; fi
    printf '%s\n' "$aus" | sed -n 's/^Puffer .*: //p' | head -1
}

echo "Teil (a) — Nummern"
check "Nummer 0"   "sys_read erreicht"  "$(nummer 0)"
check "Nummer 1"   "sys_write erreicht" "$(nummer 1)"
check "Nummer 2"   "sys_open erreicht"  "$(nummer 2)"
check "Nummer 3"   "sys_exit erreicht"  "$(nummer 3)"
check "Nummer 4"   "abgelehnt"          "$(nummer 4)"
check "Nummer -1"  "abgelehnt"          "$(nummer -1)"
check "Nummer 100" "abgelehnt"          "$(nummer 100)"

echo
echo "Teil (b) — Puffer für sys_write"
check "Puffer-Fall 1: buf=0x1000, n=0x10"               "erlaubt"   "$(puffer 0x1000 0x10)"
check "Puffer-Fall 2: buf=0x8000000000001000, n=0x1000" "abgelehnt" "$(puffer 0x8000000000001000 0x1000)"
check "Puffer-Fall 3: buf=0x7ffffffffffffff8, n=0x10"   "abgelehnt" "$(puffer 0x7ffffffffffffff8 0x10)"
check "Puffer-Fall 4: buf=0x1000, n=0xffffffffffffffff" "abgelehnt" "$(puffer 0x1000 0xffffffffffffffff)"
check "Puffer-Fall 5: buf=0xfffffffffffffff8, n=0x10"   "abgelehnt" "$(puffer 0xfffffffffffffff8 0x10)"
check "Puffer-Fall 6: buf=0x7ffffffffffffff8, n=0x8"    "erlaubt"   "$(puffer 0x7ffffffffffffff8 0x8)"
check "Puffer-Fall 7: buf=0x7fffffffffffffff, n=0x1"    "erlaubt"   "$(puffer 0x7fffffffffffffff 0x1)"
check "Puffer-Fall 8: buf=0x7fffffffffffffff, n=0x2"    "abgelehnt" "$(puffer 0x7fffffffffffffff 0x2)"

echo
echo "Teil (c) — syscall_probe"
check "syscall_probe übersetzt" "ja" "$([ -x ./syscall_probe ] && echo ja || echo nein)"

echo
echo "Ergebnis: $pass bestanden, $fail fehlgeschlagen"
[ "$fail" -eq 0 ]
