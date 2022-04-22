#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

char * start();
char  * fight(int num_fighters, char fighter_array[num_fighters][21], int stat_array[num_fighters][3]);
int end(int c, int s);



int main()
{
	int roster_len;
	char server_message[256] = "You have reached the server\n";

	int server_socket, client_socket;
	struct sockaddr_in server_address, client_address;

	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1)
		printf("Socket Creation Failed\n");
	else
		printf("Socket Created!\n");
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(9002);
	server_address.sin_addr.s_addr = INADDR_ANY;


	signal(SIGPIPE, SIG_IGN);

	if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0)
		printf("Socket bind failed\n");
	else
		printf("Socket Bound!\n");

	listen(server_socket, 10);

	printf("Socket Listening!\n");

	while (1)
{
	char *winner = malloc(21);
    	int l = sizeof(client_address);
	client_socket = accept(server_socket, (struct sockaddr *)&client_address, &l);
	char *client_ip = inet_ntoa(client_address.sin_addr);
	FILE *ip_log = fopen("ips.log","a");
	if (ip_log == NULL)
	{
		printf("Error opening IP log file\n");
	}
	else
	{
		char time_msg[24];
		struct tm *time_info;
		time_t raw_time;
		time (&raw_time);
		time_info = localtime(&raw_time);
		
		strftime(time_msg,sizeof(time_msg),"%d %b, %Y %H:%M:%S", time_info);

		fprintf(ip_log, "%s\t-\t %s\n", time_msg, client_ip);
		fclose(ip_log);
	}

	printf("Client Connected: %s\n", client_ip );
	FILE *fp = fopen("roster.txt", "w");
	if (fp == NULL)
		{
			printf("File cannot be opened\n");
			end(client_socket, server_socket);
			return 1;
		}
	printf("File opened\n");
	send(client_socket, server_message, sizeof(server_message), 0);
	char roster[30];
        char buff;

	while (1)
        {
            printf("in the big loop \n");
  	    for(int i = 0; i < 30; i++)
	    {
	    	    while (read(client_socket, &buff, 1))
                    {
                        if ((buff == '\n') || (buff == '\r'))
                        {
                            for(int j = i; j < 30; j++)
                                roster[i] = '\0';
                        }
                        else
                        {
                            if(buff != '\'')
                                roster[i] = buff;
                        }
                    }
            }
	    printf("Recieved: %s\n", roster);
            fprintf(fp,"%s", roster);
        


/* 
	while ((roster_len = read(client_socket, roster, sizeof(roster))) > 0)
	{
    	    printf("Received: '%.*s'\n", roster_len, roster);
		//take input into roster text file
	    fprintf(fp, "'%.*s", roster_len, roster);
            printf("Acutally received: %s\n", roster);
*/
		//process fight with text file
            if (strcmp(roster,"START") ==0)
            {
                fclose(fp);
                winner = start();
                char winner_message[30];
	        sprintf(winner_message, "Winner:  %s\n", winner);
		//send winner 
	        printf("sending %s", winner_message);
                send(client_socket, winner_message, sizeof(winner_message), 0);
		free(winner);
	    }

	    else if (strcmp(roster, "QUIT") == 0)
	    {
                fclose(fp);
                end(client_socket, server_socket);
                break;
	    }
	}
}
	if (roster_len < 0)
		printf("read failure\n");
        fclose(fp);
		end(client_socket, server_socket);
		break;

	if (strcmp(roster, "QUIT") == 0)
	{
    fclose(fp);
	end(client_socket, server_socket);
	break;
	}
	end(client_socket, server_socket);
	fclose(fp);

}
	close(server_socket);

	return 0;

}

//////////////////////////////////////end function//////////////////////////////////////////////
int end(int c, int s)
{
	close(c);
	close(s);
	return 1;
}


///////////////////////////////////////start function///////////////////////////////////////////

