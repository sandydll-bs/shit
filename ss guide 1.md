# Regex

| **Regex** | **Spiegazione** | **Servizi** |
|---|---|---|
| `^TH[a-zA-Z0-9]{4}\.tmp$` | Gli strumenti hollow lasciano un file che inizia con `TH` e numeri casuali. Questa regex ne mostra la presenza. | Qualsiasi servizio. Principalmente DPS, Pcasvc, Explorer, Registry, CDPU, SearchIndexer o esegui Winprefetchview e fai il dump della memoria. |
| `^[A-Z]:\\.+\.(dll)$` | Mostra solo i file `.dll` | Principalmente csrss e SearchIndexer ma funziona con qualsiasi servizio; sostituisci `.dll` con qualsiasi altra estensione per filtrarli |
| `^([a-zA-Z]:\\.+)\\?$` | Mostra percorsi completi e file con percorso incluso. | Qualsiasi servizio |
| `^[A-Z]:\\.+\.(exe\|dll)$` | Filtra solo `.exe` e `.dll`. | |
| `^[a-zA-Z0-9]{8}\.jar$` | Mostra file `.jar` di 8 caratteri. Di solito rileva Doomsday Client. Sostituisci `.jar` con un'altra estensione per mostrare file di 8 caratteri. | DComLaunch, PlugPlay, Explorer, Pcasvc, SearchIndexer |
| `^{"displaytext"((?!Exe).)*$` | Eseguibili aperti; filtrare solo displaytext con ricerca case-insensitive mostra più risultati, questa è una ricerca precisa. | Explorer, CDPU |

## Contenuto

| **Contenuto** | **Spiegazione** | **Servizio** |
|---|---|---|
| `file:///` | Mostra file, cartelle, eseguibili e altri formati visitati o aperti. Utile per analizzare cosa ha fatto il giocatore sul dispositivo. | SearchIndexer, Explorer, Registry, WinPrefetchView (se eseguito e il processo viene dumpato con System Informer) |
| `Transaction` | Mostra log di potenziali eliminazioni, rinominazioni o sostituzioni di file. | Solo SearchIndexer. |
| `" "` | C'è uno spazio tra le `""`. Può spesso mostrare comandi per process hollowing o iniezioni Java, poiché i percorsi sono spesso scritti così. | SearchIndexer, DiagTrack, Msmpeng, Nissrv, Memory Compression, Explorer, TextInputHost, Clipboardsvc, CDPU |
| `Visited:` | Mostra archivi visitati | Explorer |
| `Registry` | Mostra voci del registro o tracce registrate in regedit dell'esecuzione di file | Explorer |
| `iwr (o Invoke-Web)` | Mostra download tramite Powershell. Anche se non include sempre campioni di cheat, i nuovi bypass usano spesso questi metodi. | Registry, Eventlog, Scheduler, Clipboardsvc, Textinputhost, DiagTrack |
| `!0!` | Mostra file potenzialmente sostituiti | DPS |
| `[ o ] (poi filtra i risultati per .exe)` | Mostra esecuzioni solo per nome dell'eseguibile. Spesso mostra file che non potevano essere loggati tramite percorso. | SearchIndexer |
| `HardDiskVolume` | Mostra percorsi HardDiskVolume delle esecuzioni di file. Numeri di volume diversi indicano drive/USB diversi | DPS, PCAsvc |
| `Trace,` | Pcaclient esteso; copia tutti i risultati e incollali nel blocco note per revisione. | Solo Pcasvc |
| `Command` | Mostra comandi PowerShell registrati nei log eventi pwsh | Eventlog |
| `[System.Reflection.Assembly]` | Potenziale rilevamento di malware fileless registrati nei log eventi o in altre aree di memoria | EventLog, Memory Compression, DiagTrack, SearchIndexer, Clipboardsvc, TextInputHost, Registry |

## Regex Estese

