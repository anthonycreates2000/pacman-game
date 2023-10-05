#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
int choice = 0;
int tempValue = 0;
// Name: Anthony Kevin Oktavius
// NIM: 2301862866
// Using Circular linked list
// One of the ghost uses a breadth-first-search technique to search Pacman!

enum Direction {up = 0, down = 1,left = 2, right = 3, stay = 4};
struct Node{
	char value;
	int nodeNumber;
	int ableEnemyToTraverse;
	struct Node* enemyLastPathDir;
	struct Node* left;
	struct Node* right;
	struct Node* up;
	struct Node* down;
} typedef Node;

struct NodeQueue{
	struct Node* pathNode;
	struct NodeQueue* next;
	struct NodeQueue* prev;
} typedef NodeQueue;

struct QueueLinkedList{
	struct NodeQueue* head;
	struct NodeQueue* tail;
} typedef QueueLinkedList;

struct RandomEnemyProp{
	enum Direction directionMovement[12];
	short int currentIndex;
	struct Node* currentPath;
} typedef RandomEnemyProp;

enum Direction directionArray[12] = {left, right, up, down, left, right, up, down, left, right, up, down};

struct BashfulEnemyProp{
	enum Direction directionMovement;
	short int counterMovement;
	struct Node* currentPath;
} typedef BashfulEnemyProp;

Node* topLeftPath = NULL;
Node* pathHead = NULL;
Node* previousPath = NULL;
Node* temp = NULL;
Node* pathTail = NULL;
Node* pacmanPosition = NULL;
Node* currHeadPath = NULL;
Node* currPath = NULL;
Node* enemyPath = NULL;
RandomEnemyProp *randomEnemy = NULL;
BashfulEnemyProp *bashfulEnemy = NULL;
long long int randomizerNumber = 0;
short int isGameOver = 0;
int currentScore = 0;
int highScore = 0;
char mazeRep[600] = {};
short int currentMapIndex = 0;
int collisions = 0;
int timeNumber = 0;
long long int timeGetValue = 0;
NodeQueue* tempNodeQueue = NULL;

Node* oneNode(char value){
	temp = (Node*) malloc(sizeof(Node));
	temp->enemyLastPathDir = NULL;
	temp->ableEnemyToTraverse = 0;
	temp->value = value;
	temp->left = NULL;
	temp->right = NULL;
	temp->up = NULL;
	temp->down = NULL;
	return temp;
}
void boldCyan(){
	printf("\033[1;36m");
}
void yellowColor(){
	printf("\033[01;33m");
}
void greenColor(){
	printf("\033[1;32m");
}
void resetColor(){
	printf("\033[0m");
}
void switchNumber(enum Direction direction[], int indexOne, int indexTwo){
	enum Direction temp = direction[indexOne];
	direction[indexOne] = direction[indexTwo];
	direction[indexTwo] = temp;
}
// Random techniques using Fisher-Yates Algorithm. One of the monster implements it.

void generateRandomPosition(){
	int i = 0;
	int elementToBeSwitched = 0;
	for (i = 0; i < 12; i++){
		randomEnemy->directionMovement[i] = directionArray[i];
	}
	for (i = 0; i < 12; i++){
		if (timeGetValue < 10){
			srand(0);
			timeGetValue += rand();
		}
		timeGetValue = timeGetValue >> 1;
		elementToBeSwitched = i + (timeGetValue % (12 - i));
		switchNumber(randomEnemy->directionMovement, i, elementToBeSwitched);
	}
}

int determineIfNotWall(Node* path){
	if (path != NULL && path->value != '=' && path->value != '|' && path->value != '-'){
		return 1;
	}
	else{
		return -1;
	}
}

NodeQueue* oneNodeQueue(Node* path){
	tempNodeQueue = (NodeQueue*) malloc(sizeof(NodeQueue));
	tempNodeQueue->pathNode = path;
	tempNodeQueue->next = NULL;
	tempNodeQueue->prev = NULL;
	return tempNodeQueue;
}
void printLine(char message[]){
	printf("%s\n", message);
}
void modifyPath(Node* path, short int isVisitedNodes[]){
	isVisitedNodes[path->nodeNumber] = 1;
}

