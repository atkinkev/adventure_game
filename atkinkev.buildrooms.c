/******************************************************************************
 * Kevin Atkins
 * 2/3/2019 
 * atkinkev.buildrooms.c
 * ***************************************************************************/
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* Define struct type "room" to setup each room */
typedef struct{
int id;
char* name;
int numConnections;
int connections[6];
} room;

void setupDir(){
	int status;
	char pid[50], path[50];
	
	// setup path string
	strcpy(path, "atkinkev.rooms.");
	sprintf( pid, "%d", getpid());
	strcat(path, pid);

	// create directory with path string
	status = mkdir(path, S_IRWXU);
	if (status == -1){
		printf("Error creating rooms directory");
		exit(1);	
	}
}

void makeRooms(){
	char pid[100], path[100], roomPath[100];

	// build path for rooms to go in
	strcpy(path, "atkinkev.rooms.");
	sprintf( pid, "%d", getpid());
	strcat(path, pid);
	strcat(path, "/");
	
	// make all our rooms
	strcpy(roomPath, path);
	strcat(roomPath, "room1");
	FILE* room1 = fopen(roomPath, "w");
	
	memset(roomPath, '\0', sizeof(roomPath));
        strcpy(roomPath, path);
        strcat(roomPath, "room2");
        FILE* room2 = fopen(roomPath, "w");

        memset(roomPath, '\0', sizeof(roomPath));
        strcpy(roomPath, path);
        strcat(roomPath, "room3");
        FILE* room3 = fopen(roomPath, "w");

        memset(roomPath, '\0', sizeof(roomPath));
        strcpy(roomPath, path);
        strcat(roomPath, "room4");
        FILE* room4 = fopen(roomPath, "w");

        memset(roomPath, '\0', sizeof(roomPath));
        strcpy(roomPath, path);
        strcat(roomPath, "room5");
        FILE* room5 = fopen(roomPath, "w");

        memset(roomPath, '\0', sizeof(roomPath));
        strcpy(roomPath, path);
        strcat(roomPath, "room6");
        FILE* room6 = fopen(roomPath, "w");

        memset(roomPath, '\0', sizeof(roomPath));
        strcpy(roomPath, path);
        strcat(roomPath, "room7");
        FILE* room7 = fopen(roomPath, "w");
	
	fclose(room1);
	fclose(room2);
        fclose(room3);
        fclose(room4);
        fclose(room5);
        fclose(room6);
        fclose(room7);
}

/* isGraphFull(): return 0 if graph is not full, or 1 if all rooms have 3-6 connections */
int isGraphFull(room graph[]){
	int i;
	for(i = 0; i < 7; i++){
		if (graph[i].numConnections < 3){
			return 0;
		}
	}
	return 1;
}

room* getRandomRoom(){
	char roomName[10];
	int randRoom;
	room* newRoom;

	randRoom = rand() % 10;
	memset(roomName, '\0', sizeof(roomName));
	newRoom = malloc(sizeof(room));
	newRoom->name = malloc(10);
	 
	switch(randRoom){
		case 0:
			strcpy(roomName, "Parlour");
			break;
		case 1:
			strcpy(roomName, "Dungeon");
			break;
		case 2: 
			strcpy(roomName, "Altar");
			break;
		case 3: 
			strcpy(roomName, "Portal");
			break;
		case 4:
			strcpy(roomName, "Garden");
			break;
		case 5:
			strcpy(roomName, "Kitchen");
			break;
		case 6: 
			strcpy(roomName, "Throne");
			break;
		case 7:
			strcpy(roomName, "Void");
			break;
		case 8: 
			strcpy(roomName, "Workshop");
			break;
		case 9: 
			strcpy(roomName, "Mess");
			break;
		default:
			printf("Error - cannot assign room name");
			exit(1);
	}
	strcpy(newRoom->name, roomName);
	newRoom->id = randRoom;
	newRoom->numConnections = 0;
	return newRoom;			
}
/*************************************************************************************************
* findSuitableConnection(graph, connectionIndex): takes the current graph and the index we want
* to find a connection for. Generates random numbers until one of them is suitable. Returns -1 
* if the connection we're trying to make itself is not suitable
*************************************************************************************************/
int findSuitableConnection(room graph[], int connectMe){
	int i, j, connection, bSuitable;
	// return -1 if max connections reached
	if (graph[connectMe].numConnections >= 6){
		return -1;
	}
	
	// generate random index to try to connect to
	bSuitable = 0;
	while(bSuitable == 0){
		bSuitable = 1;
		connection = rand() % 7;
		// if connection is equal to the room index we're trying to connect, try again
		if (connection == connectMe){
			bSuitable = 0;
			continue;
		}
		// otherwise lets see if it is already a connection, if it is, try again
		for (i = 0; i < 6; i++){
			if (graph[connectMe].connections[i] ==  connection){
				bSuitable = 0;
				continue;
			}
			else if (graph[connection].numConnections >= 6){
				bSuitable = 0;
				continue;
			}
		}
	}
	// once we're out of the trenches, we should have a suitable index to connect to
	return connection;
}

