# MRTD_Automatisation_GPIB

Banc de test automatique pour mesurer la MRTD d’une caméra thermique
en pilotant un corps noir via GPIB et une UI GTK4 sur Raspberry Pi 4.

## Fonctionnalités

- Contrôle du corps noir via GPIB (GPIB-USB-HS NI, linux-gpib).
- Interface graphique GTK4 (écran tactile 5").
- Acquisition et traitement d’image avec OpenCV (calcul MRTD).
- Mode développement sur PC + exécution sur Raspberry Pi.

## Prérequis

- Raspberry Pi 4 (4 Go RAM), Raspberry Pi OS 64 bits.
- Adaptateur NI GPIB-USB-HS.
- Paquets :
  - `gtk4`, `gtk4-devel` ou équivalent.
  - `linux-gpib`, `linux-gpib-dev`.
  - `cmake`, `gcc`, `make`.
  - OpenCV (version X.Y).

## Installation

```bash
git clone https://github.com/<ton-user>/UI_Rasberry.git
cd UI_Rasberry
mkdir build && cd build
cmake ..
make -j4
