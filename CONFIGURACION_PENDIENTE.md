# THE THIEF - CONFIGURACIÓN EN 45 MINUTOS

## ✅ YA ESTÁ HECHO (C++)
- Main Menu integrado en el mismo nivel con cámara separada
- HUD con barra de progreso y iconos de visibilidad
- Pause Menu, Game End (Win/Lose)
- Sistema de estados del juego
- Al reiniciar salta el menú, va directo al juego

---

## 🚀 PASOS RÁPIDOS (45 min)

### PASO 1: Compilar (2 min)
1. Visual Studio → Ctrl+Shift+B
2. Esperar a que compile sin errores

---

### PASO 2: Crear BP_ThiefGameMode (3 min)
1. Content Browser → Click derecho → Blueprint Class
2. Buscar `ThiefGameMode` → Select
3. Nombrar: `BP_ThiefGameMode`
4. **NO LO CONFIGURES AÚN** - lo haremos al final

---

### PASO 3: Crear WBP_ThiefHUD (5 min)
1. Click derecho → User Interface → Widget Blueprint → `WBP_ThiefHUD`
2. Abrir → Class Settings → Parent Class: `ThiefHUDWidget`
3. Diseño:
```
Canvas Panel
├── [Abajo-Izq] Horizontal Box
│   ├── Image (icono moneda)
│   └── ProgressBar [Nombre: CoinsProgressBar]
├── [Arriba-Der] Image [Nombre: HiddenIcon] ← Ojo cerrado/tachado
└── [Arriba-Der] Image [Nombre: SeenIcon] ← Ojo abierto (mismo lugar)
```
4. **IMPORTANTE:** Los dos iconos van en el mismo lugar, uno se oculta y otro se muestra
5. Guardar

---

### PASO 4: Crear WBP_MainMenu (5 min)
1. Widget Blueprint → `WBP_MainMenu`
2. Parent Class: `MainMenuWidget`
3. Diseño:
```
Canvas Panel
├── BackgroundBlur (Blur: 5) ← Opcional, queda bien
└── Vertical Box (centrado)
    ├── Text "THE THIEF" (título grande)
    ├── Button [PlayButton] → "PLAY"
    ├── Button [ControlsButton] → "CONTROLS"
    ├── Button [HowToPlayButton] → "HOW TO PLAY"
    └── Button [QuitButton] → "QUIT"
```
4. Guardar

---

### PASO 5: Crear WBP_PauseMenu (5 min)
1. Widget Blueprint → `WBP_PauseMenu`
2. Parent Class: `PauseMenuWidget`
3. Diseño:
```
Canvas Panel
├── BackgroundBlur (Blur: 10)
└── Vertical Box (centrado)
    ├── Text "PAUSED"
    ├── Button [ResumeButton] → "RESUME"
    ├── Button [RestartButton] → "RESTART"
    ├── Button [ControlsButton] → "CONTROLS"
    ├── Button [HowToPlayButton] → "HOW TO PLAY"
    ├── Button [MainMenuButton] → "MAIN MENU"
    └── Button [QuitButton] → "QUIT"
```
4. Guardar

---

### PASO 6: Crear WBP_GameEnd (5 min)
1. Widget Blueprint → `WBP_GameEnd`
2. Parent Class: `GameEndWidget`
3. Diseño:
```
Canvas Panel
├── BackgroundBlur (Blur: 10)
└── Vertical Box (centrado)
    ├── Text [TitleText] ← Se cambia a "VICTORY!" o "CAPTURED!"
    ├── Text [SubtitleText] ← Mensaje secundario
    ├── Button [RestartButton] → "RESTART"
    └── Button [MainMenuButton] → "MAIN MENU"
```
4. Guardar

---

### PASO 7: Crear WBP_Controls (3 min)
1. Widget Blueprint → `WBP_Controls`
2. Parent Class: `ControlsWidget`
3. Diseño:
```
Canvas Panel
└── Border (fondo oscuro semi-transparente)
    └── Vertical Box
        ├── Text "CONTROLS"
        ├── Text "WASD - Move"
        ├── Text "Mouse - Look"
        ├── Text "Space - Jump"
        ├── Text "Shift - Sprint"
        ├── Text "E - Steal"
        ├── Text "Click - Attack"
        ├── Text "Esc - Pause"
        └── Button [BackButton] → "BACK"
```
4. Guardar

