# bsys-ic — Programme für den interaktiven Teil der IC-Stunden

Betriebssysteme (OSTEP): Hier liegen die Dateien, die in einer IC-Stunde am eigenen Notebook im bsyslab-Container bearbeitet werden, je Termin ein Ordner (`T3/`, …). Lösungen liegen hier nicht; sie werden in der Stunde besprochen.

## Einmalig: Repo klonen

Im bsyslab-Container (nach `ssh bsyslab`):

```
git clone https://github.com/htwg-syslab-bsys/bsys-ic.git ~/bsys-ic
```

## In der Stunde

```
cd ~/bsys-ic && git pull     # neuen Stand holen
cd T3
make
make test
```

## Ohne Netz im Container

Die Datei `bsys-ic-T3.tar.gz` aus Moodle (oder vom Stick) am Notebook herunterladen, dann:

```
scp bsys-ic-T3.tar.gz bsyslab:~/                                  # im Terminal des Notebooks
mkdir -p ~/bsys-ic && tar xzf ~/bsys-ic-T3.tar.gz -C ~/bsys-ic    # im Container
```
