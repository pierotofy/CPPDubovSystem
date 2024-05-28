# CPPDubovSystem

A way to generate pairings via the FIDE Swiss Dubov system.

## Table of Contents

- About
- Installation
- Use by Terminal
- Use by API
- Extra Notes
- License

## About CPPDubovSystem

CPPDubovSystem (**current version 1.0**) is a framework for generating pairings using the FIDE Swiss Dubov algorithm. It is coded entirely in C++.

This framework follows all the rules written [here](https://handbook.fide.com/chapter/C040401202507). I reccomend for you to [see also](http://www.rrweb.org/javafo/C04.pdf). Please note that this has **NOT YET BEEN APPROVED** by FIDE, however it will be sent to FIDE at one point for approval. Follow this repository for updates!

## Installation

The easiest way to install CPPDubovSystem is to download this github repository, and compile it. I have tested this all in Xcode, so Xcode simply compiled it for me. If you are on another platform such as Linux, you may have to do some research on how to compile this (like I said, Xcode did the compiling for me here). You might want to consider looking into something like g++.

```console
CPPDubovSystemUser@LinuxVirtualMachine:~$ g++ main.cpp
```

If you are on windows, you may want to look into gcc

```console
CPPDubovSystemUser@WindowsVirtualMachine:~$ gcc main.cpp -o CPPDubovSystem.exe
```

Please note: this has not been tested on any other platform. This was only tested on MacOS. So if you are a Mac user, I strongly encourage you let Xcode compile the source code for you. If you really want to compile this through terminal on Mac, make sure you have Xcode installed, as the g++ command will be installed with it. Then try executing the following

```console
CPPDubovSystemUser@MacVirtualMachine:~$ g++ -o output main.cpp
```

Generally speaking, this should work on any system. It doesn't require any external framework or library to work. I have tested this framework on my M1 Mac with Xcode 15.4.

## Use by Terminal

The best way to get the pairings is by executing the data via terminal. For this example, assume "./main" is the compiled source code.

### Standard Pairings

To get pairings for a round, execute the following:

```console
user@user:~$ ./main --pairings "path/to/tournament_data.trf"
```

The framework accepts a file coded in [TRF16](https://www.fide.com/FIDE/handbook/C04Annex2_TRF16.pdf) as per FIDE requirements. The program at the moment cannot output the pairings to a file, just the terminal. I am hoping that by version 1.1, I can output it all into a file.

**Important Note**: you must read this as it is extremely important for generating pairings. The pairing engine **needs to know how many rounds your tournament has**. Therefore, in your TRF file, you must include the following code **TNR _round\_count_** in order for the pairing engine to capture the number of rounds in the tournament. See one of the example files in the "tests" folder.

### Random Tournament Generator

You can also use this engine to generate a random tournament. FIDE defines these random tournament generators as simply an engine that creates a tournament, and simulates it with random game results. Run the command below as follows to achieve this:

```console
user@user:~$ ./main --rtg "path/to/trf.trf" --p_count 10 --rtg_rounds 5
```

The sample command above creates a random tournament with 10 players and 5 rounds. It will output this all into a trf file (TRF16 encoded). If a round cannot be completed for whatever reason (which is highly unlikely to happen), the program will output all the rounds it was able to generate until the round it could not generate pairings for.

### Free Pairings Checker

This framework can also check your pairings to see how they compare to the internal ones generated. Please note however, this pairings checker is only designed to check which pairings given violate basic rules (such as two players cannot play each other again if they met before). Since it outputs both pairings (the internal generated one and the one given for checking), you can make your comparisons and check which ones don't align with that the engine got.

Execute the following command to use the pairings checker

```console
user@user:~$ ./main --fpc "path/to/trf.trf" --fpc_rounds 3
```

Once again, we pass in a TRF16 encoded file, and we pass in what round number to check. In our example, we are asking the engine to check round 3.

### Other commands

Here is a breif list of other commands you can execute:

|Command  |Description   |
|---------|--------------|
|--help   |Standard help |
|--version|Output version|
|--sample |Example usage |

## Use by API

If you do not want to use this by terminal, you can also try executing with C++ code. Since you cannot get pairings written to a file through the terminal method, using the API method is the best alternate to get direct access to all the pairings.

### Standard Pairings

```cpp
/**
* This is a demonstration of how to get pairings by API
*/

#include <iostream>
#include <vector>
#include "Tournament.hpp" // we need this to work with tournaments
#include "Player.hpp" // we also need this to add players

int main() {
    // first make our tournament
    Tournament our_tournament(5); // we pass in 5 for 5 rounds
    
    // now we should add some players
    Player player1("Player 1", 1200, 1, 0.0); // we make a player object with a name, rating, ID, and points
    Player player2("Player 2", 1100, 2, 0.0);
    Player player3("Player 3", 1000, 3, 0.0);
    Player player4("Player 4", 900, 4, 0.0);
    
    our_tournament.addPlayer(player1); // this is how to add the player to the tournament
    our_tournament.addPlayer(player2);
    our_tournament.addPlayer(player3);
    our_tournament.addPlayer(player4);
    
    // now to get the pairings
    std::vector<Match> the_pairings = our_tournament.generatePairings(1); // we pass in the round to generate pairings for
    
    for(int i = 0; i < the_pairings.size(); i++) {
        Match the_match = the_pairings[i];
        std::cout << "White: " << the_match.white.getName() << std::endl;
        if(the_match.is_bye) {
            std::cout << "BYE\n";
        } else {
            std::cout << the_match.black.getName() << std::endl;
        }
    }
    
    return 0;
}
```

In some cases you may need to add more info to player data, such as opponents played. Here is a demonstration below of how to do this.

```cpp
/**
* This demonstration shows how to add info to the player such as color history
*/

#include <iostream>
#include "Player.hpp"

int main() {
    Player example("EXAMPLE", 2000, 1, 1.0);
    Player example2("EXAMPLE2", 1000, 2, 0.0);
    Player example3("EXAMPLE3", 900, 3, 1.0);
    
    // add opponent history
    example.addOpp(2); // we pass in the opponent id here, which in our example is 2. We basically just told the program that player EXAMPLE played whoever has the id of 2
    example2.addOpp(1); // we just marked EXAMPLE2 played EXAMPLE
    example.addOppRating(1000); // it is also VERY important to pass in the opponents ratings played
    example2.addOppRating(2000);
    
    // add color history
    example.addColor(MatchColor::Color::WHITE); // we just said that EXAMPLE already played as white
    example2.addColor(MatchColor::Color::BLACK); // same thing for EXAMPLE2
    
    // adding bye info
    example3.setByeStatus(true); // we just marked EXAMPLE3 as the player who already got a bye
    
    // you should set setByeStatus to true if the player already got the bye or won by forfeit
    
    // you can play with upfloat history as well. In fact if a player upfloated, you should work with this
    example2.incrementUpfloat(); // increments by 1, the number of times the player upfloated in the whole tournament
    example2.setUpfloatPrevStatus(true); // if the player only upfloated in the previous round, this is required to set to true

    return 0;
}
```

A word on **incrementUpfloat** and **setUpfloatPrevStatus**. If you are not familiar with the term "upfloater", this simply means someone who played another person who has a higher score then them. For instance, if Bob (who has 1.0 points) plays Joe (who has 2.0 points), Bob is the upfloater. Float history is extremely important here. For each round the player upfloats, you must signal it to the framework using **incrementUpfloat**. If the player upfloated just the round before the round you are attempting to pair, you must mark **setUpfloatPrevStatus** to true. Otherwise, mark it as false.

## Extra Notes

Please read this section if you are planning on using this framework. It is extremely important to note that as the FIDE rules say, you are required to **make sure all players are rated**. It actually says it in rule 1.1.1 (see rule book). While at least for version 1.0 this pairing engine won't give you any warnings, you must make sure all players have some kind of rating (even provisional is OK).

It is also noted in the FIDE rule book that the number of rounds the tournament has **must** be set. So remember that when using this framework through a terminal, make sure that the **TNR** code is in the TRF file. Don't worry about passing in what round to pair. CPPDubovSystem is designed to auto-detect what round to pair by counting the number of games recorded (for a single player) in the TRF file.

Once again, this pairing engine has not yet been approved by FIDE. But I will send it to them for approval. I will update this README file once they have approved it.

Also note that it is impossible for this framework to violate the 3 absolute criteria. This program does several checks, and ensures that all pairings do not violate the 3 most important criteria noted in the rule book. In other words, don't worry about an invalid pairing.

This framework **does not** do acceleration of any kind here. If you want to have Baku acceleration or another Swiss acceleration, I strongly encourage that you either attempt to code that on your own or use something like [JaVaFo](http://www.rrweb.org/javafo/JaVaFo.htm) by Roberto Ricca.

The framework cannot handle pairing restrictions at the moment as well. When I say pairing restrictions, I mean like if you tell it to stop players with the same team/club from getting paired up with each other. I am hoping that in some future version (maybe 1.1, most likely 1.2) this feature will be made available. Also, try not to "force" pairing restrictions by using the API method. In particular, try to avoid making a pairing restriction by marking two players as played before (i.e. using **addOpp** method). While this **is OK** when the number of players are even, it could create a few problems when the number of players are odd.

This framework also has the limitation in the sense that it doesn't assign board numbers for pairings. This framework is simply a engine for just purely generating pairings. Hopefully in some future version, I will add this feature.

The last limitation this framework has (in specific through the terminal method) is handling requested byes. For now, there is no easy way to signal that a player requested a bye for a pairing round in the TRF file. So if you do not want to pair somebody for a round, simply use the API method by not adding the player to the list of players in the pairing selection.

Here is a list of error codes the framework can give you:

- 0 (success)
- -1 Pairings cannot be completed for some reason (this can be for any reason, like everyone already played everyone in the tournament)
- 1 Malformed TRF file (the data cannot be read). You will actually find that an error will be thrown for this one
- 2 Missing tournament number of rounds

Also note that upon pairing error (error code = -1) CPPDubovSystem will not attempt to repair anybody. It will simply throw you an error

## License

CPPDubovSystem is open source! It is licensed under Apache 2.0 (see LICENSE file).
