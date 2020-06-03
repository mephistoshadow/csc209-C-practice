#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXNAME 80  /* maximum permitted name size, not including \0 */
#define NPITS 6  /* number of pits on a side, not including the end pit */
#define NPEBBLES 4 /* initial number of pebbles per pit */
#define MAXMESSAGE (MAXNAME + 50) /* initial number of pebbles per pit */

int port = 3000;
int listenfd;

struct player {
    int fd;
    char name[MAXNAME+1]; 
    int pits[NPITS+1];
    int new_come;  // pits[0..NPITS-1] are the regular pits 
                        // pits[NPITS] is the end pit
    //other stuff undoubtedly needed here
    struct player *next;
};
struct player *playerlist = NULL;
extern void notify(int fd);// notify the player that is not for move.
extern void trim_number(char * s);// trim the number
extern void game_state(char* message);// show the game state
extern char* search_name(int fd);// search certain name with given fd
extern void parseargs(int argc, char **argv);
extern int check_whether_in(char* name,struct player* playerlist);// check the name is used or not
extern void makelistener();
extern int compute_average_pebbles();// caompute the avg pebbles
extern int find_whole_name(char* name, int n);// find all the name when meet a network newline
extern int acception(int fd, struct player* playerlist);
extern void disconnect_player(int fd);// disconnect player
extern int game_is_over();  /* boolean */
extern int make_move (int fd, struct player* playerlist); // move the pit
extern void broadcast(struct player* playerlist,char *s);  /* you need to write this one */


