# bsys-ic — Programme für den interaktiven Teil der IC-Stunden

Betriebssysteme (OSTEP): Hier liegen die Dateien, die in einer IC-Stunde am eigenen Notebook im bsyslab-Container bearbeitet werden, je Termin ein Ordner (`T{n}/`). Lösungen liegen hier nicht; sie werden in der Stunde besprochen.

**Derzeit liegt kein Termin-Ordner vor.** Hat eine IC-Stunde einen Teil am Notebook, steht das vorher in Moodle. Aufgabe 2.1 („Der misstrauische Kernel“) läuft in T3 als Vorführung; dafür laden Sie nichts herunter.

## Einmalig: Repo klonen

Im bsyslab-Container (nach `ssh bsyslab`):

```
git clone https://github.com/htwg-syslab-bsys/bsys-ic.git ~/bsys-ic
```

## In der Stunde

```
cd ~/bsys-ic && git pull     # neuen Stand holen
cd T{n}                      # Termin-Ordner laut Moodle, z. B. T4
make
make test
```

## Ohne Netz im Container

Die Datei `bsys-ic-T{n}.tar.gz` aus Moodle (oder vom Stick) am Notebook herunterladen, dann:

```
scp bsys-ic-T{n}.tar.gz bsyslab:~/                                  # im Terminal des Notebooks
mkdir -p ~/bsys-ic && tar xzf ~/bsys-ic-T{n}.tar.gz -C ~/bsys-ic    # im Container
```
