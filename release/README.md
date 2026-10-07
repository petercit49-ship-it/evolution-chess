# Evolution 1.0

**Autor:** Pedro Bernabé Moreno Maura
**Basado en:** Stockfish 19
**Licencia:** GPLv3

---

## ¿Qué es Evolution?

Evolution es un motor de ajedrez derivado de Stockfish 19 que añade
tres estilos de juego configurables sin perder fuerza respecto al
motor original. Cada estilo tiene una personalidad propia, calibrada
con cientos de partidas de validación.

Los estilos son:

- **Killer** - Agresivo, táctico, con tropismo al rey enemigo.
- **Balanced** - Equilibrado, con personalidad sutil propia (default).
- **Positional** - Estratégico, posicional-dinámico, no materialista.
  - Modo **Dynamic** - Equilibrado con personalidad propia (default).
  - Modo **Prophylactic** - Preventivo y sólido, neutraliza al rival.
  - Modo **Constrictor** - Presión sostenida y restricción progresiva.

---

## ¿Qué versión descargar?

Evolution se distribuye en 5 compilaciones, cada una optimizada para
un tipo de CPU distinto. Elige la que mejor se ajuste a tu procesador:

| CPU | Compilación a usar |
|-----|--------------------|
| Intel 2017+ / AMD Zen 4+ | Evolution 1.0-avx512.exe |
| Intel 2013+ / AMD Zen+ | Evolution 1.0-bmi2.exe |
| Intel 2013+ / AMD 2015+ | Evolution 1.0-avx2.exe |
| Intel 2008+ / AMD 2011+ | Evolution 1.0-sse41-popcnt.exe |
| Muy antigua o desconocida | Evolution 1.0-x86-64.exe |

**Si no estás seguro, usa Evolution 1.0-avx2.exe.** Funciona en la
gran mayoría de PCs modernos (Intel desde 2013, AMD desde 2015).

**Importante:** siempre debe estar el archivo nn-1a298aa575a0.nnue
en la misma carpeta que el .exe. Sin él, el motor no arranca.

---

## Instalación

1. Copia el .exe que hayas elegido (según tu CPU) y el archivo
   nn-1a298aa575a0.nnue en la misma carpeta.
2. Ábrelo desde tu GUI de ajedrez (Arena, CuteChess, Fritz, ChessBase,
   BanksiaGUI, etc.) como motor UCI.
3. Listo. La red neuronal se carga automáticamente.

---

## Uso

### Selección de estilo

setoption name Style value Killer
setoption name Style value Balanced
setoption name Style value Positional

### Escala táctica de Killer

La opción KillerScale modula la agresividad del estilo Killer.
Aparece inmediatamente debajo de Style en el listado UCI para
reflejar su asociación. NO tiene efecto en otros estilos.

setoption name KillerScale value 1   -> Killer light
setoption name KillerScale value 2   -> Killer medio (default)
setoption name KillerScale value 3   -> Killer berserker

### Modo del estilo Positional

La opción PositionalMode elige entre los 3 modos del estilo
Positional. Aparece inmediatamente debajo de KillerScale en el
listado UCI. NO tiene efecto en otros estilos.

setoption name PositionalMode value Dynamic       -> Modo default
setoption name PositionalMode value Prophylactic  -> Modo profiláctico
setoption name PositionalMode value Constrictor   -> Modo constrictor

---

## Rendimiento validado

Cada estilo fue validado con cientos de partidas contra Stockfish 19
original, usando el mismo binario y tiempo controlado:

| Estilo y modo              | Partidas | Elo vs Stockfish |
|----------------------------|----------|-------------------|
| Killer                     | 400      | +4.34             |
| Balanced                   | 200      | -1.74             |
| Positional - Dynamic       | 200      | +12.17            |
| Positional - Prophylactic  | 100      | +13.90            |
| Positional - Constrictor   | 100      | 0.00              |

**Conclusión:** todos los estilos y modos están a menos de 14 Elo de
diferencia respecto a Stockfish 19 original. Ninguno pierde fuerza
significativa.

Ver STATS.md para el análisis completo.

---

## Novedades respecto a Stockfish 19

- Sistema de estilos configurable (Style).
- Escala táctica ajustable (KillerScale, solo para Killer).
- Modo posicional configurable (PositionalMode, solo para Positional).
- Reporte UCI consistente: el delta de estilo se aplica internamente
  durante la búsqueda pero no se filtra al reporte externo.
- Fix de consistencia de mates: no se aplica delta a scores de mate.
- Arquitectura limpia: código de estilo aislado en style.h y style.cpp.

---

## Créditos

Evolution 1.0 es obra de Pedro Bernabé Moreno Maura.

Está basado en Stockfish 19, cuyo equipo original de desarrolladores
mantiene el motor de referencia mundial. Ver AUTHORS para la lista
completa de contribuidores originales.

Este proyecto se distribuye bajo la licencia GPLv3 (ver COPYING).

---

"El estilo debe aconsejar a la NNUE, no competir con ella."
