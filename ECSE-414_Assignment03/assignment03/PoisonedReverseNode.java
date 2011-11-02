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
		// Construct messages according to the poisoned reverse rule
		// and send these messages to each neighbor
		
		
		// different message to each neighbor
		for (Node neighbor : this.getNeighbors()) {
			HashMap<Node,Float> costs = new HashMap<Node,Float>();
			
			// 
			for ( Node destination : this.getDestinations()) {
				
				// if the neighbor is the next hop to the destination
				// then tell that neighbor that the distance to that destination is infinity
				if (this.getNextHopTo(destination) == neighbor) {
					costs.put(destination, Float.POSITIVE_INFINITY);
				} else {
					costs.put(destination, this.getCostToDestination(destination));
				}
				
				neighbor.sendMessage(new Message(this, costs));
			}
			
		}
	}
}
