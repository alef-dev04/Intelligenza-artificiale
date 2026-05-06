package it.uniroma1.di.tmancini.teaching.ai.SATCodec;

import java.util.*;
import java.io.*;


public class RangeProduct extends Range {
	private List<Range> ranges = new ArrayList<Range>();
	
	/** Returns the product of the sizes of the component ranges */
	public int size() {
		int result = 1;
		for(Range r : ranges) {
			result *= r.size();
		}
		return result;
	}
	
	
	private int dimensions = 0;
	public int getDimensions() {
		if (dimensions == 0) {
			for(Range r : ranges) {
				dimensions += r.getDimensions();
			}
		}
		return dimensions;
	}	
	
	public static enum FILTER { ALL, ALLDIFF_ORDERED };
	
	//private FILTER filter = FILTER.ALL;
	
	/*public RangeProduct(String name, List<Range> ranges) {
		this(name, FILTER.ALL, ranges);
	}*/
	public RangeProduct(String name, /* FILTER f, */ List<? extends Range> ranges) {
		super(name);
		this.ranges.addAll(ranges);
		/*this.filter = f;
		if (f == FILTER.ALLDIFF_ORDERED) {
			// Checks that range is IntRange
			for(Range r : ranges) {
				if (!r.getClass().equals(IntRange.class)) {
					throw new Error("Filter ALLDIFF_ORDERED can be used only on RangeProducts composed of IntRanges");
				}
			}
		}*/
		
	}
	/*public RangeProduct(String name, Range... ranges) {
		this(name, FILTER.ALL, ranges);
	}*/
	public RangeProduct(String name, /* FILTER f, */  Range... ranges) {
		this(name, /*f,*/ Arrays.asList(ranges));
	}
	/*public RangeProduct(String name, Range range, int power) {
		this(name, FILTER.ALL, range, power);
	}*/
	public RangeProduct(String name, /* FILTER f, */  Range range, int power) {
		super(name);
		if (power <= 0) throw new Error("power must be > 0");
		for(int i=0; i<power; i++) {
			addRange(range);
		}
		/*this.filter = f;
		if (f == FILTER.ALLDIFF_ORDERED) {
			// Checks that range is IntRange
			if (!range.getClass().equals(IntRange.class)) {
				throw new Error("Filter ALLDIFF_ORDERED can be used only on RangeProducts composed of IntRanges");
			}
		}*/		
	}
	
		
	public void addRange(Range r) {
		ranges.add(r);
		dimensions = 0;
	}
	public void addRanges(List<? extends Range> lr) {
		ranges.addAll(lr);
		dimensions = 0;
	}
	
	public List<Range> getRanges() {
		return Collections.unmodifiableList(ranges);
	}
	/*public FILTER getFilter() {
		return filter;
	}*/

	public List<IntRange> flatten() {
		ArrayList<IntRange> result = new ArrayList<IntRange>();
		for(Range r : ranges) {
			result.addAll(r.flatten());
		}
		return result;
	}
	

	public class RangeProductIterator implements Iterator<List<Integer>> {
				
		protected RangeProduct p;
		protected List<Iterator<List<Integer>>> iterators;
		protected boolean hasNext = true;
		protected int size;
		protected List<List<Integer>> next = new ArrayList<List<Integer>>();
		
		
		public RangeProductIterator(RangeProduct p) {
			this.p = p;
			this.size = p.ranges.size();
			this.iterators = new ArrayList<Iterator<List<Integer>>>();
			
			for(Range r : p.ranges) {
				Iterator<List<Integer>> it = r.iterator();
				iterators.add(it);
				next.add(it.next());
			}
		}
		public boolean hasNext() {
			return hasNext;
		}
		public List<Integer> next() {
			@SuppressWarnings("unchecked")
			List<Integer> result = new ArrayList<Integer>(p.getDimensions());
			for(List<Integer> l : next) {
				result.addAll(l);
			}			
			doIncrementNext();
			return result;
		}
		
		protected void doIncrementNext() {
			// Increment next;
			int i = this.size-1;
			boolean incrementDone = false;
			while(i >= 0 && !incrementDone) {
				// Try to increment i-th iterator
				Iterator<List<Integer>> it = this.iterators.get(i);
				if (it.hasNext()) {
					next.set(i, it.next());
					incrementDone = true;
					break;
				} else {
					it = p.ranges.get(i).iterator(); 
					this.iterators.set(i, it);
					next.set(i, it.next());
					i--;
				}
			}
			if (i<0) hasNext = false;			
		}
		
		public void remove() {
			throw new UnsupportedOperationException("IntRangeProductIterator.remove() not supported.");
		}
	}
	
	
	public class RangeProductIteratorAllDiffOrdered implements Iterator<List<Integer>> {
				
		protected RangeProduct p;
		protected List<Iterator<List<Integer>>> iterators;
		protected boolean hasNext = true;
		protected int size;
		protected List<Integer> next = new ArrayList<Integer>();
		
		
		public RangeProductIteratorAllDiffOrdered(RangeProduct p) {
			this.p = p;
			this.size = p.ranges.size();
						
			for(int i=0; i<this.size; i++) {
				if (!p.ranges.get(i).getClass().equals(IntRange.class)) 
					throw new Error("iterator(FILTER.ALLDIFF_ORDERED) is supported only by RangeProducts composed exclusively by IntRanges");

				IntRange r_i = (IntRange)p.ranges.get(i);
				int v_i = (i==0 ? r_i.getMin() : next.get(i-1)+1 );
				if (!r_i.inBounds(v_i)) {
					hasNext = false;
					return;
				} 
				next.add(v_i);
			}
		}
		public boolean hasNext() {
			return hasNext;
		}
		public List<Integer> next() {
			@SuppressWarnings("unchecked")
			List<Integer> result = (List<Integer>)((ArrayList<Integer>)next).clone();
			
			doIncrementNext();
			return result;
		}
		
		protected void doIncrementNext() {
			// Increment next;
			int i = this.size-1;
			boolean incrementDone = false;
			while(i >= 0 && !incrementDone) {
				// Try to increment i-th component				
				int v_i = next.get(i);
				IntRange r_i = (IntRange)p.ranges.get(i);
				if (v_i < r_i.getMax()) {
					// do increment i-th component
					next.set(i, v_i+1);
					// and all those at its right
					incrementDone = true;
					for(int j=i+1; j < this.size; j++) {
						int v_j = next.get(j-1)+1;
						if (!((IntRange)p.ranges.get(j)).inBounds(v_j)) {
							// Not possible, go left
							incrementDone = false;
							break;
						}
						next.set(j, v_j);
					}
				}
				i--;
			}
			// Not possible: return
			if (!incrementDone) hasNext = false;
		}
		
		public void remove() {
			throw new UnsupportedOperationException("IntRangeProductIterator.remove() not supported.");
		}
	}	
	
	
	public Iterator<List<Integer>> iterator() {
		return iterator(FILTER.ALL);
	}

	public Iterator<List<Integer>> iterator(FILTER f) {
		if (f == FILTER.ALL) return new RangeProductIterator(this);
		else {
			RangeProduct rp = new RangeProduct(this.getName(), this.flatten());
			return new RangeProductIteratorAllDiffOrdered(rp);
		}
	}
	
	
} //:~