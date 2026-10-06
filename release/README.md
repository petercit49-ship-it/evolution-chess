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

### Escala tactica de Killer

La opcion KillerScale modula la agresividad del estilo Killer.
Aparece inmediatamente debajo de Style en el listado UCI para
reflejar su asociacion. NO tiene efecto en otros estilos.

    setoption name KillerScale value 1   -> Killer light
    setoption name KillerScale value 2   -> Killer medio (default)
    setoption name KillerScale value 3   -> Killer berserker
### Modo del estilo Positional

La opcion PositionalMode elige entre los 3 modos del estilo Positional.
Aparece inmediatamente debajo de KillerScale en el listado UCI.
NO tiene efecto en otros estilos.

    setoption name PositionalMode value Dynamic       -> Modo default
    setoption name PositionalMode value Prophylactic  -> Modo profilactico
    setoption name PositionalMode value Constrictor   -> Modo constrictor

---

## Rendimiento validado

Cada estilo fue validado con cientos de partidas
contra Stockfish 19 original, usando el mismo binario y tiempo controlado:

| Estilo      | Partidas | Elo vs Stockfish |
|-------------|----------|-------------|
| Killer      | 400      | +4.34       |
| Balanced    | 200      | -1.74       |
| Positional - Dynamic       | 200      | +12.17      |
| Positional - Prophylactic  | 100      | +13.90      |
| Positional - Constrictor   | 100      | 0.00        |

Conclusion: todos los estilos y modos estan a menos de 14 Elo de diferencia
respecto a Stockfish 19 original. Ninguno pierde fuerza significativa.

Ver STATS.md para el analisis completo.

---

## Novedades respecto a Stockfish 19

- Sistema de estilos configurable (Style).
- Escala tactica ajustable (KillerScale, solo para Killer).
- Modo posicional configurable (PositionalMode, solo para Positional).
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
