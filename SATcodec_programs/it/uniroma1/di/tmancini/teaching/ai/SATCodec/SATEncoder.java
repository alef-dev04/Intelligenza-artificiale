package it.uniroma1.di.tmancini.teaching.ai.SATCodec;

import java.util.*;
import java.io.*;

public class SATEncoder {

	private enum STATUS { init, adding_clauses, end };
	private STATUS status = STATUS.init;

	protected static boolean PLUS = true;
	protected static boolean NOT = false;
	
	private String problemName;
	
	private long startTime = -1;
	private PrintWriter out = new PrintWriter(System.out);
	
	private File tmpFile = null;
	private PrintWriter tmp = null;

	private boolean DEBUG = false;
	
	/** Creates a new SATEncoder. Output will be sent to outFileName. */
	public SATEncoder(String problemName, String outFileName) {
		try {
			this.problemName = problemName;
			if (outFileName != null) {
				this.out = new PrintWriter( new File(outFileName) );
			}
			this.startTime = (new java.util.Date()).getTime();
			tmpFile = File.createTempFile("_" + problemName, ".cnf");
			tmp = new PrintWriter(tmpFile);
		} catch(IOException e) {
			e.printStackTrace();
		}
	}

	/** Creates a new SATEncoder. Output will be sent to standard output */
	public SATEncoder(String problemName) {
		this(problemName, null);
	}

	
	/** Enables debug mode. In debug mode, the clauses refer to symbolic names of the variables 
		and not to their respective DIMACS codes. Also, comments are posted to the output file.
		
		NOTE: When debug mode is enabled, the output file is not in DIMACS format and cannot be
		given to a SAT solver!
	*/
	public void enableDebugMode() {
		if (status != STATUS.init) throw new Error("Debug mode must be enabled before adding clauses");
		this.DEBUG = true;
	}
	
	private long getMillisecSoFar() {
		return (new java.util.Date()).getTime() - startTime;
	}


	private HashMap<String, FamilyOfVariables> dictionary = 
		new HashMap<String, FamilyOfVariables>();

	private void printDictionary(PrintWriter out) {
		if (DEBUG) return;
		out.println("c START DICTIONARY");
		for(FamilyOfVariables family : dictionary.values()) {
			family.printDictionary(out);
		}
		out.println("c END DICTIONARY");
	}


	private int nextDimacsVar = 1;
	int getNextDimacsVar() {
		return nextDimacsVar++;
	}
	
	protected int nbVars() { return nextDimacsVar-1; }

	/** Defines a new family of variables, whose indices range over the Cartesian Product
	  	of the given ranges.
	 */
	@SuppressWarnings("unchecked")	
	public void defineFamilyOfVariables(String name, Range... indexRanges) {
		defineFamilyOfVariables(name, Arrays.asList(indexRanges));
	}
	
	public void defineFamilyOfVariables(String name, List<Range>... indexRanges) {
		RangeProduct finalRangeProd = new RangeProduct("unnamed");
		for(List<Range> lr : indexRanges) finalRangeProd.addRanges(lr);
		defineFamilyOfVariables(name, finalRangeProd);
	}
	public void defineFamilyOfVariables(String name, RangeProduct... indexRangeProd) {	
		if (status != STATUS.init) throw new Error("All variables must be defined before adding clauses");
		assert !dictionary.containsKey(name);
		
		RangeProduct finalRangeProd = new RangeProduct("unnamed", indexRangeProd);
		
		FamilyOfVariables f = new FamilyOfVariables(this, name, finalRangeProd);
		
		dictionary.put(name, f);
		
		//System.out.println("DEFINED VARIABLES:\n" + f.toString() + " (dimensions: " + f.getDimensions() + ")");
	}

	private int nbClauses = 0;
	private boolean clauseIsStarted = false;
	
	/** Adds a positive literal to the current clause
	 */
	public void addToClause(String family, Integer... indices) {	
		addToClause(PLUS, family, indices);
	}
	public void addToClause(String family, List<Integer> indices) {
		addToClause(PLUS, family, indices);
	}

	/** Adds a negative literal to the current clause
	 */
	public void addNegToClause(String family, Integer... indices) {
		addToClause(NOT, family, indices);
	}
	public void addNegToClause(String family, List<Integer> indices) {
		addToClause(NOT, family, indices);
	}

	/** Adds a literal to the current clause
	 */
	public void addToClause(boolean sign, String family, Integer... indices) {
		addToClause(sign, family, Arrays.asList(indices));
	}
	public void addToClause(boolean sign, String family, List<Integer> indices) {		
		assert (status != STATUS.end) : "Clauses cannot be added after having called method end()";
		assert dictionary.get(family).getDimensions() == indices.size();
		status = STATUS.adding_clauses;
		if (DEBUG) {
			tmp.print( (!sign ? "-" : "") + family + indices.toString()  );
		} else {
			int dimacs = dictionary.get(family).dimacs(indices);
			if (!sign) dimacs = -dimacs;
			tmp.print(dimacs);
		}
		tmp.print(" ");
		clauseIsStarted = true;
	}
	
	/** Terminates the current clause
	 */	
	public void endClause() {
		if (!clauseIsStarted) return;		
		assert (status != STATUS.end) : "Clauses cannot be added after having called method end()";
		status = STATUS.adding_clauses;
		if (!DEBUG) tmp.println("0"); else tmp.println();
		nbClauses++;
		clauseIsStarted = false;
	}

	/** Terminates the current clause and posts a comment to the output file. 
		Comments are actually inserted only if debug mode is enabled as commands 
		among clauses violate the DIMACS format.
	 */
	public void addComment(String c) {
		assert (status != STATUS.end) : "Comments cannot be added after having called method end()";
		status = STATUS.adding_clauses;
		if (!DEBUG) return;
		endClause();
		tmp.println("c\nc " + c);
	}
	

	/** Finalizes the generation of the SAT instance.
	 */
	public void end() {
		//System.err.println("Starting generate()");		
		if (status == STATUS.end) return;
		try {			
			long time = getMillisecSoFar();
			tmp.close();
			out.println("c SAT encoding of the " + problemName + " problem");
			out.println("c Generation time: " + time + " millisec");			
			if (DEBUG) out.println("c ***** DEBUG MODE: output is NOT in DIMACS format! *****");		
			printDictionary(out);
			if (!DEBUG) out.println("p cnf "+ nbVars() + " " + nbClauses);
			
			BufferedReader in = new BufferedReader(new FileReader(tmpFile));
			String line = "";
			while((line = in.readLine()) != null) {
				out.println(line);
			}
			in.close();
			out.flush();
			tmpFile.delete();
			status = STATUS.end;
		} catch(IOException e) {
			e.printStackTrace();
		}
	}

} //:~