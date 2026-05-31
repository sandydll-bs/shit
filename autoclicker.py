import tkinter as tk
from tkinter import ttk
import threading
import time
import pyautogui
import sys
import subprocess

def ensure_package(package):
    try:
        __import__(package)
    except ImportError:
        print(f"[+] Installazione automatica: {package}")
        subprocess.check_call([
            sys.executable,
            "-m",
            "pip",
            "install",
            "--upgrade",
            package
        ])

ensure_package("pyautogui")

BG = "#0b1020"
CARD = "#121a2f"
ACCENT = "#00d4ff"
TEXT = "#e6f7ff"
MUTED = "#7fa6b8"

running = False
worker = None


def click_loop():
    global running

    while running:
        try:
            cps = max(1, cps_var.get())
            pyautogui.click()
            time.sleep(1 / cps)
        except Exception:
            time.sleep(0.1)


def start_clicker():
    global running, worker

    if running:
        return

    running = True
    status_var.set("RUNNING")

    worker = threading.Thread(target=click_loop, daemon=True)
    worker.start()


def stop_clicker():
    global running

    running = False
    status_var.set("STOPPED")


def toggle_clicker(event=None):
    if running:
        stop_clicker()
    else:
        start_clicker()

root = tk.Tk()
root.title("Zapphire AutoClicker")
root.geometry("420x300")
root.resizable(False, False)
root.configure(bg=BG)

style = ttk.Style()
style.theme_use("clam")

frame = tk.Frame(
    root,
    bg=CARD,
    bd=0,
    highlightthickness=1,
    highlightbackground="#1e335c"
)
frame.place(relx=0.5, rely=0.5, anchor="center", width=380, height=260)

title = tk.Label(
    frame,
    text="ZAPPHIRE AUTOCLICKER",
    bg=CARD,
    fg=ACCENT,
    font=("Segoe UI", 16, "bold")
)
title.pack(pady=(15, 10))

subtitle = tk.Label(
    frame,
    text="F6 = Start / Stop",
    bg=CARD,
    fg=MUTED,
    font=("Segoe UI", 9)
)
subtitle.pack()

cps_var = tk.IntVar(value=15)

cps_label = tk.Label(
    frame,
    text="Clicks Per Second",
    bg=CARD,
    fg=TEXT,
    font=("Segoe UI", 10)
)
cps_label.pack(pady=(20, 5))

slider = tk.Scale(
    frame,
    from_=1,
    to=100,
    orient="horizontal",
    variable=cps_var,
    bg=CARD,
    fg=TEXT,
    troughcolor="#0f2545",
    activebackground=ACCENT,
    highlightthickness=0,
    length=250
)
slider.pack()

status_var = tk.StringVar(value="STOPPED")

status = tk.Label(
    frame,
    textvariable=status_var,
    bg=CARD,
    fg=TEXT,
    font=("Segoe UI", 11, "bold")
)
status.pack(pady=15)

button_frame = tk.Frame(frame, bg=CARD)
button_frame.pack()

start_btn = tk.Button(
    button_frame,
    text="START",
    command=start_clicker,
    bg=ACCENT,
    fg="black",
    width=12,
    relief="flat",
    font=("Segoe UI", 10, "bold")
)
start_btn.grid(row=0, column=0, padx=5)

stop_btn = tk.Button(
    button_frame,
    text="STOP",
    command=stop_clicker,
    bg="#203050",
    fg=TEXT,
    width=12,
    relief="flat",
    font=("Segoe UI", 10, "bold")
)
stop_btn.grid(row=0, column=1, padx=5)

root.bind("<F6>", toggle_clicker)

root.mainloop()
