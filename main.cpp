//NEXT TASK(S): code monster fight sequence, write deleteItem function (yikes)
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

			//Choose number of monsters to generate, 0-9 monsters per map 
			int numberOfMonsters = rand() % 10;
			bool placed = false;
			placeItem(playerPosition, numberOfMonsters, (int)monster.number);

			//Choose whether to generate treasure: 20% chance
			int placeTreasureChance = rand() % 100;
			if (placeTreasureChance >= 80) {
				placeItem(playerPosition, 1, (int)treasure.number);
			}
		}
		void placeItem(coordinates playerPosition, int iterations, int item) { //Supplements generateMap
			for (int i = 0; i < iterations; i++) {
				bool placed = false;
				while (!(placed)) {
					int random_x = rand() % MAX_X;
					int random_y = rand() % MAX_Y;
					if (!(playerPosition.x == random_x && playerPosition.y == random_y) && (grid[random_y][random_x] == (int)empty.number)) {
						grid[random_y][random_x] = item;
						placed = true;
					}
				}
			}
		}

		void removeItem(coordinates position) {
			grid[position.y][position.x] = (int)empty.number;
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

class Monster {
	private:
		const int MAX_HEALTH = 30;
		const int MAX_DAMAGE = 10;
		int health;
		int senseOfHumour; //0 is the worst sense of humour, 99 is the best
		int willingnessToSurrender; //0 is not willing at all, 99 is will fold immediately, anything in between is up to chance
		int surrenderThreshold; //The minimum number willingnessToSurrender has to be for the monster to surrender
		int damageChance;

	public:
		Monster() {
			health = (rand() % MAX_HEALTH) + 1;
			senseOfHumour = rand() % 100;
			surrenderThreshold = (rand() % 100) + 1; //Makes it so that if WTS is 0, it will never surrender
			willingnessToSurrender = rand() % 101; //WTS should be >= ST for player to successfully get monster to surrender
			damageChance = (rand() % 95) + 5;
		}

		int dropGold() {
			return (rand() % 4) + 5; //Every monster will drop 5-8 gold when defeated
		}

		//Setter - only setHealth because this is the only thing that should change
		void setHealth(int newHealth) {
			health = newHealth;
		}

		//Getters
		int getHealth() {
			return health;
		}
		int getSenseOfHumour() {
			return senseOfHumour;
		}
		int getWillingnessToSurrender() {
			return willingnessToSurrender;
		}
		int getSurrenderThreshold() {
			return surrenderThreshold;
		}
		int getDamageChance() {
			return damageChance;
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
		const int MAX_HEALTH = 50;
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
			if (inventory == NULL) {
				cout << "You check the mysterious bag on your back only to find absolutely nothing.\n";
			} else {
				item* current = inventory;
				cout << "You check the mysterious bag on your back to find that you have:\n";
				do {
					cout << current->quantity << " " << yellow(current->name) << '\n';
					current = current->next;
				} while (current != NULL);
			}
		}

		item* getItemPointer(string searchItem) {
			if (inventory == NULL) {
				return NULL;
			}

			item* current = inventory;
			while (current != NULL) {
				if ((current->name).compare(searchItem) == 0) {
					return current;
				}
				current = current->next;
			}
			return NULL;
		}

		int getQuantity(string searchItem) {
			item* position = getItemPointer(searchItem);
			if (position == NULL) {
				return -1;
			}
			return position->quantity;
		}

		void addItem(string name, int quantity, bool isCombatItem) {
			//Search for if it's there first, if so add to existing quantity, otherwise add node to linked list containing item

			item* position = getItemPointer(name);
			if (position != NULL) {
				position->quantity += quantity;
			} else {
				//Add to end of linked list
				
				item* newItem = new item {name, quantity, isCombatItem, NULL};
				if (inventory == NULL) {
					inventory = newItem;
				} else {
					item* current = inventory;
					while (current->next != NULL) {
						current = current->next;
					}
					current->next = newItem;
				}
			}
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
			if (amountOfGold < 50) {
				cout << '\n' << DOORKEEPER << "'s voice " << bold("rages") << ", quaking the very foundation of this universe.\n"; 
				cout << red("\"YOUR. GOLD. IS. INADEQUATE.\"\n");
				cout << DOORKEEPER << " hurls you away.\n";
				return false;
			}
			
			//some dialogue for 50+ gold (game won)
			return true;
		}
		void treasureInteraction() {
			//Choose random treasure, add to inventory
			//Treasure: sword, potion of health, gold, funny joke, bribe, armor

			string sword = yellow("sword");

			cout << yellow("\nWhat's this? ") << "you stumble upon";
			int item = rand() % 7;
			switch (item) {
				case 0:
					cout << " a " << sword << "!\n";
					if (getQuantity("sword") != -1) {
						cout << "Since this is not your first " << sword << ", you combine them to do " << yellow("more damage!\n");
					}
					addItem("sword", 1, true);
					break;
				case 1:
					cout << " a " << yellow("Potion of Health") << "! Use it to keep your health up!\n";
					addItem("potion_of_health", 1, true);
					break;
				case 2:
					{int quantity = (rand() % 3) + 1;
					cout << " " << quantity << yellow(" Gold") << "!\n";
					addItem("gold", quantity, false);};
					break;
				case 3:
					cout << "... a " << yellow("funny joke") << "? Huh. I'm sure that'll be useful.\n";
					addItem("funny_joke", 1, true);
					break;
				case 4:
					cout << "... an " << yellow("unfunny joke") << "? What a truly terrible joke. You wonder how this could possibly be useful to you.\n";
					addItem("unfunny_joke", 1, true);
					break;
				case 5:
					cout << "... wait, what is it? You're not sure, but you think it might be a " << yellow("bribe") << "! Better be careful with that...\n";
					addItem("bribe", 1, true);
					break;
				case 6:
					cout << " " << yellow("armor") << "! That'll get your defenses up!\n";
					addItem("armor", 1, false);
					break;
			}
		}

		int battleMonster() {
			/* return codes:
			  -1 - lost
			   0 - won
			   1 - ran away/surrendered
			*/

			Monster* thisMonster = new Monster;
			bool battleOver = false;

			while (!(battleOver)) {
				char input = '\0';

				cout << left << setw(20) << "\na: attack" << setw(20) << "i: item\n";
				cout << left << setw(20) << "s: force surrender" << setw(20) << "r: run away\n";

				while (!(input == 'a' || input == 'i' || input == 's' || input == 'r')) {
					cin >> input;
					//flush the input buffer
				}
			}

			delete thisMonster;
			return 0;
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
							newPosition.y = MAX_Y - 1;
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
						case 2:
							treasureInteraction();
							map.removeItem(newPosition);
							playerPosition = newPosition;
							returnCode = 1;
							break;
						case 3:
							if (yesOrNoInput(red("\nEngage in battle?\n")) == 'y') {
								int battleReturnCode = battleMonster();
								if (battleReturnCode == 0) { //If player wins, update coords
									map.removeItem(newPosition);
									playerPosition = newPosition;
									returnCode = 1;
								} else if (battleReturnCode == 1) { //If player runs away, nothing changes
									returnCode = 1;
								} else {
									returnCode = -1; //If player dies, game over (lost)
								}
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
						cout << "\nYour " << yellow("health") << " is at a healthy " << health << "." << endl;
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
	cout << "Use " << blue("p") << " to check your stats." << endl;
	cout << "Use " << blue("t") << " to leave your current area and teleport to " << magenta("The Doorkeeper") << "." << endl;

	cout << "\nWhen you reach a tile with a point of interest, you will automatically engage." << endl;
	if (gamestate == TUTORIAL) {
		cout << "Battle controls shall be revealed soon." << endl;
	} else {
		cout << "In battle, use " << i << " to use an item, " << blue("a") << " to attack, " << blue("s") << " to try to get them to surrender, and " << blue("r") << " to run away." << endl;
		cout << "These controls will be shown in the battle menu as well." << endl;
	}
	cout << "\nUse " << blue("h") << " to show this menu." << endl;
}