int main(int argc, char **argv) {
    // initial the bonus for check player move again.
    int bonus = 2;
    // the order for user move
    int order = 0;
    // the disconnect message
    char check_for_disconnect[MAXMESSAGE];
    // the game board
    char board [MAXMESSAGE];
    // msg used to send message
    char msg[MAXMESSAGE];
    // the client socket fd
    int client_socket;
    // the network new line position
    int name_index = 0;
    parseargs(argc, argv);
    makelistener();
    // max file discriptor
    int max_fd = listenfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(listenfd, &all_fds);
    while (!game_is_over()) {
        // set the listen fds.
        fd_set listen_fds = all_fds;
        // set the fd for all the player.
        for (struct player* p = playerlist; p; p = p -> next) {
            if (p -> fd > 0) {
                FD_SET(p -> fd,&listen_fds);
            }
            if (p -> fd > max_fd) {
                max_fd = p -> fd;
            }
        }
        // do the select step.
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }
        // if the server fd is in the set, do the connect step.
        if (FD_ISSET(listenfd,&listen_fds)) {
            client_socket = accept(listenfd, NULL,NULL);
            fprintf(stdout,"new connection found\n");
            // initialize the order for the first player.
            if (playerlist == NULL) {
                order = client_socket;
            }
            // create the playerlist.
            struct player *new_player = malloc(sizeof(struct player));
            // initialize the pebble.
            new_player -> fd = client_socket;
            for (int i = 0; i <NPITS; i ++) {
                new_player ->pits[i] = compute_average_pebbles();
            }
            // add to top of linked list
            new_player ->pits[NPITS] = 0;
            new_player -> next = playerlist;
            playerlist = new_player;
            // send welcome message.
            strcpy(msg,"Welcome to Mancala. What is your name?\r\n");
            write(client_socket,msg,strlen(msg));
            char name[MAXNAME];
            // check the network newline
            name_index = find_whole_name(name,MAXNAME);
            // if not networknewline continue read.
            while (name_index == -1){
                read(client_socket, name,sizeof(name));
                name_index = find_whole_name(name,sizeof(name));
            }
            // add the null to the end
            // check whether the name is empty or excceed the length or already in use.
            name[name_index] = '\0';
            while (check_whether_in(name,playerlist) == 1 || strcmp(name,"") == 0 || strlen(name) > MAXNAME) {
                write(client_socket, "please re_enter your name?\r\n",28);
                name_index = find_whole_name(name,MAXNAME);
                while (name_index == -1){
                    read(client_socket, name,sizeof(name));
                    name_index = find_whole_name(name,sizeof(name));
                }
                name[name_index] = '\0';
            }
            // sent message to all the user, new player has entered.
            if (check_whether_in(name,playerlist) == 0) {
                strcpy(new_player->name,name);
                strcpy(msg,name);
                strcat(msg," has joined the game\r\n");
                broadcast(playerlist,msg);
                game_state(board);
            }
            fprintf(stdout,"user finished type the name\n");
        }
        // this is gameing step.
        for (struct player* p = playerlist; p; p = p->next) {
            // check whether the fd is in the set.
            if (FD_ISSET(p->fd, &listen_fds)) {
                // make the order move.
                if (p-> next != NULL) {
                    order = p-> next -> fd;
                }else {
                    order = playerlist->fd;
                }
                // check for disconnect player.
                if (read(p->fd,check_for_disconnect,sizeof(check_for_disconnect)) == 0) {
                    strcpy(msg,p->name);
                    strcat(msg," has left the game\r\n");
                    fprintf(stdout,"one of player has left the game\n");
                    broadcast(playerlist,msg);
                    strcpy(msg,"now the game board is freshed\r\n");
                    fprintf(stdout,"now the game board is freshed\n");
                    // close the fd.
                    close(p->fd);
                    // remove from the playerlist
                    disconnect_player(p->fd);
                    FD_CLR(p->fd,&listen_fds);
                    // notify all the player.
                    broadcast(playerlist,msg);
                    // show the game board.
                    game_state(board);
                }
                // it is next move statement.
                strcpy(msg,"it is ");
                strcat(msg,search_name(order));
                strcat(msg,"'s move\r\n");
                broadcast(playerlist,msg);
                if (FD_ISSET(p->fd, &listen_fds)) {
                    fprintf(stdout,"player is moving\n");
                    if (p->fd != order) {
                        notify(p->fd);
                    }
                    bonus = make_move(order,playerlist);
                    game_state(board);
                    // for bonus step/
                    while(bonus == 1) {
                        fprintf(stdout,"player is doing the bonus move\n");
                        // notify(order,playerlist);
                        bonus = make_move(order,playerlist);
                        game_state(board);
                    }
                    // dont stop continue while loop for game.
                    continue;
                }
            }
        }
        
    }
    //broadcast("Game over!\r\n");
    printf("Game over!\n");
    for (struct player *p = playerlist; p; p = p->next) {
        int points = 0;
        for (int i = 0; i <= NPITS; i++) {
            points += p->pits[i];
        }
        printf("%s has %d points\r\n", p->name, points);
        snprintf(msg, MAXMESSAGE, "%s has %d points\r\n", p->name, points);
        //broadcast(msg);
    }

    return 0;
}
// notify the player that this is not your move.
void notify(int fd) {
    //char read_thing[MAXMESSAGE];
    char msg[MAXMESSAGE];
    strcpy(msg,"it is not your turn\r\n");
    write(fd,msg,strlen(msg));
}
// trim the number without all the white space.
void trim_number(char *str){
    char *begin, *last;
    //get non-whitespace position
    for (begin = str; *begin; begin++){
        if (!isspace((unsigned char)begin[0]))
            break;
    }
    //Find start of last all-whitespace
    for (last = begin + strlen(begin); last > begin + 1; last--)
    {
        if (!isspace((unsigned char)last[-1]))
            break;
    }
    *last = 0; 
    if (begin > str)
        memmove(str, begin, (last - begin) + 1);
}
// search for the certain name, with the given fd.
char* search_name(int fd) {
    for (struct player *p = playerlist; p; p = p->next) {
       if (p->fd == fd) {
        return p->name;
       }
    }
    return "not user";
}
// broadcast to all the player some messages.
void broadcast(struct player *playerlist,char* message) {
    for (struct player *p = playerlist; p; p = p->next) {
       write(p -> fd,message,strlen(message));
    }
}
// let user move the pit and do interatcions
int make_move (int fd, struct player* playerlist) {
    // this is used to send the entering message
    char message[MAXMESSAGE];
    strcpy(message,"please enter the valid pit number\r\n\0");
    char msg[MAXMESSAGE];
    strcpy(msg,"Your move?""\r\n");
    // find the current player.
    struct player *cur_player;
    char read_number [MAXMESSAGE];
    int oringinal = 0;
    // count the number of pits.
    int count;
    // this is the position that user choose to start from.
    int position;
    // use a for loop to get the current player.
    for (struct player *p = playerlist; p; p = p->next) {
       if (p->fd == fd) {
        cur_player = p;
       }
    }
    // write the you move message to the user.
    write(fd,msg,strlen(msg));
    // read the result from the user.
    read(fd,read_number,strlen(read_number));
    // trim the space that the number has.
    trim_number(read_number);
    // is the postition user choose doesn't have pits or not a valid nunmber. do repick.
    while(atoi(read_number) > 5 || atoi(read_number) < 0|| cur_player ->pits[atoi(read_number)] == 0) {
        write(fd,message,strlen(message));
        fprintf(stdout,"player is re_choose the pits\n");
        read(fd,read_number,strlen(read_number));
    }
    // convert string to integer.
    position = atoi(read_number);
    // get the current position's pits number.
    count = cur_player -> pits[position];
    // set the current position pits to 0.
    cur_player -> pits[position] = 0;
    // do the move actions.
    while(count > 0) {
        // is the position is not the end pits. 
        if (position < NPITS) {
            position++;
            cur_player -> pits[position] ++;
            count--;    
        }else {
            // if this player is not the last player. 
            if (cur_player -> next != NULL) {
                cur_player = cur_player -> next;
                position = 0;
                cur_player -> pits[position] ++;
                oringinal = 1;
                count--; 
            }else {
                // if the player is the last player.Then go to the top of the list.
                cur_player = playerlist;
                position = 0;
                cur_player -> pits[position] ++;
                oringinal = 1;
                count--; 
            }
        }
    }
    // give the flag that user can have a bonus turn to the main program.
    if (position == NPITS && oringinal == 0) {
        return 1;
    }
    // give the flag that user doesn't have the bonus turn
    return 0;
}
void parseargs(int argc, char **argv) {
    int c, status = 0;
    while ((c = getopt(argc, argv, "p:")) != EOF) {
        switch (c) {
        case 'p':
            port = strtol(optarg, NULL, 0);  
            break;
        default:
            status++;
        }
    }
    if (status || optind != argc) {
        fprintf(stderr, "usage: %s [-p port]\n", argv[0]);
        exit(1);
    }
}
// check whether the use's name has already been used and let them pick another one.
int check_whether_in(char* name1,struct player* playerlist){
    for (struct player *p = playerlist; p; p = p->next) {
        if (strcmp(p ->name,name1) == 0){
            return 1;
        }
    }
    return 0;
}
// this is to disconnect the player. when a player is left.
void disconnect_player(int fd) {
    struct player **player;
    for (player = &playerlist; *player && (*player)->fd != fd; player = &(*player)->next);
    if ((*player)->fd == fd) {
        *player = (*player)->next;
    }
}
// this is used to display the current game state and show it to all the player when needed.
void game_state(char * message) {
    for (struct player *p = playerlist; p; p = p->next) {
        // sepcific the board shape for one player.
        snprintf(message,MAXMESSAGE,"%s: [0]%d [1]%d [2]%d [3]%d [4]%d [5]%d [end pit]%d \r\n",p->name,p->pits[0],
           p->pits[1],p->pits[2],p->pits[3],p->pits[4],p->pits[5],p->pits[6]);
        // display to all the user.
        for (struct player *p = playerlist; p; p = p->next) {
            write(p->fd,message,strlen(message));
        }
    }
}
void makelistener() {
    struct sockaddr_in r;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, 
               (const char *) &on, sizeof(on)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    memset(&r, '\0', sizeof(r));
    r.sin_family = AF_INET;
    r.sin_addr.s_addr = INADDR_ANY;
    r.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *)&r, sizeof(r))) {
        perror("bind");
        exit(1);
    }

    if (listen(listenfd, 5)) {
        perror("listen");
        exit(1);
    }
}
/* call this BEFORE linking the new player in to the list */
int compute_average_pebbles() { 
    struct player *p;
    int i;

    if (playerlist == NULL) {
        return NPEBBLES;
    }

    int nplayers = 0, npebbles = 0;
    for (p = playerlist; p; p = p->next) {
        nplayers++;
        for (i = 0; i < NPITS; i++) {
            npebbles += p->pits[i];
        }
    }
    return ((npebbles - 1) / nplayers / NPITS + 1);  /* round up */
}
// this is used to find the newline character.
int find_whole_name(char* name, int n) {
     for (int i = 0; i < n;i ++) {
        if (name[i] == '\r' || name[i] == '\n') {
            return i;
        }
    }
    return -1;
}
int game_is_over() { /* boolean */
    int i;

    if (!playerlist) {
       return 0;  /* we haven't even started yet! */
    }

    for (struct player *p = playerlist; p; p = p->next) {
        int is_all_empty = 1;
        for (i = 0; i < NPITS; i++) {
            if (p->pits[i]) {
                is_all_empty = 0;
            }
        }
        if (is_all_empty) {
            return 1;
        }
    }
    return 0;
}