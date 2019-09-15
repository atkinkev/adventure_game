/*****************************************************************************
 * Kevin Atkins
 * 2/12/2019
 * atkinkev.adventure.c
 * Program 2
 * ***************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>

enum roomType{START = 1, MID = 2, END = 3};

// defining our mutex globally so we can access it anywhere for attempted locks/unlocks
pthread_mutex_t mutex;

// room struct
typedef struct{
int id;
char* name;
int connections[6];
int roomType;
} room;


/*****************************************************************************
hasUserWon(): simple check to see if user is in END_ROOM
*****************************************************************************/
int hasUserWon(room currentRoom){
	if (currentRoom.roomType == END){
		return 1;
	}
	return 0;
}

/*****************************************************************************
 * getRoomDirect(): returns most recent room directory in current directory
 * SOURCES: 
 * https://pubs.opengroup.org/onlinepubs/007904875/basedefs/dirent.h.html
 * https://stackoverflow.com/questions/42170824/use-stat-to-get-most-recently-modified-directory
 * **************************************************************************/
char* getRoomDirect(){
	struct dirent *dp;
	struct stat dStat;
	struct stat fStat;
	struct tm lt;
	DIR* dir;
	time_t tLatest;
	char timbuf[80];
	char date[36];
	char* mostRecent;
	tLatest = 0;

	mostRecent = malloc(1024);
	
	// open current directory
	dir = opendir(".");

	// read all files in directory
	while ((dp = readdir(dir)) != NULL){
		// clear out contents of last read into &dStat
		memset(&dStat, 0, sizeof(dStat));
		
		// make sure we can even get data on this dir without error
            	if (stat(dp->d_name, &dStat) < 0) {
                	continue;
            	}
	
		// make sure we're just grabbing directories
		if ((dStat.st_mode & S_IFDIR) != S_IFDIR){
			continue;
		}

		// check for hidden directories and exit directory (..) as these are always the most recent and 
		// not what we want
		if (dStat.st_mtime > tLatest && strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
			// if this directory is more recent than our last saved recent, update most recent
			memset(mostRecent, '\0', sizeof(mostRecent));
			strcpy(mostRecent, dp->d_name);
			tLatest = fStat.st_mtime;
		}
	}
			
	closedir(dir);
	return mostRecent;
}

/******************************************************************************************
* interpretRead(): Reference int id from room name. Also used for reading in files to determine 
* room type.
*******************************************************************************************/
int interpretRead(char* name){
	if (strcmp(name, "Parlour") == 0){
		return 0;
	}
	else if (strcmp(name, "Dungeon") == 0){
		return 1;
	}
        else if (strcmp(name, "Altar") == 0){
                return 2;
        }
        else if (strcmp(name, "Portal") == 0){
                return 3;
        }
        else if (strcmp(name, "Garden") == 0){
                return 4;
        }
        else if (strcmp(name, "Kitchen") == 0){
                return 5;
        }
        else if (strcmp(name, "Throne") == 0){
                return 6;
        }
        else if (strcmp(name, "Void") == 0){
                return 7;
        }
        else if (strcmp(name, "Workshop") == 0){
                return 8;
        }
        else if (strcmp(name, "Mess") == 0){
                return 9;
        }
        else if (strcmp(name, "START_ROOM") == 0){
                return 10;
        }
        else if (strcmp(name, "MID_ROOM") == 0){
                return 11;
        }
        else if (strcmp(name, "END_ROOM") == 0){
                return 12;
        }
	else if (strcmp(name, "time") == 0){
		return 13;
	}
	return -1;
}

/******************************************************************************************
numToName(): Reference room name from associated int id 
******************************************************************************************/
char* numToName(int i){
	switch (i){
		case 0: return "Parlour";
		case 1: return "Dungeon";
		case 2: return "Altar";
		case 3: return "Portal";
		case 4: return "Garden";
		case 5: return "Kitchen";
		case 6: return "Throne";
		case 7: return "Void";
		case 8: return "Workshop";
		case 9: return "Mess";
		default: return "Error, invalid name index";
	}
}


