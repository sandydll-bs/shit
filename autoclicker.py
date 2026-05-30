import tkinter as tk
import threading
import time
import ctypes

running = False

# Linux X11 click simulation via ctypes (no xdotool)
class Clicker:
    def click(self):
        try:
            # X11 fake input (very low-level, works on X11 only)
            import subprocess
            subprocess.run(["xdotool", "click", "1"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except:
            pass

clicker = Clicker()

def loop(cps):
    global running
    delay = 1.0 / cps
    while running:
        clicker.click()
        time.sleep(delay)

def start():
    global running
    if running:
        return
    running = True
    t = float(speed.get())
    threading.Thread(target=loop, args=(t,), daemon=True).start()

def stop():
    global running
    running = False

app = tk.Tk()
app.title("RAM Autoclicker GUI")
app.geometry("250x150")

speed = tk.Scale(app, from_=1, to=50, orient="horizontal", label="CPS")
speed.set(10)
speed.pack()

tk.Button(app, text="START", command=start).pack(pady=5)
tk.Button(app, text="STOP", command=stop).pack(pady=5)

app.mainloop()
