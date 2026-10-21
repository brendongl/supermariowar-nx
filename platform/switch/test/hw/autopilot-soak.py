#!/usr/bin/env python3
"""Unattended hardware soak through sys-autopilot (https://github.com/TooTallNate/sys-autopilot).

    SWITCH_HOST=192.168.x.y SWITCH_TOKEN=... autopilot-soak.py prepare NSP
    SWITCH_HOST=... SWITCH_TOKEN=... autopilot-soak.py launch
    SWITCH_HOST=... SWITCH_TOKEN=... autopilot-soak.py poll OUTDIR

prepare  installs the NSP and creates the autotest file (run it with no game open)
launch   opens the newest HOME menu tile, which is the freshly installed game
poll     saves a screenshot to OUTDIR, records whether the screen is still changing
         and copies any new Atmosphère crash reports. Run it every few minutes from a
         timer, then read smw.log after closing the game.
"""
import hashlib
import json
import os
import sys
import time
import urllib.error
import urllib.parse
import urllib.request

HOST = os.environ["SWITCH_HOST"]
TOKEN = os.environ.get("SWITCH_TOKEN", "")
BASE = f"http://{HOST}:4150"
GAME_DIR = "/switch/supermariowar/"


def request(method, path, body=None, timeout=30, content_type=None):
    req = urllib.request.Request(BASE + path, data=body, method=method)
    if TOKEN:
        req.add_header("Authorization", f"Bearer {TOKEN}")
    if content_type:
        req.add_header("Content-Type", content_type)
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return resp.read()


def files(path, **params):
    query = urllib.parse.urlencode({"path": path, **params})
    return f"/files?{query}"


def tap(*buttons, wait=0.6):
    request("POST", "/input/tap", json.dumps({"buttons": list(buttons), "durationMs": 100}).encode(),
            content_type="application/json")
    time.sleep(wait)


def status():
    return json.loads(request("GET", "/status", timeout=10))


def prepare(nsp):
    print("status:", status())
    size = os.path.getsize(nsp)
    with open(nsp, "rb") as f:
        req = urllib.request.Request(BASE + "/install", data=f, method="PUT")
        req.add_header("Content-Length", str(size))
        if TOKEN:
            req.add_header("Authorization", f"Bearer {TOKEN}")
        with urllib.request.urlopen(req, timeout=1800) as resp:
            print("install:", resp.read().decode()[:500])
    request("PUT", files(GAME_DIR + "autotest"), b"")
    print("autotest file created")


def launch():
    tap("HOME", wait=2.0)
    for _ in range(10):
        tap("LEFT", wait=0.3)
    tap("A", wait=15.0)


def log_tail():
    try:
        return request("GET", files(GAME_DIR + "smw.log", offset=-6000)).decode(errors="replace")
    except urllib.error.HTTPError:
        return ""


def last_heartbeat(text):
    beats = [line for line in text.splitlines() if line.startswith("[switch] frame")]
    return beats[-1] if beats else ""


def collect_crash(out, stamp):
    for name in ("smw.log", "smw.prev.log"):
        try:
            data = request("GET", files(GAME_DIR + name), timeout=60)
            open(os.path.join(out, f"{stamp}-{name}"), "wb").write(data)
        except urllib.error.HTTPError:
            pass
    for folder in ("/atmosphere/crash_reports/", "/atmosphere/fatal_errors/", "/atmosphere/erpt_reports/"):
        try:
            listing = request("GET", files(folder)).decode(errors="replace")
            open(os.path.join(out, f"{stamp}-{folder.strip('/').replace('/', '_')}.json"), "w").write(listing)
        except urllib.error.HTTPError:
            pass


def report_names():
    names = []
    for folder in ("/atmosphere/crash_reports/", "/atmosphere/fatal_errors/"):
        try:
            listing = json.loads(request("GET", files(folder)))
            names += [folder + e["name"] for e in listing.get("entries", [])]
        except Exception:
            pass
    return sorted(names)


def poll(out):
    """The game holds smw.log open for writing, and Horizon won't let another process
    read it meanwhile, so progress is judged from screenshots and new crash reports."""
    os.makedirs(out, exist_ok=True)
    state_path = os.path.join(out, "state.json")
    state = json.load(open(state_path)) if os.path.exists(state_path) else {}
    state.setdefault("shot", "")
    state.setdefault("stalls", 0)
    state.setdefault("unreachable", 0)
    state.setdefault("polls", 0)
    stamp = time.strftime("%Y%m%d-%H%M%S")
    try:
        info = status()
    except Exception as e:  # asleep, rebooting, or stuck on Atmosphère's fatal screen
        state["unreachable"] += 1
        print(f"{stamp} unreachable ({state['unreachable']} in a row): {e}")
        json.dump(state, open(state_path, "w"))
        return
    state["unreachable"] = 0
    state["polls"] += 1
    reports = report_names()
    if "reports" not in state:
        state["reports"] = reports
    new_reports = [r for r in reports if r not in state["reports"]]
    shot = b""
    for _ in range(3):
        try:
            shot = request("GET", "/screenshot", timeout=40)
            break
        except Exception:
            time.sleep(3)
    digest = hashlib.sha256(shot).hexdigest() if shot else ""
    if shot:
        open(os.path.join(out, f"{stamp}.jpg"), "wb").write(shot)
    changed = bool(digest) and digest != state["shot"]
    state["stalls"] = 0 if changed else state["stalls"] + 1
    state["shot"] = digest or state["shot"]
    print(f"{stamp} uptime={info.get('uptimeSeconds')} battery={info.get('batteryPercent')} "
          f"charging={info.get('charging')} screen_changed={changed} new_reports={new_reports}")
    if new_reports:
        state["reports"] = reports
        state.setdefault("crashes", []).extend(new_reports)
        collect_crash(out, stamp)
    json.dump(state, open(state_path, "w"))


if __name__ == "__main__":
    command = sys.argv[1]
    if command == "prepare":
        prepare(sys.argv[2])
    elif command == "launch":
        launch()
    elif command == "poll":
        poll(sys.argv[2])
    else:
        sys.exit(__doc__)