/******************************************************************************************
getRoomData(): gets room file for jth index of graph in the given directory. Returns a room struct 
*****************************************************************************************/
room* getRoomData(int j, char* roomDirect){
	room* newRoom;
	char num[2];
	char buff[255];
	char path[255];
	int currentRead, fileParser, i;

	// we don't want to disturb the roomDirect pointer, so copy it over locally
	strcpy(path, roomDirect);
	strcat(path, "/room");
	sprintf(num, "%d", (j+1));
	strcat(path, num);

	// open file room(i)	
	FILE* droom = fopen(path, "r");

	newRoom = malloc(sizeof(room));
	newRoom->name = malloc(10);

	// initialize connections to -1 for checking for empty slot
	for(i=0;i<6;i++){
		newRoom->connections[i] = -1;
	}
	
	// get 3rd white space delimitted string (name)
	fileParser = 0;
	while (fscanf(droom, "%*s %*s %s", buff) == 1){
		currentRead = interpretRead(buff);
		// first grab will always be the name and retrun value from read interpreter
		if (currentRead <= 9 && fileParser == 0){
			strcpy(newRoom->name, buff);
			newRoom->id = currentRead;
		}
		// if its not the name of the room, it must be our connections
		else if (currentRead <= 9){
			for(i=0;i<6;i++){
				if(newRoom->connections[i] == -1){
					newRoom->connections[i] = currentRead;
					break;
				}
			}
		}
		// room types return are returned at vals 9-11
		else if (currentRead > 9){
			newRoom->roomType = (currentRead - 9);
		}
		fileParser++;
	}

	// closeup our file and return the read room struct
	fclose(droom);
	return newRoom;
}


/******************************************************************************************
* displayOptions(): displays the name of the room and where the user can travel from that room 
****************************************************************************************/
void displayOptions(room currentRoom){
	int i;
	printf("\n");
	printf("CURRENT LOCATION: ");
	printf("%s\n", currentRoom.name);
	printf("POSSIBLE CONNECTIONS: ");
	for(i=0;i<6;i++){
		if (currentRoom.connections[i] != -1){
			printf("%s", numToName(currentRoom.connections[i]));
			if (i < 5 && currentRoom.connections[i+1] != -1){
				printf(", ");
			}
		}
	}
        printf("%s\n", ".");                
}


/******************************************************************************************
 * findRoom: finds graph index of room with roomIndex
 * RETURNS: graph index of passed in roomIndex
 * ***************************************************************************************/
int findRoom(room graph[], int nameIndex){
	int i;
	// look for graph index with name that matches representative int (i'm now realizing
	//  im glossing over the purpose of id in the room struct...)
	for(i = 0; i<7; i++){
		if(strcmp(graph[i].name, numToName(nameIndex)) == 0){
			return i;
		}
	}
	
	// return error if no room in the graph with that name
	printf("Error, next room not found");
	return -1;
}


/******************************************************************************************
 * promptMove(): prompts user to move in the room passed in
 * RETURNS: nameIndex associated with name entered or -1 if no room with that name exists
 * ***************************************************************************************/
int promptMove(room currentRoom){
	char buf[255];
	int read, bIsAnOption, i;
	memset(buf, '\0', sizeof(buf));
	
	// get users raw input
	printf("WHERE TO? >");
	scanf("%s", buf);

	// convert users input to representative int
	read = interpretRead(buf);

	// first check if user is attempting to access time call
	if (read == 13){
		return read;
	}
	
	// valid rooms are < 9 and -1 means invalid input
	if (read == -1 || read > 9){
		printf("\n");
		printf("%s\n", "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.");
		return -1;
	}
	
	// even if input is valid, lets make sure its an option
	bIsAnOption = 0;
	for(i=0;i<6;i++){
		if(currentRoom.connections[i] == read){
			bIsAnOption = 1;
		}
	}

	// if it wasn't an option, yell at user and return -1 for reprompt 
	if (bIsAnOption == 0){
                printf("\n");
                printf("%s\n", "HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.");
                return -1;
	}

	// once we've completed all the checks, return the valid representative int of the room name
	return read;
}

/* *****************************************************************************************
 * getTime(): function to be called by our mutex. Writes time to text file.
 * https://www.gnu.org/software/libc/manual/html_node/Formatting-Calendar-Time.html
 * Mutex method taken from pp 636-637 in The Linux Programming Interface
 * ***************************************************************************************/
