#include<bits/stdc++.h>
using namespace std;

//-------------Entity---------//
class Entity {
public:
	virtual int action() = 0;
	virtual string getName() = 0;
	virtual ~Entity() {}
};

class Snake: public Entity {
	int start, size;
	string name;
public:
	Snake(int start, int size): start(start), size(size) {
		name = "S";
	}
	int action() override {
		cout << "\nSnake Bite from " << start << "!" << endl;
		return max(1, rand() % start);
	}
	string getName() override {return name;}
};

class Ladder: public Entity {
	int start, size;
	string name;
public:
	Ladder(int start, int size): start(start), size(size) {
		name = "L";
	}
	int action() override {
		cout << "\nLadder Used from " << start << "!" << endl;
		return max(start + 1, rand() % size);
	}
	string getName() override {return name;}
};

class Empty: public Entity {
	string name;
	int size, start;
public:
	Empty(int start, int size): start(start), size(size) {
		name = "E";
	}
	int action() override {
		return start;
	}
	string getName() override {return name;}
};

//---------CELL-------------//
class Cell {
	int id;
	Entity* entity;
public:
	Cell(int num, Entity* entity): id(num), entity(entity) {}

	int get() {return id;}
	Entity* getEntity() { return entity; } // Needed for destructor cleanup

	void set(Entity* E) {
		// FIX 1: When we replace an 'Empty' entity with a Snake/Ladder,
		// we must delete the old entity to prevent a memory leak!
		if (entity != nullptr) {
			delete entity;
		}
		entity = E;
	}

	int EntityAction() {return entity->action();}
	string EntityName() {return entity->getName();}
};

//------------Players-------//
class IPlayers {
public:
	virtual string getName() = 0;
	virtual int getPosition() = 0;
	virtual void setPosition(int p) = 0;
	virtual ~IPlayers() {}
};

class Player: public IPlayers {
	// FIX 2: Decoupled Player! The player no longer holds a copy of a Cell object.
	// It only needs to know its current position integer.
	int position;
	string name;
public:
	Player(string name, int position): position(position), name(name) {}
	string getName() override {return name;}
	int getPosition() override { return position;}
	void setPosition(int p) override {position = p;}
};

//---------DICE----------//
class Dice {
	int faces;
public:
	Dice(int faces): faces(faces) {}
	int roll() {
		int face = rand() % faces + 1;
		cout << "Dice Rolled and " << face << " came up.\n";
		return face;
	}
	~Dice() {}
};

//---------BOARD---------//
class Board {
	vector<Cell> cells;
	int size;
public:
	Board(int size): size(size) {
		for (int i = 0; i <= size * size; i++) {
			cells.push_back(Cell(i, new Empty(i, size * size)));
		}
	}

	Cell getCell(int id) {return cells[id];}

	void setEntity(int cellId, Entity* E) {
		cells[cellId].set(E);
	}

	// FIX 3: makeMove now returns the integer ID of the final destination, not a copied Cell object.
	int makeMove(int start, int face) {
		if (start + face > size * size) {
			cout << "Not Allowed (Overshot the board)\n";
			return start;
		}
		int destination = start + face;
		int finalDestination = cells[destination].EntityAction();
		cout << "Move made from " << start << " to " << finalDestination << endl;
		return finalDestination;
	}

	void printBoard(vector<IPlayers*>& P) {
		for (auto p : P) {
			cout << "Player " << p->getName() << " is on cell: " << p->getPosition() << endl;
		}
		cout << "\nBoard: \n";
		int id = size * size;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				cout << cells[id--].EntityName() << ' ';
			}
			cout << endl;
		}
	}

	int Size() {return size * size;}

	// FIX 4: Proper Board Cleanup. When the board is destroyed, delete all Entities.
	~Board() {
		for (int i = 0; i < cells.size(); i++) {
			delete cells[i].getEntity();
		}
	}
};

//----------GAME---------//
class Game {
	// FIX 5: Board is now passed by Reference (&), so we don't duplicate the board in memory!
	Board& Snake_Ladder;
	Dice* dice;
	vector<IPlayers*> P;
public:
	Game(Board& B, Dice* D, vector<IPlayers*>& P): Snake_Ladder(B), dice(D), P(P) {}

	// FIX 6: Proper Game Cleanup. The Game owns the dice and players, so it deletes them when it ends.
	~Game() {
		delete dice;
		for (auto p : P) {
			delete p;
		}
	}

	void Run() {
		int playerId = 0;
		while (true) {
			Snake_Ladder.printBoard(P);
			cout << "\nPlayer " << P[playerId]->getName() << " is making move\n";

			int start = P[playerId]->getPosition();
			int face = dice->roll();
			int newPosition = Snake_Ladder.makeMove(start, face);
			cout << endl;

			P[playerId]->setPosition(newPosition);

			if (newPosition == Snake_Ladder.Size()) {
				cout << "\n🎉 Player " << P[playerId]->getName() << " Won the game! 🎉\n";
				break;
			}
			playerId = (playerId + 1) % P.size();
		}
	}
};

//----------MAIN---------//
int main() {
	freopen("input.txt" , "r" , stdin);
	freopen("output.txt" , "w" , stdout);

	srand(time(0));

	Board Snake_Ladder(3);
	Snake_Ladder.setEntity(3, new Snake(3, 9));
	Snake_Ladder.setEntity(6, new Ladder(6, 9));
	Snake_Ladder.setEntity(8, new Snake(8, 9));
	Snake_Ladder.setEntity(4, new Ladder(4, 9));

	vector<IPlayers*> Players;
	// Players just start at integer position 0 now.
	Players.push_back(new Player("Prashant", 0));
	Players.push_back(new Player("Pacific", 0));
	Players.push_back(new Player("Shreyansh", 0));
	Players.push_back(new Player("Santosh", 0));

	Game SandL(Snake_Ladder, new Dice(6), Players);
	SandL.Run();

	return 0; // Everything is automatically deleted safely here!
}