void getMapDataFromFile(){
	FILE* file = fopen("Maze.txt", "r");
	char fileChar;
	int wasNewline = 0;
	int numberOfNodes = 0;
	int hasTopNode = 0;
	while (fscanf(file, "%c", &fileChar) != EOF){
		if (topLeftPath == NULL){
			pathHead = oneNode(fileChar);
			pathHead->nodeNumber = numberOfNodes;
			pathTail = pathHead;
			topLeftPath = pathHead;
			numberOfNodes+=1;
		}
		else if (fileChar != '\n'){
			if(wasNewline == 1){
				pathHead->down = oneNode(fileChar);
				pathHead->down->up = pathHead;
				previousPath = pathHead;
				pathHead = pathHead->down;
				pathHead->nodeNumber = numberOfNodes;
				pathTail = pathHead;
				wasNewline = 0;
			}
			else{
				pathTail->right = oneNode(fileChar);
				pathTail->right->left = pathTail;
				if (hasTopNode != 0){
					pathTail->up = previousPath;
					previousPath->down = pathTail;
					previousPath = previousPath->right;
				}
				pathTail = pathTail->right;
				pathTail->nodeNumber = numberOfNodes;
			}
			if (fileChar == '$'){
				pacmanPosition = pathTail;
				pathTail->value = ' ';
			}
			else if (fileChar == ';'){
				enemyPath = pathTail;
				pathTail->value = ' ';
			}
			else if (fileChar == '?'){
				randomEnemy->currentPath = pathTail;
				pathTail->value = ' ';
			}
			else if (fileChar == '%'){
				bashfulEnemy->currentPath = pathTail;
				pathTail->value = ' ';
			}
			numberOfNodes+=1;
		}
		else{
			pathTail->right = pathHead;
			pathHead->left = pathTail;
			wasNewline = 1;
			hasTopNode+=1;
		}
	}
	fclose(file);
}

void printMaze2D(){
	currHeadPath = topLeftPath;
	currPath = currHeadPath;
	currentMapIndex = 0;
	while (currHeadPath != NULL){
		do {
			if (currPath == pacmanPosition){
				mazeRep[currentMapIndex++] = '$';
			}
			else if (currPath == enemyPath){
				mazeRep[currentMapIndex++] = ';';
			}
			else if (currPath == randomEnemy->currentPath){
				mazeRep[currentMapIndex++] = '?';
			}
			else if (currPath == bashfulEnemy->currentPath){
				mazeRep[currentMapIndex++] = '%';
			}
			else{
				mazeRep[currentMapIndex++] = currPath->value;
			}
			currPath->enemyLastPathDir = NULL;
			currPath = currPath->right;
		}while(currPath != currHeadPath);
		mazeRep[currentMapIndex++] = '\n';
		currHeadPath = currHeadPath->down;
		currPath = currHeadPath;
	}
	printf("%s", mazeRep);
}

int giveRangedValues(int min, int max, char message[]){
	do{
		printf("%s", message);
		scanf("%i", &tempValue);
		getchar();
	}while(tempValue < min || tempValue > max);
	return tempValue;
}

// 1 for pacman, 0 for enemies

Node* switchPosition(Node* oldNode, Node* newNode, int enemyOrPacman){
	if (determineIfNotWall(newNode) == 1){
		if (newNode->value == '.' && enemyOrPacman == 1){
			newNode->value = ' ';
			currentScore += 100;
		}
		return newNode;
	}
	return oldNode;
}
void printPlayerStatistics(){
	printLine("");
 	greenColor();
	printLine("==================================");
	printf("Score: %i\n", currentScore);
	printf("High Score: %i\n", highScore);
	printLine("==================================");
	resetColor();
	printLine("");
}
void printFromFile(char path[]){
	FILE* file = fopen(path, "r");
	while (fscanf(file, "%[^\n]\n", mazeRep) != EOF){
		printf("%s\n", mazeRep);
	}
	fclose(file);
}

NodeQueue* pushTail(NodeQueue* nodeHead, NodeQueue* nodeTail, NodeQueue* newNode){
	if (nodeTail == NULL){
		nodeTail = newNode;
	}
	else{
		nodeTail->next = newNode;
		newNode->prev = nodeTail;
		nodeTail = nodeTail->next;
	}
	return nodeTail;
}

NodeQueue* popHead(NodeQueue* nodeHead, NodeQueue* nodeTail){
	if (nodeHead == NULL){
		return NULL;
	}
	else if (nodeHead == nodeTail){
		free(nodeHead);
		return NULL;
	}
	else{
		nodeHead = nodeHead->next;
		free(nodeHead->prev);
		nodeHead->prev = NULL;
	}
	return nodeHead;
}

