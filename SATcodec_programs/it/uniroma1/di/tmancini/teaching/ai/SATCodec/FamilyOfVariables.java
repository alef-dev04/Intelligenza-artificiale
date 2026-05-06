package it.uniroma1.di.tmancini.teaching.ai.SATCodec;

import java.util.*;
import java.io.*;

class FamilyOfVariables {
		
	private String name;
	protected List<IntRange> indexRanges = new ArrayList<IntRange>();
	private int dim;
	
	private SATEncoder enc;
	
	private List<Object> matrix;
	
	public int getDimensions() {
		return dim;
	}
	
	public String toString() { 
		return name + " --> [\n" + matrix.toString() + "\n]\n"; }
	
	FamilyOfVariables(SATEncoder enc, String name, Range... ranges) {		
		this(enc, name, Arrays.asList(ranges));
	}		
	FamilyOfVariables(SATEncoder enc, String name, List<Range> ranges) {
		this(enc, name, new RangeProduct("unnamed", ranges));
	}
	FamilyOfVariables(SATEncoder enc, String name, RangeProduct rangeProd) {
		assert enc != null;
		this.enc = enc;
		assert name != null;
		this.name = name;
						
		this.indexRanges.addAll(rangeProd.flatten());
		this.dim = rangeProd.getDimensions();
		assert this.dim > 0;
		
		this.matrix = createListRec(0);
	}
	
	private List<Object> createListRec(int coord) {
		assert 0 <= coord && coord <= this.dim;
		List<Object> result = new ArrayList<Object>();
			if (coord == this.dim-1) {
			// base case
			for (int i=0 ; i < indexRanges.get(coord).size() ; i++) {
				result.add(new Integer(enc.getNextDimacsVar()));
			}
		} else {
			// rec. case
			for (int i=0 ; i < indexRanges.get(coord).size() ; i++) {
				result.add( createListRec(coord+1) );
			}
		}
		return result;
	}
	
	@SuppressWarnings("unchecked")
	/*int dimacs(final int... indices) {
		
		List<Integer> indicesAsList = 
			new AbstractList<Integer>() {
		    	public Integer get(int i) { return indices[i]; }
		        public int size() { return indices.length; }
		};		
		return dimacs(indicesAsList);
	}*/
	int dimacs(Integer... indices) {
		return dimacs(Arrays.asList(indices));
	}
	
	@SuppressWarnings("unchecked")
	int dimacs(List<Integer> indices) {		
		//System.err.print("dimacs(" + this.name + Arrays.toString(indices) + "): ");
		assert indices.size() == this.dim;
		List<Object> lastList = this.matrix;
		
		for (int i=0; i<this.dim-1; i++) {			
			lastList = (List<Object>)lastList.get(indices.get(i)-indexRanges.get(i).getMin());
		}
		int result = (Integer)(lastList.get(indices.get(this.dim-1)-indexRanges.get(this.dim-1).getMin()));
		//System.err.println(result);
		return result;
	}
		
	void printDictionary(PrintWriter out) {
		List<Integer> indices = new ArrayList<Integer>(this.dim);
		for(int i=0; i<this.dim; i++) {
			indices.add(this.indexRanges.get(i).getMin());
		}
		
		boolean done = false;
		while (!done) {
			out.print("c ");
			out.print(this.name);
			out.print(indices.toString());
			out.print("=");
			out.println(dimacs(indices));
			
			// increment 'indices'
			int i=this.dim-1;
			boolean incrementDone = false;
			
			while (i >= 0 && !incrementDone) {
				indices.set(i, indices.get(i)+1);
				if (indices.get(i) <= indexRanges.get(i).getMax()) incrementDone = true;
				else {
					indices.set(i, this.indexRanges.get(i).getMin());
					i--;
				}
			}
			if (i < 0) {
				// no increment possible, I'm done.
				done = true;
			}
		}
	}
} //:~