# How to install

## Step 1 - Klargør projekt

Klargør projekt til overførsel.
* Fjern alle `/build` directories. Disse findes typisk følgende steder:
  * `project-lyd/lib/build`
  * `project-lyd/slaveapp/build`
  * `project-lyd/masterapp/build`


## Step 2 - Opdatér projekt

Sørg for at benytte branch `master` og være på den seneste udgave `git pull`


## Step 3 - Forbind til turtlebot

Turtlebotten laver automatisk et Wifi-hotspot kort efter boot. Dette forbindes
til vha. en computer. Helst Linux eller Linux VM.


### Turtlebot Wifi:

SSID: `Gruppe4Turtlebot-Wifi`

Password: `turtlebot`


## Step 4 - Overførsel af projekt

Bevæg hen til mappen der indeholder `project-lyd`.

For at sende projektet benyttes `scp`:

```bash
scp -r project-lyd ubuntu@192.168.11.254:
```


## Step 5 - Login på turtlebot

For at logge ind på turtlebotten benyttes `ssh`:

```bash
ssh ubuntu@192.168.13.216
```

Login-password: `turtlebot`


## Step 6 - Find projekt på turtlebotten

Nu skal du gerne være på turtlebotten og have den seneste udgave af `project-lyd`.
*Skulle det ske at `project-lyd` ikke er overført korrekt, man man prøve at fjerne
det eksisterende projekt på turtlebotten.

Når man er logget ind havner man i `/home/ubuntu`-mappen, og kan se `project-lyd`
herfra på lokationen `/home/ubuntu/project-lyd`. Den kan derfor tilgås ved at 
`cd` direkte ind i den.

```bash
cd project-lyd
```

## Step 7 - Start af TMUX

Da vi skal have to programmer kørende samtidigt benyttes TMUX. TMUX gør det 
muligt at have flere terminaler i samme terminal-vindue.

Start TMUX:
```bash
tmux
```

For at lave et vindue ved siden af, tryk først <kbd>ctrl</kbd>+<kbd>B</kbd> og derefter <kbd>%</kbd>.

For at lave et vindue nedenunder, tryk først <kbd>ctrl</kbd>+<kbd>B</kbd> og derefter <kbd>"</kbd>

For at bevæge sig mellem vinduer, tryk først <kbd>ctrl</kbd>+<kbd>B</kbd> og så piletaster.

For at fjerne et vindue, kør: `exit`

TMUX cheat sheet: https://tmuxcheatsheet.com/

## Step 8 - Start ROS

ROS skal køre før kommandoer sendes videre til robotten.

Jeg kan ikke huske den eksakte kommando, men jeg ved at `history` viser kommandoen
var nummer 66. Derfor kan den køres ved at ekskvere *order 66*. 

```bash
!66
```

Denne kommando kører kommando nummer 66 fra historikken.

https://www.youtube.com/watch?v=sNjWpZmxDgg


## Step 9 - Kompilér `slaveapp`-delen projektet.

Bevæg ind i `slaveapp`-mappen i et andet vindue. Tjek at den ikke indeholder en `build`-mappe.
Hvis den gør det, fjernes denne mappe vha.: (Vær helt sikker på at du er inde i `slaveapp`-mappen!!!)
```bash
rm -rf build
```

Forudsat at der ikke er en en tilsvarende `build`-mappe i `project-lyd/lib`-mappen,
kan projektet kompileres. Dette sker automatisk ved at køre `create-slave-app.sh`.
Dette script kompilerer først protokol-implementationen i `project-lyd/lib` og 
kompilerer bagefter `slaveapp`-programmet, som afhænger af protokollen.

```bash
sh create-slave-app.sh
```

Det færdige program, `slaveapp` findes i `project-lyd/slaveapp/build/` og kan
eksekveres uden at bevæge sig ind i `build`-mappen:
```bash
./build/slaveapp
```

De to kommandoer kan også kombineres på en linje:
```bash
sh create-slave-app.sh && ./build/slaveapp
```

Nu står den og lytter efter beskeder fra `masterapp`.


## Ekstra

Hver gang man foretager sig ændringer i enten `projec-lyd/lib` eller `slaveapp`
skal `create-slave-app.sh` køres, da den sikrer at alle dele er projektet 
kompileres korrekt til næste test.

### Nano

Man kan ændre i filer vha. `nano`.

I nano gemmer man vha.: `ctrl+O`.

Forlader en fil vha.: `ctrl+X`

Søger i en fil vha.: `ctrl+W`

### Søgning efter tekst i alle filer

Bevæg ud i `project-lyd`-mappen

```bash
grep -r "tekst der skal søges efter"
```

### Slaveapp-mode

Man kan ændre `slaveapp` fra `TEST_MODE=OFF` til `TEST_MODE=ON` i `create-test-mode.sh`.

Hvis `TEST_MODE=ON` behøver man ikke MQTT for at kompilere programmet. MQTT bruges
til at tale med ROS.
