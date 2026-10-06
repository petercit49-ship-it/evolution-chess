# Evolution 1.0 — Estadísticas Oficiales

Motor: **Evolution 1.0**
Autor: **Pedro Bernabé Moreno Maura**
Basado en: Stockfish 19
Fecha de inicio de desarrollo: Octubre 2026

---

## Configuración del motor

### Opciones UCI propias de Evolution

| Opción | Tipo | Rango | Default | Descripción |
|--------|------|-------|---------|-------------|
| `Style` | string | Killer, Balanced, Positional | Balanced | Selecciona el estilo de juego |
| `TacticalScale` | spin | 1-3 | 2 | Escala de agresividad de Killer |

### Significado de cada estilo

- **Killer**: Estilo agresivo con tropismo al rey enemigo.
- **Balanced**: Estilo equilibrado con personalidad sutil propia (default).
- **Positional**: Estilo estrategico, posicional-dinamico, no materialista.

### Significado de cada escala táctica

- **1 (Light)**: Killer suave, toque agresivo discreto.
- **2 (Medium)**: Killer equilibrado (default).
- **3 (Berserker)**: Killer agresivo máximo.

---

## Validación de fuerza — Matches vs Stockfish 19 original

Formato: 5+0.05 seg, 1 thread, 16 MB hash, libro EPD de 20 aperturas.

### Killer vs Stockfish original

| Partidas | Elo (Stockfish vs Killer) | Margen | Notas |
|----------|---------------------------|--------|-------|
| 20 | -17.39 | ±58.68 | Muestra inicial |
| 100 (con fix de mates) | -3.47 | ±28.12 | Primera señal sólida |
| **400 (Killer suave, calibrado)** | **+4.34** | **±10.62** | **Validación definitiva** |

**Conclusión:** Killer está a ~4 Elo de Stockfish 19 original. Estadísticamente indistinguible.

### Balanced vs Stockfish original

| Partidas | Elo | Margen | Notas |
|----------|-----|--------|-------|
| 20 | 0.00 | ±0.00 | 50% exacto |
| **200** | **-1.74** | **±17.70** | **Validación final** |

**Conclusión:** Balanced es igual de fuerte que Stockfish 19 original. La personalidad no cuesta nada.

### Positional vs Stockfish original

| Partidas | Elo | Margen | Notas |
|----------|-----|--------|-------|
| 20 | +17.39 | ±32.48 | Muestra inicial |
| **200** | **+6.95** | **±14.40** | **Validación final** |

**Conclusión:** Positional está a ~7 Elo de Stockfish 19 original. Estadísticamente indistinguible.

---

## Test táctico — Suite WAC (300 posiciones, depth 15)

Mide precisión táctica pura. Cuenta cuántas posiciones acierta el motor.

*Nota: estos tests se realizaron antes de la calibración final de los estilos. Los números son indicativos, no definitivos.*

| Estilo | Escala | Aciertos | Precisión | Dif. vs Stockfish |
|--------|--------|----------|-----------|-------------------|
| **Stockfish original** | — | **236/300** | **78.7%** | — |
| **Killer** | **1** | **234/300** | **78.0%** | **-0.7%** |
| **Killer** | **2** | **236/300** | **78.7%** | **0.0%** |
| **Killer** | **3** | **234/300** | **78.0%** | **-0.7%** |

**Interpretación:**

- Killer-1 mantiene la precisión táctica casi idéntica a Stockfish original.
- **Killer-2 iguala EXACTAMENTE a Stockfish original (236/300)** aunque juega con estilo agresivo.
- Este resultado sugiere que **la agresividad bien calibrada no daña la precisión táctica**.

### Observación sobre posiciones tempranas

En las primeras 100 posiciones del WAC (las más accesibles), Killer-2 **superó ligeramente** a Stockfish original:

| Posición | Stockfish | Killer-2 |
|----------|-----------|----------|
| 20 | 85.0% | **90.0%** |
| 40 | 87.5% | **87.5%** |
| 60 | 85.0% | **86.7%** |
| 80 | 87.5% | **88.8%** |

Esto sugiere que en posiciones tácticas claras, la agresividad puede dar un **empujón** para encontrar la solución antes. Hipótesis a validar en tests futuros.

---

## Rendimiento esperado por control de tiempo (hipótesis)

Basado en la naturaleza del estilo agresivo, se espera que Killer rinda **mejor** en controles de tiempo rápidos:

| Control | Predicción | Razón |
|---------|-----------|-------|
| Clásico (10+0.1) | Killer ≈ Stockfish | La NNUE domina la evaluación |
| **Blitz (5+0.05)** | **Killer ≈ Stockfish** ✅ validado | Validado con 400 partidas |
| Blitz rápido (3+0.02) | Killer > Stockfish (+10 a +25 Elo) | Menos profundidad, más peso del estilo |
| Bullet (1+0.01) | Killer >> Stockfish (+30 a +50 Elo) | Presión de tiempo sobre el rival |

**Trabajo futuro:** correr matches a 3+0.02 y 1+0.01 para validar esta hipótesis.

---

## 🛠️ Mejoras importantes aplicadas

1. **Reporte UCI consistente**: el delta de estilo se aplica internamente durante la búsqueda, pero no se filtra al reporte externo. La GUI recibe siempre la evaluación real de la red neuronal.
2. **Mates consistentes**: no se aplica delta de estilo cuando el score es de mate. Elimina inconsistencias en los reportes de mate.
3. **Calibración de Killer**: pesos reducidos ~25% y tropismo solo positivo. Paso de +14 Elo de pérdida a +4 Elo.
4. **Calibración de Positional**: clamp ajustado de ±50 a ±25. Paso de +28 Elo de pérdida a +7 Elo.

---

## Arquitectura

Archivos nuevos creados por el autor:

- `style.h` — declaraciones del sistema de estilos
- `style.cpp` — implementación de los 3 estilos de Evolution

Archivos de Stockfish modificados:

- `search.cpp` — inyección del delta de estilo + fix de reporte UCI
- `engine.cpp` — registro de opciones UCI `Style` y `TacticalScale`
- `misc.cpp` — banner personalizado
- `Makefile` — inclusión de `style.cpp` en la compilación

---

## Historial de desarrollo

| Fecha | Hito |
|-------|------|
| Oct 2026 | Compilación base de Stockfish 19 |
| Oct 2026 | Renombrado a Evolution 1.0 |
| Oct 2026 | Implementación de sistema de estilos |
| Oct 2026 | Killer + Positional + Balanced iniciales |
| Oct 2026 | Fix de consistencia en el reporte UCI |
| Oct 2026 | Fix de mates inconsistentes |
| Oct 2026 | Calibración de Killer suave |
| Oct 2026 | Implementación de TacticalScale |
| Oct 2026 | Validación con 400 partidas |
| Oct 2026 | Test táctico WAC (Stockfish, K-1, K-2, K-3) |
| Oct 2026 | Recalibración de Positional (dinamismo + dominancia de casillas) |
| Oct 2026 | Ajuste de clamp de Positional (±50 → ±25) |
| Oct 2026 | Validación final: Killer 400, Balanced 200, Positional 200 |
| Oct 2026 | Cierre de la versión 1.0 |

---

*Documento finalizado. Versión 1.0 cerrada.*