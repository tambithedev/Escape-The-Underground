#include <iostream>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <random>
#include "terminal_colours.h"

using namespace std;
using namespace termcolour;

//Global variables initializing randomness engine
random_device rd;
mt19937 rng(rd());

enum state {
	START,
	TUTORIAL,
	OPEN_WORLD
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

		enum mapItem {
			EMPTY,
			WALL,
			PLAYER,
			MONSTER,
			DOORKEEPER,
			TREASURE
		};

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

			//Choose number of monsters to generate, 1-6 monsters per map 
			uniform_int_distribution<int> monsterChance(1,6);
			int numberOfMonsters = monsterChance(rng);
			bool placed = false;
			placeItem(playerPosition, numberOfMonsters, (int)monster.number);

			//Choose whether to generate treasure: 40% chance
			bernoulli_distribution treasureChance(0.75);
			if (treasureChance(rng)) {
				placeItem(playerPosition, 1, (int)treasure.number);
			}
		}
		void placeItem(coordinates playerPosition, int iterations, int item) { //Supplements generateMap
			for (int i = 0; i < iterations; i++) {
				bool placed = false;
				while (!(placed)) {
					uniform_int_distribution<int> random_x(0,MAX_X - 1);
					uniform_int_distribution<int> random_y(0,MAX_Y - 1);
					int try_x = random_x(rng);
					int try_y = random_y(rng);
					if (!(playerPosition.x == try_x && playerPosition.y == try_y) && (grid[try_y][try_x] == (int)empty.number)) {
						grid[try_y][try_x] = item;
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
		int senseOfHumour; //0 is the worst sense of humour, 100 is the best
		double surrenderChance; //0 is not willing at all, 100 is will fold immediately, anything in between is up to chance
		int attackChance;
		int defenseChance;
		int integrity;
		double persuadability;

	public:
		Monster() {
			uniform_int_distribution<int> healthChance(1,MAX_HEALTH);
			uniform_int_distribution<int> percentileDistribution(0,100);
			uniform_real_distribution<double> chanceDistribution(0.0,1.0);
			uniform_real_distribution<double> cappedChanceDistribution(0.0,0.3);
			uniform_int_distribution<int> attack(5,100);
			uniform_int_distribution<int> defense(10,40);

			health = healthChance(rng);
			senseOfHumour = percentileDistribution(rng);
			surrenderChance = cappedChanceDistribution(rng);
			attackChance = attack(rng);
			defenseChance = defense(rng);
			integrity = percentileDistribution(rng);
			persuadability = chanceDistribution(rng);
		}

		int dropGold() {
			uniform_int_distribution<int> goldDistribution(5,8); //Every monster will drop 5-8 gold when defeated
			return goldDistribution(rng);
		}

		int damagePlayer() {
			uniform_int_distribution<int> damageDistribution(1,MAX_DAMAGE);
			return damageDistribution(rng);
		}

		//Setters
		void reduceHealth(int modifier) {
			health -= modifier;
		}
		void changeSurrenderChance (double modifier) {
			surrenderChance += modifier;
			if (surrenderChance < 0) {
				surrenderChance = 0;
			}
		}
		void changePersuadability (double modifier) {
			persuadability += modifier;
			if (persuadability < 0) {
				persuadability = 0;
			}
			if (persuadability > 1) {
				persuadability = 1;
			}
		}
		void changeSenseOfHumour (int modifier) {
			senseOfHumour += modifier;
		}

		//Getters
		int getHealth() {
			return health;
		}
		int getSenseOfHumour() {
			return senseOfHumour;
		}
		double getSurrenderChance() {
			return surrenderChance;
		}
		int getAttackChance() {
			return attackChance;
		}
		int getDefenseChance() {
			return defenseChance;
		}
		double getIntegrity() {
			return integrity;
		}
		double getPersuadability() {
			return persuadability;
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
			health = 30;
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

		bool showCombatItems() {
			//Return true if at least one item, otherwise false
			if (inventory == NULL) {
				cout << "You have absolutely nothing, much less anything you can fight with.\n";
				return false;
			}

			int counter = 0;
			item* current = inventory;
			do {
				if (current->isCombatItem) {
					counter++;
					cout << counter << " - " << current->name << " (" << current->quantity << ")\n";
				}
				current = current->next;
			} while (current != NULL);
			
			if (counter == 0) {
				cout << "You find yourself with nothing to use in battle.\n";
				return false;
			}
			return true;
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

		item* getItemPointer(int position) {
			//This function only works for combat items by design
			if (position < 1) {
				return NULL;
			}
			if (inventory == NULL) {
				return NULL;
			}

			item* current = inventory;
			int counter = 0;
			while (current != NULL) {
				if (current->isCombatItem) {
					counter++;
					if (counter == position) {
						return current;
					}
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

		void useCombatItem(item* combatItem) {
			combatItem->quantity--;
			if (combatItem->quantity <= 0) {
				deleteItem(combatItem);
			}
		}
		void deleteItem(item* thisItem) { //This function could be overloaded with string search but there's no reason for that currently
			bool found = false;
			if (inventory == thisItem) {
				inventory = inventory->next;
			} else {
				item* previousItem = inventory;
				while (previousItem->next != NULL && !(found)) {
					if (previousItem->next == thisItem) {
						found = true;
					} else {
						previousItem = previousItem->next;
					}
				}
				previousItem->next = thisItem->next;
			}
			delete thisItem;
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
			
			cout << DOORKEEPER << " verbalizes in a language you do not understand. " << bold("You feel a shift in the universe") << ".\nSuddenly, you feel yourself freed... you have " << cyan("Escaped The Underground.\n");
			return true;
		}
		void treasureInteraction() {
			//Choose random treasure, add to inventory
			//Treasure: sword, potion of health, gold, funny joke, unfunny joke, bribe, armor

			string sword = yellow("sword");

			cout << yellow("\nWhat's this? ") << "you stumble upon";
			uniform_int_distribution<int> itemDistribution(0,6);
			int item = itemDistribution(rng);
			switch (item) {
				case 0:
					cout << " a " << sword << "!\n";
					if (getQuantity("sword") != -1) {
						cout << "Since this is not your first " << sword << ", you combine them to do " << yellow("more damage!\n");
					}
					addItem("sword", 1, false);
					break;
				case 1:
					cout << " a " << yellow("Potion of Health") << "! Use it to keep your health up!\n";
					addItem("potion_of_health", 1, true);
					break;
				case 2:
					{uniform_int_distribution<int> goldQuantity(1,3);
					int quantity = goldQuantity(rng);
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
			/* Return codes:
			   0 - won/monster surrendered
			   1 - lost
			   2 - ran away */
			
			Monster* thisMonster = new Monster;
			int prize = thisMonster->dropGold();
			int result = -1;
			string gold = yellow("Gold");
			bool triedSurrender = false;
			bool triedBribe = false;

			cout << red("\nMonster: \"GRRRRRRR\"\n");

			while (result == -1) {
				char input = '\0';

				cout << '\n';
				cout << left << setw(20) << "a: attack" << setw(20) << "i: item" << endl;;
				cout << left << setw(20) << "s: force surrender" << setw(20) << "r: run away" << endl;

				while (!(input == 'a' || input == 'i' || input == 's' || input == 'r')) {
					cin >> input;
					input = tolower(input);
					
					//Flush the input buffer, leave only '\n'
					while (cin.peek() != '\n') {
						cin.ignore();
					}
				}

				int monsterTurns = 1;
				bool showPlayerHealth = false;
				bool showMonsterHealth = false;
				cout << '\n';
				switch (input) {
					case 'a':
						//Damage calculation: a random number between 1 and 8 + 2^(number of swords) (if there are no swords, the bonus will be 0.5 and the number will be rounded down so effectively 0
						{
							uniform_int_distribution<int> percentileDistribution(0,100);
							uniform_int_distribution<int> damageDistribution(1,8);
							int swordBonus = getQuantity("sword");
							int attackRoll = percentileDistribution(rng);
							int damage = damageDistribution(rng) + pow(2,swordBonus);
							bool success = false;

							if (attackRoll >= 95) {
								cout << green("Critical hit!") << " You hit the monster for " << damage << " damage! You get an extra turn.\n";
								monsterTurns--;
								if (swordBonus > -1) {
									cout << "Your sword hit for an extra " << pow(2,swordBonus) << " damage.\n";
								}
								success = true;
								thisMonster->changeSurrenderChance(0.1);
							} else if (attackRoll <= 5) {
								cout << red("Critical miss!") << " The monster gets an extra turn!\n";
								monsterTurns++;
								thisMonster->changeSurrenderChance(-0.1);
							} else if (attackRoll > thisMonster->getDefenseChance()) {
								cout << green("Hit!") << " You hit the monster for " << damage << " damage.\n";
								if (swordBonus > -1) {
									cout << "Your sword hit for an extra " << pow(2,swordBonus) << " damage.\n";
								}
								success = true;
								thisMonster->changeSurrenderChance(0.05);
							} else {
								cout << red("Miss!") << " The monster dodged your attack.\n";
								thisMonster->changeSurrenderChance(-0.05);
							}

							if (success) {
								thisMonster->reduceHealth(damage);
								showMonsterHealth = true;
								if (thisMonster->getHealth() <= 0) {
									result = 0;
								}
							}
						}
						break;

					case 'i':
						if (showCombatItems()) {
							cout << "0 - cancel\n";
							int selection = -1;
							
							//Input which item to use
							while(selection < 0 || selection > 4) {
								cout << "Choose item: ";
								cin >> input;
								selection = input - '0';
								//Flush the input buffer, leaving only '\n'
								while (cin.peek() != '\n') {
									cin.ignore();
								}
							}

							//Find which item was selected
							item* thisItem = getItemPointer(selection);
							if (thisItem != NULL) {
								//Player has a 50% chance to not use a turn
								bernoulli_distribution noTurn(0.5);
								if (noTurn(rng)) {
									cout << green("Lucky!") << " You won't use up a turn when you use this item.\n\n";
									monsterTurns--;
								}

								bool monsterDamaged = false;
								uniform_int_distribution<int> bonus(1,3);
								int bonusChange = bonus(rng);

								if (thisItem->name == "potion_of_health") {
									uniform_int_distribution<int> increaseHealth(3,10);
									int addedHealth = increaseHealth(rng);
									cout << green("Whew!") << " You were healed for " << addedHealth << " health.\n";
									health += addedHealth;
									if (health >= MAX_HEALTH) {
										cout << "You're at " << green("MAX HEALTH!") << " Doctor who?\n";
										health = MAX_HEALTH;
									}

								} else if (thisItem->name == "bribe") {
									int monsterIntegrity = thisMonster->getIntegrity();
									if (monsterIntegrity >= 66) {
										cout << "This monster is an upstanding citizen who would never accept a bribe.\n";
										addItem("bribe", 1, true); //To "prevent" the bribe from being used up
									} else if (monsterIntegrity >= 33) {
										cout << red("Monster: \"Hmmm.\"\n");
										cout << "The monster looks conflicted...\nThe monster discretely takes your offering and " << green("gives you a gift") << " in exchange. You receive " << bonusChange << " " << gold << ".\n";
										addItem("gold", bonusChange, false);
									} else {
										cout << red("Monster: \"I'm out!\"\n");
										cout << "The monster takes your bribe and mysteriously disappears. " << green("Welp, that was easy!\n");
										result = 0;
									}
								} else {
									uniform_int_distribution<int> response(0,2);
									int jokeResponse = response(rng);

									if (thisItem->name == "funny_joke") {
										if (thisMonster->getSenseOfHumour() > 50) {
											cout << red("Monster: \"HAHAHAHA!\"\n");
											cout << "The monster";
											switch (jokeResponse) {
												case 0:
													cout << " laughs so hard that it has a stroke and " << green("dies!") << '\n';
													result = 0;
													break;
												case 1:
													cout << " laughs so much it gets stitches and " << green("loses health!") << '\n';
													thisMonster->reduceHealth(bonusChange);
													monsterDamaged = true;
													break;
												case 2:
													cout << " likes your joke.\n";
													break;
											}
										} else {
											cout << red("Monster: \"...\"\n");
											cout << "The monster doesn't get it";
											switch (jokeResponse) {
												case 0:
													cout << ".\n";
													break;
												case 1:
													cout << ", but appreciates you improving its sense of humour through good quality jokes. It gives you a gold tip! You gain " << bonusChange << " " << gold << ".\n";
													addItem("gold", bonusChange, false);
													thisMonster->changeSenseOfHumour(5);
													break;
												case 2:
													cout << ". It looks so sad about its low humour intelligence that you decide to " << red("give it another turn") << ".\n";
													monsterTurns++;
													break;
											}
										}
									} else {
										if (thisMonster->getSenseOfHumour() < 50) {
											cout << red("Monster: \"Hehehehe!\"\n");
											cout << "The monster ";
											switch (jokeResponse) {
												case 0:
													cout << "enjoys your bad joke so much it has a stroke and " << green("dies!") << '\n';
													result = 0;
													break;
												case 1:
													addItem("gold", bonusChange, false);
													cout << "thinks your terrible joke is funny! It gives you a gold tip! You gain " << bonusChange << " " << gold << ".\n";
													break;
												case 2:
													cout << "chuckles, although it's not entirely sure whether that was supposed to be funny.\n";
													break;
											}
										} else {
											cout << "The monster doesn't find your horrendous joke funny. ";
											switch (jokeResponse) {
												case 0:
													cout << '\n';
													break;
												case 1:
													cout << "It loses so many brain cells because of how stupid your joke is that it " << green("loses health") << ".\n";
													thisMonster->reduceHealth(bonusChange);
													monsterDamaged = true;
													break;
												case 2:
													cout << "In fact, it finds your joke so offensively bad that it's going to " << red("attack you one extra time.\n");
													monsterTurns++;
													break;
											}
										}
									}
								}

								useCombatItem(thisItem);
								if (monsterDamaged) {
									if (thisMonster->getHealth() <= 0) {
										result = 0;
									} else {
										showMonsterHealth = true;
									}
								}
							} else {
								if (input != '0') {
									cout << "Invalid selection, try again.\n";
								}
								monsterTurns--;
							}
						} else {
							monsterTurns--;
						}
						break;

					case 's':
						{
						cout << "You use honeyed words to try to get the monster to surrender...\n";
						bernoulli_distribution surrender(thisMonster->getSurrenderChance());

						if (thisMonster->getSurrenderChance() < 0.05 && thisMonster->getPersuadability() < 0.15) { //Balancing
							cout << "You get the divine sense that this monster will not give up. You're not going to try.\n";
							monsterTurns--;
						} else if (surrender(rng)) {
							cout << green("It worked!") << " The monster runs away! So much for never give up, never what?\n";
							result = 0;
						} else {
							cout << "You fail.\n";
							if (triedSurrender) {
								bernoulli_distribution persuade(thisMonster->getPersuadability());
								if (persuade(rng)) {
									cout << "But you have a feeling you might just get it to yield... \n";
									thisMonster->changeSurrenderChance(0.05);
									thisMonster->changePersuadability(0.05);
								} else {
									cout << "You sense the monster getting more agitated with your repeated requests...\n";
									thisMonster->changeSurrenderChance(-0.05);
									thisMonster->changePersuadability(-0.05);
								}
							}
							triedSurrender = true;
						}
						break;
						}

					case 'r':
						cout << "You run away from the monster! Hopefully that was the right choice...\n";
						result = 2;
						break;
					}

				if (result == 0) {
					cout << green("You won the battle!") << " The monster drops " << prize << " " << gold << ".\n";
					addItem("gold", prize, false);
				} else if (result == -1) {
					for (int i = 0; i < monsterTurns; i++) {
						//Armor defense calculation: +10 defense for every piece of armor
						int armorProtection = 10 * (getQuantity("armor"));
						if (armorProtection < 0) {
							armorProtection = 0;
						}

						if (i >= 1) {
							cout << "The monster uses its extra turn!\n";
						}
						uniform_int_distribution<int> percentileDistribution(0,100);
						int defenseRoll = percentileDistribution(rng) + armorProtection;
						if (defenseRoll >= 95) {
							cout << green("Critical dodge!") << " The monster tries to hit you, but you're untouchable!\n";
						} else if (defenseRoll >= thisMonster->getAttackChance()) {
							thisMonster->changeSurrenderChance(0.1);
							cout << green("Dodge!") << " The monster strikes at you, but it misses.\n";
							thisMonster->changeSurrenderChance(0.05);
						} else {
							int damage = thisMonster->damagePlayer();
							health -= damage;
							cout << red("Ouch!") << " The monster hits you for " << damage << " damage.\n";
							thisMonster->changeSurrenderChance(-0.05);
							if (health <= 0) {
								cout << red("\nAnd just like that, you see the light.\n");
								cout << red("You died...") << " but you could always get trapped in The Underground again.\n";
								result = 1;
							} else if (health <= 10) {
								cout << red("You see your life flash before your eyes...\n");
							}
							showPlayerHealth = true;
						}
					}
				}
				if (result == -1) { //Have to repeat this condition because of the case of player losing
					if (showPlayerHealth) {
						cout << "Your health is " << health << ". ";
					}
					if (showMonsterHealth) {
						cout << "The monster's health is " << thisMonster->getHealth() << ".";
					}
				}

			}

			delete thisMonster;
			return result;
		}

		void openWorldPrompt() {
			cout << "Use w,a,s,d to move or h for help: ";
		}

		void openWorldControls(Map &map) {
			char input = '\0';
			int tryMoveReturnCode = -1;
			bool movePlayer = true;

			while (!(input == 'w' || input == 'a' || input == 's' || input == 'd' || input == 'h' || input == 't' || input == 'p')) {
				map.printMap(playerPosition);
				openWorldPrompt();
				cin >> input;
				input = tolower(input);
				//I do not clear the input buffer here as that is how multiple inputs at once are accepted
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
								if (battleReturnCode == 0) { //If player wins or monster surrenders, update coords
									map.removeItem(newPosition);
									playerPosition = newPosition;
									returnCode = 1;
								} else if (battleReturnCode == 1) { //If player dies, game over (lost)
									returnCode = -1;
								} else {
									returnCode = 1; //If player runs away, nothing changes
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
		default_player.openWorldControls(default_map);
	}

	//Clear input buffer - if no bugs, only '\n' should be left
	cin.ignore();
	cout << "\nEnter any key to exit:\n";
	cin.get();
	cout << "Thanks for playing.\n";

	return 0;
}

char yesOrNoInput(string repeatInstruction) {
	char yn = '\0';
	while (!(tolower(yn) == 'y' || tolower(yn) == 'n')) {
		cout << repeatInstruction << " [y/n]: ";
		cin >> yn;
		//Clear input buffer, leaving only '\n'
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
	cout << "You will have to " << red("fight to the death") << " to escape... or will you?" << endl;
	cout << magenta("Monsters") << " can be found randomly throughout " << underground << ". The only way to escape is to get " << yellow("Gold") << " from Monsters to give to " << doorkeeper << "." << endl;
	cout << doorkeeper << " can be found in " << magenta("The Heart of ") << underground << ". The areas outside of this place are never constant, ever shifting. Once you leave an area, you may never find another one like it." << endl;
	cout << "Each new are you find will have a certain chance to spawn " << magenta("treasure") << ". The items you might find will be crucial for battle, so explore as much as you can or wish to.\n";
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
		cout << "When you use an item, you have a 50% chance to get an " << green("extra turn") << ".\n";
		cout << green("Tip:") << " Be careful trying to get monsters to surrender or giving them a bribe - some might be swayed eventually, while others might get more stubborn the more you try to push the issue.\n";
		cout << green("Tip:") << " Try to understand the monster's personality. Some actions will work better on certain monsters than on others.\n";
	}
	cout << "\nUse " << blue("h") << " to show this menu." << endl;
}
