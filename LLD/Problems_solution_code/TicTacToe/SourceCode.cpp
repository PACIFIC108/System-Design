#include<bits/stdc++.h>
using namespace std;

void INIT() {
	freopen("input.txt" , "r" , stdin);
	freopen("output.txt" , "w" , stdout);
}

class ISymbol {
public:
	virtual ~ISymbol() = default; // Added virtual destructor
	virtual string get() = 0;
	virtual void set(string s) = 0;
};

class Symbol: public ISymbol {
	string name;
public:
	Symbol(string s): name(s) {}
	string get() override { return name; }
	void set(string s) override { name = s; }
};

class IPlayers {
public:
	virtual ~IPlayers() = default; // Added virtual destructor
	virtual string getSymbol() = 0;
	virtual string getName() = 0;
};

class Player: public IPlayers {
	ISymbol* symbol;
	string name;
public:
	Player(string name, ISymbol* s): name(name), symbol(s) {}
	~Player() { delete symbol; } // Cleans up the symbol it owns
	string getSymbol() override { return symbol->get(); }
	string getName() override { return name; }
};

class Cell {
	ISymbol *symbol;
	array <int, 2> orientation;
public:
	Cell(ISymbol* symbol, array<int, 2> orientation): orientation(orientation), symbol(symbol) {}
	~Cell() { delete symbol; } // Cleans up the symbol it owns
	void set(string s) { symbol->set(s); }
	string get() { return symbol->get(); }
};

class Board {
	int size;
	int remCap;
public:
	vector < vector<Cell*>> grid;
	Board(int size): size(size) {
		remCap = size * size;
		grid = vector < vector < Cell*>>(size, vector<Cell*>(size));
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				grid[i][j] = new Cell(new Symbol("E"), {i, j});
			}
		}
	}

	~Board() {
		for (auto& row : grid) {
			for (auto& cell : row) {
				delete cell; // Cleans up cells
			}
		}
	}

	bool makeMove(int i, int j, string s) {
		if (i < 0 || j < 0 || i >= size || j >= size || grid[i][j]->get() != "E") {
			cout << "illegal move\n";
			return false;
		}
		remCap--;
		grid[i][j]->set(s);
		printBoard();
		return true;
	}

	void printBoard() {
		for (auto i : grid) {
			for (auto j : i) cout << j->get() << ' ';
			cout << endl;
		} cout << endl;
	}
	bool isFull() { return remCap == 0; }
	int getSize() { return size; }
	string getSymbol(int i, int j) { return grid[i][j]->get(); }
};

class Game {
	Board* TicTakToe;
	IPlayers *X, *O;
public:
	Game(Board* B, IPlayers* X, IPlayers* Y): TicTakToe(B), X(X), O(Y) {}

	void Run(IPlayers* P) {
		if (TicTakToe->isFull()) {
			cout << "Draw\n";
			return;
		}
		cout << P->getName() << " turns\n";
		int x = -1, y = -1;
		cin >> x >> y;
		if (x == -1 || y == -1) return;

		if (!TicTakToe->makeMove(x, y, P->getSymbol())) {
			Run(P);
			return;
		}
		checkWin(x, y, P);
	}

	void checkWin(int x, int y, IPlayers* P) {
		string s = P->getSymbol();
		bool ok = true;

		// Check Column
		for (int i = 0; i < TicTakToe->getSize(); i++) {
			ok &= (s == TicTakToe->getSymbol(i, y));
		}
		if (ok) {
			cout << P->getName() << " won\n";
			return;
		}

		// Check Row
		ok = true;
		for (int i = 0; i < TicTakToe->getSize(); i++) {
			ok &= (s == TicTakToe->getSymbol(x, i));
		}
		if (ok) {
			cout << P->getName() << " won\n";
			return;
		}

		// Check Main Diagonal
		if (x == y) {
			ok = true;
			for (int i = 0; i < TicTakToe->getSize(); i++) {
				ok &= (s == TicTakToe->getSymbol(i, i));
			}
			if (ok) {
				cout << P->getName() << " won\n";
				return;
			}
		}

		// Check Anti-Diagonal
		if (x + y == TicTakToe->getSize() - 1) {
			ok = true;
			for (int i = 0; i < TicTakToe->getSize(); i++) {
				ok &= (s == TicTakToe->getSymbol(i, TicTakToe->getSize() - 1 - i));
			}
			if (ok) {
				cout << P->getName() << " won\n";
				return;
			}
		}

		// Proceed to next player
		if (P->getSymbol() == "O") Run(X);
		else Run(O);
	}
};

int main() {
	INIT();

	ISymbol* SX = new Symbol("X");
	ISymbol* SO = new Symbol("O");
	IPlayers* X = new Player("playerX", SX);
	IPlayers* O = new Player("playerO", SO);

	Board* B = new Board(3);
	Game* TicTakToe = new Game(B, X, O);

	TicTakToe->Run(X);

	// Fixed Memory cleanup using the correct variable names
	delete TicTakToe;
	delete B;
	delete X;
	delete O;

	return 0;
}