void insertPathForEnemy(Node* currQueuePath){
	currPath = currQueuePath;
	currPath->enemyLastPathDir = currQueuePath;
}
int checkIfAbleTraverse(Node* path, short int isVisitedNodes[]){
	if (path != NULL && isVisitedNodes[path->nodeNumber] != 1 && determineIfNotWall(path) == 1){
		return 1;
	}
	return -1;
}

void constructPaths(Node* sourcePath){
	Node* currentPath = sourcePath;
	while (currentPath != NULL){
		currentPath->ableEnemyToTraverse = 1;
		currentPath = currentPath->enemyLastPathDir;
	}
}

void doBFSSearchEnemy(){
	short int isVisitedNodes[500] = {};
	QueueLinkedList *queueNodes = (QueueLinkedList*) malloc(sizeof(QueueLinkedList));
	queueNodes->head = NULL;
	queueNodes->tail = NULL;
	Node* currQueuePath = NULL;
	queueNodes->tail = pushTail(queueNodes->head, queueNodes->tail, oneNodeQueue(enemyPath));
	queueNodes->head = queueNodes->tail;
	modifyPath(enemyPath, isVisitedNodes);
	while (queueNodes->head != NULL){
		currQueuePath = queueNodes->head->pathNode;
		currPath = currQueuePath;
		if (checkIfAbleTraverse(currQueuePath->left, isVisitedNodes) == 1){
			queueNodes->tail = pushTail(queueNodes->head, queueNodes->tail, oneNodeQueue(currQueuePath->left));
			currPath->left->enemyLastPathDir = currQueuePath;
			if (currQueuePath->left == pacmanPosition){
				constructPaths(currQueuePath->left);
				break;
			}
			modifyPath(currQueuePath->left, isVisitedNodes);
		}
		if (checkIfAbleTraverse(currQueuePath->right, isVisitedNodes) == 1){
			queueNodes->tail = pushTail(queueNodes->head, queueNodes->tail, oneNodeQueue(currQueuePath->right));
			currPath->right->enemyLastPathDir = currQueuePath;
			if (currQueuePath->right == pacmanPosition){
				constructPaths(currQueuePath->right);
				break;
			}
			modifyPath(currQueuePath->right, isVisitedNodes);
		}
		if (checkIfAbleTraverse(currQueuePath->down, isVisitedNodes) == 1){
			queueNodes->tail = pushTail(queueNodes->head, queueNodes->tail, oneNodeQueue(currQueuePath->down));
			currPath->down->enemyLastPathDir = currQueuePath;
			if (currQueuePath->down == pacmanPosition){
				constructPaths(currQueuePath->down);
				break;
			}
			modifyPath(currQueuePath->down, isVisitedNodes);
		}
		if (checkIfAbleTraverse(currQueuePath->up, isVisitedNodes) == 1){
			queueNodes->tail = pushTail(queueNodes->head, queueNodes->tail, oneNodeQueue(currQueuePath->up));
			currPath->up->enemyLastPathDir = currQueuePath;
			if (currQueuePath->up == pacmanPosition){
				constructPaths(currQueuePath->up);
				break;
			}
			modifyPath(currQueuePath->up, isVisitedNodes);
		}
		queueNodes->head = popHead(queueNodes->head, queueNodes->tail);
	}
	while (queueNodes->head != NULL){
		queueNodes->head = popHead(queueNodes->head, queueNodes->tail);
	}
	queueNodes->tail = NULL;
	free(queueNodes);
	queueNodes = NULL;
}
void readyMoveEnemy(Node* path){
	path->ableEnemyToTraverse = 0;
	enemyPath = switchPosition(enemyPath, path, 2);
}

void moveSmartEnemy(){
	enemyPath->ableEnemyToTraverse = 0;
	if (enemyPath->right->ableEnemyToTraverse == 1){
		readyMoveEnemy(enemyPath->right);
	}
	else if (enemyPath->left->ableEnemyToTraverse == 1){
		readyMoveEnemy(enemyPath->left);
	}
	else if (enemyPath->up->ableEnemyToTraverse == 1){
		readyMoveEnemy(enemyPath->up);
	}
	else if (enemyPath->down->ableEnemyToTraverse == 1){
		readyMoveEnemy(enemyPath->down);
	}
	else{
		doBFSSearchEnemy();
	}
}

