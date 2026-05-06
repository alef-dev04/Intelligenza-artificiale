package it.uniroma1.di.tmancini.teaching.ai.SATCodec;

import java.util.*;
import java.io.*;


public abstract class Range implements Iterable<List<Integer>> {
	private String name;
	
	public Range(String name) {
		this.name = name;
	}
	public String getName() {
		return this.name;
	}
	public abstract int size();
	
	public abstract int getDimensions();
	
	public abstract List<IntRange> flatten();
}
	