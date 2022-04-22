#this is a client program designed to send a roster of fighters and stats to a remote server and recieve the winner.

import socket
import os

#initialize default IPv4 TCP socket

def main():
    s = socket.socket()
    clear =  lambda: os.system('clear')
    #change to ip and port of server
    server = '127.0.0.1'
    port = 9002
    try:
        s.connect((server, port))
    except:
        print("unable to connect to server")
#        quit()
#    message = s.recv(1024)
#    print(message)
    roster = []
    #intro screen goes here
    
    #menu
    title = '''
        ,gggg,                                             ,gggggggggggggg                                                   
    ,88"""Y8b,           ,dPYb,                         dP""""""88""""""               ,dPYb,      I8                      
    d8"     `Y8           IP'`Yb                         Yb,_    88                     IP'`Yb      I8                      
    d8'   8b  d8           I8  8I                          `""    88    gg               I8  8I   88888888                   
    ,8I    "Y88P'           I8  8'                              ggg88gggg""               I8  8'      I8                      
    I8'          gg     gg  I8 dP       ,ggg,    ,gggggg,          88   8gg     ,gggg,gg  I8 dPgg,    I8    ,ggg,    ,gggggg, 
    d8           I8     8I  I8dP   88ggi8" "8i   dP""""8I          88    88    dP"  "Y8I  I8dP" "8I   I8   i8" "8i   dP""""8I 
    Y8,          I8,   ,8I  I8P    8I  I8, ,8I  ,8'    8I    gg,   88    88   i8'    ,8I  I8P    I8  ,I8,  I8, ,8I  ,8'    8I 
    `Yba,,_____,,d8b, ,d8I ,d8b,  ,8I  `YbadP' ,dP     Y8,    "Yb,,8P  _,88,_,d8,   ,d8I ,d8     I8,,d88b, `YbadP' ,dP     Y8,
    `"Y8888888P""Y88P"8888P'"Y88P"' 888P"Y8888P      `Y8      "Y8P'  8P""Y8P"Y8888P"88888P     `Y88P""Y8888P"Y8888P      `Y8
                    ,d8I'                                                         ,d8I'                                     
                    ,dP'8I                                                        ,dP'8I                                      
                ,8"  8I                                                       ,8"  8I                                      
                I8   8I                                                       I8   8I                                      
                `8, ,8I                                                       `8, ,8I                                      
                    `Y8P"                                                         `Y8P"                                       
    '''
    menu = '''
    Please select an option. 
    1. Load roster from file.
    2. Enter Roster Manually
    3. View Roster
    4. Launch
    5. Quit
    '''

    #print(title)
    selection = 2
    validSelections = ("1", "2", "3", "4", "5")


    while selection != "5":
        clear()
        print(title)
        selection = input(menu)
        if selection not in validSelections:
            print("that is an invalid selection, please try again")
        else:
            #print(f"You chose {selection}")
            if selection == "1":
                clear()
                roster = getFile(roster)
                print(roster)
                input("Press any key to continue")
            if selection == "2":
                clear()
                roster = userInput(roster)
                print(roster)
                input("Press any key to continue")
            if selection == "3":
                clear()
                print("Name\t\t\tAttack\tDodge\tLuck")
                for line in roster:
                    line = (line.split(":"))
                    print(f"{line[0] :<20}\t{line[1]}\t{line[2]}\t{line[3]}")
                input("Press any key to continue")
            if selection == "4":
                print("SENDING IT!")
                message = b'3\n'
                s.send(message)
                for line in roster:  
                    try:
                        print(f"sending {line.encode()}")
                        message = line + '\n'
                        s.send(message.encode())              
                    except IOError as e:
                        if e.errno == errno.EPIPE:
                            pass
                        print("an error occurred")
                startMessage = "START"
                s.send(startMessage.encode())
                winner = s.recv(1024)
                print(winner)
                input("Press any key to continue")
            if selection == "5":
                quit()
            
def getFile(roster):
    numFighters = 0
    filename = input(
'''
Please enter the name of the file in the current folder or the path to the file. 
The File should be formatted as follows:
<name> <attack> <dodge> <luck>
Bobby 35 35 30
Jimbo 25 45 30

Remember that each fighter must have exactly 100 total points in stats. 
The minimum value for each stat is 10 and the maximum is 80.

File: '''
            )
    print(f"you chose the file {filename}")
    #encoding handling here
    with open(filename) as fhand:
        #check for valid input
        for line in fhand:
            error = 0
            stats = line.split(':')
            name = stats[0]
            stats = [int(x) for x in stats[1:]]
            sum = 0
            for i in stats:
                if i >=10 and i <=80:
                    sum += i
                else:
                    error = 1
                    print(f"Error with line {line}. {i} does not fall within range 10-80. Fighter {name} not registered")
            if sum == 100:
                roster.append(line.strip())
                print(f"Fighter {name} successfully registered. {line}")
                numFighters += 1
            else:
                error = 2
                print(f"Error with line {line}. The fighters stats must equal exactly 100. Fighter {name} was not registered because his stats totalled {sum}.")
    print(f"{numFighters} successfully registered!")
    return roster


def userInput(roster):

    fighters = 0
    cont = "Y"
    validCont = ["y", "Y", "Yes", "YES", "yes", "sure", "Sure", "Roger", "roger"]
    while cont in validCont:
        points = 100
        while True:
            name = input("Please input figher name (15 characters max):")
            if len(name) > 15:
                print("Name does not match standards. 15 characters max.")
                continue
            else:
                break    
        print(f"Please enter the three stats for fighter {name}. Remember each stat must be between 10 and 80 and the total of all stats must be 100")
        stats = [0, 0, 0]
        names = ["Attack: ", "Dodge: "]
        points = 100
        sum = 0
        max = 80
        for i in range(0,2):
            while True:
                if stats[i] == 0:
                    try:
                        stats[i] = int(input(f"Please enter a number between 10 and {max}\n{names[i]}"))
                        if stats[i] >= 10 and stats[i] <= max:
                            points -= stats[i]
                            sum += stats[i]
                            max = min(80, points - 10)
                            break
                        else:
                            stats[i] = 0
                            print(f"ERROR!\nYou must enter a number between 10 and {max}\n\n")

                    except ValueError:
                        print("ERROR! You must enter an integer\n\n")
        stats[2] = points
        print(f"Luck: {stats[2]}")
        line = [name]
        for i in stats:
            line.append(str(i))
        roster.append(':'.join(line))
        cont = input("Would you like to continue adding fighters?")
    return roster
main()
