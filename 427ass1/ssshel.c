//simple shell

main () { 
    /* Initialize the shell parameters */ 
    parsePath(pathv);  
    // read the environment variables and determine PATH 
    // other initializations might go here such as setting prompt and 
    // all other parameters 
    loop = TRUE; 
    while(loop) { 
        printPrompt(); 
        /* Read the command line and parse it */ 
        readCommand(commandLine) ; 
        parseCommand(commandLine, command); 
        /* Get the full pathname for the file */ 
        command.name = lookupPath(command.argv, pathv); 
        if(command.name == NULL) { 
            /* Report error */ 
            continue; 
        } 
        /* Create child and execute the command */ 
        /* Wait for the child to terminate */ 
    } 
    /* Shell termination */ 
} 
 
