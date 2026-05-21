<div align="center">

# 🔒 J6LR - Windows Lock Screen Demo

**Educational Windows Security Research Tool**

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache%202.0-red.svg)](https://www.apache.org/licenses/LICENSE-2.0)
[![Platform](https://img.shields.io/badge/Platform-Windows-blue.svg)](https://www.microsoft.com/windows)
[![Visual Studio](https://img.shields.io/badge/VS-2026-purple.svg)](https://visualstudio.microsoft.com/)
[![C++](https://img.shields.io/badge/C++-WinAPI-00599C.svg)](https://docs.microsoft.com/en-us/windows/win32/)
[![Windows API](https://img.shields.io/badge/Windows-Win32-0078D4.svg)](https://docs.microsoft.com/en-us/windows/win32/)
[![Security Research](https://img.shields.io/badge/Security-Research-yellow.svg)]()
[![Educational](https://img.shields.io/badge/Educational-Purpose-green.svg)]()

</div>

---

## 🏷️ Tags
`#windows` `#security` `#win32` `#cpp` `#educational` `#lock-screen` `#cybersecurity` `#windows-api` `#research` `#demo`

---

## 🎯 About J6LR

**J6LR** (pronounced "Jailer") is an **educational Windows security research tool** demonstrating lock-screen mechanisms, hardware fingerprinting, and recovery techniques.

> ⚠️ **IMPORTANT**: FOR EDUCATIONAL PURPOSES ONLY. Test ONLY in isolated virtual environments.

---

## ✨ Features

- 🔐 Full-screen system locking (topmost window)
- 💻 Hardware fingerprinting (volume serial + uptime)
- 🔑 Dynamic unlock key generation (`UNLOCK-XXXXXXXX`)
- 🚀 Startup persistence (registry autorun)
- 🛡️ Emergency recovery (RESCUE mode + backdoor)
- ⌨️ Hotkey blocking (WinKey, Alt+Tab, Alt+F4)
- 🎨 Green-on-black terminal aesthetic

---

## 🚀 Getting Started (Visual Studio 2026)

### Build

1. Open `J6LR.sln` in Visual Studio 2026
2. `Build → Build Solution (Ctrl+Shift+B)`
3. Find executable in `Debug/` or `Release/`

### Project Settings

| Setting | Value |
|---------|-------|
| Platform | x86 / x64 |
| Character Set | Unicode |
| Subsystem | Windows |

---

## 🔓 How to Unlock

| Method | Action |
|--------|--------|
| **Backdoor** | `Ctrl + Shift + K` → shows unlock key |
| **Rescue Mode** | `J6LR.exe RESCUE` |
| **Direct Entry** | Type key → Enter |

---

## 🛡️ Recovery

| Problem | Solution |
|---------|----------|
| Ran accidentally | `Ctrl+Shift+Esc` → kill process |
| Explorer killed | `Win+R` → `cmd` → `J6LR.exe RESCUE` |
| 3 attempts exceeded | Safe Mode → delete executable |

---

## 📝 License

**Apache License 2.0** - See [LICENSE](LICENSE) file.

---

## 🏷️ GitHub Topics

When creating your repository, add these topics:
windows
security
win32
cpp
educational
lock-screen
cybersecurity
windows-api
research
demo
winapi
system-lock

text

---

<div align="center">

**⭐ Star if educational | ⚠️ Test only in VMs**

</div>
