#include "Board.h"
#include <memory>
#include <iostream>

const uint8_t g_new_squares_piece[64] = { WR, WN, WB, WK, WQ, WB, WN, WR,
											WP, WP, WP, WP, WP, WP, WP, WP,
											NN, NN, NN, NN, NN, NN, NN, NN,
											NN, NN, NN, NN, NN, NN, NN, NN,
											NN, NN, NN, NN, NN, NN, NN, NN,
											NN, NN, NN, NN, NN, NN, NN, NN,
											BP, BP, BP, BP, BP, BP, BP, BP,
											BR, BN, BB, BK, BQ, BB, BN, BR };

Board::Board()
{
	Reset();
}

void Board::Reset()
{
	m_turn = 1;
	memcpy(m_squares_piece, g_new_squares_piece, sizeof(uint8_t) * 64);
}

void Board::Print()
{
	for (int i = 63; i >= 0; --i) {
		if (m_squares_piece[i] < 10)
			std::cout << "0" << (int) m_squares_piece[i];
		else
			std::cout << (int) m_squares_piece[i];
		if (i % 8 == 0)
			std::cout << std::endl;
	}
}

void Board::Move(uint32_t move)
{
	uint8_t piece, src, dst;
	piece = PIECE(move);
	src = SRC(move);
	dst = DST(move);
	m_squares_piece[dst] = m_squares_piece[src];
	m_squares_piece[src] = NN;
	m_turn = -m_turn;
}

void Board::Undo()
{
}

bool Board::IsCheck(uint16_t piece_pos)
{
	return false;
}

void Board::GetLegalMoves(uint32_t* moves, size_t* n_moves, const size_t max_n_moves)
{
	*n_moves = 0;
	for (uint8_t i = 0; i < 64; ++i) {
		size_t piece_n_moves = 0;
		GetLegalMoves(i, moves + *n_moves, &piece_n_moves, max_n_moves - *n_moves);
		*n_moves += piece_n_moves;
		if (*n_moves >= max_n_moves)
			return;
	}
}

void Board::GetLegalMoves(uint8_t src, uint32_t* moves, size_t* n_moves, const size_t max_n_moves)
{
	*n_moves = 0;
	uint8_t piece = m_squares_piece[src];
	int8_t piece_player = PLAYER(piece);

	if (piece_player != m_turn) {
		return;
	}

	int src_x = src % 8;
	int src_y = src / 8;
	int dst_x, dst_y, dst;
	switch (piece) {
	case WK:
	case BK:
		AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x, src_y + 1);
		AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x, src_y - 1);
		AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x + 1, src_y);
		AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x - 1, src_y);
		break;
	case WP:
		AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x, src_y + 1);
		if (src_y == 1)
			AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x, src_y + 2);
		AddMoveIfTakes(moves, n_moves, piece, src_x, src_y, src_x - 1, src_y + 1);
		AddMoveIfTakes(moves, n_moves, piece, src_x, src_y, src_x + 1, src_y + 1);
		// TODO: en-passant and promotion
		break;
	case BP:
		AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x, src_y - 1);
		if (src_y == 6)
			AddMoveIfEmpty(moves, n_moves, piece, src_x, src_y, src_x, src_y - 2);
		AddMoveIfTakes(moves, n_moves, piece, src_x, src_y, src_x - 1, src_y - 1);
		AddMoveIfTakes(moves, n_moves, piece, src_x, src_y, src_x + 1, src_y - 1);
		// TODO: en-passant and promotion
		break;
	case WN:
	case BN:
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x + 1, src_y + 2);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x + 1, src_y - 2);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x - 1, src_y + 2);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x - 1, src_y - 2);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x + 2, src_y + 1);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x + 2, src_y - 1);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x - 2, src_y + 1);
		AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, src_x - 2, src_y - 1);
		break;
	case WQ:
	case BQ:
	case WB:
	case BB:
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x + i;
			int dst_y = src_y + i;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x + i;
			int dst_y = src_y - i;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x - i;
			int dst_y = src_y + i;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x - i;
			int dst_y = src_y - i;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		if (piece == WB || piece == BB)
			break;
	case WR:
	case BR:
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x + i;
			int dst_y = src_y;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x - i;
			int dst_y = src_y;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x;
			int dst_y = src_y + i;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		for (int i = 1; i < 8; ++i) {
			int dst_x = src_x;
			int dst_y = src_y - i;
			int dst = dst_x + dst_y * 8;
			AddMoveIfEmptyOrTakes(moves, n_moves, piece, src_x, src_y, dst_x, dst_y);
			if (m_squares_piece[dst] != NN)
				break;
		}
		break;
	}
}

bool Board::AddMoveIfEmpty(uint32_t* moves, size_t* n_moves, int piece, int src_x, int src_y, int dst_x, int dst_y)
{
	int src = src_x + src_y * 8;
	int dst = dst_x + dst_y * 8;
	if (ON_BOARD(dst_x, dst_y) && m_squares_piece[dst] == NN) {
		moves[(*n_moves)++] = MOVE(piece, src, dst);
		return true;
	}
	return false;
}

bool Board::AddMoveIfTakes(uint32_t* moves, size_t* n_moves, int piece, int src_x, int src_y, int dst_x, int dst_y)
{
	int src = src_x + src_y * 8;
	int dst = dst_x + dst_y * 8;
	if (ON_BOARD(dst_x, dst_y) && PLAYER(m_squares_piece[src]) == -PLAYER(m_squares_piece[dst])) {
		moves[(*n_moves)++] = MOVE(piece, src, dst);
		return true;
	}
	return false;
}

bool Board::AddMoveIfEmptyOrTakes(uint32_t* moves, size_t* n_moves, int piece, int src_x, int src_y, int dst_x, int dst_y)
{
	int src = src_x + src_y * 8;
	int dst = dst_x + dst_y * 8;
	if (ON_BOARD(dst_x, dst_y) && PLAYER(m_squares_piece[src]) != PLAYER(m_squares_piece[dst])) {
		moves[(*n_moves)++] = MOVE(piece, src, dst);
		return true;
	}
	return false;
}