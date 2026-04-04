//NEXT TASK(S): create treasure interaction (including add to inventory), monster class
#include <iostream>
#include <cstring>
#include <iomanip>
#include <time.h>
#include "terminal_colours.h"

using namespace std;
using namespace termcolour;

enum state {
	START,
	TUTORIAL,
	OPEN_WORLD,
	EXIT
};

enum mapItem {
	EMPTY,
	WALL,
	PLAYER,
	MONSTER,
	DOORKEEPER,
	TREASURE
};

struct coordinates {
	int x;
	int y;
};

char yesOrNoInput(string);
void tutorial(state);
void help(state);

class Map{
	private:
		const int MAX_X = 15; const int MAX_Y = 5;
		const int MAP_X = 17; const int MAP_Y = 7;

		struct mapEntry {
			mapItem number;
			char character;
		};
		const mapEntry empty = {EMPTY, '.'};
		const mapEntry wall = {WALL, 'x'};
		const mapEntry player = {PLAYER, 'P'};
		const mapEntry monster = {MONSTER, 'M'};
		const mapEntry doorkeeper = {DOORKEEPER, 'D'};
		const mapEntry treasure = {TREASURE, '?'};
		const coordinates DOORKEEPER_POSITION = {7,2}; //Using index counting, not intuitive

		int grid[5][15] = {0};
		bool isHOTU;

	public:
		Map() {
			//Initializing how the map will look at game start - in HOTU with player near doorkeeper
			isHOTU = true;
			grid[DOORKEEPER_POSITION.y][DOORKEEPER_POSITION.x] = (int)doorkeeper.number;
		}

		void printMap(coordinates playerPosition) {
			//First row of walls
			cout << '\n';
			for (int i = 0; i < MAP_X; i++) {
				cout << wall.character;
			}
			cout << '\n';

			for (int j = 0; j < MAX_Y; j++) {
				//Left wall border
				cout << wall.character;
				//Map items
				for (int i = 0; i < MAX_X; i++) {
					if (j == playerPosition.y && i == playerPosition.x) {
						cout << player.character;
						continue;
					}
					switch (grid[j][i]) {
						case 0:
							cout << empty.character; break;
						case 1:
							cout << wall.character; break;
						case 3:
							cout << monster.character; break;
						case 4:
							cout << doorkeeper.character; break;
						case 5:
							cout << treasure.character; break;
						default:
							cout << empty.character;
					}
				}
				//Right wall border
				cout << wall.character << '\n';
			}

			//Last row of walls
			for (int i = 0; i < MAP_X; i++) {
				cout << wall.character;
			}
			cout << '\n';
		}

		void generateMap(coordinates playerPosition) {
			isHOTU = false;
			clearMap();

			//Choose number of monsters to generate, 0-10 monsters per map 
			int numberOfMonsters = rand() % 10;
			bool placed = false;
			placeItem(playerPosition, numberOfMonsters, (int)monster.number);

			//Choose whether to generate treasure: 20% chance
			int placeTreasureChance = rand() % 99;
			if (placeTreasureChance >= 80) {
				placeItem(playerPosition, 1, (int)treasure.number);
			}
		}
		void placeItem(coordinates playerPosition, int iterations, int item) { //Supplements generateMap
			bool placed = false;
			int random_x = -1, random_y = -1;

			for (int i = 0; i < iterations; i++) {
				while (!(placed)) {
					random_x = rand() % MAX_X;
					random_y = rand() % MAX_Y;
					if (!(playerPosition.x == random_x && playerPosition.y == random_y) && (grid[random_y][random_x] == (int)empty.number)) {
						grid[random_y][random_x] = item;
						placed = true;
					}
				}
				placed = false;
			}
		}

		void enterHOTU(coordinates &playerPosition) { //Teleporting to the HOTU
			isHOTU = true;
				if (playerPosition.x == DOORKEEPER_POSITION.x && playerPosition.y == DOORKEEPER_POSITION.y) {
					playerPosition.x--;
				}
			clearMap();
			grid[DOORKEEPER_POSITION.y][DOORKEEPER_POSITION.x] = (int)doorkeeper.number;
		}

		int tryMove(coordinates newPosition) {
			/*return codes:
			  0 - coordinates successfully updated and no further actions taken
			  1 - doorkeeper interaction
			  2 - treasure interaction
			  3 - monster interaction
			*/

			if (isHOTU) {
				if ((newPosition.x == DOORKEEPER_POSITION.x) && (newPosition.y == DOORKEEPER_POSITION.y)) {
					return 1;
				}
				return 0;
			}

			if (grid[newPosition.y][newPosition.x] == (int)treasure.number) {
				return 2;
			} else if (grid[newPosition.y][newPosition.x] == (int)monster.number) {
				return 3;
			}
			return 0;
				
			//update logic for if is not HOTU and there is something there - trigger monster encounter if is monster, trigger treasure encounter if is treasure, trigger leave area prompt if is wall
		}
		void clearMap() {
			for (int i = 0; i < MAX_X; i++) {
				for (int j = 0; j < MAX_Y; j++) {
						grid[j][i] = 0;
				}
			}
		}