void* getTime(){
	// setup file
	FILE* dTimeFile = fopen("currentTime.txt", "w");
	char tBuf[255];
	struct tm* dtime;

	// lock the mutex in order to limit access to the time business
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_lock(&mutex); 

	// seed time and assign time to tm struct 
	time_t start = time(NULL);
	dtime = localtime(&start);
	
	// format date like the program specs show and put it in buffer
	strftime(tBuf, sizeof(tBuf), "%l:%m%P, %A, %B %d, %Y", dtime);
	fputs(tBuf, dTimeFile);
	fclose(dTimeFile);

        // once we do our thing, unlock the mutex and return to business as usual
        pthread_mutex_unlock(&mutex);
        pthread_mutex_destroy(&mutex);	
}

/* *****************************************************************************************
* readTime(): Basic file reading function, reading from the hardcoded file currentTime.txt
* File is read out to the console with newlines before and after 
****************************************************************************************/
void readTime(){
	FILE* dTimeFile = fopen("currentTime.txt", "r");
	char fileBuf[255];
	
	// read until eof
	while(fgets(fileBuf, 255, (FILE*) dTimeFile)){
		printf("\n%s\n", fileBuf);
	}
	fclose(dTimeFile);
}

/* *****************************************************************************************
* threadSetup(): setting up our thread here and running the mutex within the time function to ensure
* this thread is the only function that will have access to the getTime function
* Following the example program on pp 636-637 in The Linux Programming Interface
****************************************************************************************/
void threadSetup(){
	pthread_t thread;
	int threadID;

	// run our time function on its own thread
	threadID = pthread_create(&thread, NULL, getTime, NULL);
	
	pthread_join(thread, NULL);
	
}
		

/* *****************************************************************************************
* ***************************************  MAIN  ******************************************* 
* *****************************************************************************************/
int main(){
	int i, nextRoom, steps;
	room graph[7];
	room* readRoom;
	char* roomDirect;
	room currentRoom;
	int path[100];
	 
	// get directory of most recent room directory
	roomDirect = getRoomDirect();
	
	// fill up our path array with -1 to denote empty slot
	for(i=0;i<100;i++){
		path[i] = -1;
	}
	
	// fill in our graph array with rooms from file. Free up the read room after adding it to graph
	for(i=0;i<7;i++){
		readRoom = getRoomData(i, roomDirect);
		graph[i] = *readRoom;
		free(readRoom);
	}

	// graph[0] will always hold the starting room
	currentRoom = graph[0];
	path[0] = interpretRead(currentRoom.name);	

	/************ GAMEPLAY LOOP ***********/
	steps = 1;
	while(hasUserWon(currentRoom) == 0){
		// give user options of where to travel from current room and place input into nextRoom
		displayOptions(currentRoom);
		nextRoom = promptMove(currentRoom);

		// promptMove will return -1 if invalid input. We want to skip the rest of the loop until 
		// valid input has been entered
		if (nextRoom == -1){
			continue;
		}
		// if user entered time, "nextRoom" will be 13
		else if (nextRoom == 13){
			//by calling the thread here, the time function will automatically be called from the thread
			threadSetup();
			readTime();
			continue;
		}
		
		// input was valid, lets increment to next step
		steps++;

		// findRoom will return the graph index to set new current room to
		currentRoom = graph[findRoom(graph, nextRoom)];

		// add representative int of next room to path array
		for(i = 0; i < 100; i++){
			if (path[i] == -1){
				path[i] = nextRoom;
				break;
			}
		}
	}
	
	// after gameplay loop has exited, congratuate user on winning
	printf("\n%s\n", "YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!");
	printf("%s%d%s", "YOU TOOK ", steps, " STEPS. ");
	printf("%s\n", "YOUR PATH TO VICTORY WAS:");

	// use function 'numToName' to reference representative int to it's actual name
	for(i = 0; i < 100; i++){
		if (path[i] != -1){
			printf("%s\n", numToName(path[i]));
		}
	}

	// just a little more housekeeping for roomDirectory string memory freeing
	free(roomDirect);
	for (i=0;i<7;i++){
		free(graph[i].name);
	}
	return 0;
}
