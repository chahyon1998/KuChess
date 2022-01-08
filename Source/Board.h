#pragma once

#include <cstdint>

#define NN 0
#define WP 1
#define WB 2
#define WN 3
#define WR 4
#define WQ 5
#define WK 6
#define BP 7
#define BB 8
#define BN 9
#define BR 10
#define BQ 11
#define BK 12
#define WHITE 1
#define BLACK -1
#define PLAYER(piece) ((piece) < WP ? NN : ((piece) < BP ? WHITE : BLACK))
#define ON_BOARD(x, y) (0 <= (x) && (x) <= 7 && 0 <= (y) && (y) <= 7)
#define MOVE(piece, src, dst) ((((int) (piece)) << 16 & 0xff0000) | (((int) (src)) << 8 & 0xff00) | ((int) (dst)))
#define PIECE(move) (((move) >> 16) & 0xff)
#define SRC(move) (((move) >> 8) & 0xff)
#define DST(move) ((move) & 0xff)

class Board {
public:
	Board();
	/**
	 * @brief Resets the board to initial position
	*/
	void Reset();
	/**
	 * @brief Prints the board to cout
	*/
	void Print();
	/**
	 * @brief Make move
	 * @param move A chess move where
						the 2nd byte is the piece,
						the 3rd byte is the source square,
						and the 4th byte is the destination square
	*/
	void Move(uint32_t move);
	/**
	 * @brief Undo the previous move as saved in m_history
	*/
	void Undo();
	/**
	 * @brief Check if the piece at 
	 * @param piece_pos 
	 * @return 
	*/
	bool IsCheck(uint16_t pos);
	void GetLegalMoves(uint32_t* moves, size_t* n_moves, const size_t max_n_moves);
	void GetLegalMoves(uint8_t src, uint32_t* moves, size_t* n_moves, const size_t max_n_moves);
	bool AddMoveIfEmpty(uint32_t* moves, size_t* n_moves, int piece, int src_x, int src_y, int dst_x, int dst_y);
	bool AddMoveIfTakes(uint32_t* moves, size_t* n_moves, int piece, int src_x, int src_y, int dst_x, int dst_y);
	bool AddMoveIfEmptyOrTakes(uint32_t* moves, size_t* n_moves, int piece, int src_x, int src_y, int dst_x, int dst_y);

	int8_t m_turn;
	uint8_t m_squares_piece[64];
	uint32_t m_history[100];
};