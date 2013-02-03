#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum { false, true } bool;

//function prototypes
void deleteElement(int idx);
void createMap();
void printMap();
void printClosed();
void printOpen();
void aStar();
void findS();
int computeH(int x, int y);
bool isOnClosed(int x, int y);
bool isOnOpen(int x, int y);
void evalCell(int x, int y);

//struct used for cells
struct cell{
    int f, g, h, x, y, pIdx;
};

/*  List used for A* algorithm
 *
 *  Open list   - Holds all cells that are being considered
 *  Closed list - Holds all cells that are part of the final path   */
struct cell closedList[256];
struct cell openList[256];

//used to keep track of how many items are on both the openList and closedList
int closedLength = 0;
int openLength = 0;

//the destination cell
struct cell destination;

//height of file being read
int mapHeight = 20;
int mapWidth = 20;

char mapLayout[20][20];

int main()
{
    createMap();
    printMap();
    aStar();

    return 0;
}

//performs a* algorithm to find the shortest path between point A and B on the map
void aStar(){
    int i, x, y;

    //loop until path has been found
    while(1){
        findS();

        for(i=0;i<4;i++){
            //calculate each cell adjacent to S and add it to the open list if it qualifies
            x = closedList[closedLength - 1].x;
            y = closedList[closedLength - 1].y;
            switch (i) {
            case 0:
                evalCell((x-1), y);
                break;
            case 1:
                evalCell(x, (y-1));
              break;
            case 2:
                evalCell((x+1), y);
              break;
            case 3:
                evalCell(x, (y+1));
              break;
            default:
                printf("ERROR: in checking T for S, loop ran for more than 4");
              break;
            }
        }//end of looping through T tiles

        if((closedList[closedLength-1].x == destination.x) && (closedList[closedLength-1].y == destination.y)){
            //PATH FOUND!
            //printOpen();
            printClosed();

            /*
            for(i=0;i<closedLength;i++){
                x = closedList[i].x;
                y = closedList[i].y;
                mapLayout[x][y] = '.';
            }*/

            i=closedLength-1;
            mapLayout[destination.x][destination.y] = '.';
            while(1){
                x = closedList[closedList[i].pIdx].x;
                y = closedList[closedList[i].pIdx].y;
                mapLayout[x][y] = '.';
                if(closedList[i].pIdx <= 0)
                    break;
                i = closedList[i].pIdx;
            }

            printMap();
            system("pause");
            break;
        }
    }

    return;
}


//searches through the all cells in openList and points s to the one with the best F
void findS(){
    int i, fTemp = 256;
    int indexOfS = 0;

    //if openList isn't empty
    if(openLength >= 1){
        //loop through open list and find the cell with the lowest F value
        for(i=0;i<openLength;i++){
            if(openList[i].f <= fTemp){
                fTemp = openList[i].f;
                indexOfS = i;
            }
        }

        //remove S from openList into closedList and then remove it from openList.
        if(!isOnClosed(openList[indexOfS].x,openList[indexOfS].y)){
            closedList[closedLength] = openList[indexOfS];
            closedLength++;
        }

        deleteElement(indexOfS);
    }
    return;
}




// Used to remove a cell from openList
void deleteElement(int idx){
    int c;

    if ( idx > (openLength - 1) )
        printf("Failed to remove cell from openList because the index provided is greater than the number of items.\n");
    else{
		for ( c = idx ; c < openLength ; c++ ){

			openList[c] = openList[c+1];
		}
    }
    openLength--;

    return;
}

//if cell is an obstacle or is already on closedList ignore it.  Otherwise add cell to openList.
void evalCell(int x, int y){
    if((!isOnClosed(x, y)) && (!isOnOpen(x, y)) && (mapLayout[x][y] != 'x')){
        openList[openLength].g = (closedList[closedLength - 1].g + 1);
        openList[openLength].h = computeH(x, y);
        openList[openLength].f = openList[openLength].h + openList[openLength].g;
        openList[openLength].x = x;
        openList[openLength].y = y;
        openList[openLength].pIdx = closedLength-1;


        openLength++;
    }

    return;
}

//read in string from file and populate map with obstacle
void createMap(){
    FILE *file = fopen("map.txt", "r");
    int x = 0;
    int y = 0;
    int c;

    if (file == NULL)
        return; //could not open file

    while ((c = fgetc(file)) != EOF)
    {
        if(c == '\n'){
            y++;
            x = 0;
        }else{
            mapLayout[x][y] = c;

            if(c == 'A'){
                //add A to the closed list
                openList[0].x = x;
                openList[0].y = y;
                openList[0].f = 0;
                openList[0].g = 0;
                openList[0].pIdx = -1;
                //increment how many items are on the closed list
                openLength++;
            }else if(c == 'B'){
                //fill in information about the destination  cell
                destination.x = x;
                destination.y = y;
            }

            x++;
        }
    }
    fclose(file);
    //print out location of A and B
    printf("A - x:%d, y:%d\nB - x:%d, y:%d\n", openList[0].x, openList[0].y, destination.x, destination.y);
    return;
}

//print out map
void printMap(){
    int x=0,y=0;

    printf("\n\n");
    printf("\t              1111111111\n",y);
    printf("\t    01234567890123456789\n",y);
    printf("\t    ||||||||||||||||||||\n",y);
    for(y=0;y<mapHeight;y++){
        if(y<10){
            printf("\t%d - ",y);
        }else{
            printf("\t%d- ",y);
        }
        for(x=0;x<mapWidth;x++){
            printf("%c", mapLayout[x][y]);
        }
        printf("\n");
    }
    printf("\n\n");

    return;
}

//debug function used to print out the contents of the closedList
void printClosed(){
    int i;
    printf("\n\n\t--closedList[%d]--\n", closedLength);
    for(i=0;i<closedLength;i++){
        printf("\nclosedList[%d]:\tg:%d\th:%d\tf:%d\tpIdx:%d\tcoord:(%d,%d)", i, closedList[i].g, closedList[i].h, closedList[i].f, closedList[i].pIdx, closedList[i].x, closedList[i].y);
    }
    printf("\n\n\t--closedList[%d]--\n", closedLength);
    return;
}
//debug function used to print out the contents of the openList
void printOpen(){
    int i;
    printf("\n\n\t--openList[%d]--\n", openLength);
    for(i=0;i<openLength;i++){
        printf("\nopenList[%d]:\tf:%d\tcoord:(%d,%d)", i, openList[i].f,openList[i].x,openList[i].y);
    }
    printf("\n\n\t--openList[%d]--\n", openLength);
    return;
}

//compute the H (Manhattan Distance) score of a given cell
int computeH(int x, int y){
    int xDif = destination.x - x;
    int yDif = destination.y - y;

    xDif *= xDif;
    yDif *= yDif;

    return sqrt(xDif) + sqrt(yDif);
}

//returns true if a cell on the closed list has the same coordinates as that given
bool isOnClosed(int x, int y){
    int i;

    for(i=0;i<closedLength;i++){
        if((closedList[i].x == x) && (closedList[i].y == y))
            return true;
    }

    return false;
}

//returns true if a cell on the closed list has the same coordinates as that given
bool isOnOpen(int x, int y){
    int i;

    for(i=0;i<openLength;i++){
        if((openList[i].x == x) && (openList[i].y == y))
            return true;
    }

    return false;
}
