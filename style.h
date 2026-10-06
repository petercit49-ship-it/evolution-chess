/*
  Evolution, a UCI chess playing engine derived from Stockfish.
  Copyright (C) 2026 Pedro Bernabe Moreno Maura

  Evolution is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Evolution is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED

#include <string>

namespace Stockfish {

class Position;

namespace Style {

// Los tres estilos de Evolution.
enum class StyleType {
    Killer,      // Tactico, agresivo, tropismo hacia el rey enemigo
    Balanced,    // Equilibrado (default)
    Positional,  // Estrategico, estructura, control posicional
};


// Los tres modos del estilo Positional.
enum class PositionalMode {
    Prophylactic,  // Previene las ideas del rival
    Dynamic,       // Acumula ventajas y actividad (default)

    Constrictor,   // Presiona y restringe al rival
};
// Devuelve el estilo actualmente seleccionado (leido de la opcion UCI "Style").
StyleType current_style();

// Actualiza el estilo a partir del string de la opcion UCI.
void set_style_from_string(const std::string& s);

// Ajusta la escala tactica de Killer (1=suave, 2=medio, 3=agresivo).
void set_killer_scale(int s);

// Ajusta el modo del estilo Positional (Prophylactic, Dynamic, Constrictor).
void set_positional_mode(const std::string& s);

// Calcula el delta de estilo que se sumara al score de la NNUE.
// Devuelve centipeones (cp). Positivo = bueno para el lado a mover.
int style_delta(const Position& pos);

}  // namespace Style

}  // namespace Stockfish

#endif  // #ifndef STYLE_H_INCLUDED
