#!/bin/sh
# Einmalig: installiert das Abhol-Skript ic nach /usr/local/bin/ic (braucht sudo, kein Git).
#   curl -fsSLo /tmp/ic-install.sh https://raw.githubusercontent.com/htwg-syslab-bsys/bsys-ic/main/install.sh && sh /tmp/ic-install.sh
# Ohne Netz, im entpackten bsys-ic-Ordner:  sudo install -m 755 ic /usr/local/bin/ic
set -eu

URL="https://raw.githubusercontent.com/htwg-syslab-bsys/bsys-ic/main/ic"
ZIEL="${IC_PROGRAMM:-/usr/local/bin/ic}"

tmp=$(mktemp "${TMPDIR:-/tmp}/ic.XXXXXX")
trap 'rm -f "$tmp"' EXIT

if command -v curl >/dev/null 2>&1; then
    curl -fsSL --connect-timeout 10 --max-time 60 -o "$tmp" "$URL" || laden=nein
elif command -v wget >/dev/null 2>&1; then
    wget -q -T 20 -O "$tmp" "$URL" || laden=nein
else
    echo "install.sh: weder curl noch wget vorhanden." >&2; exit 1
fi
if [ "${laden:-ja}" = nein ] || [ ! -s "$tmp" ] || ! head -n 1 "$tmp" | grep -q '^#!.*bash'; then
    echo "install.sh: ic konnte nicht geladen werden (kein Netz?)." >&2
    echo "Ersatzweg: bsys-ic aus Moodle oder vom Stick entpacken, dann im Ordner:" >&2
    echo "    sudo install -m 755 ic /usr/local/bin/ic" >&2
    exit 1
fi

if [ -w "$(dirname "$ZIEL")" ]; then
    install -m 755 "$tmp" "$ZIEL"
elif command -v sudo >/dev/null 2>&1; then
    sudo install -m 755 "$tmp" "$ZIEL"
else
    echo "install.sh: sudo fehlt; $ZIEL lässt sich nicht anlegen." >&2; exit 1
fi

"$ZIEL" --version
echo "Fertig. Aufruf z. B.:  ic T3"
