<div align="center">

```
███████╗██╗      ██████╗  ██████╗ ██████╗ ███████╗ ██████╗ ██████╗  ██████╗ ███████╗
██╔════╝██║     ██╔═══██╗██╔═══██╗██╔══██╗██╔════╝██╔═══██╗██╔══██╗██╔════╝ ██╔════╝
█████╗  ██║     ██║   ██║██║   ██║██║  ██║█████╗  ██║   ██║██████╔╝██║  ███╗█████╗  
██╔══╝  ██║     ██║   ██║██║   ██║██║  ██║██╔══╝  ██║   ██║██╔══██╗██║   ██║██╔══╝  
██║     ███████╗╚██████╔╝╚██████╔╝██████╔╝██║     ╚██████╔╝██║  ██║╚██████╔╝███████╗
╚═╝     ╚══════╝ ╚═════╝  ╚═════╝ ╚═════╝ ╚═╝      ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚══════╝
```

# 🌊 FloodForge
### AI-Powered Flood Simulation & Early Warning System

---

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus)](https://isocpp.org/)
[![Qt](https://img.shields.io/badge/Qt-6.x-41CD52?logo=qt)](https://www.qt.io/)
[![Python](https://img.shields.io/badge/Python-3.10+-3776AB?logo=python)](https://python.org)
[![Azure](https://img.shields.io/badge/Azure-Blob_Storage-0078D4?logo=microsoftazure)](https://azure.microsoft.com/)
[![Groq](https://img.shields.io/badge/Groq-LLM_API-F55036)](https://groq.com/)
[![OpenGL](https://img.shields.io/badge/OpenGL-Shaders-5586A4?logo=opengl)](https://www.opengl.org/)
[![Status](https://img.shields.io/badge/Status-Active-brightgreen)]()

<br/>

> **FloodForge** is a full-stack geospatial intelligence platform that fuses real terrain elevation data, live weather feeds, a high-performance C++ simulation engine, AI-generated natural language alerts, and Azure cloud storage — into a single desktop application that puts actionable flood risk insight in the hands of disaster management teams within minutes.

<br/>

[🚀 Quick Start](#-installation) · [📸 Screenshots](#-screenshots) · [🎥 Demo](#-demo-workflow) · [🏗 Architecture](#-system-architecture) · [🤖 AI Components](#-ai-components) · [☁️ Cloud](#-cloud-architecture)

</div>

---

## 📋 Table of Contents

1. [Problem Statement](#-problem-statement)
2. [Solution Overview](#-solution-overview)
3. [System Architecture](#-system-architecture)
4. [Full Pipeline Explanation](#-full-pipeline)
5. [Features](#-features)
6. [Technology Stack](#-technology-stack)
7. [Repository Structure](#-repository-structure)
8. [Demo Workflow](#-demo-workflow)
9. [Screenshots](#-screenshots)
10. [Installation](#-installation)
11. [Usage](#-usage)
12. [Cloud Architecture](#-cloud-architecture)
13. [AI Components](#-ai-components)
14. [Simulation Engine](#-simulation-engine)
15. [Data Pipeline](#-data-pipeline)
16. [Future Work](#-future-work)
17. [Credits](#-credits)

---

## 🌍 Problem Statement

Floods are the **world's most frequent and destructive natural disasters**, responsible for over **$40 billion in annual economic damage** and thousands of casualties globally. Yet the tools available to disaster response teams are often:

- 🔴 **Siloed** — terrain data, weather forecasts, and risk models live in separate systems
- 🔴 **Slow** — traditional hydrological models require hours or days to produce results
- 🔴 **Inaccessible** — GIS expertise is required to interpret outputs
- 🔴 **Opaque** — raw data does not translate into human-actionable intelligence
- 🔴 **Expensive** — enterprise solutions are out of reach for local disaster management authorities

There is a critical gap between **raw geospatial data** and **timely, interpretable, on-the-ground flood intelligence**.

---

## 💡 Solution Overview

**FloodForge** closes this gap by combining four powerful capabilities into one cohesive desktop platform:

| Capability | Description |
|---|---|
| 🌐 **Geospatial Ingestion** | Downloads real terrain elevation data (DEM) for any geographic bounding box via OpenTopography API |
| ⛈️ **Weather Integration** | Fetches historical and forecasted rainfall via Meteostat / RapidAPI |
| ⚙️ **Simulation Engine** | High-performance C++ engine models water accumulation and flood depth across the terrain grid |
| 🤖 **AI Intelligence Layer** | Groq LLM generates natural-language flood alerts and technical impact reports |
| ☁️ **Cloud Persistence** | All simulation artifacts are versioned and stored on Azure Blob Storage for auditability and replay |

FloodForge is **open, fast, interpretable, and deployable** — designed for the first 24 hours of a flood emergency when decisions matter most.

---

## 🏗 System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          FLOODFORGE SYSTEM ARCHITECTURE                     │
└─────────────────────────────────────────────────────────────────────────────┘

  ┌──────────────────────┐     ┌──────────────────────┐
  │   OpenTopography API │     │   Meteostat RapidAPI  │
  │   (DEM GeoTIFF)      │     │   (Rainfall Data)     │
  └──────────┬───────────┘     └──────────┬────────────┘
             │                            │
             ▼                            ▼
  ┌──────────────────────────────────────────────────────┐
  │              DATA ACQUISITION LAYER                  │
  │         [C++ / Python API Integration Module]        │
  └──────────────────────────┬───────────────────────────┘
                             │
             ┌───────────────┼───────────────┐
             ▼               ▼               ▼
     ┌──────────────┐ ┌─────────────┐ ┌──────────────┐
     │ DEM GeoTIFF  │ │ Rainfall    │ │  dem.csv     │
     │  (GDAL)      │ │  (mm)       │ │  (grid)      │
     └──────┬───────┘ └──────┬──────┘ └──────────────┘
            │                │
            └───────┬────────┘
                    ▼
  ┌──────────────────────────────────────────────────────┐
  │              SIMULATION ENGINE (C++)                 │
  │                                                      │
  │   demGrid[row][col]  ──►  floodGrid[row][col]        │
  │   Elevation Grid            Flood Depth Grid         │
  └──────────────────────────────────────────────────────┘
                    │
                    ▼
  ┌──────────────────────────────────────────────────────┐
  │           RISK CLASSIFICATION ENGINE                 │
  │                                                      │
  │   LOW RISK  ◄── [Blue]   flood_depth < threshold_1   │
  │   MED RISK  ◄── [Orange] flood_depth < threshold_2   │
  │   HIGH RISK ◄── [Red]    flood_depth ≥ threshold_2   │
  └──────────────────────────────────────────────────────┘
                    │
          ┌─────────┴──────────┐
          ▼                    ▼
  ┌──────────────┐    ┌─────────────────────────────┐
  │  risk_map.png│    │     GROQ LLM API             │
  │  (Heatmap)   │    │  ┌─────────────────────────┐ │
  └──────────────┘    │  │  flood_alert.txt         │ │
                      │  │  impact_report.txt       │ │
                      │  └─────────────────────────┘ │
                      └─────────────────────────────┘
                                   │
                                   ▼
  ┌──────────────────────────────────────────────────────┐
  │           AZURE BLOB STORAGE                         │
  │   Container: flood-data                              │
  │   Path: runs/YYYY-MM-DD/session_HHMMSS/             │
  │                                                      │
  │   📄 dem.csv          📄 flood_depth.csv             │
  │   📄 metadata.json    🖼️  risk_map.png               │
  │   📄 flood_alert.txt  📄 impact_report.txt          │
  └──────────────────────────────────────────────────────┘
                                   │
                    ┌──────────────┘
                    ▼
  ┌──────────────────────────────────────────────────────┐
  │              QT DESKTOP APPLICATION                  │
  │  ┌────────────┐  ┌────────────┐  ┌────────────────┐  │
  │  │ 2D Terrain │  │ 3D OpenGL  │  │ Console Logger │  │
  │  │ Map View   │  │ Renderer   │  │ (HTML styled)  │  │
  │  └────────────┘  └────────────┘  └────────────────┘  │
  └──────────────────────────────────────────────────────┘
```

---

## 🔄 Full Pipeline

FloodForge executes a **12-stage deterministic pipeline**, from raw geographic data to AI-generated flood intelligence.

---

### Stage 1 — Terrain Data Acquisition

```
User Input: Geographic Bounding Box  ──►  OpenTopography API  ──►  DEM GeoTIFF File
            (lat/lon min/max)               (REST request)          (elevation raster)
```

The user defines a **geographic bounding box** by specifying minimum and maximum latitude/longitude coordinates. FloodForge sends an authenticated request to the **OpenTopography API**, which responds with a **Digital Elevation Model (DEM)** in **GeoTIFF format** — a georeferenced raster image where each pixel encodes terrain height above sea level.

This raw GeoTIFF forms the foundation for all downstream processing.

---

### Stage 2 — DEM Processing

```
GeoTIFF (GDAL) ──► Raster Extraction ──► demGrid[row][col] ──► dem.csv
```

The GeoTIFF is loaded and decoded using **GDAL (Geospatial Data Abstraction Library)**. The raster band is read into memory and reshaped into a 2D elevation grid:

```
demGrid[row][col] = elevation (meters)
```

Each cell represents a terrain tile. The full grid is exported as `dem.csv` for downstream reproducibility, cloud archiving, and ML experiments.

---

### Stage 3 — Terrain Visualization

The elevation grid is rendered inside the Qt desktop application in two modes:

| Mode | Technology | Description |
|---|---|---|
| **2D Terrain Map** | Qt/QPainter | Top-down color-coded elevation heatmap |
| **3D Terrain Rendering** | OpenGL + GLSL Shaders | Interactive 3D surface with lighting and depth |

Users can **click any terrain cell** to inspect:
- Row and column index
- Elevation value (meters)
- Flood depth after simulation (meters)

---

### Stage 4 — Rainfall Data Integration

```
User selects date range ──► Meteostat RapidAPI ──► Rainfall series ──► Aggregated Total (mm)
```

The user selects a **start date** and **end date**. FloodForge queries the **Meteostat API (via RapidAPI)** to retrieve historical precipitation records for the region.

The time series is aggregated to produce:

```
Average Monthly Rainfall    = 24.62 mm
Total Rainfall (simulation) = 738.60 mm     (scaled for simulation window)
```

This total is passed directly into the simulation engine as the water input parameter.

---

### Stage 5 — Flood Simulation Engine

```
demGrid[row][col] + rainfall_mm ──► C++ Engine ──► floodGrid[row][col]
```

The core of FloodForge is a **high-performance flood simulation engine written in C++**. Given the terrain elevation grid and total rainfall, the engine:

1. Identifies low-lying cells (natural basins and valleys)
2. Models water accumulation based on elevation differentials
3. Propagates water flow across neighboring cells
4. Outputs a flood depth for every terrain cell

```cpp
// Output:
floodGrid[row][col] = predicted_flood_depth (meters)
```

The full output grid is exported as `flood_depth.csv`.

---

### Stage 6 — Flood Risk Classification

Flood depth values are **normalized** and each cell classified into one of three risk tiers:

| Risk Level | Criteria | Color |
|---|---|---|
| 🟦 **Low Risk** | Depth below lower threshold | Blue |
| 🟧 **Medium Risk** | Depth between thresholds | Orange |
| 🟥 **High Risk** | Depth above upper threshold | Red |

Classification statistics are computed and stored:

```json
{
  "low_risk_cells": 4821,
  "medium_risk_cells": 1203,
  "high_risk_cells": 476
}
```

---

### Stage 7 — Risk Map Generation

```
floodGrid + RiskClassification ──► Heatmap Renderer ──► risk_map.png
```

A **2D spatial heatmap** is generated from the classified flood grid. Each cell is colored by its risk tier (Blue / Orange / Red), producing an intuitive, print-ready risk map saved as `risk_map.png`.

---

### Stage 8 — AI Flood Alert Generation

```
[rainfall, max_depth, risk_counts] ──► Groq LLM Prompt ──► flood_alert.txt
```

FloodForge assembles a structured prompt incorporating:
- Total rainfall (mm)
- Maximum predicted flood depth (m)
- Risk cell counts per tier

This prompt is submitted to the **Groq LLM API**, which returns a **human-readable, actionable flood alert**:

```
⚠️  FLOOD ALERT — HIGH RISK
━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total Rainfall    : 738.60 mm
Max Flood Depth   : 3.4 m
High Risk Cells   : 476

Water depth may reach 3.4 meters in low-lying zones.
Residents in affected areas should evacuate to higher ground.
Emergency services should pre-position equipment at designated flood zones.
```

Saved as: `flood_alert.txt`

---

### Stage 9 — AI Impact Report

A **second, distinct LLM prompt** generates a technical impact assessment. This report covers:
- Flood severity distribution across terrain zones
- Infrastructure stress analysis (roads, utilities, drainage)
- Recommended mitigation actions

> ⚠️ **Accuracy Safeguard**: The prompt explicitly instructs the model: *"DO NOT fabricate population figures or economic loss estimates."* Only simulation-derived data is incorporated.

Saved as: `impact_report.txt`

---

### Stage 10 — Cloud Storage

```
Local Artifacts ──► Azure Blob Storage SDK ──► flood-data/runs/YYYY-MM-DD/session_HHMMSS/
```

All six simulation artifacts are uploaded to **Microsoft Azure Blob Storage**:

| File | Contents |
|---|---|
| `dem.csv` | Terrain elevation grid |
| `flood_depth.csv` | Predicted flood depth per cell |
| `metadata.json` | Simulation parameters and statistics |
| `risk_map.png` | Visual risk classification heatmap |
| `flood_alert.txt` | AI-generated public alert |
| `impact_report.txt` | AI-generated technical impact report |

Every simulation run occupies its own timestamped path:
```
flood-data/runs/2026-03-07/session_213015/
```

---

### Stage 11 — Metadata Tracking

A structured `metadata.json` is generated for every run, enabling full reproducibility:

```json
{
  "timestamp": "2026-03-07T21:30:15Z",
  "rainfall_mm": 738.60,
  "grid_rows": 100,
  "grid_cols": 100,
  "max_flood_depth": 3.41,
  "risk_cell_counts": {
    "low": 4821,
    "medium": 1203,
    "high": 476
  }
}
```

---

### Stage 12 — Desktop Application Logging

The Qt interface logs all pipeline events in real time:

```
[✓] DEM loaded — grid: 100×100, max elevation: 847m
[✓] Rainfall fetched — avg: 24.62mm, total used: 738.60mm
[⚙] Simulation running...
[✓] Risk map generated — risk_map.png
[✓] AI flood alert generated — flood_alert.txt
[✓] Impact report generated — impact_report.txt
[☁] Files uploaded to Azure — runs/2026-03-07/session_213015/
```

All messages are styled with **HTML formatting** for clarity and color-coded severity.

---

## ✨ Features

- 🗺️ **Real-World DEM Ingestion** — Any geographic bounding box, powered by OpenTopography
- 📡 **Live Rainfall Integration** — Historical precipitation from Meteostat via RapidAPI
- ⚙️ **C++ Simulation Core** — High-performance flood propagation engine
- 🎨 **Dual Terrain Visualization** — 2D heatmap and 3D OpenGL terrain renderer
- 🖱️ **Interactive Cell Inspector** — Click any cell to view elevation and flood depth
- 🏷️ **3-Tier Risk Classification** — Low / Medium / High with spatial statistics
- 🖼️ **Risk Map Export** — Color-coded PNG heatmap for reports and briefings
- 🤖 **AI Flood Alert** — Groq LLM-generated natural-language public warning
- 📋 **AI Impact Report** — Technical analysis without fabricated statistics
- ☁️ **Azure Cloud Archiving** — Versioned, timestamped simulation storage
- 📄 **Metadata Tracking** — Full reproducibility via `metadata.json`
- 🖥️ **Styled Console Logger** — Real-time pipeline status in the desktop UI

---

## 🧰 Technology Stack

| Layer | Technology | Purpose |
|---|---|---|
| **Simulation Engine** | C++17 | Core flood propagation algorithm |
| **Desktop UI** | Qt 6.x | Cross-platform native application framework |
| **3D Rendering** | OpenGL + GLSL Shaders | Interactive terrain visualization |
| **GIS / Raster** | GDAL | GeoTIFF parsing, DEM extraction |
| **DEM API** | OpenTopography API | Digital Elevation Model downloads |
| **Weather API** | Meteostat via RapidAPI | Historical/forecast rainfall data |
| **AI / LLM** | Groq API | Flood alerts and impact report generation |
| **Cloud Storage** | Azure Blob Storage | Simulation artifact versioning |
| **ML Experiments** | Python 3.10+ | Data analysis and model prototyping |
| **Build System** | CMake | Cross-platform C++ build configuration |

---

## 📁 Repository Structure

```
FLOOD-FORGE/
│
├── 📂 simulation-engine/          # C++ flood simulation core
│   ├── src/
│   │   ├── FloodEngine.cpp        # Main simulation algorithm
│   │   ├── DEMProcessor.cpp       # Elevation grid processing
│   │   ├── RiskClassifier.cpp     # Flood depth → risk tier mapping
│   │   └── OutputExporter.cpp     # CSV / PNG export utilities
│   ├── include/
│   │   ├── FloodEngine.h
│   │   ├── DEMProcessor.h
│   │   └── RiskClassifier.h
│   └── CMakeLists.txt
│
├── 📂 desktop-app/                # Qt desktop application
│   ├── src/
│   │   ├── main.cpp               # Application entry point
│   │   ├── MainWindow.cpp         # Primary UI controller
│   │   ├── TerrainWidget.cpp      # 2D terrain map renderer
│   │   ├── TerrainGLWidget.cpp    # OpenGL 3D terrain renderer
│   │   ├── SimulationRunner.cpp   # Pipeline orchestration
│   │   ├── APIClient.cpp          # OpenTopography & Meteostat calls
│   │   ├── GroqClient.cpp         # LLM API integration
│   │   └── AzureUploader.cpp      # Azure Blob Storage uploader
│   ├── include/
│   │   ├── MainWindow.h
│   │   ├── TerrainWidget.h
│   │   └── ...
│   ├── ui/
│   │   └── mainwindow.ui          # Qt Designer form
│   ├── shaders/
│   │   ├── terrain.vert           # Vertex shader (3D terrain)
│   │   └── terrain.frag           # Fragment shader (lighting)
│   ├── resources/
│   │   └── resources.qrc          # Qt resource bundle
│   └── CMakeLists.txt
│
├── 📂 ml-model/                   # Python ML experiments
│   ├── notebooks/
│   │   ├── dem_analysis.ipynb     # Elevation data exploration
│   │   └── flood_risk_model.ipynb # ML risk prediction prototyping
│   ├── scripts/
│   │   ├── preprocess_dem.py
│   │   └── rainfall_analysis.py
│   └── requirements.txt
│
├── 📂 data-apis/                  # API integration utilities
│   ├── opentopo_client.py         # OpenTopography wrapper
│   ├── meteostat_client.py        # Meteostat/RapidAPI wrapper
│   └── README.md
│
├── 📂 sample-data/                # Example simulation outputs
│   ├── dem.csv
│   ├── flood_depth.csv
│   ├── risk_map.png
│   ├── flood_alert.txt
│   ├── impact_report.txt
│   └── metadata.json
│
├── 📄 CMakeLists.txt              # Root build configuration
├── 📄 .env.example                # Environment variables template
├── 📄 LICENSE
└── 📄 README.md
```

---

## 🎥 Demo Workflow

Follow these steps to run a complete FloodForge simulation from scratch:

```
Step 1          Step 2          Step 3          Step 4
┌──────────┐   ┌──────────┐   ┌──────────┐   ┌──────────┐
│  Load /  │──►│  Fetch   │──►│  Run     │──►│  View    │
│ Generate │   │ Rainfall │   │  Flood   │   │ Risk Map │
│   DEM    │   │  Data    │   │  Sim     │   │          │
└──────────┘   └──────────┘   └──────────┘   └──────────┘

Step 5          Step 6          Step 7
┌──────────┐   ┌──────────┐   ┌──────────┐
│ Generate │──►│ Generate │──►│  Upload  │
│  Flood   │   │  Impact  │   │  Azure   │
│  Alert   │   │  Report  │   │  Cloud   │
└──────────┘   └──────────┘   └──────────┘
```

**Detailed Steps:**

1. **Load or Generate DEM**
   - Enter a geographic bounding box (min/max lat/lon)
   - Click **"Fetch DEM"** — FloodForge downloads and processes the GeoTIFF
   - Terrain renders in both 2D and 3D views

2. **Fetch Rainfall Data**
   - Select a date range using the date picker controls
   - Click **"Fetch Rainfall"** — Meteostat data is retrieved and aggregated

3. **Run Flood Simulation**
   - Click **"Run Simulation"**
   - The C++ engine processes `demGrid` + `rainfall_mm` → `floodGrid`
   - Progress is logged in the console panel

4. **View Risk Map**
   - The 2D heatmap updates automatically post-simulation
   - Blue / Orange / Red cells indicate Low / Medium / High risk zones
   - Click any cell to inspect flood depth

5. **Generate AI Flood Alert**
   - Click **"Generate Alert"**
   - Groq LLM synthesizes a natural-language public warning
   - Alert displayed in UI and saved to `flood_alert.txt`

6. **Generate AI Impact Report**
   - Click **"Generate Report"**
   - Full technical analysis output to `impact_report.txt`

7. **Upload Results to Azure Cloud**
   - Click **"Upload to Cloud"**
   - All six artifacts uploaded to `flood-data/runs/YYYY-MM-DD/session_HHMMSS/`
   - Confirmation and path logged to console

---

## 📸 Screenshots

> _Replace the placeholders below with actual screenshots of your running application._

<br/>

**2D Terrain & Flood Risk Map**
```
┌─────────────────────────────────────────────┐
│        [ Screenshot: 2D Terrain View ]      │
│   dem_2d_view.png — elevation heatmap with  │
│   flood risk overlay (blue/orange/red)      │
└─────────────────────────────────────────────┘
```
`screenshots/2d_terrain_risk_map.png`

<br/>

**3D OpenGL Terrain Renderer**
```
┌─────────────────────────────────────────────┐
│      [ Screenshot: 3D OpenGL Render ]       │
│   terrain_3d.png — lit 3D surface mesh,     │
│   cell inspector tooltip visible            │
└─────────────────────────────────────────────┘
```
`screenshots/3d_terrain_opengl.png`

<br/>

**Risk Map Export (risk_map.png)**
```
┌─────────────────────────────────────────────┐
│     [ Screenshot: Generated Risk Map ]      │
│   risk_map.png — full color-coded           │
│   spatial flood risk heatmap                │
└─────────────────────────────────────────────┘
```
`screenshots/risk_map_output.png`

<br/>

**AI Flood Alert Output**
```
┌─────────────────────────────────────────────┐
│    [ Screenshot: AI Alert in Desktop UI ]   │
│   flood_alert.txt displayed in-app          │
│   with Groq response visible                │
└─────────────────────────────────────────────┘
```
`screenshots/ai_flood_alert.png`

<br/>

**Azure Cloud Upload Confirmation**
```
┌─────────────────────────────────────────────┐
│  [ Screenshot: Console Log — Azure Upload ] │
│   Timestamped session path, all 6 files     │
│   confirmed uploaded                        │
└─────────────────────────────────────────────┘
```
`screenshots/azure_upload_log.png`

<br/>

**🎥 Demo Video**
```
┌─────────────────────────────────────────────┐
│           [ DEMO VIDEO PLACEHOLDER ]        │
│   demo/floodforge_demo.mp4                  │
│   Full end-to-end walkthrough (~3 min)      │
└─────────────────────────────────────────────┘
```

---

## 🛠 Installation

### Prerequisites

Ensure the following dependencies are installed on your system:

| Dependency | Version | Install |
|---|---|---|
| CMake | ≥ 3.21 | [cmake.org](https://cmake.org) |
| Qt | 6.x | [qt.io](https://www.qt.io/download) |
| GDAL | ≥ 3.4 | `sudo apt install libgdal-dev` |
| OpenGL | ≥ 4.1 | System-provided (GPU driver) |
| Python | ≥ 3.10 | [python.org](https://python.org) |
| Azure SDK (C++) | Latest | See [azure-sdk-for-cpp](https://github.com/Azure/azure-sdk-for-cpp) |

---

### 1. Clone the Repository

```bash
git clone https://github.com/YOUR_USERNAME/FloodForge.git
cd FloodForge
```

### 2. Configure Environment Variables

```bash
cp .env.example .env
```

Edit `.env` and populate your credentials:

```env
# OpenTopography
OPENTOPO_API_KEY=your_opentopography_api_key

# RapidAPI / Meteostat
RAPIDAPI_KEY=your_rapidapi_key

# Groq LLM
GROQ_API_KEY=your_groq_api_key

# Azure Blob Storage
AZURE_STORAGE_CONNECTION_STRING=DefaultEndpointsProtocol=https;AccountName=...
AZURE_CONTAINER_NAME=flood-data
```

### 3. Build the Desktop Application

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 4. Install Python Dependencies (ML / API utils)

```bash
cd ml-model
pip install -r requirements.txt
```

### 5. Run the Application

```bash
./build/desktop-app/FloodForge
```

---

## 🚀 Usage

### Running a Simulation via the GUI

```
1.  Launch FloodForge
2.  Enter bounding box:  min_lat, max_lat, min_lon, max_lon
3.  Click  [Fetch DEM]
4.  Set date range for rainfall
5.  Click  [Fetch Rainfall]
6.  Click  [Run Simulation]
7.  Inspect risk map in 2D / 3D views
8.  Click  [Generate Alert]
9.  Click  [Generate Report]
10. Click  [Upload to Cloud]
```

### Running the Simulation Engine Directly (CLI)

```bash
# From the simulation-engine directory:
./FloodSimEngine \
  --dem dem.csv \
  --rainfall 738.60 \
  --output flood_depth.csv
```

### Fetching a DEM Programmatically (Python)

```python
from data_apis.opentopo_client import fetch_dem

fetch_dem(
    min_lat=37.7,
    max_lat=37.9,
    min_lon=-122.5,
    max_lon=-122.3,
    output_path="dem.tif"
)
```

### Fetching Rainfall Data (Python)

```python
from data_apis.meteostat_client import fetch_rainfall

rainfall = fetch_rainfall(
    lat=37.8,
    lon=-122.4,
    start="2024-01-01",
    end="2024-12-31"
)
print(f"Total rainfall: {rainfall['total_mm']:.2f} mm")
```

---

## ☁️ Cloud Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                    AZURE BLOB STORAGE                            │
│                                                                  │
│  Storage Account: floodforgestorage                              │
│  Container:       flood-data                                     │
│                                                                  │
│  flood-data/                                                     │
│  └── runs/                                                       │
│      ├── 2026-03-07/                                             │
│      │   ├── session_213015/                                     │
│      │   │   ├── dem.csv              (terrain elevation grid)   │
│      │   │   ├── flood_depth.csv      (simulation output)        │
│      │   │   ├── metadata.json        (run parameters + stats)   │
│      │   │   ├── risk_map.png         (visual risk heatmap)      │
│      │   │   ├── flood_alert.txt      (AI public warning)        │
│      │   │   └── impact_report.txt    (AI technical report)      │
│      │   └── session_180245/                                     │
│      │       └── ...                                             │
│      └── 2026-03-06/                                             │
│          └── ...                                                 │
└──────────────────────────────────────────────────────────────────┘
```

**Design Principles:**
- **Immutable runs** — each session directory is write-once
- **Timestamped paths** — `YYYY-MM-DD/session_HHMMSS/` enables chronological browsing
- **Self-contained** — every run directory contains everything needed for full replay
- **Metadata-indexed** — `metadata.json` in every run enables search and filtering
- **Cost-efficient** — blob tiers can be configured (Hot → Cool → Archive) as runs age

---

## 🤖 AI Components

### Component 1 — Flood Alert Generator

| Property | Detail |
|---|---|
| **API** | Groq LLM (e.g., `llama3-70b-8192`) |
| **Input** | Total rainfall (mm), max flood depth (m), risk cell counts |
| **Output** | Human-readable public flood alert |
| **File** | `flood_alert.txt` |
| **Tone** | Clear, non-technical, emergency-ready language |

**Prompt Pattern:**
```
You are a flood warning system.
Given:
- Total Rainfall: {rainfall_mm} mm
- Maximum Flood Depth: {max_depth} m
- Low Risk Cells: {low}, Medium Risk: {medium}, High Risk: {high}

Generate a concise, clear, public-facing flood alert.
Specify risk level, expected water depth, and evacuation guidance.
```

---

### Component 2 — Impact Report Generator

| Property | Detail |
|---|---|
| **API** | Groq LLM |
| **Input** | Full simulation statistics + risk classification |
| **Output** | Technical impact assessment |
| **File** | `impact_report.txt` |
| **Constraint** | `DO NOT fabricate population or economic loss numbers` |

**Prompt Pattern:**
```
You are a civil engineering flood analyst.
Given simulation data:
- [rainfall, flood depth distribution, risk cell counts]

Generate a technical impact report covering:
1. Flood severity distribution
2. Infrastructure stress analysis
3. Mitigation recommendations

IMPORTANT: Do not fabricate population figures or economic loss estimates.
Base all analysis solely on the provided simulation data.
```

---

## ⚙️ Simulation Engine

The C++ simulation engine is the computational heart of FloodForge.

### Algorithm Overview

```
Input:   demGrid[R][C]    (elevation in meters)
         rainfall_mm      (total rainfall as scalar)

Process:
  1. For each cell (i,j):
       water_input[i][j] = rainfall_mm (uniform distribution)

  2. Iterative accumulation:
       For each cell, compute relative depth vs neighbors
       Water flows from higher to lower elevation cells
       Accumulation continues until equilibrium

  3. Output:
       floodGrid[i][j] = net flood depth (meters)
```

### Performance Characteristics

| Parameter | Value |
|---|---|
| Language | C++17 |
| Grid sizes tested | Up to 500×500 cells |
| Typical runtime | < 2 seconds (100×100 grid) |
| Memory footprint | O(R × C) |

---

## 📊 Data Pipeline

```
┌─────────────┐    ┌─────────────┐    ┌──────────────────┐
│ OpenTopo    │    │ Meteostat   │    │ User Input       │
│ GeoTIFF DEM │    │ Rainfall    │    │ Bounding Box     │
│ (raw raster)│    │ (JSON/CSV)  │    │ Date Range       │
└──────┬──────┘    └──────┬──────┘    └────────┬─────────┘
       │                  │                    │
       ▼                  ▼                    ▼
┌──────────────────────────────────────────────────────┐
│                  GDAL / C++ Processing               │
│  GeoTIFF → demGrid[R][C]   Rainfall → total_mm       │
└──────────────────────────────┬───────────────────────┘
                               │
                               ▼
                   ┌───────────────────────┐
                   │  C++ Simulation Core  │
                   │  floodGrid[R][C]      │
                   └───────────┬───────────┘
                               │
              ┌────────────────┼──────────────────┐
              ▼                ▼                  ▼
     ┌───────────────┐ ┌────────────────┐ ┌───────────────┐
     │  dem.csv      │ │flood_depth.csv │ │ risk_map.png  │
     └───────────────┘ └────────────────┘ └───────────────┘
              │
              ▼
     ┌───────────────────────────────────────────┐
     │  Groq LLM API                             │
     │  flood_alert.txt  +  impact_report.txt    │
     └───────────────────────────────────────────┘
              │
              ▼
     ┌───────────────────────────────────────────┐
     │  Azure Blob Storage                       │
     │  All 6 artifacts → timestamped run path   │
     └───────────────────────────────────────────┘
```

---

## 🔭 Future Work

| Priority | Feature | Description |
|---|---|---|
| 🔴 High | **Real-time stream gauge integration** | Incorporate live river level sensors into simulation inputs |
| 🔴 High | **ML flood prediction model** | Replace physics-based engine with trained neural network for faster inference |
| 🟡 Medium | **Multi-source DEM fusion** | Combine OpenTopography + satellite SAR data for higher resolution |
| 🟡 Medium | **Temporal simulation** | Model flood progression over time (hourly snapshots) |
| 🟡 Medium | **Web dashboard** | React/Next.js frontend for cloud-hosted simulation results |
| 🟢 Low | **Mobile alert push** | Push flood_alert.txt to registered mobile numbers via Twilio |
| 🟢 Low | **GIS export** | Export risk maps as shapefiles or GeoJSON for GIS platforms |
| 🟢 Low | **Multi-language alerts** | LLM prompting for multilingual emergency communication |
| 🟢 Low | **Simulation API** | REST API exposing simulation engine for third-party integrations |

---

## 🏆 Credits

### Core Team

| Name | Role |
|---|---|
| **[Your Name]** | Lead Developer — Simulation Engine, Qt Application, Azure Integration |
| **[Team Member 2]** | AI / LLM Integration — Groq API, Prompt Engineering |
| **[Team Member 3]** | GIS / Data Pipeline — GDAL, OpenTopography, Meteostat |

### APIs & Services

| Service | Usage |
|---|---|
| [OpenTopography](https://opentopography.org/) | Digital Elevation Model (DEM) data |
| [Meteostat via RapidAPI](https://rapidapi.com/meteostat/api/meteostat) | Historical rainfall data |
| [Groq](https://groq.com/) | LLM inference for alert and report generation |
| [Microsoft Azure Blob Storage](https://azure.microsoft.com/en-us/products/storage/blobs) | Cloud artifact storage |

### Open Source Dependencies

| Library | License | Purpose |
|---|---|---|
| [GDAL](https://gdal.org/) | MIT/X | GeoTIFF and raster processing |
| [Qt 6](https://www.qt.io/) | LGPL v3 | Desktop application framework |
| [OpenGL](https://www.opengl.org/) | — | 3D terrain rendering |

---

<div align="center">

---

**Built with 💧 to protect communities from flood disasters.**

_FloodForge — From terrain data to life-saving intelligence in minutes._

---

[![GitHub Stars](https://img.shields.io/github/stars/YOUR_USERNAME/FloodForge?style=social)](https://github.com/YOUR_USERNAME/FloodForge)
[![GitHub Forks](https://img.shields.io/github/forks/YOUR_USERNAME/FloodForge?style=social)](https://github.com/YOUR_USERNAME/FloodForge/fork)

</div>
