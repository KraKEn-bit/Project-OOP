# 🎵 PocketTrack
### A Lightweight Offline Song Management & Recommendation System (C++ / OOP)

PocketTrack is a term project for **CSE 4302: Object Oriented Programming Lab** at **Islamic University of Technology (IUT)**.  
It is designed for students and casual listeners who want a **private, low-resource way** to manage local music collections and receive **personalized recommendations** without paid streaming services.

---

# 📸 Project Preview

<p align="center">
  <img src="Images/Project Display-1.png" width="45%" />
  <img src="Images/Project Display-2.png" width="45%" />
</p>

---

# 🚀 Features

## Core Functionalities (MVP)

### 🎧 Local Metadata Management
- Load song data from folder scans or CSV files  
- Manage local music collection  
- Lightweight offline storage  

### 🎼 Playlist Control
- Create playlists  
- Edit playlists  
- Delete playlists  
- Auto playlist generation  

### ▶️ Playback Simulation
- Play / Pause  
- Next / Previous  
- Queue system  
- Playback state management  

### ⭐ Rating System
- Rate songs (1-5)  
- Track listening history  
- Maintain play count  
- Preference learning  

### 💾 Persistent Storage
- Save user preferences  
- Save playlists  
- Store ratings  
- Text / Binary serialization  

---

# 🧠 Hybrid Recommendation Engine

### Offline Recommendation
Content-based filtering using:
- Genre  
- Mood tags  
- Ratings  
- Listening history  

### Online (Simulated)
YouTube-style recommendation based on:
- Mood selection  
- Search query  
- User activity  

---

# 🏗️ System Architecture

The project follows **Object-Oriented Modular Design**

### Core Classes

**Song**
- Title  
- Artist  
- Album  
- Duration  
- Rating  

**UserProfile**
- Favorite songs  
- Playlists  
- Preferences  

**Player**
- Queue management  
- Playback state  
- Controls  

**Recommender**
- Content-based filtering  
- Mood matching  
- History analysis  

**Library Management**
- SongLibrary  
- Playlist  
- Album  
- Artist  
- Genre  

---

# 👥 Team Members  
### Marks Oriented Programming - 1A

| Name | ID | Role |
|------|----|------|
Ahmed Samin Yasar | 230041113 | System Architect & Class Designer  
Rafsan Kabir Taaseen | 230041137 | Offline Engine & Recommendation Developer  
Mahir Labib | 230041139 | Online Services & Connectivity Handler  
Raed Rahman | 230041115 | UI, Interaction & Documentation Lead  

---

# 🛠️ Installation

### Clone Repository

```bash
git clone https://github.com/KraKEn-bit/Project-OOP.git
cd Project-OOP
```

### Build (CMake)

```bash
mkdir build
cd build
cmake ..
make
```

---

# 📂 Data Setup

Place song metadata inside:

```
data/
```

Supported format:
- CSV file  
- Folder scan (planned)

---

# 🧩 Planned Extensions

- GUI improvements (Qt / SFML)  
- Real audio playback (SDL2 / PortAudio)  
- Collaborative filtering  
- Smart recommendation engine  
- Search optimization  

---

# 📘 Course Information

**Course:** CSE 4302 – Object Oriented Programming Lab  
**Institution:** Islamic University of Technology (IUT)  
**Course Teachers:** Faisal Hussain, Farzana Tabassum  
**Submission Date:** 08-12-2025  

---

# ⭐ Project Goal

PocketTrack provides:

- Offline music management  
- Lightweight performance  
- Personalized recommendation  
- Privacy-focused listening  
