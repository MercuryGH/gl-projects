#pragma once

namespace brep {

struct Solid;

Solid* create_solid();

Solid* create_holed_cuboid();

Solid* create_double_holed_cuboid();

Solid* create_complicated();

}

/**
 * piped cube:
 * m_vfs
 * m_ev ^ 3
 * m_ef  // we have 2 faces, 2 loops and 4 vertices (8 hedges) now
 * m_ev ^ 4
 * m_ef ^ 4 // top, left, right, front, back are OK
 * m_ev ^ 4
 * m_ef
 * k_e,m_r  // cutted faces are OK
 * m_ev ^ 4
 * m_ef ^ 4
 * k_f,m_rh
*/