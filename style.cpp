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

#include "style.h"

#include <algorithm>
#include <cstdlib>
#include <string>

#include "attacks.h"
#include "bitboard.h"
#include "position.h"
#include "types.h"

namespace Stockfish {
namespace Style {

static StyleType g_current_style = StyleType::Balanced;
static int        g_tactical_scale = 2;

StyleType current_style() { return g_current_style; }

void set_style_from_string(const std::string& s) {
    if (s == "Killer")
        g_current_style = StyleType::Killer;
    else if (s == "Positional")
        g_current_style = StyleType::Positional;
    else if (s == "Pure")
        g_current_style = StyleType::Pure;
    else
        g_current_style = StyleType::Balanced;
}

void set_tactical_scale(int s) { g_tactical_scale = std::clamp(s, 1, 3); }
// =============================================================
// Utilidades comunes
// =============================================================

static inline int chebyshev(Square a, Square b) {
    const int df = std::abs(int(file_of(a)) - int(file_of(b)));
    const int dr = std::abs(int(rank_of(a)) - int(rank_of(b)));
    return std::max(df, dr);
}

static Bitboard king_ring(Square ksq) {
    const int kf = int(file_of(ksq));
    const int kr = int(rank_of(ksq));
    Bitboard  ring = 0;
    for (int df = -1; df <= 1; ++df)
        for (int dr = -1; dr <= 1; ++dr) {
            if (df == 0 && dr == 0)
                continue;
            const int f = kf + df;
            const int r = kr + dr;
            if (f >= 0 && f < 8 && r >= 0 && r < 8)
                ring |= square_bb(Square(r * 8 + f));
        }
    return ring;
}

// Centro extendido: casillas c3-f3-f6-c6
static Bitboard extended_center() {
    return square_bb(SQ_C3) | square_bb(SQ_D3) | square_bb(SQ_E3) | square_bb(SQ_F3)
         | square_bb(SQ_C6) | square_bb(SQ_D6) | square_bb(SQ_E6) | square_bb(SQ_F6)
         | square_bb(SQ_C4) | square_bb(SQ_D4) | square_bb(SQ_E4) | square_bb(SQ_F4)
         | square_bb(SQ_C5) | square_bb(SQ_D5) | square_bb(SQ_E5) | square_bb(SQ_F5);
}

// Material no-peon de un bando
static int non_pawn_material(Color c, const Position& pos) {
    int m = 0;
    m += popcount(pos.pieces(c, KNIGHT)) * PieceValue[KNIGHT];
    m += popcount(pos.pieces(c, BISHOP)) * PieceValue[BISHOP];
    m += popcount(pos.pieces(c, ROOK))   * PieceValue[ROOK];
    m += popcount(pos.pieces(c, QUEEN))  * PieceValue[QUEEN];
    return m;
}

// =============================================================
// KILLER: tropismo al rey + ataques al ring + castigo por
// pasividad. Tope: +/- 80 cp.
// =============================================================

static int killer_delta(const Position& pos) {
    const Color  us          = pos.side_to_move();
    const Color  them        = ~us;
    const Square theirKingSq = pos.square<KING>(them);

    const Bitboard occupied = pos.pieces();
    const Bitboard ring     = king_ring(theirKingSq);

    constexpr int wTropism[PIECE_TYPE_NB] = {0, 0, 2, 1, 2, 4, 0, 0};

    const PieceType pts[4] = {KNIGHT, BISHOP, ROOK, QUEEN};
    Bitboard        our[4] = {pos.pieces(us, KNIGHT), pos.pieces(us, BISHOP),
                              pos.pieces(us, ROOK),   pos.pieces(us, QUEEN)};

    int delta     = 0;
    int attackers = 0;

    for (int i = 0; i < 4; ++i) {
        Bitboard bb = our[i];
        while (bb) {
            const Square s    = pop_lsb(bb);
            const int    dist = chebyshev(s, theirKingSq);

            delta += std::max(0, 5 - dist) * wTropism[pts[i]];

            const Bitboard atk = Attacks::attacks_bb(pts[i], s, occupied);
            if (atk & ring)
                ++attackers;
        }
    }

    delta += attackers * 5;

    if (attackers == 0)
        delta -= 6;

    delta = std::clamp(delta, -15, 60);
    return (delta * g_tactical_scale) / 2;
}

// =============================================================
// POSITIONAL: estructura, caballos vs alfiles, simplificacion.
// Tope: +/- 50 cp.
// =============================================================

static int pawn_structure_score(Color c, const Position& pos) {
    int score = 0;
    const Bitboard ourPawns   = pos.pieces(c, PAWN);
    const Bitboard theirPawns = pos.pieces(~c, PAWN);

    // Peones doblados
    for (int f = 0; f < 8; ++f) {
        const int cnt = popcount(ourPawns & file_bb(File(f)));
        if (cnt > 1)
            score -= (cnt - 1) * 12;
    }

    // Peones aislados
    Bitboard bb = ourPawns;
    while (bb) {
        const Square s = pop_lsb(bb);
        const int    f = int(file_of(s));
        Bitboard adj = 0;
        if (f > 0)
            adj |= file_bb(File(f - 1));
        if (f < 7)
            adj |= file_bb(File(f + 1));
        if (!(ourPawns & adj))
            score -= 14;
    }

    // Peones pasados
    bb = ourPawns;
    while (bb) {
        const Square s = pop_lsb(bb);
        const int    f = int(file_of(s));
        const int    r = int(rank_of(s));

        Bitboard aheadMask = 0;
        for (int ff = std::max(0, f - 1); ff <= std::min(7, f + 1); ++ff) {
            const Bitboard fm = file_bb(File(ff));
            if (c == WHITE)
                aheadMask |= fm & ~((Bitboard(1) << ((r + 1) * 8)) - 1);
            else
                aheadMask |= fm & ((Bitboard(1) << (r * 8)) - 1);
        }

        if (!(theirPawns & aheadMask)) {
            const int advance = (c == WHITE) ? r : 7 - r;
            score += 10 + advance * 8;
        }
    }

    return score;
}

static int minor_balance_score(Color c, const Position& pos) {
    const int totalPawns = popcount(pos.pieces(PAWN));
    const int knights    = popcount(pos.pieces(c, KNIGHT));
    const int bishops    = popcount(pos.pieces(c, BISHOP));

    int score = 0;
    if (totalPawns >= 12)
        score += knights * (totalPawns - 10) * 3;
    else if (totalPawns <= 8)
        score += bishops * (10 - totalPawns) * 3;

    return score;
}

// Detecta si el rey esta expuesto (falta de escudo de peones)
static bool king_exposed(Color c, const Position& pos) {
    const Square ksq = pos.square<KING>(c);
    const int    kf  = int(file_of(ksq));
    const int    kr  = int(rank_of(ksq));
    const Bitboard ourPawns = pos.pieces(c, PAWN);

    int shieldCount = 0;
    const int dir = (c == WHITE) ? 1 : -1;   // hacia adelante
    for (int df = -1; df <= 1; ++df) {
        const int f = kf + df;
        if (f < 0 || f > 7)
            continue;
        const int r1 = kr + dir;
        const int r2 = kr + 2 * dir;
        if (r1 >= 0 && r1 < 8 && (ourPawns & square_bb(Square(r1 * 8 + f))))
            ++shieldCount;
        else if (r2 >= 0 && r2 < 8 && (ourPawns & square_bb(Square(r2 * 8 + f))))
            ++shieldCount;
    }
    return shieldCount < 2;
}

static int simplification_score(Color c, const Position& pos) {
    const int matUs   = non_pawn_material(c, pos);
    const int matThem = non_pawn_material(~c, pos);
    const int diff    = matUs - matThem;

    // Si no tenemos ventaja material clara, no premiamos simplificar
    if (diff < 400)
        return 0;

    // Si el rey rival esta expuesto, NO simplificamos (mantenemos ataque)
    if (king_exposed(~c, pos))
        return 0;

    const int totalPieces = popcount(pos.pieces() & ~pos.pieces(PAWN) & ~pos.pieces(KING));
    const int simplicity  = std::max(0, 20 - totalPieces);

    // Escala creciente con la magnitud de la ventaja
    const int scale = std::min(3, (diff - 400) / 200 + 1);
    return simplicity * scale;
}

// Dinamismo: movilidad y actividad de las piezas (contrapeso al
// estatico del estilo posicional).
static int dynamics_score(Color c, const Position& pos) {
    int score = 0;
    const Bitboard occupied = pos.pieces();

    const PieceType pts[4]     = {KNIGHT, BISHOP, ROOK, QUEEN};
    const int       weights[4] = {5, 4, 4, 2};

    for (int i = 0; i < 4; ++i) {
        Bitboard bb = pos.pieces(c, pts[i]);
        while (bb) {
            const Square s = pop_lsb(bb);
            score += popcount(Attacks::attacks_bb(pts[i], s, occupied)) * weights[i];
        }
    }
    return score;
}

// Dominio de casillas: premia atacar el campo rival, especialmente
// las casillas centrales y cercanas al rey enemigo. Refuerza el
// principio de que en ajedrez domina quien controla mas casillas.
static int square_dominance_score(Color c, const Position& pos) {
    int score = 0;
    const Bitboard occupied = pos.pieces();

    // Rango a partir del cual consideramos "campo rival"
    // (blancas: rank >= 4 en 0-indexado = 5a fila real; negras: rank <= 3)
    const Bitboard theirHalf = (c == WHITE) ? (Rank5BB | Rank6BB | Rank7BB | Rank8BB)
                                            : (Rank1BB | Rank2BB | Rank3BB | Rank4BB);

    const PieceType pts[4]     = {KNIGHT, BISHOP, ROOK, QUEEN};
    const int       weights[4] = {4, 3, 5, 2};

    for (int i = 0; i < 4; ++i) {
        Bitboard bb = pos.pieces(c, pts[i]);
        while (bb) {
            const Square s   = pop_lsb(bb);
            const Bitboard a = Attacks::attacks_bb(pts[i], s, occupied);
            score += popcount(a & theirHalf) * weights[i];
        }
    }
    return score;
}

static int positional_delta(const Position& pos) {
    const Color us   = pos.side_to_move();
    const Color them = ~us;

    int delta = 0;
    delta += pawn_structure_score(us, pos) - pawn_structure_score(them, pos);
    delta += minor_balance_score(us, pos) - minor_balance_score(them, pos);
    delta += simplification_score(us, pos) - simplification_score(them, pos);
    delta += dynamics_score(us, pos) - dynamics_score(them, pos);
    delta += square_dominance_score(us, pos) - square_dominance_score(them, pos);

    return std::clamp(delta, -25, 25);
}

// =============================================================
// BALANCED: punto medio con personalidad. Centro extendido,
// coordinacion de menores, peones pasados conectados, rey
// activo en finales, penalizacion por piezas colgadas.
// Tope: +/- 25 cp.
// =============================================================

// Cuenta cuantas casillas del centro extendido atacan nuestras piezas.
static int center_control_score(Color c, const Position& pos) {
    const Bitboard center   = extended_center();
    const Bitboard occupied = pos.pieces();
    int            score    = 0;

    const PieceType pts[4] = {KNIGHT, BISHOP, ROOK, QUEEN};
    for (PieceType pt : pts) {
        Bitboard bb = pos.pieces(c, pt);
        while (bb) {
            const Square s   = pop_lsb(bb);
            const Bitboard a = Attacks::attacks_bb(pt, s, occupied);
            score += popcount(a & center);
        }
    }
    return score;
}

// Coordinacion: bonus si un menor esta defendido por otro menor,
// o si dos menores atacan la misma casilla.
static int minor_coordination_score(Color c, const Position& pos) {
    int            score    = 0;

    const Bitboard minors = pos.pieces(c, KNIGHT) | pos.pieces(c, BISHOP);
    Bitboard       bb     = minors;
    while (bb) {
        const Square s = pop_lsb(bb);
        // Piezas amigas que defienden s
        const Bitboard defenders = pos.attackers_to(s) & pos.pieces(c) & ~square_bb(s);
        if (defenders & minors)
            score += 6;
    }
    return score;
}

// Peones pasados conectados: bonus extra si un peon pasado tiene
// otro peon amigo en columna adyacente.
static int connected_passed_score(Color c, const Position& pos) {
    const Bitboard ourPawns   = pos.pieces(c, PAWN);
    const Bitboard theirPawns = pos.pieces(~c, PAWN);
    int            score      = 0;

    Bitboard bb = ourPawns;
    while (bb) {
        const Square s = pop_lsb(bb);
        const int    f = int(file_of(s));
        const int    r = int(rank_of(s));

        Bitboard aheadMask = 0;
        for (int ff = std::max(0, f - 1); ff <= std::min(7, f + 1); ++ff) {
            const Bitboard fm = file_bb(File(ff));
            if (c == WHITE)
                aheadMask |= fm & ~((Bitboard(1) << ((r + 1) * 8)) - 1);
            else
                aheadMask |= fm & ((Bitboard(1) << (r * 8)) - 1);
        }

        if (theirPawns & aheadMask)
            continue;  // no es pasado

        // Es pasado: miramos si tiene amigo en columna adyacente
        Bitboard adj = 0;
        if (f > 0)
            adj |= file_bb(File(f - 1));
        if (f < 7)
            adj |= file_bb(File(f + 1));

        if (ourPawns & adj)
            score += 15;  // pasado conectado
    }
    return score;
}

// Rey activo en finales: bonus si el rey esta centralizado y
// queda poco material en el tablero.
static int king_activity_score(Color c, const Position& pos) {
    const int totalMat = non_pawn_material(WHITE, pos) + non_pawn_material(BLACK, pos);
    if (totalMat > 2000)  // todavia es medio juego
        return 0;

    const Square ksq = pos.square<KING>(c);
    const int    df  = std::min(int(file_of(ksq)), 7 - int(file_of(ksq)));
    const int    dr  = std::min(int(rank_of(ksq)), 7 - int(rank_of(ksq)));
    const int    centralization = 6 - (df + dr);
    return std::max(0, centralization) * 3;
}

// Piezas colgadas: penaliza piezas atacadas por el rival que no
// estan defendidas por nosotros (excluye peones y rey).
static int hanging_pieces_score(Color c, const Position& pos) {
    int            score = 0;


    const PieceType pts[4] = {KNIGHT, BISHOP, ROOK, QUEEN};
    for (PieceType pt : pts) {
        Bitboard bb = pos.pieces(c, pt);
        while (bb) {
            const Square s = pop_lsb(bb);
            // Atacada por el rival?
            const Bitboard attackers = pos.attackers_to(s) & pos.pieces(~c);
            if (!attackers)
                continue;
            // Defendida por nosotros?
            const Bitboard defenders = pos.attackers_to(s) & pos.pieces(c) & ~square_bb(s);
            if (!defenders)
                score -= PieceValue[pt] / 100;  // peso suave
        }
    }
    return score;
}

static int balanced_delta(const Position& pos) {
    const Color us   = pos.side_to_move();
    const Color them = ~us;

    int delta = 0;
    delta += center_control_score(us, pos) - center_control_score(them, pos);
    delta += minor_coordination_score(us, pos) - minor_coordination_score(them, pos);
    delta += connected_passed_score(us, pos) - connected_passed_score(them, pos);
    delta += king_activity_score(us, pos) - king_activity_score(them, pos);
    delta += hanging_pieces_score(us, pos) - hanging_pieces_score(them, pos);

    return std::clamp(delta, -25, 25);
}

// =============================================================
// Punto de entrada
// =============================================================

int style_delta(const Position& pos) {
    switch (g_current_style) {
    case StyleType::Killer:
        return killer_delta(pos);
    case StyleType::Positional:
        return positional_delta(pos);
    case StyleType::Balanced:
        return balanced_delta(pos);
    case StyleType::Pure:
    default:
        return 0;
    }
}

}  // namespace Style
}  // namespace Stockfish
