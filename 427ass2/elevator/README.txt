elevator simulator

ECSE 427
Stepan Salenikovich
260326129
Feb 27, 20

README 

The program is all contained within one file: "main.c".

Please see the usage in the source file, or by running the program with bad args for argument definitions.

As far as it has been tested, it runs according to the specifications.

This simulator is designed to behave more or less like a real elevator.  That is, workers may only get on it when they are on the same floor as the elevator.

There is a potentially weird behaviour in the elevator sweeping algorithm that can occur under certain circumstances.
It is weird in the sense that it causes the elevator to not behave as expected in the real world.
When the elevator is empty (and stopped) and it receives a new request (on a floor below or above it), it will start moving in the direction of the request.  However, it may pick people up on the way there.  If the request which triggered it to go in the a given direction actually wanted to then travel in the opposite (eg: elevator is on floor 5, request is on floor 2 to go to floor 4), then the elevator may pass by that requester without picking him up if there is someone in the elevator who needs to keep going down.
Essentially, the elevator will go to service the first request it gets when its empty; however, it will prioritize anyone who gets in the elevator on the way there.

Also, the simulation does not terminate by itself when its over