Node* whichNodeToMove(Node* currentEnemyPath, enum Direction direction){
	if (direction == left && determineIfNotWall(currentEnemyPath->left) == 1){
		return currentEnemyPath->left;
	}
	else if (direction == right && determineIfNotWall(currentEnemyPath->right) == 1){
		return currentEnemyPath->right;
	}
	else if (direction == up && determineIfNotWall(currentEnemyPath->up) == 1){
		return currentEnemyPath->up;
	}
	else if (direction == down && determineIfNotWall(currentEnemyPath->down) == 1){
		return currentEnemyPath->down;
	}
	return currentEnemyPath;
}

int countAvailablePaths(Node* path){
	int count = 0;
	if (determineIfNotWall(path->right) == 1){
		count+=1;
	}
	if (determineIfNotWall(path->left) == 1){
		count+=1;
	}
	if (determineIfNotWall(path->up) == 1){
		count+=1;	
	}
	if (determineIfNotWall(path->down) == 1){
		count+=1;
	}
	return count;
}
enum Direction giveOppositeDirection(enum Direction direction){
	if (direction == left){
		return right;
	}
	else if (direction == right){
		return left;
	}
	else if (direction == up){
		return down;
	}
	else if (direction == down){
		return up;	
	}
	else{
		return stay;
	}
}
void doFisherYates(){
	if (randomEnemy->currentIndex > 9){
		randomEnemy->currentIndex = 0;
		generateRandomPosition();
	}
	else{
		randomEnemy->currentIndex += 1;
	}
}
Node* nextNode = NULL;
void moveRandomEnemy(){
	nextNode = NULL;
	int availablePaths = countAvailablePaths(randomEnemy->currentPath);
	if (availablePaths > 2){
		while (whichNodeToMove(randomEnemy->currentPath, randomEnemy->directionMovement[randomEnemy->currentIndex]) == randomEnemy->currentPath ){
			doFisherYates();
		}
		nextNode = whichNodeToMove(randomEnemy->currentPath, randomEnemy->directionMovement[randomEnemy->currentIndex]);
	}
	else if (availablePaths == 2){
		while (whichNodeToMove(randomEnemy->currentPath, randomEnemy->directionMovement[randomEnemy->currentIndex]) == randomEnemy->currentPath){
			doFisherYates();
		}
		nextNode = whichNodeToMove(randomEnemy->currentPath, randomEnemy->directionMovement[randomEnemy->currentIndex]);
	}
	else if (availablePaths == 1){
		nextNode = whichNodeToMove(randomEnemy->currentPath, giveOppositeDirection(randomEnemy->directionMovement[randomEnemy->currentIndex]));
		randomEnemy->directionMovement[randomEnemy->currentIndex] = giveOppositeDirection(randomEnemy->directionMovement[randomEnemy->currentIndex]);
	}
	randomEnemy->currentPath = switchPosition(randomEnemy->currentPath, nextNode, 2);
}
// Start from here!
int checkLinearPacmanPosition(Node* path, enum Direction direction){
	int counter = 0;
	while (counter < 10 && determineIfNotWall(path) == 1){
		counter += 1;
		if (path == pacmanPosition){
			return 1;
		}
		if (direction == left){
			path = path->left;
		}
		else if (direction == right){
			path = path->right;
		}
		else if (direction == up){
			path = path->up;
		}
		else if (direction == down){
			path = path->down;
		}
	}
	return -1;
}