1. `^([A-Za-z]:[\\/]|\\\\.+?\\).+[\\/]$` — Mostra percorsi estesi, inclusi WSL e simili. Usabile in qualsiasi processo. Può mostrare percorsi con caratteri Unicode da verificare.
2. `^/.+/$` — Mostra formati di percorso di file insoliti.
3. `^[A-Z]:/[a-zA-Z0-9_\\-\\.]+/$` — Potenziale rilevamento di caratteri whitespace misti a lettere ASCII.
4. `[a-zA-Z]:\\(?:[^\\:*?"<>|\r\n]+\\)*[^\\:*?"<>|\r\n]+(?:\.\w+)?` — Ricerca estesa di percorsi e directory Windows in SearchIndexer.
5. `\b[a-zA-Z0-9_.-]+\.exe\b` — Rileva solo nomi di eseguibili, SearchIndexer.
6. `\b[a-zA-Z0-9_.-]+\.exe\s+\/[a-zA-Z]+(?:\s+[a-zA-Z]+)*\b` — Rileva potenziali argomenti da riga di comando, SearchIndexer/Scheduler.
7. `",trusted\b` — Eseguibili trusted, SearchIndexer.
8. `[\w~!@#$%^&*()-=+{}[\]:;'"<>?/|\\]+\.exe(?:[^\x00-\x7F]+|[^\x20-\x7E])` — Pattern insoliti.
9. `[#%&+-_~]*[a-zA-Z0-9_.-]+\.exe` — Pattern di percorso Unicode estesi; sostituisci `.exe` con altre estensioni per cercarle.

---

## Passaggi durante uno Screenshare 1.19+

### Win + R:
- `[1]` Controlla `C:\$Recycle.Bin` | 3 puntini, Opzioni, Visualizza, Mostra elementi nascosti, Mostra file di sistema protetti.
- `[2]` Controlla `shell:recent` | Utile per cheat non `.exe`.
- `[3]` Controlla `C:\ProgramData\Oracle\Java\.oracle_jre_usage` | Per trovare quando è stato eseguito un `.jar`, utile per Prestige/Doomsday.
- `[4]` Controlla `%temp%` | Cerca `imgui-java64` che può indicare l'uso di `imgui` in client (non bannare solo per questo — potrebbe essere legittimo; controlla cosa è successo in quel momento nel Journal USN o LastActivityViewer).

### Analisi Mod:
- `[1]` Vai ai resource pack e clicca su "apri cartella pack".
- `[2]` Vai a `.minecraft` dalla cartella dei resource pack.
- `[3]` Cerca la cartella mods e copia il percorso.
- `[4]` Esegui CMD (come ADMIN) e incolla: `powershell Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass && powershell Invoke-Expression (Invoke-RestMethod https://raw.githubusercontent.com/HadronCollision/PowershellScripts/refs/heads/main/HabibiModAnalyzer.ps1)`, poi incolla il percorso della cartella mods e attendi il completamento della scansione.

### System Informer: [https://systeminformer.com/canary](https://systeminformer.com/canary)
- Impostazioni | Colonna Sistema (in alto a sinistra) | Opzioni | 1. Abilita driver Kernel 2. Abilita simboli non decorati 3. Verifica firme digitali di servizi e immagini 4. Includi l'uso di processi compressi 5. Mostra opzioni avanzate.
  
NOTA: In Strumenti, Sistema > Punti di Reparse NTFS e Identificatori Oggetto, è possibile controllare molte informazioni sui file presenti/presenti sul dispositivo, quali sono stati eseguiti, scaricati, ecc. Tutto può essere copiato selezionando e premendo CTRL-C per eseguire, ad esempio, verificatori di firma sulla lista.

### Javaw.exe (Minecraft):
NOTA: Opzioni di ricerca stringhe per TUTTI! | Memoria | Opzioni | Deseleziona "Nascondi pagine libere e riservate" | Stringhe | 5 | Privata, Mappata, Immagine | Rileva Unicode ed Unicode esteso (Filtra per Contiene, case insensitive per tutti, salvo indicazione contraria).

