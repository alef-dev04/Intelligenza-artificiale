package it.uniroma1.di.tmancini.teaching.ai.SATCodec;

import java.util.*;
import java.io.*;


public class IntRange extends Range {
	private int min;
	private int max;
	
	public IntRange(String name, int min, int max) {
		super(name);
		if (min > max) throw new RuntimeException("IntRange(): min value must be <= max value");
		this.min = min;
		this.max = max;
	}
	
	public int getMin() {
		return min;
	}
	public int getMax() {
		return max;
	}
	public int size() {
		return max-min+1;
	}
	public int getDimensions() {
		return 1;
	}
	
	public boolean inBounds(int v) {
		return v >= min && v <= max;
	}
	
	public List<IntRange> flatten() {
		return Collections.singletonList(this);
	}
	
	public String toString() {
		return getName() + ": [" + min + ".." + max + "]";
	}
	
	public class IntRangeIterator implements Iterator<List<Integer>> {
		private IntRange range;
		private List<Integer> next = new ArrayList<Integer>();
		public IntRangeIterator(IntRange r) {
			range = r;
			next.clear();
			next.add( r.min );
		}
		public boolean hasNext() {
			return next.get(0) <= range.max;
		}
		public List<Integer> next() {
			next.set(0, next.get(0)+1);
			return next;
		}
		public void remove() {
			throw new UnsupportedOperationException("IntRangeIterator.remove() not supported.");
		}
	}
	
	public Iterator<List<Integer>> iterator() {
		return new IntRangeIterator(this);
	}


	/** Returns a view of this as an unmodifiable List. So, this.values() can be used in a for loop */
	public List<Integer> values() {
		return new AbstractList<Integer>() {
			public Integer get(int index) {
				if (index < 0 || index >= size()) throw new IndexOutOfBoundsException();				
				return min + index;
			}
			public int size() {
				return IntRange.this.size();
			}
		};
	}

} //:~
