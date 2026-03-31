//NEXT TASK(S): refine logic for when player goes to POIs
#include <iostream>
#include <cstring>
#include <iomanip>
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
		/*
		 - Previous implementation
		const char EMPTY = '.';
		const char WALL = 'x';
		const char PLAYER = 'P';
		const char MONSTER = 'M';
		const char DOORKEEPER = 'D';
		const char TREASURE = '?';
		*/
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
			isHOTU = true; //Setting this to true because when the map is first initialized, the player will spawn in the HOTU
		}

		void printMap() {
			//First row of walls
			cout << '\n';
			for (int i = 0; i < MAP_X; i++) {
				cout << wall.character;
			}
			cout << '\n';

			for (int j = 0; j < MAX_Y; j++) {
				cout << wall.character;
				for (int i = 0; i < MAX_X; i++) {
					switch (grid[j][i]) {
						case 0:
							cout << empty.character; break;
						case 1:
							cout << wall.character; break;
						case 2:
							cout << player.character; break;
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
				cout << wall.character << '\n';
			}

			//Last row of walls
			for (int i = 0; i < MAP_X; i++) {
				cout << wall.character;
			}
			cout << '\n';
		}

		void generateMap() {
			//choose number of monsters to generate, 0-10 monsters per map 
			//choose whether to generate treasure: 20% chance
		}

		bool updatePlayerPosition(coordinates playerPosition) {
			//add logic for if either coord is < 0 - should trigger (leave this area) confirmation

			if (isHOTU) {
				clearMap();
				if (playerPosition.x == DOORKEEPER_POSITION.x && playerPosition.y == DOORKEEPER_POSITION.y) {
					//call function to speak to the doorkeeper
					return false;
				}
				grid[playerPosition.y][playerPosition.x] = (int)player.number;
				grid[DOORKEEPER_POSITION.y][DOORKEEPER_POSITION.x] = (int)doorkeeper.number;
				return true;
			}
			
			//update logic for if is not HOTU and there is something there - trigger monster encounter if is monster, trigger treasure encounter if is treasure, trigger leave area prompt if is wall
		}
		void clearMap() {
			for (int i = 0; i < MAX_X; i++) {
				for (int j = 0; j < MAX_Y; j++) {
						grid[j][i] = 0;
				}
			}
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
		item* inventory; //I'm naming it inventory, but this is just the first entry of the inventory
		coordinates playerPosition;
		int health;
		int level;
		int returnCode = 1;

	public:
		char name = 'P';
		
		//Constructor
		Player() {
			health = 10;
			level = 1;
			inventory = NULL;
			playerPosition.x = 3;
			playerPosition.y = 2;
		}

		item* getLastItem() { //i started writing this function (as just getItem) then realized when I was almost done that this will only return the last item. To get a specific item, I will need to do a search function
			//I will also need another function to list every item in the inventory
			if (inventory == NULL) {
				return NULL;
			}

			item* current = inventory;
			while (current->next != NULL) {
				current = current->next;
			}
			return current;
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

		int enterHOTU(Map map) {
			if (map.updatePlayerPosition(getPlayerPosition())) {
				map.printMap();
			}
			return 1;
		}

		void doorkeeperInteraction(Player thisPlayer) {
			int amountOfGold = getQuantity("gold");
			const string DOORKEEPER = magenta("The Doorkeeper");

			cout << DOORKEEPER << " stares into your soul." << endl;
			if (amountOfGold == -1) {
				cout << DOORKEEPER << " speaks in a gravelly voice that carries the depths of the cosmos." << endl; 
				cout << red("\"YOU. SHALL. NOT. PASS.\"") << endl;
				cout << "You're petrified. All you can do is listen." << endl;
				cout << red("\n\"You. Must. Pay. The. Price. Bring. Me. 50. Gold.\"") << endl;
				cout << "You have nary a cent to your name. How can you bring this entity 50 gold?" << endl;
				cout << "As if reading your thoughts, " << DOORKEEPER << " continues:" << endl;
				cout << red("\n\"Fight. Earn. Your. Right. To. Leave.\"");
				cout << "And that is all you are left with." << endl;
			}
		}

		void openWorldPrompt() {
			cout << "Use w,a,s,d to move or h for help: ";
		}

		void openWorldControls(Map map) {
			char input = '\0';
			int returnCode;

			while (!(input == 'w' || input == 'a' || input == 's' || input == 'd' || input == 'h')) {
				openWorldPrompt();
				cin >> input;
				input = tolower(input);
			}
			switch (input) {
				case 'w':
					playerPosition.y--; break;
				case 'a':
					playerPosition.x--; break;
				case 's':
					playerPosition.y++; break;
				case 'd':
					playerPosition.x++; break;
				case 'h':
					help(OPEN_WORLD); break;
				default:
					cout << "Unrecognized command. Try again?" << endl;
					returnCode = 1;
			}
			if (map.updatePlayerPosition(playerPosition)) {
				map.printMap();
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
	while (default_player.getReturnCode() > - 1) {
		/*
		Return codes:
		 -1 - exit game
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
	cout << doorkeeper << " can be found in the heart of " << underground << ". The areas outside of this place are never constant, ever shifting. Once you leave an area, you may never find another one like it." << endl;
	cout << "Should you fight them? Or should you make more strategic decisions? It's all up to you..." << endl;
	cout << "Keep an eye on your stats: your " << yellow("Health") << ", " << yellow("Level") << ", and " << yellow("Inventory") << " will all be important to you." << endl;
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