void writeGraphToFile(room graph[]){
	int i, status;
	FILE* roomFile;
	char pid[100], path[100],roomPath[100], roomNum[2];

        strcpy(path, "atkinkev.rooms.");
        sprintf(pid, "%d", getpid());
        strcat(path, pid);
        strcat(path, "/");       
	
	// open each file and write to it   
	for (i=0;i<7;i++){
		// format file path for each room file
		memset(roomPath, '\0', sizeof(roomPath));
		strcpy(roomPath, path);
		strcat(roomPath, "room");

		// each room is referenced by an index + 1
		sprintf(roomNum,"%d", (i+1));
		strcat(roomPath, roomNum);

		// open up the file for writing
		roomFile = fopen(roomPath, "w");
		if (roomFile == NULL){
			printf("ERROR: Unable to open room file for writing");
		}

		// start writing all the important data using a file stream
		fputs("ROOM NAME: ", roomFile);
		fputs(graph[i].name, roomFile);
		fputs("\nCONNECTION 1: ", roomFile);
		fputs(graph[graph[i].connections[0]].name, roomFile);
                fputs("\nCONNECTION 2: ", roomFile);
                fputs(graph[graph[i].connections[1]].name, roomFile);
                fputs("\nCONNECTION 3: ", roomFile);
                fputs(graph[graph[i].connections[2]].name, roomFile);
		
		// handle rooms with > 3 connections the same way
		if(graph[i].numConnections > 3){
                fputs("\nCONNECTION 4: ", roomFile);
                fputs(graph[graph[i].connections[3]].name, roomFile);
		}
		if(graph[i].numConnections > 4){
	                fputs("\nCONNECTION 5: ", roomFile);
	                fputs(graph[graph[i].connections[4]].name, roomFile);
		}		
                if(graph[i].numConnections > 5){
                        fputs("\nCONNECTION 6: ", roomFile);
                        fputs(graph[graph[i].connections[5]].name, roomFile);
                }

		// because all of our rooms are random every run, assigning the start and
		// end rooms to fixed indexes will still keep everything random
		switch (i){
			case 0:
				fputs("\nROOM TYPE: START_ROOM", roomFile);
				break;
			case 1:
				fputs("\nROOM TYPE: END_ROOM", roomFile);
				break;
			default:
				fputs("\nROOM TYPE: MID_ROOM", roomFile);
				break;
		}
                fputs("\n", roomFile);
		fclose(roomFile);
	}
}

int main(){
	// seed our pseudo random generator and declare some vars
	srand(time(0));
	int rids[7] = {-1};
	int i,j;
	int bUnique;
	int connectIndex;
	room graph[7];
	room* newRoom;

	// basic housekeeping functions to setup our file directory and each room's file
        setupDir();
        makeRooms();

	// generate random rooms until they're all unique, using id to ensure unique name
	for(i = 0; i<7; i++){
		bUnique = 0;	
		while (bUnique == 0){
			bUnique = 1;
			newRoom = getRandomRoom();
			int j;
			for(j = 0; j<7; j++){
				if (rids[j] == newRoom->id){
					bUnique = 0;
					free(newRoom->name);
					free(newRoom);
					break;	
				}
			}
		}
		// add the unique new room to the graph array and add its id to our "uniqueness" array
		graph[i] = *newRoom;
                rids[i] = graph[i].id;
		// done with this instance of new room
		free(newRoom);
	}
	
	// init all connections indexes to -1 so we know they are currently a void connection
	for (i = 0; i<7; i++) {
		for (j = 0; j<6; j++) {
			graph[i].connections[j] = -1;
		}
	}

	// connect all our rooms together using in/out method
	while(isGraphFull(graph) == 0){
		for (i = 0; i < 7; i++){
			// look for a suitable connection (connectIndex returns -1 if no connections are valid)
			connectIndex = findSuitableConnection(graph, i);
			if(connectIndex != -1){
				// append new connection to next available slot
				for (j=0;j<6;j++){
					if(graph[i].connections[j] == -1){
						graph[i].connections[j] = connectIndex;
						graph[i].numConnections++;
						break;
					}
				}
				// append new connection to next available slot
				for (j=0;j<6;j++){
					if(graph[connectIndex].connections[j] == -1){
						graph[connectIndex].connections[j] = i;
						graph[connectIndex].numConnections++;
						break;
					}
				}
			}
		}
	}

	// now that we have a full graph, lets actually build these files
	writeGraphToFile(graph);
	
	// free up the space we made for name	
	for(i=0;i<7;i++){
		free(graph[i].name);
	}
	return 0;	
}



