char * start()
{    
    printf("Populating roster\n");
    char *winner = malloc(21);
    FILE *roster_file = fopen("roster.txt", "r");
    if (roster_file == NULL)
    {
        printf("Could not open file\n");
        
	return NULL;
    }
    int num_fighters = 0;
    char num[3];
    //get rid of garbage values for each line
    while(!feof(roster_file))
    {
        char c = fgetc(roster_file);
        if (c == '\'')
        {
            c = fgetc(roster_file);
        }
        //get number of fighters
        if (num_fighters == 0)
        {
            for (int i = 0; i < 3; i++)
            {
                num[i] = c;
                c = fgetc(roster_file);
                if ((c == '\n') || (c == '\r'))
                    break;
            }
            num_fighters = atoi(num);
            printf("fighters %d\n", num_fighters);
        }
        //populate array
        char fighter_array[num_fighters][21];
        int stat_array[num_fighters][3];
        int stat[2];

        for (int i = 0; i < num_fighters; i++)
        {
            //get name            
            for (int j = 0; j < 21; j++)
            {
                c = fgetc(roster_file);
                if ((c == '\'') || (c == '\n'))
                    c = fgetc(roster_file);  
                if (c == ':')
                    break;
                fighter_array[i][j] = c;
                  
            }
            //get stats
            for (int k = 0; k < 3; k++)
            {
                if (c == ':')
                    c = fgetc(roster_file);
                for (int l = 0; l < 2; l++)
                {
                    stat[l] = c;
                    c = fgetc(roster_file);
                }
                stat_array[i][k] = ((stat[0] - '0') * 10 + (stat[1] - '0'));
            }
        }
        break;
        winner = fight(num_fighters, fighter_array, stat_array);
    }
    printf("Start winner: %s\n",winner);
    return winner;
}


/////////////////////////////////////fight function/////////////////////////////////////////////

char  * fight(int num_fighters, char fighter_array[num_fighters][21], int stats_array[num_fighters][3])
{

    printf("Starting Fight!\n");
	//initialize randomizer
    srand(time(0));
    char *winner = malloc(21);
    int A = 0;
    uint8_t dmg;
    for (int B = 1; B < num_fighters; B++)
    {   int hp_A = 200;
        int hp_B = 200;   
        while (1)
        {
            //See if A hits B
            if (stats_array[B][1] <= (rand() % 100))
            {
                dmg = (rand() % stats_array[A][0]);
                if(stats_array[A][2] >= (rand() % 100))
                {
                    hp_B -= (dmg * 2);
                    printf("%s Scores a critical hit! %d damage!\n", fighter_array[A], (dmg * 2));
                }
                else    
                {
                    hp_B -= dmg;
                    printf("%s Gets in a punch! %d damage!\n", fighter_array[A], dmg);
                }
            }
            else
                printf("%s Dodged the attack!\n", fighter_array[B]);
            //see if B hits A
            if (stats_array[A][1] <= (rand() % 100))
            {
                dmg = (rand() % stats_array[B][0]);
                if(stats_array[B][2] >= (rand() % 100))
                {
                    hp_A -= (dmg * 2);
                    printf("%s Scores a critical hit! %d damage!\n", fighter_array[B], dmg);
                }
                else    
                {
                    hp_A -= dmg;
                    printf("%s lands a hit! %d damage!\n", fighter_array[B], dmg);
                }
            }
            else
                printf("%s Dodged the attack!\n", fighter_array[A]);

            //determine winner
            printf("%s: %d; \t%s, %d\n", fighter_array[A], hp_A, fighter_array[B], hp_B);
            if ((hp_A <= 0) || (hp_B <= 0))
            {
                if (hp_A < hp_B)
                {
                    A = B;
                    printf("%s Wins the round!\n", fighter_array[B]);
                    break;
                }
                else if ((hp_B < hp_A))
                {
                    printf("%s Wins the round!\n", fighter_array[A]);
                    break;
                }
                else
                    printf("%s and %s tied the round!\n", fighter_array[A], fighter_array[B]);
            }
        }
    }//end for loop
    printf("The winner is..\n\n%s\n\n", fighter_array[A]);
    winner = fighter_array[A];
    printf("fight winner: %s\n",winner);
    return winner;
}