- `[1]` Stringhe da cercare: `Autototem | Auto crystal | Cw crystal | Anchor macro | Anchormacro` (potrebbe flaggare nomi in gioco) `| Auto anchor | TriggerBot | AutoDhand | SlientAim | AutoInventoryTotem | aimassist | AutoCrystal | prestige | argon | stop_cracking | self de`
- `[2]` Apri il processo javaw.exe e vai in Thread e Handle; ordina i thread e inizia ad analizzare: di solito il thread più in cima è l'ultimo creato. Poiché in System Informer le iniezioni DLL vengono classificate come caricamento di nuovi thread, è logico supporre che le `.dll` in cima siano le più sospette. IMMAGINE: [https://i.imgur.com/B3fJIJQ.png](https://i.imgur.com/B3fJIJQ.png)
- `[3]` Quando si inietta un file personalizzato o uno script cheat direttamente nel processo javaw, gli Handle mostreranno tutto ciò che è stato caricato in memoria, ad esempio un log ESP `.txt`. Esempio: [https://i.imgur.com/YUgqVtW.png](https://i.imgur.com/YUgqVtW.png)
- `[4]` Infine, fai doppio clic sulla `.dll` sospetta nella sezione Thread (nel mio caso `oracle.dll`) per visualizzarne il contenuto in memoria. Puoi cercare `.pdb`, `.db`, o "application", oltre a molti altri identificatori che riveleranno immediatamente il cheat nascosto in un posto apparentemente legittimo. (Passi 2-4 sono necessari.)

### explorer.exe
NOTA: come sopra per le opzioni di ricerca stringhe.

- `[1]` Stringa: `file:///`
- `[2]` Stringa: `PcaClient` (Per lo più inutile per 1.21+ ma mantenuto)
- `[2]` Stringa: `{"displayText"`
- `[3]` Stringa: `cpu usage`

### Pcasvc (svchost.exe)
NOTA: come sopra.

- `[1]` Stringa: `e,0a000000,Reason,00002100`

### PlugPlay (svchost.exe)
NOTA: come sopra.

- `[1]` Stringa: `jar | .jar | java -jar`

### dnscache (svchost.exe)
NOTA: come sopra.

- `[1]` Stringa: `vape|whiteout|.wtf|intent.store|neverlack|dreamclient|wise|slapp.in|echo.ac|paladin.ac|cpn.ac|entropy.club|dopp.in|porn` — Cerca se l'utente ha visitato siti contenenti queste stringhe. Banna se appare wiseFH. Il "porn" è uno scherzo, ma usalo lo stesso xD.

### Diagtrack (svchost.exe)
NOTA: come sopra.

- `[1]` Stringa: `volume1|volume2|...|volume23` (Regex)

---

## PowerShell (come Admin)

- `(Get-PSReadlineOption).HistorySavePath` — Cronologia PowerShell. Win + R per aprire la directory mostrata.
- `Get-WinEvent Microsoft-Windows-Kernel-PnP/Configuration | findstr 410 > usb.log` — Cerca "USB".

---

## Everything: [https://www.voidtools.com/downloads/](https://www.voidtools.com/downloads/)

- Cerca `file attrib:h` e scorri i file.
- Cerca `.dmp` e controlla i nomi dei file sospetti.
- Cerca `appcrash` e controlla i nomi dei file sospetti.

---

## BONUS: Jar, Rilevare Archivi Java — DUMP DA ECHO CERT

[Link OneDrive](https://onedrive.live.com/view.aspx?resid=A2088A7D005E853A!sf5888e89d22d41eaa19b869d65c1d3d3&migratedtospo=true&redeem=aHR0cHM6Ly8xZHJ2Lm1zL3AvYy9hMjA4OGE3ZDAwNWU4NTNhL0lRU0pqb2oxTGRMcVFhR2JocDFsd2RQVEFSRlVHUnQ4ekFEdWExemhHenlNSnNRP2VtPTImYW1wO3dkQXI9MS43Nzc3Nzc3Nzc3Nzc3Nzc3&wdSlideId=263&wdModeSwitchTime=1746213527017)

---

## MACRO — TUTTI
NOTA: Rilevamento tramite Brand del Mouse (File/Posizioni di Configurazione del Software)

La posizione esatta e il formato dei dati di configurazione delle macro variano significativamente tra produttori e anche tra versioni diverse dello stesso software. Ecco le posizioni note o comunemente sospette per diversi brand popolari:

### Logitech:
- **Logitech Gaming Software (LGS - Vecchio):** Controlla `%localappdata%\Logitech\Logitech Gaming Software\`. Cerca file come `settings.json` o `.xml` con profili e macro.
- **Logitech G HUB (Nuovo):** Controlla `%localappdata%\LGHUB\`. La configurazione principale è spesso in `settings.db` (database SQLite, richiede un browser DB) o file/cartelle correlati. Analizza prima i timestamp di modifica.

### Razer:
- **Synapse 2 (Legacy):** Esamina le cartelle di installazione e `%localappdata%\Razer` o `%programdata%\Razer` per file di profilo/macro (spesso `.xml`).
- **Synapse 3:** Configurazione più complessa, possibilmente con sync cloud. Controlla `C:\ProgramData\Razer\Synapse3\Accounts\` e `%localappdata%\Razer\Synapse3\Log\` per log di attività che potrebbero indicare uso di macro o cambio di profilo.

### SteelSeries:
- **SteelSeries Engine / GG:** Controlla `%localappdata%\steelseries-engine-3-client\Local Storage\leveldb\` o simili. SteelSeries usa spesso database LevelDB che richiedono strumenti specializzati per l'ispezione.

### Roccat:
- **Roccat Swarm:** Controlla `%appdata%\ROCCAT\SWARM\`. Cerca sottocartelle come `macro` con file come `custom_macro_list.xml` o `macro_list.dat`.

### Red Dragon:
- I percorsi variano per modello. Controlla `%homepath%\Documents\` per cartelle tipo `M### Gaming Mouse`. Dentro, cerca sottocartelle `MacroDB` con file come `MacroData.db`.

### Glorious:
- Controlla `%appdata%\BY-COMBO2\`. Cerca `.json` o `.ini`. Esamina sottocartelle `Mac` o `Macro`.

### Cooler Master:
- Controlla `%localappdata%\CoolerMaster`, `%appdata%\CoolerMaster`, o `%programdata%\CoolerMaster`.

### Bloody:
- Controlla la directory di installazione, ad es. `C:\Program Files (x86)\Bloody7\Bloody7\Data\Mouse\English\ScriptsMacros\GunLib\`. Cerca anche file `.ini`, `.amc2`.

### Mad Catz, Mars Gaming, Ayax (Noganet), Krom (Kolt), BlackWeb, YanPol, MotoSpeed, Asus (ROG Armoury), Corsair (iCUE):
- Controlla le rispettive directory di installazione e percorsi AppData indicati nel documento originale.

---

## Passaggi di Analisi per Macro Software:

1. **Identifica il mouse** — Brand e modello esatto.
2. **Controlla la GUI del software** — Apri il software ufficiale e naviga alle sezioni macro e assegnazioni tasti.
3. **Controlla i percorsi dei file di configurazione** — Come indicato per il brand.
4. **Esamina le date di modifica** — Modifiche recenti prima dello screenshare sono altamente sospette.
5. **Analizza il contenuto dei file** — Cerca parole chiave come `macro`, `click`, `delay`, `repeat`, binding dei tasti, ecc.

---

## Rilevamento delle Macro On-Board
NOTA: Le macro on-board sono memorizzate direttamente nella memoria interna del mouse e funzionano anche senza il software del produttore installato.

### Identificazione del Modello (Passo Fondamentale):
Conoscere il modello esatto è essenziale. Molti mouse da gaming hanno pulsanti extra oltre ai 5 standard. Metodi di identificazione:
- Chiedi direttamente al giocatore.
- Controlla Impostazioni Windows: Bluetooth e Dispositivi > Dispositivi > Mouse.
- Gestione Dispositivi: cerca in "Mouse e altri dispositivi di puntamento".
- Strumenti cronologia USB: USBDeview (Nirsoft) o l'alternativa Echo.
- Conferma visiva (se consentito dalle policy del server).

### Procedura di Test dei Pulsanti del Mouse:
- Usa un tester di pulsanti affidabile online (es. `cpstest.org/mouse-test/` o `cps-check.com/mouse-buttons-test`).
- Istruisci il giocatore a premere ogni singolo pulsante fisico del mouse, uno alla volta.
- Osserva attentamente l'output nel tester.
- **Identifica le discrepanze (segnale d'allarme):** La chiave per rilevare una macro on-board è un'incoerenza costante tra il pulsante fisico premuto e quello registrato dal tester. Ad esempio: il giocatore preme il pulsante "Avanti" laterale, ma il tester registra un "Clic Sinistro".

### Motivazione del Rilevamento:
Per memorizzare e attivare una macro usando la memoria on-board, uno dei pulsanti fisici del mouse deve essere riprogrammato nel firmware per eseguire la sequenza macro invece della sua funzione predefinita. Questo causa la discrepanza osservata nel test.

---

## Tecniche di Bypass Comuni nello Screen Sharing (DA REDLOTUS)

### Estensioni Spoofate
**Descrizione:** Tecnica comune che consiste nel camuffare un file eseguibile (tipicamente `.exe`) rinominandone l'estensione in qualcosa di apparentemente innocuo (ad es. `.txt`, `.dll`, `.png`, `.tmp`).

**Il bypass si basa su metodi alternativi per avviare processi che non dipendono dall'estensione `.exe`:**
- Comandi PowerShell come `Start-Process C:\percorso\file_rinominato.tmp`.
- Comandi WMI, in particolare `wmic process class create "C:\percorso\file.dat"`.
- Task pianificati o altri metodi di scripting.

**Rilevamento:**
- **Analisi Prefetch:** Spesso il Prefetch registra l'esecuzione con il nome mascherato. Trovare file non `.exe` nel Prefetch è un segnale d'allarme.
- **Analisi della Memoria dei Processi:** Strumenti come System Informer possono rivelare i percorsi completi dei file eseguiti, inclusi quelli con estensioni spoofate.
- **Analisi Firma/Contenuto:** Eseguire controlli di firma su tutti i file sospetti (mostreranno "NotSigned" o "HashMismatch"). Strumenti come Detect It Easy o la ricerca di intestazioni PE possono esporre gli eseguibili mascherati.
- **Log di Esecuzione:** BAM, Activities Cache e i log eventi potrebbero registrare l'esecuzione sotto il nome mascherato.

### Offuscamento del Codice
**Descrizione:** Rende il codice sorgente o il bytecode compilato difficile da leggere e analizzare. Comune con cheat distribuiti come mod Minecraft (`.jar`) o applicazioni Java standalone.

**Meccanismi:**
- **Rinominazione:** Sostituzione di nomi significativi con caratteri casuali.
- **Offuscamento del Flusso di Controllo:** Inserimento di codice spazzatura o predicati opachi.
- **Crittografia delle Stringhe:** Le stringhe letterali vengono cifrate per non apparire in chiaro.
- **Packing/Crittografia:** Il codice principale viene compresso/cifrato e incorporato in uno stub loader.

**Rilevamento:**
- **Decompilazione/Disassemblaggio:** Se il codice prodotto è largamente illeggibile, è probabilmente offuscato.
- **Analisi dell'Entropia:** File compressi o cifrati hanno alta entropia (spesso >7.0/8). Strumenti come DiE o VirusTotal la calcolano.
- **Strumenti di Rilevamento Packer:** DiE include firme per identificare packer comuni (UPX, Themida, VMProtect).
- **Regole del Server:** Molti server vietano qualsiasi mod significativamente offuscata.

---

## Identificazione di Account Alternativi durante lo ScreenShare (BASE)

### Artefatti Username e Account nei File:
- **File di Log:** Log del client di gioco, log del launcher possono contenere username o UUID di account usati sul dispositivo.
- **File di Configurazione del Launcher:** Molti launcher memorizzano info sugli account (es. `accounts.json`).
- **Ricerche Generali nei File:** Cerca username bannati in `C:\Users\%username%`, Desktop, Download, Documenti e cartelle AppData.

### Script PowerShell per Alt:
- Lo script `ADVANCE ALT CHECK` su [https://pastebin.com/raw/LBGh2Cyb](https://pastebin.com/raw/LBGh2Cyb) cerca ricorsivamente nelle directory utente file con estensioni comuni (`.txt`, `.log`, `.json`, `.jar`) che contengono un username specificato.

---

## Script PowerShell: (Esegui in CMD come ADMIN)

- **RL Signature Check:** Verifica lo stato delle firme digitali Authenticode di file elencati (Valida, NonFirmata, HashMismatch, NonTrusted, ecc.)
- **Prefetch Integrity Analyzer:** Scansiona `C:\Windows\Prefetch` per anomalie (file di sola lettura, intestazione "MAM" errata, hash duplicati).
- **RL BAM Script:** Analizza le chiavi di registro BAM, mostra timestamp di esecuzione, percorso applicazione, risolve il SID utente ed esegue controlli di firma.
- **Streams Script:** Scansiona una cartella per file, recupera hash (MD5), proprietario, timestamp, attributi e Alternate Data Streams (ADS) incluso il contenuto di Zone.Identifier.
- **ActivitiesCache Parser:** Scarica ed esegue un parser per estrarre dati dall'ActivitiesCache.db.
- **ManualTasks.ps1:** Elenca i task pianificati creati dall'utente corrente.
- **SuspiciousScheduler.ps1:** Elenca i task pianificati e segnala quelli che coinvolgono programmi potenzialmente sospetti.
- **Task-Scheduler-Parser:** Analizza file XML in `C:\Windows\System32\Tasks`, estrae comandi e argomenti, segnala task con parole chiave sospette.
- **HardDiskVolume Converter:** Converte percorsi `\Device\HarddiskVolumeX` in percorsi standard con lettera di unità.

---

## Strumenti Utilizzati durante lo SS:

- [https://github.com/spokwn?tab=repositories](https://github.com/spokwn?tab=repositories)
- [http://dl.echo.ac/tool/](http://dl.echo.ac/tool/)

### Echo BAM (bam.exe):
Strumento grafico per visualizzare, filtrare e riordinare le voci del Background Activity Moderator (BAM). Semplifica l'accesso ai dati BAM, che registra le esecuzioni di programmi.

### Echo Journal Tool (journal-tool.exe):
Parser per il Journal USN NTFS (`$J`). Permette di filtrare eventi come eliminazioni, creazioni e rinominazioni di file. Analizza simultaneamente tutti i drive NTFS.

### Echo UserAssist View (userassist.exe):
Visualizzatore per i dati UserAssist del registro di Windows. Traccia l'esecuzione di applicazioni GUI e mostra se il file target esiste ancora.

### Echo String Tool (strings-tool.exe):
Permette di cercare più stringhe specifiche nella memoria di un processo selezionato simultaneamente. Utile per testare rapidamente rilevamenti personalizzati.

### Echo USBDEVIEW:
Simile a USBDeview di Nirsoft, mostra la cronologia dei dispositivi USB collegati al PC con tempi di connessione/disconnessione.