void moveBashfulEnemy(){
	if (countAvailablePaths(bashfulEnemy->currentPath) > 2 || whichNodeToMove(bashfulEnemy->currentPath, bashfulEnemy->directionMovement) == bashfulEnemy->currentPath){
		if (checkLinearPacmanPosition(bashfulEnemy->currentPath, left) == 1){
			bashfulEnemy->directionMovement = left;
		}
		else if (checkLinearPacmanPosition(bashfulEnemy->currentPath, right) == 1){
			bashfulEnemy->directionMovement = right;
		}
		else if (checkLinearPacmanPosition(bashfulEnemy->currentPath, up) == 1){
			bashfulEnemy->directionMovement = up;
		}
		else if (checkLinearPacmanPosition(bashfulEnemy->currentPath, down) == 1){
			bashfulEnemy->directionMovement = down;
		}
		else{
			bashfulEnemy->directionMovement = stay;
		}
	}
	Node* nextPath = whichNodeToMove(bashfulEnemy->currentPath, bashfulEnemy->directionMovement);
	bashfulEnemy->currentPath = switchPosition(bashfulEnemy->currentPath, nextPath, 2);
}
int determineIfGhost(Node* path){
	if (path == enemyPath || path == bashfulEnemy->currentPath || path == randomEnemy->currentPath){
		return 1;
	}
	return -1;
}
void resetGameplay(){
	srand(time(0));
	timeGetValue = rand() + 123;
	randomizerNumber = 0;
	currentScore = 0;
	isGameOver = 0;
	randomEnemy->currentPath = NULL;
	randomEnemy->currentIndex = 100;
	bashfulEnemy->currentPath = NULL;
	bashfulEnemy->directionMovement = stay;
	currentMapIndex = 0;
	pacmanPosition = NULL;
}
void clearMapData(){
	temp = topLeftPath;
	pathHead = temp;
	Node* nodeToBeDeleted = NULL;
	while (pathHead != NULL){
		temp = pathHead;
		pathHead = pathHead->down;
		while (temp->right != NULL){
			nodeToBeDeleted = temp;
			temp = temp->right;
			nodeToBeDeleted->up = NULL;
			nodeToBeDeleted->right = NULL;
			nodeToBeDeleted->left = NULL;
			nodeToBeDeleted->down = NULL;
			free(nodeToBeDeleted);
		}
	}
	topLeftPath = NULL;
	pathHead = NULL;
	pathTail = NULL;
}
void mainGame(){
	char inputValue = 'S';
	getMapDataFromFile();
	generateRandomPosition();
	DWORD start_time, check_time;
	do{
		system("cls");
		printPlayerStatistics();
		printMaze2D();
		start_time = GetTickCount();
		check_time = start_time + 250;
		printLine("Use w, a, s, d to control Pacman ($)!");
		printLine("This is just a debugging process! Press any key to test!");
		while (check_time > GetTickCount()){
			if (kbhit()){
				inputValue = getch();
			}
		}
		if (pacmanPosition == enemyPath || pacmanPosition == randomEnemy->currentPath || pacmanPosition == bashfulEnemy->currentPath){
			isGameOver = 1;
			break;
		}
		else if (inputValue == 'W' || inputValue == 'w'){
			
			pacmanPosition = switchPosition(pacmanPosition, pacmanPosition->up, 1);
		}
		else if (inputValue == 'A' || inputValue == 'a'){
			if (determineIfGhost(pacmanPosition->left) == 1){
				isGameOver = 1;
				break;
			}
			pacmanPosition = switchPosition(pacmanPosition, pacmanPosition->left, 1);
		}
		else if (inputValue == 'S' || inputValue == 's'){
			if (determineIfGhost(pacmanPosition->down) == 1){
				isGameOver = 1;
				break;
			}
			pacmanPosition = switchPosition(pacmanPosition, pacmanPosition->down, 1);
		}
		else if (inputValue == 'D' || inputValue == 'd'){
			if (determineIfGhost(pacmanPosition->right) == 1){
				isGameOver = 1;
				break;
			}
			pacmanPosition = switchPosition(pacmanPosition, pacmanPosition->right, 1);
		}
		moveSmartEnemy();
		moveBashfulEnemy();
		moveRandomEnemy();
	}while(isGameOver == 0);
	system("cls");
	printLine("======================================================");
	if (isGameOver == 1){
		printLine("Your player has hit the ghost! You lose the game!\n");
		printLine("The end.");
	}
	else{
		printLine("Congratulations! You won the game!");
		printLine("Pac-Man has lived happily ever after!");
	}
	if (currentScore > highScore){
		printLine("Congratulations! It's a high score!");
		highScore = currentScore;
	}
	clearMapData();
	printLine("======================================================");
}

void mainMenu(){
	
	bashfulEnemy = (BashfulEnemyProp*) malloc(sizeof(BashfulEnemyProp));
	randomEnemy = (RandomEnemyProp*) malloc(sizeof(RandomEnemyProp));
	do{
		resetGameplay();
		system("cls");
		boldCyan();
		printLine("Welcome to Pacman Game!");
		printLine("Please select what would you like to do...");
		yellowColor();
		printLine("1. Play Game");
		printLine("2. High Score");
		printLine("3. How to Play");
		printLine("4. Exit");
		resetColor();
		choice = giveRangedValues(1, 4, "Choose: ");
		if (choice == 1){
			mainGame();
		}
		else if (choice == 2){
			if (highScore != 0){
				printf("Your high score is: %i\n", highScore);
			}
			else{
				printLine("You haven't played my game yet! Let's play together!");
			}
		}
		else if (choice == 3){
			printFromFile("How to Play.txt");
		}
		else if (choice == 4){
			printLine("Goodbye!");	
		}
		getchar();
	}while(choice != 4);
}
int main(){
	mainMenu();
	return 0;
}