---

### PASO 8: Crear WBP_HowToPlay (3 min)
1. Widget Blueprint → `WBP_HowToPlay`
2. Parent Class: `HowToPlayWidget`
3. Diseño:
```
Canvas Panel
└── Border (fondo oscuro)
    └── Vertical Box
        ├── Text "HOW TO PLAY"
        ├── Text "Steal coins from villagers"
        ├── Text "Avoid guards"
        ├── Text "Collect all coins to win"
        └── Button [BackButton] → "BACK"
```
4. Guardar

---

### PASO 9: Configurar Submenús (2 min)
1. Abrir `WBP_MainMenu` → Class Defaults:
   - ControlsWidgetClass: `WBP_Controls`
   - HowToPlayWidgetClass: `WBP_HowToPlay`

2. Abrir `WBP_PauseMenu` → Class Defaults:
   - ControlsWidgetClass: `WBP_Controls`
   - HowToPlayWidgetClass: `WBP_HowToPlay`

---

### PASO 10: Añadir Cámara del Menú (3 min)
1. En tu nivel, añadir un `CameraActor`
2. Posicionarlo mirando a algo bonito (plaza, castillo, etc.)
3. Seleccionar la cámara → Details → Tags → Add → escribir: `MenuCamera`
4. Guardar nivel

---

### PASO 11: Configurar BP_ThiefGameMode (5 min)
1. Abrir `BP_ThiefGameMode`
2. En Class Defaults:

**Game:**
- ✅ bStartWithMainMenu = true
- MenuCameraTag = `MenuCamera`

**UI:**
- MainMenuWidgetClass: `WBP_MainMenu`
- HUDWidgetClass: `WBP_ThiefHUD`
- PauseMenuWidgetClass: `WBP_PauseMenu`
- GameEndWidgetClass: `WBP_GameEnd`

3. Guardar

---

### PASO 12: Asignar GameMode al Nivel (2 min)
1. Abrir tu nivel
2. Window → World Settings
3. GameMode Override: `BP_ThiefGameMode`
4. Guardar

---

### PASO 13: Configurar Project Settings (2 min)
1. Edit → Project Settings → Maps & Modes
2. Game Instance Class: `ThiefGameInstance`
3. Cerrar

---

## ✅ ¡LISTO! Prueba el juego

**Flujo esperado:**
1. Inicias → Ves cámara del menú + Main Menu
2. Click PLAY → Transición suave a cámara del jugador
3. Juegas → ESC para pausar
4. Ganas/Pierdes → Pantalla de fin
5. RESTART → Salta menú, directo al juego
6. MAIN MENU → Vuelve al menú

---

## 📝 NOMBRES DE BOTONES (DEBEN SER EXACTOS)

| Widget | Botones |
|--------|---------|
| MainMenu | `PlayButton`, `ControlsButton`, `HowToPlayButton`, `QuitButton` |
| PauseMenu | `ResumeButton`, `RestartButton`, `ControlsButton`, `HowToPlayButton`, `MainMenuButton`, `QuitButton` |
| GameEnd | `RestartButton`, `MainMenuButton` |
| Controls | `BackButton` |
| HowToPlay | `BackButton` |
| HUD | `CoinsProgressBar`, `HiddenIcon`, `SeenIcon` |
| GameEnd | `TitleText`, `SubtitleText` |

---

## 🎨 ICONOS NECESARIOS

Solo necesitas 2 imágenes para el HUD:
1. **Ojo cerrado/tachado** - Para cuando estás oculto
2. **Ojo abierto** - Para cuando te ven

Ponlos en el mismo lugar en el widget. El código oculta uno y muestra el otro automáticamente.

---

## 🐛 SI ALGO NO FUNCIONA

| Problema | Solución |
|----------|----------|
| Botones no hacen nada | Verifica nombres EXACTOS |
| No aparece Main Menu | Verifica GameMode en World Settings |
| Cámara no cambia | Verifica tag `MenuCamera` en la cámara |
| Iconos no cambian | Verifica nombres `HiddenIcon` y `SeenIcon` |