		//Getters
		bool getIsHOTU() {
			return isHOTU;
		}

		//Setters
		void setIsHOTU(bool set) {
			isHOTU = set;
		}
};

class Player {
	struct item { //Inventory will be a linked list of this
		string name;
		int quantity;
		bool isCombatItem;
		item *next;
	};

	private:
		const int MIN_X = 0; const int MAX_X = 15; const int MIN_Y = 0; const int MAX_Y = 5;
		item* inventory; //I'm naming it inventory, but this is just the first entry of the inventory (head of LL)
		coordinates playerPosition;
		int health;
		int returnCode = 1;

	public:
		char name = 'P';
		
		//Constructor
		Player() {
			health = 25;
			inventory = NULL;
			playerPosition.x = 3;
			playerPosition.y = 2;
		}

		void showInventory() {
			bool noItems = false;
			if (inventory == NULL) {
				noItems = true;
				cout << "You check the mysterious bag on your back only to find absolutely nothing.\n";
			}

			if (!(noItems)) {
				item* current = inventory;
				cout << "You check the mysterious bag on your back to find that you have:\n";
				while (current->next != NULL) {
					cout << current->quantity << " " << current->name << '\n';
					current = current->next;
				}
			}
		}

		int getQuantity(string searchItem) {
			item* current = inventory;
			bool found;
			int quantity;

			if (current == NULL) {
				return -1;
			}

			while (current != NULL && !(found)) {
				if ((current->name).compare(searchItem) == 0) {
					found = true;
					quantity = current->quantity;
				} else {
					current = current->next;
				}
			}
			if (!(found)) {
				return -1;
			}
			return quantity;
		}

		int enterHOTU(Map &map) {
			map.enterHOTU(playerPosition);
			return 1;
		}

		bool doorkeeperInteraction() {
			int amountOfGold = getQuantity("gold");
			const string DOORKEEPER = magenta("The Doorkeeper");

			cout << '\n' << DOORKEEPER << " stares into your soul." << endl;
			if (amountOfGold == -1) {
				cout << DOORKEEPER << " speaks in a gravelly voice that carries the depths of the cosmos." << endl; 
				cout << red("\"YOU. SHALL. NOT. PASS.\"") << endl;
				cout << "You're petrified. All you can do is listen." << endl;
				cout << red("\n\"You. Must. Pay. The. Price. Bring. Me. ") << yellow("50. Gold.\"") << endl;
				cout << "You have nary a cent to your name. How can you bring this entity 50 gold?" << endl;
				cout << "As if reading your thoughts, " << DOORKEEPER << " continues:" << endl;
				cout << red("\n\"Fight. Earn. Your. Right. To. Leave.\"") << endl;
				cout << "And that is all you are left with." << endl;
				return false;
			}
			if (amountOfGold <= 50) {
				//some dialogue
				return false;
			}
			
			//some dialogue for 50+ gold (game won)
			return true;
		}

		void openWorldPrompt() {
			cout << "Use w,a,s,d to move or h for help: ";
		}

		void openWorldControls(Map &map) {
			char input = '\0';
			int returnCode;
			int tryMoveReturnCode = -1;
			bool movePlayer = true;

			while (!(input == 'w' || input == 'a' || input == 's' || input == 'd' || input == 'h' || input == 't' || input == 'p')) {
				map.printMap(playerPosition);
				openWorldPrompt();
				cin >> input;
				input = tolower(input);
			}
			if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
				coordinates newPosition = playerPosition;
				switch (input) {
					case 'w':
						newPosition.y--; break;
					case 'a':
						newPosition.x--; break;
					case 's':
						newPosition.y++; break;
					case 'd':
						newPosition.x++; break;
				}
				//Generate new map if player moves out of bounds
				if (newPosition.x < MIN_X || newPosition.x >= MAX_X || newPosition.y < MIN_Y || newPosition.y >= MAX_Y) {
					if (yesOrNoInput("\nLeave this area behind and enter a new one?") == 'n') {
						movePlayer = false;
					} else {
						if (newPosition.x < MIN_X) {
							newPosition.x = MAX_X - 1;
						}
						if (newPosition.x >= MAX_X) {
							newPosition.x = MIN_X;
						}
						if (newPosition.y < MIN_Y) {
							newPosition.y = MAX_X - 1;
						}
						if (newPosition.y >= MAX_Y) {
							newPosition.y = MIN_Y;
						}
					map.generateMap(newPosition);
					cout << "You have left this realm behind...\n";
					}
				}
				if (movePlayer) {
					tryMoveReturnCode = map.tryMove(newPosition);
					switch (tryMoveReturnCode) {
						case 0:
							playerPosition = newPosition;
							returnCode = 1;
							break;
						case 1:
							if (doorkeeperInteraction()) {
								returnCode = 0; //Game over, won
							} else {
								returnCode = 1;
							}
							break;
						default:
							returnCode = 1;
					}
				}
			} else {
				switch (input) {
					case 'h':
						help(OPEN_WORLD); returnCode = 1; break;
					case 't':
						if (map.getIsHOTU()) {
							cout << "You are already in " << magenta("The Heart of The Underground") << "." << endl;
						} else {
							if (yesOrNoInput("Enter " + magenta("The Heart of The Underground") + "?") == 'y') {
								enterHOTU(map);
							}
						}
						break;
					case 'p':
						cout << "Your health is at a healthy " << health << "." << endl;
						showInventory();
						break;
					default:
						cout << "Unrecognized command. Try again?" << endl;
				}
				returnCode = 1;
			}
		}

		//Getters
		coordinates getPlayerPosition() {
			return playerPosition;
		}
		int getReturnCode() {
			return returnCode;
		}
};

