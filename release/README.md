# Evolution 1.0

**Autor:** Pedro Bernabe Moreno Maura
**Basado en:** Stockfish 19
**Licencia:** GPLv3

---

## Que es Evolution?

Evolution es un motor de ajedrez derivado de Stockfish 19 que anade
tres estilos de juego configurables sin perder fuerza respecto al
motor original. Cada estilo tiene una personalidad propia, calibrada
con cientos de partidas de validacion.

Los estilos son:

- **Killer** - Agresivo, tactico, con tropismo al rey enemigo.
- **Balanced** - Equilibrado, con personalidad sutil propia (default).
- **Positional** - Estrategico, posicional-dinamico, no materialista.
- **Pure** - Sin estilo. Comportamiento identico a Stockfish 19 original.

---

## Instalacion

1. Copia Evolution.exe y nn-1a298aa575a0.nnue en la misma carpeta.
2. Abrelo desde tu GUI de ajedrez (Arena, CuteChess, Fritz, etc.) como motor UCI.
3. Listo. La red neuronal se carga automaticamente.

---

## Uso

### Seleccion de estilo

    setoption name Style value Killer
    setoption name Style value Balanced
    setoption name Style value Positional
    setoption name Style value Pure

### Escala tactica de Killer

La opcion TacticalScale modula la agresividad del estilo Killer.
Aparece inmediatamente debajo de Style en el listado UCI para
reflejar su asociacion. NO tiene efecto en otros estilos.

    setoption name TacticalScale value 1   -> Killer light
    setoption name TacticalScale value 2   -> Killer medio (default)
    setoption name TacticalScale value 3   -> Killer berserker

---

## Rendimiento validado

Cada estilo fue validado con cientos de partidas contra Stockfish 19
puro (Style=Pure) usando el mismo binario y tiempo controlado:

| Estilo      | Partidas | Elo vs Pure |
|-------------|----------|-------------|
| Killer      | 400      | +4.34       |
| Balanced    | 200      | -1.74       |
| Positional  | 200      | +6.95       |

Conclusion: los tres estilos estan a menos de 7 Elo de diferencia
respecto a Stockfish 19 original. Ninguno pierde fuerza significativa.

Ver STATS.md para el analisis completo.

---

## Novedades respecto a Stockfish 19

- Sistema de estilos configurable (Style).
- Escala tactica ajustable (TacticalScale, solo para Killer).
- Reporte UCI consistente: el delta de estilo se aplica internamente
  durante la busqueda pero no se filtra al reporte externo.
- Fix de consistencia de mates: no se aplica delta a scores de mate.
- Arquitectura limpia: codigo de estilo aislado en style.h y style.cpp.

---

## Creditos

Evolution 1.0 es obra de Pedro Bernabe Moreno Maura.

Esta basado en Stockfish 19, cuyo equipo original de desarrolladores
mantiene el motor de referencia mundial. Ver AUTHORS para la lista
completa de contribuidores originales.

Este proyecto se distribuye bajo la licencia GPLv3 (ver COPYING).

---

"El estilo debe aconsejar a la NNUE, no competir con ella."
