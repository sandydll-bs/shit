# Regex Collection

## -s dps (EXE)

Filtra gli eseguibili, escludendo la maggior parte dei processi e servizi Windows comuni.

```regex
(?i)^.*!!(?!(?:audiodg|aggregatorhost|applicationframehost|backgroundtaskhost|conhost|consent|csrss|ctfmon|dllhost|dwm|explorer|fontdrvhost|gamebarpresencewriter|lockapp|logonui|lsaiso|lsass|mousocoreworker|monotificationux|msmpeng|mpdefendercoreservice|nissrv|runtimebroker|searchapp|searchhost|searchindexer|searchprotocolhost|securityhealthhost|securityhealthservice|securityhealthsystray|shellexperiencehost|sihost|smartscreen|smss|spoolsv|sppsvc|startmenuexperiencehost|svchost|systemsettings|taskhostw|taskmgr|textinputhost|useroobebroker|wermgr|widgetservice|widgets|wininit|winlogon|wlanext|wmiprvse|wuauclt|vmcompute|vmmem|wslservice|filecoauth|onedrive(?:\.sync\.service)?|microsoftedgeupdate|onedrivestandaloneupdater|platform_experience_helper|searchfilterhost|wecsvc|services|powershell|pwsh|cmd|reg|rundll32|taskkill|tasklist|schtasks|sc|wmic|wevtutil|whoami|netsh|dism|sfc|bcdedit|control|msconfig|msinfo32|eventvwr|mmc|werfault|jhi_service|trustedinstaller|tiworker|poqexec|ngciso|secinit|bytecodegenerator|mdmdiagnosticstool|drvinst|vds|vdsldr|elevation_service)\.exe!).*\.exe!
```

---

## diagtrack (JAR)

Estrae i file `.jar` usati con il parametro `-jar`.

```regex
(?i)-jar\s+"?([^"\r\n]+\.jar)"?
```

---

## msmpeng (JAR)

Trova esecuzioni Java con `-jar`, escludendo `mail.jar`.

```regex
(?i)^(?!.*(?:\\|/)mail\.jar\b).*java(?:w)?\.exe.*?-jar\s+"?([^"\r\n]+\.jar)
```

---

## msmpeng (REGSVR32)

Trova invocazioni semplici di `regsvr32` con DLL.

```regex
(?i)^(?:regsvr32)(?:\.exe)?\s+[^\s"]+\.dll(?:\s+[A-Za-z_][A-Za-z0-9_]*)?$
```

---

## msmpeng (RUNDLL32)

Trova invocazioni semplici di `rundll32` con DLL.

```regex
(?i)"?(?:[a-z]:\\windows\\system32\\)?rundll32\.exe"?\s+[a-z0-9_.-]+\.dll(?:\s+[a-z])?(?=\s|$)
```

---

## dwm (Cambio Ora)

Trova modifiche dell'orario nel formato:

```text
- time 10:10
```

```regex
(?i)-\s*time\s+\d{1,2}:\d{2}
```

---

## -s dps (Estensioni Spoofate)

Mostra file con estensioni diverse da `.exe` nel formato:

```text
!!file.ext!YYYY/MM/DD:HH:MM:SS!hash!
```

```regex
(?i)^!![^!]+\.(?!(?:exe)!)[a-z0-9]+!\d{4}/\d{2}/\d{2}:\d{2}:\d{2}:\d{2}![0-9a-f]+!$
```
