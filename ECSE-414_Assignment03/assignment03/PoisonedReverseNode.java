package assignment03;
/**
 * ECSE 414 - Homework Assignment 3, Problem 5
 * Michael Rabbat
 * McGill University
 * michael.rabbat@mcgillca
 * 17 October 2011
 */

import java.util.HashMap;

/**
 * @author michaelrabbat
 *
 */
public class PoisonedReverseNode extends Node {
	public PoisonedReverseNode(String name) {
		super(name);
	}
	
	@Override
	protected void notifyNeighbors() {
		// Step 3: Fill in this method
		
		// Construct messages according to the poisoned reverse rule
		// and send these messages to each neighbor
	}
}
