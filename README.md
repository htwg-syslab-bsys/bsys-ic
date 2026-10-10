# bsys-ic — Programme für den interaktiven Teil der IC-Stunden

Betriebssysteme (OSTEP): Hier liegen die Dateien, die in einer IC-Stunde am eigenen Notebook im bsyslab-Container bearbeitet werden, je Termin ein Ordner (`T2/`, …). Lösungen liegen hier nicht; sie werden in der Stunde besprochen.

## Einmalig: `ic` installieren

Im bsyslab-Container:

```
curl -fsSLo /tmp/ic-install.sh https://raw.githubusercontent.com/htwg-syslab-bsys/bsys-ic/main/install.sh
sh /tmp/ic-install.sh
```

## In der Stunde

```
ic T2          # holt die Dateien nach ~/ic/T2/
cd ~/ic/T2
make
make test
```

`ic T2` lässt sich beliebig oft aufrufen. Eigene Änderungen bleiben erhalten: Nur unveränderte Dateien werden aufgefrischt; liegt zu einer geänderten Datei eine neue Fassung vor, kommt sie als `DATEI.neu` daneben.

## Ohne Netz

Die Datei aus Moodle (oder vom Stick) in den Container holen, dann:

```
ic T2 --quelle PFAD/ZUR/DATEI        # tar.gz, zip oder entpacktes Verzeichnis
```

Fehlt `ic` noch: im entpackten Ordner `sudo install -m 755 ic /usr/local/bin/ic`.
