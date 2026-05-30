```bash
#!/bin/bash

STATE_FILE="/tmp/autoclicker_state"

start_clicking() {
  echo "1" > "$STATE_FILE"

  (
    while [ -f "$STATE_FILE" ] && [ "$(cat $STATE_FILE)" = "1" ]; do
      xdotool click 1
      sleep 0.1   # velocità click (0.1 = 10 cps)
    done
  ) &
}

stop_clicking() {
  rm -f "$STATE_FILE"
}

while true; do
  ACTION=$(zenity --list \
    --title="Linux Autoclicker Test" \
    --text="Controllo autoclicker" \
    --column="Azione" \
    "Start" "Stop" "Exit")

  case "$ACTION" in
    "Start")
      start_clicking
      ;;
    "Stop")
      stop_clicking
      ;;
    "Exit")
      stop_clicking
      exit 0
      ;;
    *)
      exit 0
      ;;
  esac
done
```
