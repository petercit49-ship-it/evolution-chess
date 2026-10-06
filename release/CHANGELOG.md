# Changelog - Evolution

Todas las novedades notables de Evolution se documentan en este archivo.

---

## [1.0] - Octubre 2026

Primera version publica de Evolution.

### Anadido

- Sistema de estilos configurable (opcion UCI Style):
  - Killer: estilo agresivo con tropismo al rey enemigo.
  - Balanced: estilo equilibrado con personalidad sutil (default).
  - Positional: estilo estrategico, posicional-dinamico, no materialista.

- Escala tactica ajustable (opcion UCI KillerScale, 1-3):
  - Solo se aplica al estilo Killer.
  - 1 = Killer light, 2 = Killer medio (default), 3 = Killer berserker.
- Modo posicional configurable (opcion UCI PositionalMode, solo para Positional):
  - Dynamic: modo equilibrado con personalidad propia (default).
  - Prophylactic: modo preventivo y solido, neutraliza las ideas del rival.
  - Constrictor: modo de presion sostenida y restriccion progresiva.

- Componentes de estilo nuevos en style.cpp:
  - Killer: tropismo al rey, ataques al king ring, castigo por pasividad.
  - Positional: estructura de peones, caballos vs alfiles,
    simplificacion selectiva con regla de rey expuesto, dinamismo
    (movilidad), dominancia de casillas.
  - Balanced: centro extendido, coordinacion de menores,
    peones pasados conectados, rey activo en finales,
    piezas colgadas.

### Cambiado

- Banner UCI personalizado:
  - id name Evolution 1.0
  - id author Pedro Bernabe Moreno Maura (based on Stockfish)

- Reporte UCI consistente: el delta de estilo se aplica internamente
  durante la busqueda, pero no se filtra al reporte externo. La GUI
  recibe siempre la evaluacion real de la red neuronal.

### Corregido

- Consistencia de mates: el delta de estilo no se aplica cuando el
  score es de mate. Elimina las advertencias de "sign mismatch in
  mate scores" y garantiza reportes de mate coherentes.

- Calibracion de Killer: pesos reducidos ~25% y tropismo solo positivo
  para eliminar la perdida de fuerza. Paso de +14 Elo de perdida a
  +4 Elo (estadisticamente indistinguible de Stockfish puro).

- Calibracion de Positional: se definieron 3 modos con pesos propios.
  Dynamic: clamp +/-22, simplificacion x0.5, dinamismo x1.4.
  Prophylactic: clamp +/-15, estructura x1.2, dinamismo x0.75.
  Constrictor: clamp +/-22, estructura x0.75, menores x1.25,
  dominancia x1.5. Cada modo con su propia funcion independiente.

### Validacion

- Killer: 400 partidas contra Stockfish 19 puro. Resultado: +4.34 Elo.
- Balanced: 200 partidas contra Stockfish 19 puro. Resultado: -1.74 Elo.
- Positional - Dynamic: 200 partidas. Resultado: +12.17 Elo.
- Positional - Prophylactic: 100 partidas. Resultado: +13.90 Elo.
- Positional - Constrictor: 100 partidas. Resultado: 0.00 Elo.
- Test tactico WAC (300 posiciones): realizado con version pre-calibracion.

Todos los estilos y modos estan a menos de 14 Elo de Stockfish 19 original.
Ver STATS.md para el analisis completo.

### Arquitectura

- style.h: declaraciones del sistema de estilos.
- style.cpp: implementacion de los 3 estilos de Evolution.
- search.cpp: inyeccion del delta de estilo y fix de reporte UCI.
- engine.cpp: registro de opciones UCI Style, KillerScale y PositionalMode.
- misc.cpp: banner personalizado.
- Makefile: inclusion de style.cpp en la compilacion.

---

## Base

Evolution 1.0 esta basado en Stockfish 19.

Creditos originales: ver AUTHORS.
Licencia: GPLv3 (ver COPYING).
