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

# **Features:**

## Core Functionalities (MVP)

### Local Metadata Management
- Load song data from folder scans or CSV files  
- Manage local music collection  
- Lightweight offline storage  

### Playlist Control
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

### Persistent Storage
- Save user preferences  
- Save playlists  
- Store ratings  
- Text / Binary serialization  

---

# Hybrid Recommendation Engine

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

# **System Architecture**

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

# **Running with Qt Creator (Recommended):**

PocketTrack uses **Qt Creator + CMake** for building and running the project.

## Step 1: Install Qt Creator

Download Qt Creator from:
https://www.qt.io/download

During installation make sure you select:

- Qt Creator
- Desktop Qt (MinGW or MSVC)
- CMake
- Ninja (optional but recommended)

---

## Step 2: Clone the Repository

```bash
git clone https://github.com/KraKEn-bit/Project-OOP.git
cd Project-OOP
```

---

## Step 3: Open Project in Qt Creator

1. Open **Qt Creator**
2. Click **Open Project**
3. Select `CMakeLists.txt`
4. Choose Desktop Kit (MinGW / MSVC)
5. Click **Configure Project**

Qt Creator will automatically run:

- CMake configure  
- Build setup  
- Compiler detection  

---

## Step 4: Build & Run

Click:

▶ Run button (bottom left)

OR

Ctrl + R

Qt Creator will:

- Build project  
- Run executable  
- Launch PocketTrack UI  

---

## Build Requirements

- C++17 compatible compiler  
- Qt Creator 8+  
- CMake 3.16+  
- Desktop Qt Kit  


---

## Notes

- Make sure `CMakeLists.txt` is in the root directory  
- If build fails → Run **Build → Run CMake** again  
- If compiler not detected → Configure Kits in Qt Creator  



---

# **Team Members**  
### Marks Oriented Programming - 1A

| Name | ID | Role |
|------|----|------|
Ahmed Samin Yasar | 230041113 | System Architect & Class Designer  
Rafsan Kabir Taaseen | 230041137 | Offline Engine & Recommendation Developer  
Mahir Labib | 230041139 | Online Services & Connectivity Handler  
Raed Rahman | 230041115 | UI, Interaction & Documentation Lead  

---




# **Course Information:**

**Course:** CSE 4302 – Object Oriented Programming Lab  
**Institution:** Islamic University of Technology (IUT)  
**Course Teachers:** Faisal Hussain, Farzana Tabassum  
---

- Personalized recommendation  
- Privacy-focused listening  