int main() {
	string gameName = cyan("Escape The Underground");
	char yn = '\0';
	srand(time(0));

	state gamestate = START;
	cout << "Welcome to " << gameName << "!" << "\nYour mission is simple.\nYou are trapped. Escape at any cost." << "\nAnd it will cost you..." << endl;
	cout << "\n" << gameName << " has a short tutorial. ";
	yn = yesOrNoInput("Show the tutorial?");

	if (yn == 'y') {
		cout << '\n';
		gamestate = TUTORIAL;
		tutorial(gamestate);
	}
	yn = '\0';

	cout << '\n';
	yn = yesOrNoInput("Begin adventure?");
	if (yn != 'y') {
		cout << "Too bad. No one ever " << bold("asks") << " to be trapped in The Underground." << endl;
	}
	yn = '\0';

	cout << "Here we go.\n" << endl;

	gamestate = OPEN_WORLD;

	Player default_player; //Initialize the player
	Map default_map; //Initialize the map

	default_player.enterHOTU(default_map); //Player enters HOTU for the first time - begin game loop
	while (default_player.getReturnCode() > 0) {
		/*
		Return codes:
		 -1 - exit game (lost)
		  0 - exit game (won)
		  1 - get player input
		*/
		switch (default_player.getReturnCode()) {
			case 1:
				default_player.openWorldControls(default_map); break;
		}
	}
	cout << "Goodbye." << endl;

	return 0;
}

char yesOrNoInput(string repeatInstruction) {
	char yn = '\0';
	while (!(tolower(yn) == 'y' || tolower(yn) == 'n')) {
		cout << repeatInstruction << " [y/n]: ";
		cin >> yn;
		while (cin.peek() != '\n') {
			cin.ignore();
		}
	}
	return tolower(yn);
}

void tutorial(state gamestate) {
	string underground = magenta("The Underground");
	string doorkeeper = magenta("The Doorkeeper");

	cout << "You are " << magenta("P") << ", the Player. You find yourself trapped in a mysterious world you know only as " << underground << "." << endl;
	cout << "You will have to " << red("fight to the death") << " to escape. Or will you?" << endl;
	cout << magenta("Monsters") << " can be found randomly throughout " << underground << ". The only way to escape is to get " << yellow("Gold") << " from Monsters to give to " << doorkeeper << "." << endl;
	cout << doorkeeper << " can be found in " << magenta("The Heart of ") << underground << ". The areas outside of this place are never constant, ever shifting. Once you leave an area, you may never find another one like it." << endl;
	cout << "Should you fight them? Or should you make more strategic decisions? It's all up to you..." << endl;
	cout << "Keep an eye on your stats: your " << yellow("Health") << " and " << yellow("Inventory") << " will both be important to you." << endl;
	cout << "Will you perish? Will you manage to escape? Or will you be trapped here " << red("Forever") << "? It all rests on you, Player. Good luck.\n" << endl;
	help(gamestate);
}

void help(state gamestate) {
	string i = blue("i");
	cout << "Controls:" << endl;
	cout << "Use " << blue("w,a,s,d") << " to traverse the map. You can input multiple moves at once to save time." << endl;
	cout << "\nUse " << i << " to check your inventory." << endl;
	cout << "Use " << blue("p") << " to check your stats." << endl;
	cout << "Use " << blue("t") << " to leave your current area and teleport to " << magenta("The Doorkeeper") << "." << endl;

	cout << "\nWhen you reach a tile with a point of interest, you will automatically engage." << endl;
	if (gamestate == TUTORIAL) {
		cout << "Battle controls shall be revealed soon." << endl;
	} else {
		cout << "In battle, use " << i << " to use an item, " << blue("s") << " to select a weapon (if you have one), " << blue("a") << " to attack and " << blue("r") << " to run away." << endl;
		cout << "These controls will be shown in the battle menu as well." << endl;
	}
	cout << "\nUse " << blue("h") << " to show this menu." << endl;
}
